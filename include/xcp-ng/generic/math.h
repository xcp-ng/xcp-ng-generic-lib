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

#ifndef _XCP_NG_MATH_H_
#define _XCP_NG_MATH_H_

#include "xcp-ng/generic/global.h"

// =============================================================================

#ifdef __cplusplus
extern "C" {
#endif // ifdef __cplusplus

#define XCP_MIN(A, B) ({ \
  __typeof__(A) _a = (A); \
  __typeof__(B) _b = (B); \
  _a < _b ? _a : _b; \
})

#define XCP_MAX(A, B) ({ \
  __typeof__(A) _a = (A); \
  __typeof__(B) _b = (B); \
  _a > _b ? _a : _b; \
})

#ifdef __cplusplus
}
#endif // ifdef __cplusplus

#endif // _XCP_NG_MATH_H_ included
