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

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>

#include "xcp-ng/generic/io.h"

// =============================================================================

XcpError xcp_fd_close (int fd) {
  do {
    if (close(fd) == 0)
      return XCP_ERR_OK;
  } while (errno == EINTR);

  return XCP_ERR_ERRNO;
}

XcpError xcp_fd_dup (int fildes, int fildes2) {
  do {
    const int ret = dup2(fildes, fildes2);
    if (ret >= 0)
      return ret;
  } while (errno == EINTR);
  return XCP_ERR_ERRNO;
}

XcpError xcp_fd_set_close_on_exec (int fd, bool status) {
  int flags;

  if ((flags = fcntl(fd, F_GETFD)) < 0)
    return XCP_ERR_ERRNO;

  if (status)
    flags |= FD_CLOEXEC;
  else
    flags &= ~FD_CLOEXEC;

  if (fcntl(fd, F_SETFD, flags) < 0)
    return XCP_ERR_ERRNO;

  return XCP_ERR_OK;
}

// -----------------------------------------------------------------------------

XcpError xcp_fd_wait_for_rdata (int fd, int timeout) {
  struct pollfd fds = { fd, POLLIN, 0 };
  do {
    const int ret = poll(&fds, 1, timeout);
    if (ret > 0)
      return XCP_ERR_OK;
    if (ret == 0)
      return XCP_ERR_TIMEOUT;
  } while (errno == EAGAIN || errno == EINTR);
  return XCP_ERR_ERRNO;
}

XcpError xcp_fd_read (int fd, void *buf, size_t count) {
  do {
    const ssize_t ret = read(fd, buf, count);
    if (ret >= 0) return ret;

  XCP_C_WARN_PUSH
  XCP_C_WARN_DISABLE_LOGICAL_OP
  } while (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR);
  XCP_C_WARN_POP

  return XCP_ERR_ERRNO;
}

XcpError xcp_fd_wait_read (int fd, void *buf, size_t count, int timeout) {
  if (timeout) {
    const XcpError ret = xcp_fd_wait_for_rdata(fd, timeout);
    if (ret != XCP_ERR_OK) return ret;
  }
  return xcp_fd_read(fd, buf, count);
}

XcpError xcp_fd_read_all (int fd, void *buf, size_t count, int timeout, size_t *offset) {
  size_t pos = 0;
  do {
    const XcpError ret = xcp_fd_wait_read(fd, (char *)buf + pos, count - pos, timeout);
    if (ret < 0) {
      if (offset)
        *offset = pos;
      return XCP_ERR_ERRNO;
    }
    if (ret == 0) break;
    pos += (size_t)ret;
  } while (pos < count);
  if (offset)
    *offset = pos;
  return (XcpError)pos;
}

// -----------------------------------------------------------------------------

XcpError xcp_fd_write (int fd, const void *buf, size_t count) {
  do {
    const ssize_t ret = write(fd, buf, count);
    if (ret >= 0) return ret;

  XCP_C_WARN_PUSH
  XCP_C_WARN_DISABLE_LOGICAL_OP
  } while (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR);
  XCP_C_WARN_POP

  return XCP_ERR_ERRNO;
}

XcpError xcp_fd_write_all (int fd, const void *buf, size_t count, size_t *offset) {
  size_t pos = 0;
  do {
    const XcpError ret = xcp_fd_write(fd, (char *)buf + pos, count - pos);
    if (ret < 0) {
      if (offset)
        *offset = pos;
      return XCP_ERR_ERRNO;
    }
    pos += (size_t)ret;
  } while (pos < count);
  if (offset)
    *offset = pos;
  return (XcpError)pos;
}

// -----------------------------------------------------------------------------

XcpError xcp_fd_pread (int fd, void *buf, size_t count, off_t offset) {
  do {
    const ssize_t ret = pread(fd, buf, count, offset);
    if (ret >= 0) return ret;

  XCP_C_WARN_PUSH
  XCP_C_WARN_DISABLE_LOGICAL_OP
  } while (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR);
  XCP_C_WARN_POP

  return XCP_ERR_ERRNO;
}

// -----------------------------------------------------------------------------

XcpError xcp_poll (struct pollfd *fds, nfds_t nfds, int timeout) {
  do {
    const int ret = poll(fds, nfds, timeout);
    if (ret > 0)
      return ret;
    if (ret == 0)
      return XCP_ERR_TIMEOUT;
  } while (errno == EAGAIN || errno == EINTR);
  return XCP_ERR_ERRNO;
}
