#pragma once

namespace dnxr {

class TerminalControl {
public:
    static void clear();
    static void moveCursorHome();
    static void hideCursor();
    static void showCursor();
};

} // namespace dnxr
