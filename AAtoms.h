#ifndef A_ATOMS_H
#define A_ATOMS_H


#include <X11/Xlib.h>


enum { WM_PROTOCOLS, WM_DELETE, WM_STATE, 
    WM_TAKE_FOCUS, WM_N_ATOMS };

enum { NET_ACTIVE_WINDOW, NET_SUPPORTED,
    NET_WM_NAME, NET_WM_STATE, NET_WM_CHECK,
    NET_WM_FULLSCREEN, NET_WM_WINDOW_TYPE,
    NET_WM_WINDOW_TYPE_DIALOG, NET_CLIENT_LIST,
    NET_N_ATOMS };

class AAtoms {
public:
    Atom utf8String;
    Atom wm[WM_N_ATOMS];
    Atom net[NET_N_ATOMS];

    AAtoms(Display* display);
};


#endif /* A_ATOMS_H */