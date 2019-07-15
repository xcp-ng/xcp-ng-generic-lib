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

#include <stdlib.h>
#include <string.h>

#include "xcp-ng/generic/path.h"

// =============================================================================

bool xcp_path_is_abs (const char *pathname) {
  return *pathname == '/';
}

char *xcp_path_combine (const char *pathname, const char *subpath) {
  if (*pathname == '\0' || xcp_path_is_abs(subpath))
    return strdup(subpath);

  const size_t pathnameLen = strlen(pathname);
  const size_t subpathLen = strlen(subpath);

  const bool addSeparator = pathname[pathnameLen - 1] != '/';

  char *res = malloc(pathnameLen + subpathLen + 1 + addSeparator);
  if (!res)
    return NULL;

  // 1. Copy pathname.
  memcpy(res, pathname, pathnameLen);

  // 2. Copy separator if necessary.
  if (addSeparator)
    res[pathnameLen] = '/';

  // 3. Copy subpath.
  memcpy(res + pathnameLen + addSeparator, subpath, subpathLen + 1);

  return res;
}
