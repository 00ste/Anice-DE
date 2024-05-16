#ifndef A_WINDOW_MANAGER_H
#define A_WINDOW_MANAGER_H


#include "AMonitor.h"
// #include "AAtoms.h"

#include <list>
#include <functional>


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
    // AAtoms atoms;
    bool running;

    std::list<AMonitor*> monitors;
    AMonitor* activeMonitor{};
    AWorkspace* activeWorkspace{};

    // Atoms
    Atom utf8StringAtom;
    Atom wmAtoms[WM_N_ATOMS];
    Atom netAtoms[NET_N_ATOMS];

    // Event handlers
    std::function<void(XEvent*)> eventHandlers[LASTEvent];
    void mapRequestHandler(XEvent* e);

    void updateMonitors();
    void manage(Window xWindow, XWindowAttributes* attributes);
    AWindow* getWindowFromXWindow(Window xWindow);
};


#endif /* A_WINDOW_MANAGER_H */ 