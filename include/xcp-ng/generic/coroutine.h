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

#ifndef _XCP_NG_COROUTINE_H_
#define _XCP_NG_COROUTINE_H_

#include "xcp-ng/generic/global.h"

// =============================================================================

#ifdef __cplusplus
extern "C" {
#endif // ifdef __cplusplus

#define XCP_COROUTINE_STACK_SIZE (1024UL * 1024UL)

typedef struct XcpCoroutine XcpCoroutine;

typedef void (*XcpCoroutineCb)(void *userData);

// Make a new coroutine.
XCP_NO_DISCARD XcpCoroutine *xcp_coroutine_create (XcpCoroutineCb cb, void *userData);

// Return the current coroutine of the execution thread.
XCP_NO_DISCARD XcpCoroutine *xcp_coroutine_get_self ();

// Resume a coroutine.
void xcp_coroutine_resume (XcpCoroutine *coroutine);

// Return to the caller and execute pending coroutines.
void xcp_coroutine_yield ();

// Execute a coroutine directly if we are not curently in a coroutine. In the other cases
// the coroutine is added to the coroutine pending list.
void xcp_coroutine_process (XcpCoroutine *coroutine);

#ifdef __cplusplus
}
#endif // ifdef __cplusplus

#endif // _XCP_NG_COROUTINE_H_ included
