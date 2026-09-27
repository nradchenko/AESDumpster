// AESDumpster - extract AES-256 encryption keys from Unreal Engine executables.
// Cross-platform fork of AESDumpster by GHFear: https://github.com/GHFear/AESDumpster
// Fork modifications are MIT licensed; see LICENSE and NOTICE.

#include "OtherTools.h"

OtherTools::OtherTools()
{
}

void OtherTools::PrintIntro() {
  pcout << "AESDumpster 1.2.5 - By GHFear @ IllusorySoftware" << std::endl;
  pcout << "Supports Unreal Engine 4.19 -> 5.3 | (Will soon support UE 4.0 - 4.18 as well)\n\n" << std::endl;
}

void OtherTools::PrintInstructions() {
  pcout << "Usage:\n";
#if defined _WIN32
  pcout << "-Drag and drop Unreal Engine executables onto AESDumpster.exe.\n";
  pcout << "-Wait for the tool to finish.\n";
  // Keep the drag-and-drop console window open.
  std::cin.ignore();
#else
  pcout << "-Run AESDumpster with the path(s) to Unreal Engine executables as arguments.\n";
#endif
}

void OtherTools::PrintFileName(HANDLE hConsole, const pathchar* argv) {
  SetConsoleTextAttribute(hConsole, 14);
  pcout << argv << std::endl;
  SetConsoleTextAttribute(hConsole, 15);
}

void OtherTools::PrintOutro(HANDLE hConsole) {
  SetConsoleTextAttribute(hConsole, 10);
  pcout << "Done!\n";
  SetConsoleTextAttribute(hConsole, 15);
}

int OtherTools::CreateExeBuffer(pathstring filepath) {

  // Open file (exe).
  FILE* exe_file = nullptr;
#if defined _WIN32
  _wfopen_s(&exe_file, filepath.c_str(), L"rb");
#else
  exe_file = fopen(filepath.c_str(), "rb");
#endif
  if (exe_file == NULL) {
    assert("Error opening exe file");
    return 1;
  }

#if defined _WIN32
  // Save start position.
  uint64_t exe_start_pos = _ftelli64(exe_file);
  // Save archive size and go back to start position.
  _fseeki64(exe_file, 0, SEEK_END);
  uint64_t exe_size = _ftelli64(exe_file);
  _fseeki64(exe_file, exe_start_pos, SEEK_SET);
#else
  // Save start position.
  uint64_t exe_start_pos = ftello(exe_file);
  // Save archive size and go back to start position.
  fseeko(exe_file, 0, SEEK_END);
  uint64_t exe_size = ftello(exe_file);
  fseeko(exe_file, exe_start_pos, SEEK_SET);
#endif

  // Create buffer.
  char* file_buffer = (char*)malloc(exe_size);
  if (file_buffer == NULL) {
    assert("Error allocating memory");
    return 1;
  }

  // Read file into buffer.
  fread(file_buffer, exe_size, 1, exe_file);

  //Fill struct return values.
  retval.buffer = file_buffer;
  retval.size = exe_size;

  return 0;
}

OtherTools::~OtherTools()
{
}