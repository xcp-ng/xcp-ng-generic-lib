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

#include "xcp-ng/generic/string.h"

// =============================================================================

XCP_NO_DISCARD int xcp_str_to_int (const char *str, bool *ok) {
  const longlong value = xcp_str_to_longlong(str, ok);
  if (ok && value != (int)value)
    *ok = false;
  return (int)value;
}

XCP_NO_DISCARD long xcp_str_to_long (const char *str, bool *ok) {
  const longlong value = xcp_str_to_longlong(str, ok);
  if (ok && value != (long)value)
    *ok = false;
  return (long)value;
}

XCP_NO_DISCARD longlong xcp_str_to_longlong (const char *str, bool *ok) {
  errno = 0;

  char *end;
  const longlong value = strtoll(str, &end, 10);

  if (ok)
    *ok = end != str && errno != ERANGE;

  return value;
}
