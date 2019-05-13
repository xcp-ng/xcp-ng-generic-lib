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

#ifndef _VTBX_NETWORK_H_
#define _VTBX_NETWORK_H_

#include <sys/socket.h>
#include <sys/un.h>

#include "vtbx/global.h"

// =============================================================================

#ifdef __cplusplus
extern "C" {
#endif // ifdef __cplusplus

#define VTBX_SOCK_UNIX_PATH_MAX \
  VTBX_MEMBER_SIZE(struct sockaddr_un, sun_path) / \
  VTBX_MEMBER_SIZE(struct sockaddr_un, sun_path[0])

VtbxError vtbx_sock_connect (int sock, const struct sockaddr *addr, socklen_t addrlen);

VtbxError vtbx_sock_send_shared_fd (int sock, const void *buf, size_t count, int sharedFd);

#ifdef __cplusplus
}
#endif // ifdef __cplusplus

#endif // _VTBX_NETWORK_H_ included
