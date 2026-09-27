// AESDumpster - extract AES-256 encryption keys from Unreal Engine executables.
// Cross-platform fork of AESDumpster by GHFear: https://github.com/GHFear/AESDumpster
// Fork modifications are MIT licensed; see LICENSE and NOTICE.

#include "OtherTools.h"
#include "KeyDumpster.h"
#include <vector>

#if defined(_WIN32) && defined(_DEBUG)
// Debug main logic
void debugmain(HANDLE hConsole)
{
  // Debug exe path (set your own path to an exe on your ssd here or this won't work)
  pathstring exe_path = L"Z:\\Exes\\NotProtected\\SessionGame-Win64-Shipping.exe";

  // Print filename (exe full path).
  OtherTools* other_tools = new OtherTools();
  other_tools->PrintFileName(hConsole, exe_path.c_str());

  //Create filebuffer using the given filepath.
  if (other_tools->CreateExeBuffer(exe_path) != 0) { return; }
  if (other_tools->retval.buffer == nullptr) { printf("retval.buffer == nullptr.\n"); return; }

  // Create KeyDumpster object, scan for keys and print them.
  KeyDumpster* key_dumpster = new KeyDumpster();
  if (!key_dumpster->FindAESKeys(other_tools->retval.buffer, other_tools->retval.size))
    printf("There were no keys to be found or a problem occurred.\n");
  else
    key_dumpster->PrintKeyInformation();

  // Destruct KeyDumpster
  key_dumpster->~KeyDumpster();
  // Free buffer when done with it.
  free(other_tools->retval.buffer);
  // Print outro before exiting.
  other_tools->PrintOutro(hConsole);
  // Destruct OtherTools
  other_tools->~OtherTools();

  return;
}
#endif

// Release main logic
void releasemain(HANDLE hConsole, const std::vector<pathstring>& args)
{
  OtherTools* other_tools = new OtherTools();

  // Print intro.
  other_tools->PrintIntro();

  if (args.empty()) {
    // Print instructions if launched without command.
    other_tools->PrintInstructions();
    return;
  }

  // Main loop.
  for (const pathstring& exe_path : args) {
    // Print filename (exe full path).
    other_tools->PrintFileName(hConsole, exe_path.c_str());
    // Create filebuffer using the given filepath.
    if (other_tools->CreateExeBuffer(exe_path) != 0) { return; }
    if (other_tools->retval.buffer == nullptr) { printf("retval.buffer == nullptr.\n");  break; }

    // Create KeyDumpster object, scan for keys and print them.
    KeyDumpster* key_dumpster = new KeyDumpster();
    if (!key_dumpster->FindAESKeys(other_tools->retval.buffer, other_tools->retval.size))
      printf("There were no keys to be found or a problem occurred.\n");
    else
      key_dumpster->PrintKeyInformation();

    // Destruct KeyTool object
    key_dumpster->~KeyDumpster();
    // Free buffer when done with it.
    free(other_tools->retval.buffer);
  }
  // Print outro before exiting.
  other_tools->PrintOutro(hConsole);
  // Destruct OtherTools object
  other_tools->~OtherTools();

  return;
}

#if defined _WIN32
// Entrypoint
int wmain(int argc, wchar_t* argv[])
{
  // Console handle.
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

  // Collect command line arguments.
  std::vector<pathstring> args(argv + 1, argv + argc);

#if defined _DEBUG
  debugmain(hConsole);
#else
  releasemain(hConsole, args);
#endif

  std::cin.ignore();
  return 0;
}
#else
// Entrypoint
int main(int argc, char* argv[])
{
  // Console handle.
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

  // Collect command line arguments.
  std::vector<pathstring> args(argv + 1, argv + argc);

  releasemain(hConsole, args);

  return 0;
}
#endif