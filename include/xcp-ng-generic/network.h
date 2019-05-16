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

#ifndef _XCP_NG_GENERIC_NETWORK_H_
#define _XCP_NG_GENERIC_NETWORK_H_

#include <sys/socket.h>
#include <sys/un.h>

#include "xcp-ng-generic/global.h"

// =============================================================================

#ifdef __cplusplus
extern "C" {
#endif // ifdef __cplusplus

#define XCP_SOCK_UNIX_PATH_MAX \
  XCP_MEMBER_SIZE(struct sockaddr_un, sun_path) / \
  XCP_MEMBER_SIZE(struct sockaddr_un, sun_path[0])

XcpError xcp_sock_connect (int sock, const struct sockaddr *addr, socklen_t addrlen);

XcpError xcp_sock_send_shared_fd (int sock, const void *buf, size_t count, int sharedFd);

#ifdef __cplusplus
}
#endif // ifdef __cplusplus

#endif // _XCP_NG_GENERIC_NETWORK_H_ included
