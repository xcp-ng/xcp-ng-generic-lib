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

#ifndef _XCP_NG_GENERIC_IO_H_
#define _XCP_NG_GENERIC_IO_H_

#include <poll.h>
#include <sys/uio.h>

#include "xcp-ng/generic/global.h"

// =============================================================================

#ifdef __cplusplus
extern "C" {
#endif // ifdef __cplusplus

// TODO: For xcp_fd_wait_read and xcp_poll functions, timeout must be recomputed if an interruption is triggered.

XcpError xcp_fd_close (int fd);

XcpError xcp_fd_dup (int fildes, int fildes2);

XcpError xcp_fd_set_close_on_exec (int fd, bool status);

// -----------------------------------------------------------------------------

// Wait `timeout` milliseconds for available readable data in fd.
XcpError xcp_fd_wait_for_rdata (int fd, int timeout);

// Block until read. (/!\ Same behavior with O_NONBLOCK flag! => Blocking /!\)
XcpError xcp_fd_read (int fd, void *buf, size_t count);

// Wait and read.
XcpError xcp_fd_wait_read (int fd, void *buf, size_t count, int timeout);

// Wait and read `count` bytes.
// If less bytes than expected are returned:
// - EOF is reached for regular files.
// - Pipe is empty and there is no more writer.
// - Broken socket.
XcpError xcp_fd_read_all (int fd, void *buf, size_t count, int timeout, size_t *offset);

// -----------------------------------------------------------------------------

// Write. (/!\ Do not use (bypass) the O_NONBLOCK flag. /!\)
XcpError xcp_fd_write (int fd, const void *buf, size_t count);

// Wait and write `count` bytes.
XcpError xcp_fd_write_all (int fd, const void *buf, size_t count, size_t *offset);

// -----------------------------------------------------------------------------

XcpError xcp_fd_pread (int fd, void *buf, size_t count, off_t offset);

XcpError xcp_fd_preadv (int fd, const struct iovec *iovs, size_t iovCount, off_t offset);

// -----------------------------------------------------------------------------

XcpError xcp_poll (struct pollfd *fds, nfds_t nfds, int timeout);

#ifdef __cplusplus
}
#endif // ifdef __cplusplus

#endif // _XCP_NG_GENERIC_IO_H_ included
