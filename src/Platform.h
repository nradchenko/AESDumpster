#pragma once
// Platform compatibility layer for AESDumpster.
// Provides the Win32 console API and byte types on non-Windows platforms and
// selects the path character type (wide on Windows, narrow UTF-8 elsewhere).

#include <cstdint>
#include <iostream>
#include <string>

#if defined _WIN32
#include <windows.h>

// Paths are wide on Windows, narrow UTF-8 everywhere else.
using pathchar = wchar_t;
using pathstring = std::wstring;
inline auto& pcout = std::wcout;
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

using pathchar = char;
using pathstring = std::string;
inline auto& pcout = std::cout;
#endif