// AESDumpster - extract AES-256 encryption keys from Unreal Engine executables.
// Cross-platform fork of AESDumpster by GHFear: https://github.com/GHFear/AESDumpster
// Fork modifications are MIT licensed; see LICENSE and NOTICE.

#pragma once
// Platform compatibility layer for AESDumpster.
// Provides the Win32 console API and byte types on non-Windows platforms and
// selects the path character type (wide on Windows, narrow UTF-8 elsewhere).

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>
#include <sys/stat.h>

#if defined _WIN32
#include <windows.h>
#include <io.h>

// Paths are wide on Windows, narrow UTF-8 everywhere else.
using pathchar = wchar_t;
using pathstring = std::wstring;
inline auto& pcout = std::wcout;
inline auto& pcerr = std::wcerr;
#else
#define STD_OUTPUT_HANDLE ((std::uint32_t)-11)
using HANDLE = int;

using BYTE = std::uint8_t;
using PBYTE = BYTE*;
using WORD = std::uint16_t;
using PWORD = WORD*;

inline HANDLE GetStdHandle(std::uint32_t handle_id) { return 0; }

// Maps the console attributes used by the codebase to ANSI escape codes.
inline void SetConsoleTextAttribute(HANDLE console, int attribute)
{
  switch (attribute) {
    case 10: std::cout << "\033[32m"; break; // green
    case 12: std::cout << "\033[31m"; break; // red
    case 6: std::cout << "\033[33m"; break; // yellow
    case 14: std::cout << "\033[93m"; break; // bright yellow
    default: std::cout << "\033[0m"; break; // reset
  }
}

#include <sys/mman.h>

using pathchar = char;
using pathstring = std::string;
inline auto& pcout = std::cout;
inline auto& pcerr = std::cerr;
#endif

// Kind of file behind an open stdio stream: a regular file has a known
// size up front; anything else that is readable (pipe, FIFO, character
// device, process substitution) has to be read until EOF.
enum class FileKind { Regular, Directory, Stream, Error };

// Classifies an open file; sets size for regular files. Error leaves errno set.
inline FileKind GetFileKind(FILE* file, std::uint64_t& size)
{
#if defined _WIN32
  struct _stat64 st;
  if (_fstat64(_fileno(file), &st) != 0) { return FileKind::Error; }
  switch (st.st_mode & _S_IFMT) {
    case _S_IFREG: size = (std::uint64_t)st.st_size; return FileKind::Regular;
    case _S_IFDIR: return FileKind::Directory;
    default: return FileKind::Stream;
  }
#else
  struct stat st;
  if (fstat(fileno(file), &st) != 0) { return FileKind::Error; }
  if (S_ISREG(st.st_mode)) { size = (std::uint64_t)st.st_size; return FileKind::Regular; }
  if (S_ISDIR(st.st_mode)) { return FileKind::Directory; }
  return FileKind::Stream;
#endif
}

// Maps the first size bytes of an open regular file read-only. The mapping
// stays valid after the file is closed. Returns nullptr if the file cannot be
// mapped (e.g. a filesystem without mmap support) so the caller can fall back
// to reading it.
inline void* MapFile(FILE* file, std::uint64_t size)
{
#if defined _WIN32
  HANDLE file_handle = (HANDLE)_get_osfhandle(_fileno(file));
  HANDLE mapping = CreateFileMappingW(file_handle, NULL, PAGE_READONLY, 0, 0, NULL);
  if (mapping == NULL) { return nullptr; }
  void* view = MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, (SIZE_T)size);
  // The view keeps the mapping object alive.
  CloseHandle(mapping);
  return view;
#else
  void* view = mmap(nullptr, (size_t)size, PROT_READ, MAP_PRIVATE, fileno(file), 0);
  if (view == MAP_FAILED) { return nullptr; }
  // The scanner makes one pass per signature, so ask for the whole file to
  // be read ahead rather than MADV_SEQUENTIAL's drop-behind.
  posix_madvise(view, (size_t)size, POSIX_MADV_WILLNEED);
  return view;
#endif
}

// Releases a mapping created by MapFile().
inline void UnmapFile(void* view, std::uint64_t size)
{
#if defined _WIN32
  UnmapViewOfFile(view);
#else
  munmap(view, (size_t)size);
#endif
}
