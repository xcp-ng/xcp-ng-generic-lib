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
#include <unistd.h>

#include "xcp-ng/generic/file.h"

// =============================================================================

XcpError xcp_file_close (FILE *fp) {
  do {
    if (fclose(fp) == 0)
      return XCP_ERR_OK;
  } while (errno == EINTR);

  return XCP_ERR_ERRNO;
}

char *xcp_readlink (const char *pathname) {
  size_t bufSize = 16;
  char *buf = malloc(bufSize);
  if (!buf) return NULL;

  ssize_t ret;
  while ((size_t)(ret = readlink(pathname, buf, bufSize)) == bufSize) {
    char *p = realloc(buf, bufSize << 1);
    if (!p) {
      free(buf);
      return NULL;
    }
    buf = p;
  }

  if (ret == -1) {
    free(buf);
    return NULL;
  }

  return buf;
}
