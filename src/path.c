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

#define _GNU_SOURCE
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

static inline const char *get_first_slash (const char *start, const char *pos) {
  const char *p;
  for (p = pos - 1; p >= start; --p)
    if (*p != '/')
      break;
  return p + 1;
}

// Examples:
// "/" => /
// "/toto" => /
// "titi/a" => titi
// "gfrg/grg/" => gfrg
// "/gfrg/grg/" => /gfrg
// "////gfrg///grg///" => ////gfrg
// "///gfrg////grg" => ///gfrg
// "/gfrg////grg" => /gfrg
// "titia" => .
// "/toto/" => /
// "//" => //
// "///" => /
// "////" => /
// "//a" => //
// "///b" => /
// "////c" => /
// "////c/d" => ////c
char *xcp_path_parent_dir (const char *pathname) {
  // 1. Find last slash in pathname.
  const char *slash = pathname ? strrchr(pathname, '/') : NULL;

  // 2. Slash is not the first char and there is no char after it.
  // So we are in a directory, we must find the previous '/'.
  if (slash && slash != pathname && slash[1] == '\0') {
    const char *p = get_first_slash(pathname, slash);
    if (p != pathname)
      slash = memrchr(pathname, '/', (size_t)(p - pathname));
  }

  // 3. If there is no slash, return default '.' path.
  if (!slash) {
    char *dir = malloc(2);
    if (!dir) return NULL;
    dir[0] = '.';
    dir[1] = '\0';
    return dir;
  }

  const char *p = get_first_slash(pathname, slash);
  if (p == pathname) {
    // Preserve two consecutives slashes or keep only one slash in other cases.
    // See: http://pubs.opengroup.org/onlinepubs/009695399/basedefs/xbd_chap04.html
    // 4.11 Pathname Resolution
    // "A pathname that begins with two successive slashes may be interpreted in
    // an implementation-defined manner, although more than two leading slashes shall
    // be treated as a single slash."
    slash = (slash == pathname + 1) ? slash + 1 : pathname + 1;
  } else
    slash = p;

  const size_t len = (size_t)(slash - pathname);
  char *dir = strndup(pathname, len + 1);
  if (dir)
    dir[len] = '\0';

  return dir;
}
