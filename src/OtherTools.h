// AESDumpster - extract AES-256 encryption keys from Unreal Engine executables.
// Cross-platform fork of AESDumpster by GHFear: https://github.com/GHFear/AESDumpster
// Fork modifications are MIT licensed; see LICENSE and NOTICE.

#pragma once
#include <iomanip>
#include <sstream>
#include <string>
#include <algorithm>
#include <map>
#include <iostream>
#include <fstream>
#include <cassert>
#include <sys/stat.h>
#include "Platform.h"
#include <vector>

class OtherTools
{
public:
  OtherTools();
  void PrintIntro();
  void PrintUsage();
  void PrintInstructions();
  void PrintFileName(HANDLE hConsole, const pathchar* argv);
  void PrintOutro(HANDLE hConsole);
  int CreateExeBuffer(pathstring filepath);
	~OtherTools();

public:
  struct RETVAL { char* buffer = nullptr; uint64_t size = 0; } retval;

};

