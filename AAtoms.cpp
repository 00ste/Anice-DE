#include "AAtoms.h"

AAtoms::AAtoms(Display* display) {
    utf8String = XInternAtom(display, "UTF8_STRING", False);
	wm[WM_PROTOCOLS] = XInternAtom(display, "WM_PROTOCOLS", False);
	wm[WM_DELETE] = XInternAtom(display, "WM_DELETE_WINDOW", False);
	wm[WM_STATE] = XInternAtom(display, "WM_STATE", False);
	wm[WM_TAKE_FOCUS] = XInternAtom(display, "WM_TAKE_FOCUS", False);
	net[NET_ACTIVE_WINDOW] = XInternAtom(display, "_NET_ACTIVE_WINDOW", False);
	net[NET_SUPPORTED] = XInternAtom(display, "_NET_SUPPORTED", False);
	net[NET_WM_NAME] = XInternAtom(display, "_NET_WM_NAME", False);
	net[NET_WM_STATE] = XInternAtom(display, "_NET_WM_STATE", False);
	net[NET_WM_CHECK] = XInternAtom(display, "_NET_SUPPORTING_WM_CHECK", False);
	net[NET_WM_FULLSCREEN] = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", False);
	net[NET_WM_WINDOW_TYPE] = XInternAtom(display, "_NET_WM_WINDOW_TYPE", False);
	net[NET_WM_WINDOW_TYPE_DIALOG] = XInternAtom(display, "_NET_WM_WINDOW_TYPE_DIALOG", False);
	net[NET_CLIENT_LIST] = XInternAtom(display, "_NET_CLIENT_LIST", False);
}