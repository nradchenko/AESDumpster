// AESDumpster - extract AES-256 encryption keys from Unreal Engine executables.
// Cross-platform fork of AESDumpster by GHFear: https://github.com/GHFear/AESDumpster
// Fork modifications are MIT licensed; see LICENSE and NOTICE.

#pragma once
// Single source of truth for the version string. The Makefile overrides this
// at compile time with git describe output; MSVC builds fall back to "dev".
#ifndef AESDUMPSTER_VERSION
#define AESDUMPSTER_VERSION "dev"
#endif