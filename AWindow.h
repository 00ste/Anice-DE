#ifndef A_WINDOW_H
#define A_WINDOW_H


#include "AUtil.h"

#include <string>
#include <X11/Xlib.h>

class AWindow {
public:
    AWindow(int x, int y, int w, int h, int minW, int minH, int maxW,
        int maxH, Window win);
    ~AWindow() = default;

    // Getters
    std::string getTitle() const { return title; }
    APoint getPosition() const { return position; }
    ASize getSize() const { return size; }
    ASize getMaxSize() const { return maxSize; }
    ASize getMinSize() const { return minSize; }
    const Window& getXWindow() const { return xWindow; }

    // Setters
    void setTitle(const std::string& newTitle) { title = newTitle; }
    void setX(int x, int y) { size = { x, y }; }
    void setSize(int width, int height) { size = { width, height }; }
    void setMaxSize(int width, int height) { maxSize = { width, height }; }
    void setMinSize(int width, int height) { minSize = { width, height }; }

private:
    std::string title;
    APoint position;
    ASize size;
    ASize maxSize;
    ASize minSize;
    Window xWindow;
};


#endif /* A_WINDOW_MANAGER_H */

