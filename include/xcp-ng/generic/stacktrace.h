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

#ifndef _XCP_NG_STACKTRACE_H_
#define _XCP_NG_STACKTRACE_H_

#include "xcp-ng/generic/global.h"

// =============================================================================

#ifdef __cplusplus
extern "C" {
#endif // ifdef __cplusplus

typedef void (*XcpCrashHandler)(int signal);

// Set the default crash handler. It displays a stack trace and call xcp_clear_crash_handler to
// remove the default handler. Then the default signal action is executed, so if there is
// a segfault the program is killed.
int xcp_set_default_crash_handler ();

// Set a custom crash handler. Supported signals:
// SIGBUS, SIGFPE, SIGILL, SIGPIPE, SIGSEGV, SIGSYS, SIGTRAP, SIGXCPU, SIGXFSZ
// /!\ When called, if one handler is already set for one signal, it is overriden.
int xcp_set_crash_handler (XcpCrashHandler handler);

// Remove crash handler. If it called in a crash handler context, the default signal action is executed at
// the handler end.
int xcp_clear_crash_handler ();

// Similar to the GNU backstrace API.
// The file name and line number of each frame is displayed in the trace contrary to
// the backtrace_symbols function.
// See: man 3 backtrace
//
// Basic example:
//
// int main (int argc, char *argv[]) {
//   void *buffer[128];
//   const int size = xcp_stacktrace(buffer, XCP_ARRAY_LEN(buffer));
//
//   char **strings = xcp_stacktrace_symbols(buffer, (size_t)size);
//   if (!strings)
//     return 0;
//
//   for (int i = 0; i < size; ++i)
//     printf("%s\n", strings[i]);
//
//   free(strings);
//
//   return 0;
// }
//
// Output:
//
// #0 [0x000000000350d7] /home/ronan/Projets/xcp-ng-generic-lib/build/../src/stacktrace.c:300  xcp_stacktrace() in a.out
// #1 [0x000000000351d3] /home/ronan/Projets/xcp-ng-generic-lib/build/../src/stacktrace.c:323  main() in a.out
// #2 [0x00000000024223] ??:0  __libc_start_main() in /usr/lib/libc.so.6
// #3 [0x0000000003446e] ??:0  _start() in a.out
int xcp_stacktrace (void **buffer, size_t size);

char **xcp_stacktrace_symbols (void *const *buffer, size_t size);

int xcp_stacktrace_symbols_fd (void *const *buffer, size_t size, int fd);

#ifdef __cplusplus
}
#endif // ifdef __cplusplus

#endif // _XCP_NG_STACKTRACE_H_ included
