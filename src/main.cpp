#include "cli/cli.h"

#ifdef _WIN32
#include <windows.h> // For SetConsoleOutputCP
#endif

int main() {
#ifdef _WIN32
  // Set console code page to UTF-8 on Windows
  SetConsoleOutputCP(CP_UTF8);
  // SetConsoleCP(CP_UTF8); // Optional: Set input codepage too if needed
#endif

  CLInterface cli;
  cli.Loop();
  return 0;
}