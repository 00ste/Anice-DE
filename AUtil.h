#ifndef A_UTIL_H
#define A_UTIL_H


#include <string>


// ENUMS

enum { WM_PROTOCOLS, WM_DELETE, WM_STATE,
    WM_TAKE_FOCUS, WM_N_ATOMS };

enum { NET_ACTIVE_WINDOW, NET_SUPPORTED,
    NET_WM_NAME, NET_WM_STATE, NET_WM_CHECK,
    NET_WM_FULLSCREEN, NET_WM_WINDOW_TYPE,
    NET_WM_WINDOW_TYPE_DIALOG, NET_CLIENT_LIST,
    NET_N_ATOMS };


// STRUCTS

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