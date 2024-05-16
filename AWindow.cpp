#include "AWindow.h"

#include <X11/Xlib.h>

AWindow::AWindow(int x, int y, int w, int h, int minW, int minH, int maxW,
    int maxH, Window win) : position{x, y}, size{w, h},
    minSize{minW, minH}, maxSize{maxW, maxH}, xWindow{win} {}

