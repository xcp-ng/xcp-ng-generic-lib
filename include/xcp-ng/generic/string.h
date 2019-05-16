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

#ifndef _XCP_NG_GENERIC_STRING_H_
#define _XCP_NG_GENERIC_STRING_H_

#include "xcp-ng/generic/global.h"

// =============================================================================

#ifdef __cplusplus
extern "C" {
#endif // ifdef __cplusplus

XCP_NO_DISCARD int xcp_str_to_int (const char *str, bool *ok);
XCP_NO_DISCARD long xcp_str_to_long (const char *str, bool *ok);
XCP_NO_DISCARD longlong xcp_str_to_longlong (const char *str, bool *ok);

#ifdef __cplusplus
}
#endif // ifdef __cplusplus

#endif // _XCP_NG_GENERIC_STRING_H_ included
