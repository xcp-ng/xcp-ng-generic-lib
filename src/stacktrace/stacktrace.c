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

#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "xcp-ng/generic/stacktrace.h"

// =============================================================================

static void default_crash_handler (int signal) {
  xcp_set_crash_handler(SIG_DFL);

  void *buffer[128];
  const int size = xcp_stacktrace(buffer, XCP_ARRAY_LEN(buffer));
  xcp_stacktrace_symbols_fd(buffer, (size_t)size, STDERR_FILENO);
  _exit(128 + signal);
}

// -----------------------------------------------------------------------------

int xcp_set_default_crash_handler () {
  return xcp_set_crash_handler(default_crash_handler);
}

int xcp_set_crash_handler (XcpCrashHandler handler) {
  signal(SIGBUS, handler);
  signal(SIGFPE, handler);
  signal(SIGILL, handler);
  signal(SIGPIPE, handler);
  signal(SIGSEGV, handler);
  signal(SIGSYS, handler);
  signal(SIGTRAP, handler);
  signal(SIGXCPU, handler);
  signal(SIGXFSZ, handler);

  return 0;
}

int xcp_clear_crash_handler () {
  return xcp_set_crash_handler(SIG_DFL);
}

int xcp_stacktrace (void **buffer, size_t size) {
  return backtrace(buffer, (int)size);
}

int xcp_stacktrace_symbols_fd (void *const *buffer, size_t size, int fd) {
  char **strings = xcp_stacktrace_symbols(buffer, size);
  if (!strings)
    return -1;

  for (size_t i = 0; i < size; ++i)
    dprintf(fd, "%s\n", strings[i]);

  free(strings);

  return 0;
}
