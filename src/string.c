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
#include <string.h>

#include "xcp-ng/generic/string.h"

// =============================================================================

int xcp_str_to_int (const char *str, bool *ok) {
  const longlong value = xcp_str_to_longlong(str, ok);
  if (ok && value != (int)value)
    *ok = false;
  return (int)value;
}

long xcp_str_to_long (const char *str, bool *ok) {
  const longlong value = xcp_str_to_longlong(str, ok);
  if (ok && value != (long)value)
    *ok = false;
  return (long)value;
}

longlong xcp_str_to_longlong (const char *str, bool *ok) {
  errno = 0;

  char *end;
  const longlong value = strtoll(str, &end, 10);

  if (ok)
    *ok = end != str && errno != ERANGE;

  return value;
}

// -----------------------------------------------------------------------------

static inline char *xcp_create_hex_buf (const void *buf, size_t count, bool reverse) {
  if (!count) return NULL;

  static const char prefix[] = "0x";
  static const size_t prefixSize = sizeof prefix - 1;

  const size_t hexBufSize = prefixSize + count * 2 + 1;
  char *hexBuf = malloc(hexBufSize);
  if (!hexBuf) return NULL;

  strncpy(hexBuf, prefix, prefixSize);
  hexBuf[hexBufSize - 1] = '\0';

  static const char hex[] = "0123456789ABCDEF";

  if (reverse) {
    char *pos = hexBuf + hexBufSize - 2;
    for (size_t i = 0; i < count; ++i) {
      const uchar byte = ((unsigned char *)buf)[i];
      *pos-- = hex[byte & 0xF];
      *pos-- = hex[byte >> 4];
    }
  } else {
    char *pos = hexBuf + prefixSize;
    for (size_t i = 0; i < count; ++i) {
      const uchar byte = ((unsigned char *)buf)[i];
      *pos++ = hex[byte >> 4];
      *pos++ = hex[byte & 0xF];
    }
  }

  return hexBuf;
}

char *xcp_buf_to_hex (const void *buf, size_t count) {
  return xcp_create_hex_buf(buf, count, false);
}

char *xcp_buf_to_reverse_hex (const void *buf, size_t count) {
  return xcp_create_hex_buf(buf, count, true);
}
