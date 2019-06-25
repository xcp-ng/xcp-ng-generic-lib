/*
 * Copyright (C) 2019  Vates SAS - ronan.abhamon@vates.fr
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

// =============================================================================
// See: https://www.gnu.org/software/pth/ (pth_mctx.c)
// Implementation of: http://runtime.bordeaux.inria.fr/ssep/Biblio/Eng_gnupth_usenix00.pdf
// Old paper: www.mit.edu/afs.new/sipb/user/nathanw/work/sched/papers/rse-pmt.ps
// =============================================================================

#include <assert.h>
#include <pthread.h>
#include <setjmp.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/queue.h>
#include <unistd.h>

#include "config.h"

#ifdef HAVE_VALGRIND
  #include <valgrind/valgrind.h>
#endif // ifdef HAVE_VALGRIND

#include "xcp-ng/generic/coroutine.h"
#include "xcp-ng/generic/math.h"

// =============================================================================

static_assert(XCP_COROUTINE_STACK_SIZE >= MINSIGSTKSZ, "");

// -----------------------------------------------------------------------------

static void *xcp_coroutine_create_stack (size_t *stackSize) {
  const size_t pageSize = (size_t)sysconf(_SC_PAGESIZE);
  // The last page is used as guardpage.
  *stackSize = XCP_ROUND_UP_2(XCP_COROUTINE_STACK_SIZE, pageSize) + pageSize;

  void *stack = mmap(NULL, *stackSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (stack == MAP_FAILED)
    return NULL;

  // See: https://devarea.com/using-mprotect-system-call-to-debug-memory-problems/
  // And: https://rethinkdb.com/blog/handling-stack-overflow-on-custom-stacks/
  if (mprotect(stack, pageSize, PROT_NONE)) {
    munmap(stack, *stackSize);
    return NULL;
  }

  return stack;
}

static void xcp_coroutine_destroy_stack (void *stack) {
  const size_t pageSize = (size_t)sysconf(_SC_PAGESIZE);
  const size_t stackSize = XCP_ROUND_UP_2(XCP_COROUTINE_STACK_SIZE, pageSize) + pageSize;
  munmap(stack, stackSize);
}

// ---------------------------------------------------------------------------

typedef enum {
  XcpCoroutineStatusRunning = 1,
  XcpCoroutineStatusSuspend = 2,
  XcpCoroutineStatusTerminated = 3
} XcpCoroutineStatus;

// Using sigsetjmp seems better... See `man 3 setjmp`:
//
// POSIX does not specify whether setjmp() will save the signal mask (to
// be later restored during longjmp()).  In System V it will not.  In
// 4.3BSD it will, and there is a function _setjmp() that will not.  The
// behavior under Linux depends on the glibc version and the setting of
// feature test macros.
//
// So, it seems nice to have the same behavior on all supported platforms.

struct XcpCoroutine {
  XcpCoroutine *caller;
  void *stack;
  sigjmp_buf env;

  XcpCoroutineCb cb;

  void *arg;

  #ifdef HAVE_VALGRIND
    uint valgrindStackId;
  #endif // ifdef HAVE_VALGRIND

  STAILQ_ENTRY(XcpCoroutine) next;
  STAILQ_HEAD( , XcpCoroutine) pendings;
};

// -----------------------------------------------------------------------------

typedef struct {
  sigjmp_buf env;
  volatile sig_atomic_t called;
  XcpCoroutine *coroutine;
} Trampoline;

typedef struct {
  XcpCoroutine *current;

  Trampoline trampoline;

  XcpCoroutine dummy;
} XcpCoroutineThreadData;

static XcpCoroutineThreadData *xcp_coroutine_get_thread_data () {
  static __thread XcpCoroutineThreadData threadData;
  if (!threadData.current)
    threadData.current = &threadData.dummy;
  return &threadData;
}

// -----------------------------------------------------------------------------

static int xcp_coroutine_exec (XcpCoroutine *caller, XcpCoroutine *callee, XcpCoroutineStatus status) {
  xcp_coroutine_get_thread_data()->current = callee;

  const int ret = sigsetjmp(caller->env, 0);
  if (ret == 0)
    siglongjmp(callee->env, status);
  return ret;
}

static void xcp_coroutine_run () {
  XcpCoroutine *coroutine = xcp_coroutine_get_thread_data()->trampoline.coroutine;

  // 10. Save context for the first user callback execution.
  if (!sigsetjmp(coroutine->env, 0))
    // 11. Return to xcp_coroutine_init for the last time.
    siglongjmp(*(sigjmp_buf *)coroutine->arg, 1);

  (*coroutine->cb)(coroutine->arg);
  xcp_coroutine_exec(coroutine, coroutine->caller, XcpCoroutineStatusTerminated);

  // Not reachable.
  abort();
}

static void xcp_coroutine_trampoline (int signal) {
  XCP_UNUSED(signal);

  XcpCoroutineThreadData *threadData = xcp_coroutine_get_thread_data();
  xcp_coroutine_get_thread_data()->trampoline.called = true;

  // 5. Save context and return to the xcp_coroutine_init function.
  if (!sigsetjmp(threadData->trampoline.env, 0))
    return;

  // 9. Called again! Go to a clean stack frame!
  xcp_coroutine_run();
}

static void xcp_coroutine_destroy (XcpCoroutine *coroutine) {
  #ifdef HAVE_VALGRIND
    VALGRIND_STACK_DEREGISTER(coroutine->valgrindStackId);
  #endif // ifdef HAVE_VALGRIND

  xcp_coroutine_destroy_stack(coroutine->stack);
  free(coroutine);
}

static void xcp_coroutine_init (XcpCoroutine *coroutine, size_t stackSize) {
  // 1. Block temporarily SIGUSR1 and save old sig mask set.
  sigset_t set, oldSet;
  sigemptyset(&set);
  sigaddset(&set, SIGUSR1);
  if (pthread_sigmask(SIG_BLOCK, &set, &oldSet))
    abort();

  // 2. Save the current sigaction for the SIGUSR1 signal + add custom handler with alternate stack flag.
  struct sigaction sa, oldSa;
  sa.sa_handler = xcp_coroutine_trampoline;
  sa.sa_flags = SA_ONSTACK;
  sigfillset(&sa.sa_mask);
  if (sigaction(SIGUSR1, &sa, &oldSa))
    abort();

  // 3. Save the current alternate signal stack + replace it with our custom stack.
  stack_t ss, oldSs;
  ss.ss_sp = coroutine->stack;
  ss.ss_size = stackSize;
  ss.ss_flags = 0;
  if (sigaltstack(&ss, &oldSs))
    abort();

  // 4. Exec trampoline for the first time.
  XcpCoroutineThreadData *threadData = xcp_coroutine_get_thread_data();
  threadData->trampoline.called = false;
  threadData->trampoline.coroutine = coroutine;

  pthread_kill(pthread_self(), SIGUSR1);
  sigfillset(&set);
  sigdelset(&set, SIGUSR1);
  while (!threadData->trampoline.called)
    sigsuspend(&set);
    // 5. Saving context in the trampoline function.
    // ...

  #ifdef HAVE_VALGRIND
    coroutine->valgrindStackId = VALGRIND_STACK_REGISTER(coroutine->stack, (char *)coroutine->stack + stackSize);
  #endif // ifdef HAVE_VALGRIND

  // 6. Restore previous stack, sigaction for SIGUSR1 and sig mask set.
  ss.ss_flags = SS_DISABLE;
  if (
    sigaltstack(&ss, NULL) ||
    sigaltstack(&oldSs, NULL) ||
    sigaction(SIGUSR1, &oldSa, NULL) ||
    pthread_sigmask(SIG_SETMASK, &oldSet, NULL)
  )
    abort();

  // 7. Save current context.
  sigjmp_buf oldEnv;
  coroutine->arg = &oldEnv;

  if (!sigsetjmp(oldEnv, 0))
    // 8. Restore previous context in the trampoline.
    siglongjmp(threadData->trampoline.env, 1);
    // 9-11: Trampoline execution...

  // 12. So far so good! :)
}

// -----------------------------------------------------------------------------

XcpCoroutine *xcp_coroutine_create (XcpCoroutineCb cb, void *userData) {
  // TODO: Maybe use a pool of XcpCoroutines.

  // 0. Create coroutine + stack.
  XcpCoroutine *coroutine = malloc(sizeof *coroutine);
  if (!coroutine) return NULL;

  size_t stackSize;
  if (!(coroutine->stack = xcp_coroutine_create_stack(&stackSize))) {
    free(coroutine);
    return NULL;
  }
  coroutine->caller = NULL;
  coroutine->cb = cb;

  STAILQ_INIT(&coroutine->pendings);

  // Initialization is in a separate function because a -Wclobbered warn is triggered in release mode.
  xcp_coroutine_init(coroutine, stackSize);

  coroutine->arg = userData;
  return coroutine;
}

XcpCoroutine *xcp_coroutine_get_self () {
  return xcp_coroutine_get_thread_data()->current;
}

void xcp_coroutine_resume (XcpCoroutine *coroutine) {
  // 1. Add the coroutine in a pending list.
  STAILQ_HEAD( , XcpCoroutine) pendings;
  STAILQ_INIT(&pendings);
  STAILQ_INSERT_TAIL(&pendings, coroutine, next);

  XcpCoroutine *self = xcp_coroutine_get_self();
  while (!STAILQ_EMPTY(&pendings)) {
    // 2.a. Fetch and resume the first pending coroutine.
    XcpCoroutine *callee = STAILQ_FIRST(&pendings);
    STAILQ_REMOVE_HEAD(&pendings, next);
    if (callee->caller)
      abort(); // Already called!
    callee->caller = self;

    const int ret = xcp_coroutine_exec(self, callee, XcpCoroutineStatusRunning);

    // 2.b. If there are pendings (async) coroutines on the last executed
    // coroutine, add them in the main pending list.
    STAILQ_CONCAT(&callee->pendings, &pendings);
    STAILQ_CONCAT(&pendings, &callee->pendings);

    switch (ret) {
      case XcpCoroutineStatusTerminated:
        xcp_coroutine_destroy(coroutine);
      case XcpCoroutineStatusSuspend:
        break;
      default:
        abort();
    }
  }
}

void xcp_coroutine_yield () {
  XcpCoroutine *self = xcp_coroutine_get_self();
  if (!self->caller)
    abort(); // Cannot yield if there is no caller.

  XcpCoroutine *caller = self->caller;
  self->caller = NULL;
  xcp_coroutine_exec(self, caller, XcpCoroutineStatusSuspend);
}

void xcp_coroutine_process (XcpCoroutine *coroutine) {
  const XcpCoroutineThreadData *threadData = xcp_coroutine_get_thread_data();
  if (!threadData->current->caller)
    xcp_coroutine_resume(coroutine);
  else {
    XcpCoroutine *self = xcp_coroutine_get_self();
    assert(coroutine != self); // Avoid recursion...
    STAILQ_INSERT_TAIL(&self->pendings, coroutine, next);
  }
}
