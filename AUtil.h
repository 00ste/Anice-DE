#ifndef A_UTIL_H
#define A_UTIL_H


#include <string>
#include <functional>
#include <sstream>
#include <X11/Xlib.h>
#include <X11/keysym.h>


// ENUMS

enum { WM_PROTOCOLS, WM_DELETE, WM_STATE,
    WM_TAKE_FOCUS, WM_N_ATOMS };

enum { NET_ACTIVE_WINDOW, NET_SUPPORTED,
    NET_WM_NAME, NET_WM_STATE, NET_WM_CHECK,
    NET_WM_FULLSCREEN, NET_WM_WINDOW_TYPE,
    NET_WM_WINDOW_TYPE_DIALOG, NET_CLIENT_LIST,
    NET_N_ATOMS };

enum { CURSOR_NORMAL, CURSOR_RESIZE, CURSOR_MOVE, CURSORS_N};

// ENUM MAPS / ARRAYS

static std::string eventTypeToString[LASTEvent] = {
        "0", "1", "KeyPress", "KeyRelease", "ButtonPress",
        "ButtonRelease", "MotionNotify", "EnterNotify", "LeaveNotify",
        "FocusIn", "FocusOut", "KeymapNotify", "Expose",
        "GraphicsExpose", "NoExpose", "VisibilityNotify",
        "CreateNotify", "DestroyNotify", "UnmapNotify", "MapNotify",
        "MapRequest", "ReparentNotify", "ConfigureNotify",
        "ConfigureRequest", "GravityNotify", "ResizeRequest",
        "CirculateNotify", "CirculateRequest", "PropertyNotify",
        "SelectionClear", "SelectionRequest", "SelectionNotify",
        "ColormapNotify", "ClientMessage", "MappingNotify",
        "GenericEvent"
};

static std::string modToString(unsigned int mod) {
    std::stringstream output;

    if ((mod & ShiftMask) != 0 )    output << "Shift";
    if ((mod & LockMask) != 0)      output << "Num Lock";
    if ((mod & ControlMask) != 0)   output << "Ctrl";
    if ((mod & Mod1Mask) != 0)      output << "Mod1";
    if ((mod & Mod2Mask) != 0)      output << "Mod2";
    if ((mod & Mod3Mask) != 0)      output << "Mod3";
    if ((mod & Mod4Mask) != 0)      output << "Mod4";
    if ((mod & Mod5Mask) != 0)      output << "Mod5";

    return output.str();
}

/* FROM X.h ALL THE EVENT TYPES AND THEIR RESPECTIVE ID:
#define KeyPress                2
#define KeyRelease              3
#define ButtonPress             4
#define ButtonRelease           5
#define MotionNotify            6
#define EnterNotify             7
#define LeaveNotify             8
#define FocusIn                 9
#define FocusOut                10
#define KeymapNotify            11
#define Expose                  12
#define GraphicsExpose          13
#define NoExpose                14
#define VisibilityNotify        15
#define CreateNotify            16
#define DestroyNotify           17
#define UnmapNotify             18
#define MapNotify               19
#define MapRequest              20
#define ReparentNotify          21
#define ConfigureNotify         22
#define ConfigureRequest        23
#define GravityNotify           24
#define ResizeRequest           25
#define CirculateNotify         26
#define CirculateRequest        27
#define PropertyNotify          28
#define SelectionClear          29
#define SelectionRequest        30
#define SelectionNotify         31
#define ColormapNotify          32
#define ClientMessage           33
#define MappingNotify           34
#define GenericEvent            35
*/

// STRUCTS

typedef struct {
    int pressType;                      // such as KeyPress or KeyRelease (defined in Xlib.h)
    unsigned int mod;
    KeySym keysym;
    std::function<void(void)> action;
} AKeybind;

struct _APoint {
    int x;
    int y;
};

typedef _APoint APoint;
typedef _APoint ASize;

struct _ARect {
    int x;
    int y;
    int w;
    int h;
};

// CLASSES

static std::string LOG_FILE_PATH = "/home/stefano/code/c-cpp/anice-cpp/logs/log.txt";

class ALogger {
public:
    enum Tag { INFO, DEBUG, WARNING, ERROR };
    static const char* tagToString(ALogger::Tag tag);
    static void logMessageToFile(ALogger::Tag tag, const std::string& message);

};


typedef _ARect ARect;

#endif /* A_UTIL_H */
