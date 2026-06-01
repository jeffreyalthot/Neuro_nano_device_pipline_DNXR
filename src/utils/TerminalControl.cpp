#include "TerminalControl.h"

#include <iostream>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace dnxr {
namespace {

void enableAnsiOnWindows() {
#ifdef _WIN32
    static bool enabled = false;
    if (enabled) {
        return;
    }

    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    if (output == INVALID_HANDLE_VALUE) {
        return;
    }

    DWORD mode = 0;
    if (!GetConsoleMode(output, &mode)) {
        return;
    }

    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(output, mode);
    enabled = true;
#endif
}

void writeAnsi(const char* sequence) {
    enableAnsiOnWindows();
    std::cout << sequence;
}

} // namespace

void TerminalControl::clear() {
    writeAnsi("\033[2J\033[H");
}

void TerminalControl::moveCursorHome() {
    writeAnsi("\033[H");
}

void TerminalControl::hideCursor() {
    writeAnsi("\033[?25l");
}

void TerminalControl::showCursor() {
    writeAnsi("\033[?25h");
}

} // namespace dnxr
