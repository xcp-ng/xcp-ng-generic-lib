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

#include "vtbx/io.h"

// =============================================================================

VtbxError vtbx_fd_close (int fd) {
  do {
    if (close(fd) == 0)
      return VTBX_ERR_OK;
  } while (errno == EINTR);

  return VTBX_ERR_ERRNO;
}

VtbxError vtbx_fd_dup (int fildes, int fildes2) {
  do {
    const int ret = dup2(fildes, fildes2);
    if (ret >= 0)
      return ret;
  } while (errno == EINTR);
  return VTBX_ERR_ERRNO;
}

VtbxError vtbx_fd_set_close_on_exec (int fd, bool status) {
  int flags;

  if ((flags = fcntl(fd, F_GETFD)) < 0)
    return VTBX_ERR_ERRNO;

  if (status)
    flags |= FD_CLOEXEC;
  else
    flags &= ~FD_CLOEXEC;

  if (fcntl(fd, F_SETFD, flags) < 0)
    return VTBX_ERR_ERRNO;

  return VTBX_ERR_OK;
}

VtbxError vtbx_fd_wait_for_rdata (int fd, int timeout) {
  struct pollfd fds = { fd, POLLIN, 0 };
  do {
    const int ret = poll(&fds, 1, timeout);
    if (ret > 0)
      return VTBX_ERR_OK;
    if (ret == 0)
      return VTBX_ERR_TIMEOUT;
  } while (errno == EAGAIN || errno == EINTR);
  return VTBX_ERR_ERRNO;
}

VtbxError vtbx_fd_read (int fd, void *buf, size_t count) {
  do {
    const ssize_t ret = read(fd, buf, count);
    if (ret >= 0) return ret;

  VTBX_C_WARN_PUSH
  VTBX_C_WARN_DISABLE_LOGICAL_OP
  } while (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR);
  VTBX_C_WARN_POP

  return VTBX_ERR_ERRNO;
}

VtbxError vtbx_fd_wait_read (int fd, void *buf, size_t count, int timeout) {
  if (timeout) {
    const VtbxError ret = vtbx_fd_wait_for_rdata(fd, timeout);
    if (ret != VTBX_ERR_OK) return ret;
  }
  return vtbx_fd_read(fd, buf, count);
}

VtbxError vtbx_fd_read_all (int fd, void *buf, size_t count, int timeout, size_t *offset) {
  *offset = 0;
  do {
    const VtbxError ret = vtbx_fd_wait_read(fd, (char *)buf + *offset, count - *offset, timeout);
    if (ret < 0) return VTBX_ERR_ERRNO;
    if (ret == 0) break;
    *offset += (size_t)ret;
  } while (*offset < count);
  return (VtbxError)*offset;
}

VtbxError vtbx_fd_write (int fd, const void *buf, size_t count) {
  do {
    const ssize_t ret = write(fd, buf, count);
    if (ret >= 0) return ret;

  VTBX_C_WARN_PUSH
  VTBX_C_WARN_DISABLE_LOGICAL_OP
  } while (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR);
  VTBX_C_WARN_POP

  return VTBX_ERR_ERRNO;
}

VtbxError vtbx_fd_write_all (int fd, const void *buf, size_t count, size_t *offset) {
  *offset = 0;
  do {
    const VtbxError ret = vtbx_fd_write(fd, (char *)buf + *offset, count - *offset);
    if (ret < 0) return VTBX_ERR_ERRNO;
    *offset += (size_t)ret;
  } while (*offset < count);
  return (VtbxError)*offset;
}

VtbxError vtbx_select (struct pollfd *fds, nfds_t nfds, int timeout) {
  do {
    const int ret = poll(fds, nfds, timeout);
    if (ret > 0)
      return ret;
    if (ret == 0)
      return VTBX_ERR_TIMEOUT;
  } while (errno == EAGAIN || errno == EINTR);
  return VTBX_ERR_ERRNO;
}
