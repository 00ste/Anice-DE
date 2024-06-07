#ifndef A_DRAWABLE_H
#define A_DRAWABLE_H

#include <X11/Xft/Xft.h>
#include <vector>

#include "AUtil.h"

/*
class AFont {
public:
    AFont(const char* font);
    ~AFont();

    Display* display;
    unsigned int height;
    XftFont* xFont;
    FcPattern* pattern;
};
*/
class ADrawable {
public:
    ADrawable() = default;
    ADrawable(Display* display_, int xScreenNumber_, Window rootWindow_, ASize size_);
    ~ADrawable();

    void mapWindow(Window xWindow, APoint pos, ASize size); 

private:
    Display* display;
    int xScreenNumber;
    ASize size;
    Window rootWindow;
    Drawable drawable;
    GC context;
    // std::vector<AFont> fonts;
    std::vector<XftColor> colors;
};

#endif /* A_DRAWABLE_H */
