#ifndef A_WINDOW_MANAGER_H
#define A_WINDOW_MANAGER_H


#include "AMonitor.h"

#include <list>
#include <functional>
#include <vector>


class AWindowManager {
public:
    AWindowManager();

    void eventLoop();

private:
    Display* display;
    int xScreenNumber;
    int xScreenWidth;
    int xScreenHeight;
    Window rootWindow;
    Window wmWindow;
    bool running;
    Cursor cursors[CURSORS_N];
    std::vector<AKeybind> keybinds;

    unsigned int numLockMask;

    std::list<AMonitor*> monitors;
    AMonitor* activeMonitor{};
    AWorkspace* activeWorkspace{};

    // Atoms
    Atom utf8StringAtom;
    Atom wmAtoms[WM_N_ATOMS];
    Atom netAtoms[NET_N_ATOMS];

    inline unsigned int cleanMask(unsigned int mask) {
        return (mask & ~(numLockMask|LockMask) & (ShiftMask|ControlMask|Mod1Mask|Mod2Mask|Mod3Mask|Mod4Mask|Mod5Mask));
    }

    void focusWindow(AWindow* window);
    void grabKeys();

    // Event handlers
    static int xErrorHandler(Display* display, XErrorEvent* e);
    std::function<void(XEvent*)> eventHandlers[LASTEvent];
    void mapRequestHandler(XEvent* e);
    void mappingNotifyHandler(XEvent* e);
    void keyPressHandler(XEvent* e);

    void updateMonitors();
    void manage(Window xWindow, XWindowAttributes* attributes);
    AWindow* getWindowFromXWindow(Window xWindow);
};


#endif /* A_WINDOW_MANAGER_H */ 