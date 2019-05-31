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
#include <assert.h>
#include <execinfo.h>
#include <inttypes.h>
#include <link.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PACKAGE 1
#include <bfd.h>

#include "xcp-ng/generic/io.h"
#include "xcp-ng/generic/math.h"
#include "xcp-ng/generic/stacktrace.h"
#include "xcp-ng/generic/file.h"

#define HEX_ADDRESS_LEN ((int)(sizeof(uintptr_t) * 2))

// =============================================================================

// -----------------------------------------------------------------------------
// Symbol API.
// -----------------------------------------------------------------------------

typedef struct Symbol {
  const char *filepath; // The pathname from which the shared object was loaded.
  void *baseAddress;    // Base object address.
  void *address;        // Virtual symbol address from backstrace.
} Symbol;

typedef struct SymbolInfo {
  const char *filename;
  const char *function;
  uint line;
  const void *physAddress;
  const char *libraryName;
} SymbolInfo;

static int symbol_info_to_str (const SymbolInfo *info, char *buf, size_t size) {
  const char *function = info->function;
  if (!function || *function == '\0')
    function = "??";

  const char *filename = info->filename;
  if (!filename || *filename == '\0')
    filename = "??";

  return snprintf(
    buf, size, "[%#0*" PRIxPTR "] %s:%u  %s() in %s",
    HEX_ADDRESS_LEN, (uintptr_t)info->physAddress, filename, info->line, function, info->libraryName
  );
}

// -----------------------------------------------------------------------------

static int read_symbols (bfd *bin, asymbol ***symbols) {
  if (!(bfd_get_file_flags(bin) & HAS_SYMS))
    return -1; // No symbols in object file.

  uint size;
  long count = bfd_read_minisymbols(bin, false, (void *)symbols, &size);
  if (count == 0)
    count = bfd_read_minisymbols(bin, true, (void *)symbols, &size);

  if (count < 0)
    return -1; // Unable to read symbols.

  return 0;
}

// -----------------------------------------------------------------------------

static bool find_physical_address_info (bfd *bin, asection *section, asymbol **symbols, SymbolInfo *info) {
  if ((bfd_get_section_flags(bin, section) & SEC_ALLOC) == 0)
    return false; // No debug info in this section.

  const bfd_vma physAddress = (bfd_vma)info->physAddress;
  const bfd_vma vma = bfd_get_section_vma(bin, section);
  if (physAddress < vma)
    return false;

  const bfd_size_type size = bfd_section_size(bin, section);
  if (physAddress >= vma + size)
    return false;

  return bfd_find_nearest_line(bin, section, symbols, physAddress - vma, &info->filename, &info->function, &info->line);
}

static char *physical_address_to_str_from_syms (bfd *bin, asymbol **symbols, const void *physAddress) {
  SymbolInfo info = { .physAddress = physAddress };

  bool found = false;
  for (asection *section = bin->sections; section; section = section->next)
    if (find_physical_address_info(bin, section, symbols, &info)) {
      found = true;
      break;
    }

  if (!found) {
    memset(&info, 0, sizeof info);
    info.physAddress = physAddress;
  }

  info.libraryName = bin->filename;
  const int size = symbol_info_to_str(&info, NULL, 0) + 1;
  if (size <= 0)
    return NULL;

  char *str = malloc((size_t)size);
  if (str && symbol_info_to_str(&info, str, (size_t)size) < 0) {
    free(str);
    return NULL;
  }
  return str;
}

static char *physical_addr_to_str_from_file (const char *filename, const void *physAddress) {
  bfd *bin = bfd_openr(filename, NULL);
  if (!bin)
    return NULL;

  asymbol **symbols;

  if (!bfd_check_format(bin, bfd_object) || read_symbols(bin, &symbols) < 0) {
    bfd_close(bin);
    return NULL;
  }

  char *buf = physical_address_to_str_from_syms(bin, symbols, physAddress);
  free(symbols);

  bfd_close(bin);
  return buf;
}

static int find_symbol_location (struct dl_phdr_info *info, size_t size, void *data) {
  XCP_UNUSED(size);

  Symbol *symbol = data;

  // See: http://man7.org/linux/man-pages/man3/dl_iterate_phdr.3.html
  const ElfW(Addr) baseAddress = info->dlpi_addr;

  const ElfW(Phdr) *headers = info->dlpi_phdr;
  const ElfW(Half) headerCount = info->dlpi_phnum;

  for (ElfW(Half) i = 0; i < headerCount; ++i) {
    const ElfW(Phdr) *header = &headers[i];
    if (header->p_type != PT_LOAD)
      continue; // Ignore unloadable program segment.

    const ElfW(Addr) address = baseAddress + header->p_vaddr;
    if (symbol->address >= (void *)address && symbol->address < (void *)(address + header->p_memsz)) {
      symbol->filepath = info->dlpi_name;
      symbol->baseAddress = (void *)info->dlpi_addr;

      return true; // It's good! Stop foreach. \o/
    }
  }

  return false;
}

static char **stacktrace_symbols (void *const *buffer, size_t size) {
  // 1. Ensure bfd initialization.
  bfd_init();

  // 2. Find for each symbol: source file and line.
  char *self = xcp_readlink("/proc/self/exe");
  if (!self)
    return NULL;

  int i = (int)size;

  size_t charCount = 0;
  char **locations = malloc(size * sizeof(char **));
  if (!locations)
    goto fail;

  for (--i; i >= 0; --i) {
    Symbol symbol = { .address = buffer[i] };

    if (!dl_iterate_phdr(find_symbol_location, &symbol)) {
      const int size = asprintf(&locations[i], "[0x\?\?\?\?] \?\?:\?\?  \?\?()");
      if (size < 0)
        goto fail;

      charCount += (size_t)size + 1;
      continue;
    }

    // Physical address. Can be used directly by addr2line. :)
    const void *physAddr = (void *)((char *)buffer[i] - (char *)symbol.baseAddress);

    if (symbol.filepath && *symbol.filepath)
      // Shared library.
      locations[i] = physical_addr_to_str_from_file(symbol.filepath, physAddr);
    else
      // Binary.
      locations[i] = physical_addr_to_str_from_file(self, physAddr);

    if (!locations[i])
      goto fail;

    charCount += strlen(locations[i]) + 1;
  }

  // 3. Build symbol strings.
  // strings contains:
  // One big string at end containing all symbol strings.
  // `size` pointers on the big string.
  const int sizeLength = (int)log10((double)XCP_MIN(1u, size - 1)) + 2;
  // `sizeLength + 1` because there is a `#` in the front of frame. ;)
  charCount += (size_t)(sizeLength + 1) * size;

  char **strings = malloc(charCount + size * sizeof(char *));
  if (!strings)
    goto fail;

  char *data = (char *)(strings + size);

  for (int j = (int)size - 1; j >= 0; --j) {
    const size_t offset = (size_t)sprintf(data, "#%-*u", sizeLength, j);
    strcpy(data + offset, locations[j]);
    strings[j] = data;
    data += offset + strlen(locations[j]) + 1;
    free(locations[j]);
  }
  assert(charCount == (size_t)(data - (char *)(strings + size)));

  free(locations);
  free(self);

  return strings;

fail:
  for (; i < (int)size; ++i)
    free(locations[i]);
  free(locations);
  free(self);

  return NULL;
}

static void default_crash_handler () {
  xcp_set_crash_handler(SIG_DFL);

  void *buffer[128];
  const int size = xcp_stacktrace(buffer, XCP_ARRAY_LEN(buffer));
  xcp_stacktrace_symbols_fd(buffer, (size_t)size, STDERR_FILENO);
}

// -----------------------------------------------------------------------------

int xcp_set_default_crash_handler () {
  return xcp_set_crash_handler(default_crash_handler);
}

int xcp_set_crash_handler (XcpCrashHandler handler) {
  signal(SIGBUS, handler);
  signal(SIGFPE, handler);
  signal(SIGILL, handler);
  signal(SIGPIPE, handler);
  signal(SIGSEGV, handler);
  signal(SIGSYS, handler);
  signal(SIGTRAP, handler);
  signal(SIGXCPU, handler);
  signal(SIGXFSZ, handler);

  return 0;
}

int xcp_clear_crash_handler () {
  return xcp_set_crash_handler(SIG_DFL);
}

int xcp_stacktrace (void **buffer, size_t size) {
  return backtrace(buffer, (int)size);
}

char **xcp_stacktrace_symbols (void *const *buffer, size_t size) {
  return stacktrace_symbols(buffer, size);
}

int xcp_stacktrace_symbols_fd (void *const *buffer, size_t size, int fd) {
  char **strings = xcp_stacktrace_symbols(buffer, size);
  if (!strings)
    return -1;

  for (size_t i = 0; i < size; ++i)
    dprintf(fd, "%s\n", strings[i]);

  free(strings);

  return 0;
}
