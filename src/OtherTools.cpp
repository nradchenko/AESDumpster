// AESDumpster - extract AES-256 encryption keys from Unreal Engine executables.
// Cross-platform fork of AESDumpster by GHFear: https://github.com/GHFear/AESDumpster
// Fork modifications are MIT licensed; see LICENSE and NOTICE.

#include "OtherTools.h"
#include "Version.h"
#include <cerrno>
#include <system_error>

OtherTools::OtherTools()
{
}

void OtherTools::PrintIntro() {
  pcout << "AESDumpster " << AESDUMPSTER_VERSION << std::endl;
  pcout << "Supports Unreal Engine 4.19 -> 5.3 | (Will soon support UE 4.0 - 4.18 as well)\n\n" << std::endl;
}

void OtherTools::PrintUsage() {
  pcout << "Usage: aesdumpster <exe> [exe ...]\n\n";
  pcout << "Scans Unreal Engine executables (UE 4.19 - 5.3) for AES-256\n";
  pcout << "encryption keys and prints candidates ranked by entropy.\n\n";
  pcout << "  -h, --help     show this help and exit\n";
  pcout << "  -V, --version  show version and exit\n";
#if defined _WIN32
  pcout << "\nOn Windows you can also drag and drop executables onto AESDumpster.exe.\n";
#endif
}

void OtherTools::PrintInstructions() {
  PrintUsage();
#if defined _WIN32
  // Keep the drag-and-drop console window open.
  std::cin.ignore();
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

// Reports a file error on stderr, e.g. "Error: game.exe: cannot open: No such file or directory".
void OtherTools::PrintFileError(const pathstring& filepath, const char* what, int err) {
  pcerr << "Error: " << filepath << ": " << what << ": " << std::generic_category().message(err).c_str() << std::endl;
}

int OtherTools::CreateExeBuffer(pathstring filepath) {

  // Open file (exe).
  FILE* exe_file = nullptr;
#if defined _WIN32
  int open_err = _wfopen_s(&exe_file, filepath.c_str(), L"rb");
#else
  exe_file = fopen(filepath.c_str(), "rb");
  int open_err = errno;
#endif
  if (exe_file == NULL) {
    PrintFileError(filepath, "cannot open", open_err);
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
    PrintFileError(filepath, "cannot allocate buffer", ENOMEM);
    return 1;
  }

  // Read file into buffer.
  fread(file_buffer, exe_size, 1, exe_file);

  //Fill struct return values.
  retval.buffer = file_buffer;
  retval.size = exe_size;

  return 0;
}

// Releases the buffer from CreateExeBuffer() and clears retval so a stale
// pointer is never reused for the next file.
void OtherTools::FreeExeBuffer() {
  free(retval.buffer);
  retval.buffer = nullptr;
  retval.size = 0;
}

OtherTools::~OtherTools()
{
}