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

#ifndef _VTBX_GLOBAL_H_
#define _VTBX_GLOBAL_H_

#include <sys/types.h>

#ifndef __cplusplus
  #include <stdbool.h>
#endif // ifndef __cplusplus

// =============================================================================

#if defined(__GNUC__)
  #define VTBX_C_GNU
  #ifdef __clang__
    #define VTBX_C_CLANG
  #endif // ifdef __clang__
#else
  #error "Unable to find a valid compiler."
#endif // if defined(__GNUC__)

// -----------------------------------------------------------------------------

#define VTBX_DECL_UNUSED __attribute__((__unused__))

#if defined(__cplusplus) && __cplusplus >= 201703L
  #define VTBX_NO_DISCARD [[ nodiscard ]]
#else
  #define VTBX_NO_DISCARD
#endif // if defined(__cplusplus) && __cplusplus >= 201703L

#define VTBX_UNUSED(ARG) ((void)ARG)

// -----------------------------------------------------------------------------

#define VTBX_C_PRAGMA(WARNING) _Pragma(#WARNING)

#if defined(VTBX_C_CLANG)
  #define VTBX_C_WARN_PUSH VTBX_C_PRAGMA(clang diagnostic push)
  #define VTBX_C_WARN_POP VTBX_C_PRAGMA(clang diagnostic pop)
  #define VTBX_C_WARN_DISABLE_CLANG(TEXT) VTBX_C_PRAGMA(clang diagnostic ignored TEXT)
  #define VTBX_C_WARN_DISABLE_GCC(TEXT)
#elif defined(VTBX_C_GNU)
  #define VTBX_C_WARN_PUSH VTBX_C_PRAGMA(GCC diagnostic push)
  #define VTBX_C_WARN_POP VTBX_C_PRAGMA(GCC diagnostic pop)
  #define VTBX_C_WARN_DISABLE_CLANG(TEXT)
  #define VTBX_C_WARN_DISABLE_GCC(TEXT) VTBX_C_PRAGMA(GCC diagnostic ignored TEXT)
#endif // if defined(VTBX_C_CLANG)

#define VTBX_C_WARN_DISABLE_LOGICAL_OP VTBX_C_WARN_DISABLE_GCC("-Wlogical-op")

// -----------------------------------------------------------------------------

typedef long long longlong;

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned long long ulonglong;

// -----------------------------------------------------------------------------

typedef ssize_t VtbxError;

#define VTBX_ERR_OK 0
#define VTBX_ERR_ERRNO -1
#define VTBX_ERR_TIMEOUT -2

// -----------------------------------------------------------------------------

#ifndef foreach
  #define foreach(IT, ARR) \
    for (IT = (ARR); IT < (ARR) + sizeof(ARR) / sizeof (ARR)[0]; ++IT)
#endif // ifndef foreach

// -----------------------------------------------------------------------------

#define VTBX_ARRAY_LEN(ARR) (sizeof(ARR) / sizeof((ARR)[0]))

#define VTBX_MEMBER_SIZE(TYPE, MEMBER) sizeof(((TYPE *)NULL)->MEMBER)

#endif // _VTBX_GLOBAL_H_ included
