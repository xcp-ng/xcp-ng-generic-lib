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

#ifndef _XCP_NG_ENDIAN_H_
#define _XCP_NG_ENDIAN_H_

#include <stdint.h>

#include "xcp-ng/generic/global.h"

// =============================================================================

#ifdef __cplusplus
extern "C" {
#endif // ifdef __cplusplus

#define XCP_BIG_ENDIAN 4321
#define XCP_LITTLE_ENDIAN 1234

#if defined(__i386) || defined(__i386__) || defined(_M_IX86) || \
    defined(__x86_64) || defined(__x86_64__) || defined(__amd64) || defined(_M_X64)
  #define XCP_BYTE_ORDER XCP_LITTLE_ENDIAN
#else
  #error "Unable to set XCP_BYTE_ORDER."
#endif

// -----------------------------------------------------------------------------

static inline uint16_t xcp_swap_endian_u16 (uint16_t src) {
  return (uint16_t)(((src & 0x00FF) << 8) | ((src & 0xFF00) >> 8));
}

XCP_DECL_UNUSED static inline int16_t xcp_swap_endian_16 (int16_t src) {
  return (int16_t)xcp_swap_endian_u16((uint16_t)src);
}

static inline uint32_t xcp_swap_endian_u32 (uint32_t src) {
  return ((src & 0x000000FF) << 24) |
    ((src & 0x0000FF00) << 8) |
    ((src & 0x00FF0000) >> 8) |
    ((src & 0xFF000000) >> 24);
}

XCP_DECL_UNUSED static inline int32_t xcp_swap_endian_32 (int32_t src) {
  return (int32_t)xcp_swap_endian_u32((uint32_t)src);
}

static inline uint64_t xcp_swap_endian_u64 (uint64_t src) {
  return ((src & 0x00000000000000FF) << 56) |
    ((src & 0x000000000000FF00) << 40) |
    ((src & 0x0000000000FF0000) << 24) |
    ((src & 0x00000000FF000000) << 8) |
    ((src & 0x000000FF00000000) >> 8) |
    ((src & 0x0000FF0000000000) >> 24) |
    ((src & 0x00FF000000000000) >> 40) |
    ((src & 0xFF00000000000000) >> 56);
}

XCP_DECL_UNUSED static inline int64_t xcp_swap_endian_64 (int64_t src) {
  return (int64_t)xcp_swap_endian_u64((uint64_t)src);
}

// -----------------------------------------------------------------------------

static inline uint16_t xcp_from_le_u16 (uint16_t src) {
  #if XCP_BYTE_ORDER == XCP_BIG_ENDIAN
    return xcp_swap_endian_u16(src);
  #else
    return src;
  #endif // if XCP_BYTE_ORDER == XCP_BIG_ENDIAN
}

static inline int16_t xcp_from_le_16 (int16_t src) {
  return (int16_t)xcp_from_le_u16((uint16_t)src);
}

static inline uint32_t xcp_from_le_u32 (uint32_t src) {
  #if XCP_BYTE_ORDER == XCP_BIG_ENDIAN
    return xcp_swap_endian_u32(src);
  #else
    return src;
  #endif // if XCP_BYTE_ORDER == XCP_BIG_ENDIAN
}

static inline int32_t xcp_from_le_32 (int32_t src) {
  return (int32_t)xcp_from_le_u32((uint32_t)src);
}

static inline uint64_t xcp_from_le_u64 (uint64_t src) {
  #if XCP_BYTE_ORDER == XCP_BIG_ENDIAN
    return xcp_swap_endian_u64(src);
  #else
    return src;
  #endif // if XCP_BYTE_ORDER == XCP_BIG_ENDIAN
}

static inline int64_t xcp_from_le_64 (int64_t src) {
  return (int64_t)xcp_from_le_u64((uint64_t)src);
}

// -----------------------------------------------------------------------------

static inline uint16_t xcp_from_be_u16 (uint16_t src) {
  #if XCP_BYTE_ORDER == XCP_LITTLE_ENDIAN
    return xcp_swap_endian_u16(src);
  #else
    return src;
  #endif // if XCP_BYTE_ORDER == XCP_BIG_ENDIAN
}

static inline int16_t xcp_from_be_16 (int16_t src) {
  return (int16_t)xcp_from_be_u16((uint16_t)src);
}

static inline uint32_t xcp_from_be_u32 (uint32_t src) {
  #if XCP_BYTE_ORDER == XCP_LITTLE_ENDIAN
    return xcp_swap_endian_u32(src);
  #else
    return src;
  #endif // if XCP_BYTE_ORDER == XCP_BIG_ENDIAN
}

static inline int32_t xcp_from_be_32 (int32_t src) {
  return (int32_t)xcp_from_be_u32((uint32_t)src);
}

static inline uint64_t xcp_from_be_u64 (uint64_t src) {
  #if XCP_BYTE_ORDER == XCP_LITTLE_ENDIAN
    return xcp_swap_endian_u64(src);
  #else
    return src;
  #endif // if XCP_BYTE_ORDER == XCP_BIG_ENDIAN
}

static inline int64_t xcp_from_be_64 (int64_t src) {
  return (int64_t)xcp_from_be_u64((uint64_t)src);
}

// -----------------------------------------------------------------------------

XCP_DECL_UNUSED static inline uint16_t xcp_from_le_u16_p (uint16_t *src) {
  return (*src = xcp_from_le_u16(*src));
}

XCP_DECL_UNUSED static inline int16_t xcp_from_le_16_p (int16_t *src) {
  return (*src = xcp_from_le_16(*src));
}

XCP_DECL_UNUSED static inline uint32_t xcp_from_le_u32_p (uint32_t *src) {
  return (*src = xcp_from_le_u32(*src));
}

XCP_DECL_UNUSED static inline int32_t xcp_from_le_32_p (int32_t *src) {
  return (*src = xcp_from_le_32(*src));
}

XCP_DECL_UNUSED static inline uint64_t xcp_from_le_u64_p (uint64_t *src) {
  return (*src = xcp_from_le_u64(*src));
}

XCP_DECL_UNUSED static inline int64_t xcp_from_le_64_p (int64_t *src) {
  return (*src = xcp_from_le_64(*src));
}

// -----------------------------------------------------------------------------

XCP_DECL_UNUSED static inline uint16_t xcp_from_be_u16_p (uint16_t *src) {
  return (*src = xcp_from_be_u16(*src));
}

XCP_DECL_UNUSED static inline int16_t xcp_from_be_16_p (int16_t *src) {
  return (*src = xcp_from_be_16(*src));
}

XCP_DECL_UNUSED static inline uint32_t xcp_from_be_u32_p (uint32_t *src) {
  return (*src = xcp_from_be_u32(*src));
}

XCP_DECL_UNUSED static inline int32_t xcp_from_be_32_p (int32_t *src) {
  return (*src = xcp_from_be_32(*src));
}

XCP_DECL_UNUSED static inline uint64_t xcp_from_be_u64_p (uint64_t *src) {
  return (*src = xcp_from_be_u64(*src));
}

XCP_DECL_UNUSED static inline int64_t xcp_from_be_64_p (int64_t *src) {
  return (*src = xcp_from_be_64(*src));
}

#ifdef __cplusplus
}
#endif // ifdef __cplusplus

#endif // _XCP_NG_ENDIAN_H_ included
