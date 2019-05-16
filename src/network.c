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
#include <stdlib.h>

#include "vtbx/io.h"
#include "vtbx/network.h"

// =============================================================================

VtbxError vtbx_sock_connect (int sock, const struct sockaddr *addr, socklen_t addrlen) {
  do {
    const int ret = connect(sock, addr, addrlen);
    if (ret >= 0)
      return ret;
  } while (errno == EINTR);
  return VTBX_ERR_ERRNO;
}

VtbxError vtbx_sock_send_shared_fd (int sock, const void *buf, size_t count, int sharedFd) {
  // See example: http://man7.org/linux/man-pages/man3/cmsg.3.html
  // Other example: https://blog.cloudflare.com/know-your-scm_rights/
  struct iovec vec;
  vec.iov_base = (void *)buf;
  vec.iov_len = count;

  union {
    char buf[CMSG_SPACE(sizeof sharedFd)];
    struct cmsghdr align;
  } u;
  memset(u.buf, 0, sizeof u);

  struct msghdr msg;
  msg.msg_name = NULL;
  msg.msg_namelen = 0;
  msg.msg_iov = &vec;
  msg.msg_iovlen = 1;
  msg.msg_control = u.buf;
  msg.msg_controllen = sizeof(u.buf);

  struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
  cmsg->cmsg_level = SOL_SOCKET;
  cmsg->cmsg_type = SCM_RIGHTS;
  cmsg->cmsg_len = CMSG_LEN(sizeof sharedFd);
  *(int *)(CMSG_DATA(cmsg)) = sharedFd;

  do {
    // Try to send buf and shared socket.
    const ssize_t ret = sendmsg(sock, &msg, 0);
    if (ret >= 0) {
      if (ret >= (ssize_t)count)
        return VTBX_ERR_OK;

      // Send last bytes if necessary.
      size_t offset;
      return vtbx_fd_write_all(sock, (char *)buf + ret, count - (size_t)ret, &offset);
    }

  VTBX_C_WARN_PUSH
  VTBX_C_WARN_DISABLE_LOGICAL_OP
  } while (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR);
  VTBX_C_WARN_POP

  return VTBX_ERR_ERRNO;
}
