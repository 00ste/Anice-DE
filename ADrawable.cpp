#include "ADrawable.h"

#include <X11/Xlib.h>


ADrawable::ADrawable(Display* display_, int xScreenNumber_, Window rootWindow_, ASize size_)
    : display{display_}, xScreenNumber{xScreenNumber_}, rootWindow{rootWindow_}, size{size_}
{
    drawable = XCreatePixmap(display, rootWindow, size.x, size.y, DefaultDepth(display, xScreenNumber));
    context = XCreateGC(display, rootWindow, 0, nullptr);
    XSetLineAttributes(display, context, 1, LineSolid, CapButt, JoinMiter);
}

ADrawable::~ADrawable() {
    XFreePixmap(display, drawable);
    XFreeGC(display, context);
    // TODO: Delete list of fonts
}

void ADrawable::mapWindow(Window xWindow, APoint windowPos, ASize windowSize) {
    XCopyArea(display, drawable, xWindow, context, windowPos.x, windowPos.y,
        windowSize.x, windowSize.y, windowPos.x, windowPos.y);
    XSync(display, False);
}
