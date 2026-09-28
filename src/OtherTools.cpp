// AESDumpster - extract AES-256 encryption keys from Unreal Engine executables.
// Cross-platform fork of AESDumpster by GHFear: https://github.com/GHFear/AESDumpster
// Fork modifications are MIT licensed; see LICENSE and NOTICE.

#include "OtherTools.h"
#include "Version.h"
#include <cerrno>
#include <cstdint>
#include <cstdlib>
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

  // Regular files are mapped (or read) in one go using their size;
  // directories are rejected (fopen() succeeds on them on Linux); pipes and
  // other streams have no size up front and are read until EOF.
  uint64_t exe_size = 0;
  FileKind kind = GetFileKind(exe_file, exe_size);
  if (kind == FileKind::Error || kind == FileKind::Directory) {
    PrintFileError(filepath, "cannot read", kind == FileKind::Directory ? EISDIR : errno);
    fclose(exe_file);
    return 1;
  }
  if (kind == FileKind::Stream) {
    int err = ReadStream(exe_file);
    fclose(exe_file);
    if (err != 0) {
      PrintFileError(filepath, "read failed", err);
      return 1;
    }
    return 0;
  }
  // The whole file must fit in the address space (matters for 32-bit builds).
  if (exe_size > SIZE_MAX) {
    PrintFileError(filepath, "cannot allocate buffer", EFBIG);
    fclose(exe_file);
    return 1;
  }

  // Map the file instead of copying it into memory; fall back to reading it
  // if mapping is not supported.
  if (exe_size > 0) {
    void* view = MapFile(exe_file, exe_size);
    if (view != nullptr) {
      fclose(exe_file);
      retval.buffer = (char*)view;
      retval.size = exe_size;
      retval.mapped = true;
      return 0;
    }
  }

  // Create buffer.
  char* file_buffer = (char*)malloc(exe_size);
  if (file_buffer == NULL && exe_size > 0) {
    PrintFileError(filepath, "cannot allocate buffer", ENOMEM);
    fclose(exe_file);
    return 1;
  }

  // Read file into buffer. A short read (I/O error, file truncated while
  // reading) would otherwise leave part of the buffer uninitialized.
  if (exe_size > 0 && fread(file_buffer, exe_size, 1, exe_file) != 1) {
    PrintFileError(filepath, "read failed", ferror(exe_file) ? errno : EIO);
    free(file_buffer);
    fclose(exe_file);
    return 1;
  }
  fclose(exe_file);

  //Fill struct return values.
  retval.buffer = file_buffer;
  retval.size = exe_size;

  return 0;
}

// Reads a stream of unknown length until EOF into a growing buffer and
// fills retval. Returns 0 on success, otherwise an errno value.
int OtherTools::ReadStream(FILE* stream) {
  size_t capacity = 1 << 20;
  size_t size = 0;
  char* buffer = (char*)malloc(capacity);
  if (buffer == NULL) { return ENOMEM; }

  for (;;) {
    size += fread(buffer + size, 1, capacity - size, stream);
    if (size < capacity) {
      if (ferror(stream)) {
        int err = errno ? errno : EIO;
        free(buffer);
        return err;
      }
      break; // EOF
    }
    // Buffer full: double it and keep reading.
    char* grown = (capacity <= SIZE_MAX / 2) ? (char*)realloc(buffer, capacity * 2) : NULL;
    if (grown == NULL) {
      free(buffer);
      return ENOMEM;
    }
    buffer = grown;
    capacity *= 2;
  }

  retval.buffer = buffer;
  retval.size = size;
  return 0;
}

// Releases the buffer from CreateExeBuffer() and clears retval so a stale
// pointer is never reused for the next file.
void OtherTools::FreeExeBuffer() {
  if (retval.mapped) {
    UnmapFile(retval.buffer, retval.size);
  }
  else {
    free(retval.buffer);
  }
  retval.buffer = nullptr;
  retval.size = 0;
  retval.mapped = false;
}

OtherTools::~OtherTools()
{
}