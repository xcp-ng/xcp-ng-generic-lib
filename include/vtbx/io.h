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

#ifndef _VTBX_IO_H_
#define _VTBX_IO_H_

#include <poll.h>

#include "vtbx/global.h"

// =============================================================================

#ifdef __cplusplus
extern "C" {
#endif // ifdef __cplusplus

// TODO: For vtbx_fd_wait_read and vtbx_select functions, timeout must be recomputed if an interruption is triggered.

VtbxError vtbx_fd_close (int fd);

VtbxError vtbx_fd_dup (int fildes, int fildes2);

VtbxError vtbx_fd_set_close_on_exec (int fd, bool status);

// Wait `timeout` milliseconds for available readable data in fd.
VtbxError vtbx_fd_wait_for_rdata (int fd, int timeout);

// Block until read. (/!\ Same behavior with O_NONBLOCK flag! => Blocking /!\)
VtbxError vtbx_fd_read (int fd, void *buf, size_t count);

// Wait and read.
VtbxError vtbx_fd_wait_read (int fd, void *buf, size_t count, int timeout);

// Wait and read `count` bytes.
// If less bytes than expected are returned:
// - EOF is reached for regular files.
// - Pipe is empty and there is no more writer.
// - Broken socket.
VtbxError vtbx_fd_read_all (int fd, void *buf, size_t count, int timeout, size_t *offset);

// Write. (/!\ Do not use (bypass) the O_NONBLOCK flag. /!\)
VtbxError vtbx_fd_write (int fd, const void *buf, size_t count);

// Wait and write `count` bytes. Can returns less bytes than expected.
VtbxError vtbx_fd_write_all (int fd, const void *buf, size_t count, size_t *offset);

VtbxError vtbx_select (struct pollfd *fds, nfds_t nfds, int timeout);

#ifdef __cplusplus
}
#endif // ifdef __cplusplus

#endif // _VTBX_IO_H_ included
