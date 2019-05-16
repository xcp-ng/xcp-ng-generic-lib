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

#ifndef _XCP_NG_GENERIC_ALGORITHM_H_
#define _XCP_NG_GENERIC_ALGORITHM_H_

#include <string.h>

#include "xcp-ng/generic/global.h"

// =============================================================================

#ifdef __cplusplus
extern "C" {
#endif // ifdef __cplusplus

// -----------------------------------------------------------------------------

typedef bool (*XcpPredicate)(const void *elem, const void *needle);

XCP_NO_DISCARD static inline bool xcp_pred_str_equal (const void *elem, const void *needle) {
  return !strcmp(*(const char **)elem, (const char *)needle);
}

// -----------------------------------------------------------------------------

XCP_NO_DISCARD static inline size_t xcp_arr_index_of (
  const void *arr,
  size_t elemSize,
  size_t n,
  const void *needle,
  XcpPredicate predicate
) {
  const char *p = (const char *)arr;
  const char *end = p + n * elemSize;
  for (; p < end; p += elemSize)
    if (predicate(p, needle))
      return (p - (char *)arr) / elemSize;
  return (size_t)-1;
}

// -----------------------------------------------------------------------------

XCP_NO_DISCARD XCP_DECL_UNUSED static inline size_t xcp_str_arr_index_of (
  const char **arr,
  size_t n,
  const char *needle
) {
  return xcp_arr_index_of(arr, sizeof(char *), n, needle, xcp_pred_str_equal);
}

#ifdef __cplusplus
}
#endif // ifdef __cplusplus

#endif // _XCP_NG_GENERIC_ALGORITHM_H_ included
