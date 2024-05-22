#include "AWindowManager.h"

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <csignal>
#include <sstream>


AWindowManager::AWindowManager() {
    // Do not transform children into zombies when they terminate
    struct sigaction sa{};
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_NOCLDSTOP | SA_NOCLDWAIT | SA_RESTART;
	sa.sa_handler = SIG_IGN;
	sigaction(SIGCHLD, &sa, nullptr);

    // Init fields
    display = XOpenDisplay(nullptr);
    xScreenNumber = DefaultScreen(display);
    xScreenWidth = DisplayWidth(display, xScreenNumber);
    xScreenHeight = DisplayHeight(display, xScreenNumber);
    rootWindow = RootWindow(display, xScreenNumber);
    running = true;

    updateMonitors();

    wmWindow = XCreateSimpleWindow(display, rootWindow,
        0, 0, 1, 1, 0, 0, 0);

    // Atoms
    utf8StringAtom = XInternAtom(display, "UTF8_STRING", False);
    wmAtoms[WM_PROTOCOLS] = XInternAtom(display, "WM_PROTOCOLS", False);
    wmAtoms[WM_DELETE] = XInternAtom(display, "WM_DELETE_WINDOW", False);
    wmAtoms[WM_STATE] = XInternAtom(display, "WM_STATE", False);
    wmAtoms[WM_TAKE_FOCUS] = XInternAtom(display, "WM_TAKE_FOCUS", False);
    netAtoms[NET_ACTIVE_WINDOW] = XInternAtom(display, "_NET_ACTIVE_WINDOW", False);
    netAtoms[NET_SUPPORTED] = XInternAtom(display, "_NET_SUPPORTED", False);
    netAtoms[NET_WM_NAME] = XInternAtom(display, "_NET_WM_NAME", False);
    netAtoms[NET_WM_STATE] = XInternAtom(display, "_NET_WM_STATE", False);
    netAtoms[NET_WM_CHECK] = XInternAtom(display, "_NET_SUPPORTING_WM_CHECK", False);
    netAtoms[NET_WM_FULLSCREEN] = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", False);
    netAtoms[NET_WM_WINDOW_TYPE] = XInternAtom(display, "_NET_WM_WINDOW_TYPE", False);
    netAtoms[NET_WM_WINDOW_TYPE_DIALOG] = XInternAtom(display, "_NET_WM_WINDOW_TYPE_DIALOG", False);
    netAtoms[NET_CLIENT_LIST] = XInternAtom(display, "_NET_CLIENT_LIST", False);

    // Cursors
    for (int i = 0; i < CURSORS_N; i++) {
        cursors[i] = XCreateFontCursor(display, i);
    }
    
    // Set window manager properties
    XChangeProperty(display, wmWindow, netAtoms[NET_WM_CHECK], XA_WINDOW, 32,
                    PropModeReplace, (unsigned char *) &wmWindow, 1);
	XChangeProperty(display, wmWindow, netAtoms[NET_WM_NAME], utf8StringAtom, 8,
                    PropModeReplace, (unsigned char *) "Anice", 5);
	XChangeProperty(display, rootWindow, netAtoms[NET_WM_CHECK], XA_WINDOW, 32,
                    PropModeReplace, (unsigned char *) &wmWindow, 1);
	XChangeProperty(display, rootWindow, netAtoms[NET_SUPPORTED], XA_ATOM, 32,
                    PropModeReplace, (unsigned char *) netAtoms, NET_N_ATOMS);
    XDeleteProperty(display, rootWindow, netAtoms[NET_CLIENT_LIST]);

    // Select events
    XSetWindowAttributes attributes;
    attributes.cursor = cursors[CURSOR_NORMAL];
	attributes.event_mask = SubstructureRedirectMask
		| SubstructureNotifyMask | ButtonPressMask | PointerMotionMask
		| EnterWindowMask | LeaveWindowMask | StructureNotifyMask
		| PropertyChangeMask | KeyPressMask | KeyReleaseMask;
    
	XChangeWindowAttributes(display, rootWindow, CWEventMask | CWCursor, &attributes);
	XSelectInput(display, rootWindow, attributes.event_mask);

    // Init event handlers
    eventHandlers[MapRequest]       = [this](XEvent* e) { this->mapRequestHandler(e); };
    eventHandlers[KeyPress]         = [this](XEvent* e) { this->keyPressHandler(e); };
    eventHandlers[KeyRelease]       = [this](XEvent* e) { this->keyPressHandler(e); };
    eventHandlers[MappingNotify]    = [this](XEvent* e) { this->mappingNotifyHandler(e); };

    // TODO: move keybind definitions somewhere else
    keybinds.push_back({
        KeyPress,
        Mod4Mask,
        XK_Return,
        []() { system("/usr/bin/gnome-terminal"); }
    });

    // Grab keys
    grabKeys();

    // TODO(DEBUG)
    system("/usr/bin/gnome-terminal");
    ALogger::logMessageToFile(ALogger::Tag::INFO, "Finished window manager setup");
}

void AWindowManager::eventLoop() {
    XEvent event;

    XSync(display, False);
    XSetErrorHandler(xErrorHandler);

    ALogger::logMessageToFile(ALogger::Tag::INFO, "Starting event loop");
    while (running && !XNextEvent(display, &event)) {
        if (eventHandlers[event.type] != nullptr) {
            std::stringstream message;
            message << "Handling event of type " << eventTypeToString[event.type];
            ALogger::logMessageToFile(ALogger::Tag::DEBUG, message.str());
            eventHandlers[event.type](&event);
        }
    }
}

int AWindowManager::xErrorHandler(Display* display, XErrorEvent* e) {
    std::stringstream message;
    message << "Fatal error! request code " << e->request_code << ", error code " << e->error_code;
    ALogger::logMessageToFile(ALogger::Tag::ERROR, message.str());
    return 0;
}

void AWindowManager::updateMonitors() {
    bool dirty = false;

#ifdef XINERAMA
    // ...
#endif /* XINERAMA */
    {
        if (monitors.empty()) {
            AMonitor* monitor = new AMonitor(xScreenWidth, xScreenHeight);
            monitors.push_back(monitor);
            monitor->addWorkspace("Workspace 1");
            activeMonitor = monitor;
            activeWorkspace = monitor->getWorkspaces()[0];
        }
        for (AMonitor* monitor : monitors) {
            ASize size = monitor->getSize();
            if (size.x != xScreenWidth || size.y != xScreenHeight) {
                dirty = true;
                monitor->setSize(xScreenWidth, xScreenHeight);
            }
        }
    }

    if (dirty) {
        // ...
    }
}

void AWindowManager::manage(Window xWindow, XWindowAttributes* attributes) {
    ALogger::logMessageToFile(ALogger::Tag::DEBUG, "(manage) Inside manage()");
    if (attributes == nullptr) {
        ALogger::logMessageToFile(ALogger::Tag::ERROR, "(manage) attributes was nullptr!!");
        return;
    }

    if (activeMonitor == nullptr) {
        ALogger::logMessageToFile(ALogger::Tag::ERROR, "(manage) activeMonitor was nullptr!!");
        return;
    }

    AWindow* window = new AWindow(
        attributes->x, attributes->y,
        attributes->width, attributes->height,
        10, 10,
        activeMonitor->getSize().x, activeMonitor->getSize().y,
        xWindow
    );

    APoint winPosition = window->getPosition();
    ASize winSize = window->getSize();

    // TODO: the window might need to be spawned in a different workspace or monitor,
    // for example if it is a transient window for some other window
    activeWorkspace->addWindow(window);

    XSelectInput(display, xWindow, EnterWindowMask | FocusChangeMask
        | PropertyChangeMask | StructureNotifyMask);

    XChangeProperty(display, rootWindow, netAtoms[NET_CLIENT_LIST], XA_WINDOW, 32, PropModeAppend,
                    (unsigned char *) &(window->getXWindow()), 1);

    XMoveResizeWindow(display, window->getXWindow(), winPosition.x, winPosition.y, winSize.x, winSize.y);

    XMapWindow(display, window->getXWindow());
    ALogger::logMessageToFile(ALogger::Tag::DEBUG, "(manage) Window mapped successfully!");
}

void AWindowManager::mapRequestHandler(XEvent* e) {
    ALogger::logMessageToFile(ALogger::Tag::DEBUG, "Received map request");
    XWindowAttributes attributes;
    XMapRequestEvent* event = &e->xmaprequest;

    if (!XGetWindowAttributes(display, event->window, &attributes))
        return;
    if (attributes.override_redirect)
        return;
    if (getWindowFromXWindow(event->window) != nullptr)
        return;

    manage(event->window, &attributes);
}

void AWindowManager::mappingNotifyHandler(XEvent* e) {
    XMappingEvent* event = &e->xmapping;
    XRefreshKeyboardMapping(event);
    if (event->request == MappingKeyboard)
        grabKeys();
}

void AWindowManager::keyPressHandler(XEvent* e) {
    ALogger::logMessageToFile(ALogger::Tag::DEBUG, "Inside keyPressHandler");
    XKeyEvent* event = &e->xkey;
    KeySym keysym = XKeycodeToKeysym(display, (KeyCode)event->keycode, 0);

    for (AKeybind keybind : keybinds) {
        if (keysym != keybind.keysym)
            continue;
        if (event->type != keybind.pressType)
            continue;
        if (cleanMask(keybind.mod) != cleanMask(event->state))
            continue;
        if (keybind.action != nullptr)
            keybind.action();
    }
}

AWindow* AWindowManager::getWindowFromXWindow(Window xWindow) {
    // TODO: Implement this
    return nullptr;
}

// TODO: Implement this
void AWindowManager::focusWindow(AWindow* window) {
    if (window == nullptr) {
        XSetInputFocus(display, rootWindow, RevertToPointerRoot, CurrentTime);
        XDeleteProperty(display, rootWindow, netAtoms[NET_ACTIVE_WINDOW]);
    }
    else {

    }

}

void AWindowManager::grabKeys() {
    // update num lock mask
    numLockMask = 0;

    std::vector<unsigned int> modifiers = { 0, LockMask, numLockMask, numLockMask | LockMask };
    XModifierKeymap* modmap = XGetModifierMapping(display);
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < modmap->max_keypermod; j++)
            if (modmap->modifiermap[i * modmap->max_keypermod + j]
                == XKeysymToKeycode(display, XK_Num_Lock))
                numLockMask = (1 << i);
    XFreeModifiermap(modmap);

    // ungrab keys
    XUngrabKey(display, AnyKey, AnyModifier, rootWindow);

    // get keyboard information
    int start, end, skip;
    XDisplayKeycodes(display, &start, &end);
    KeySym* syms = XGetKeyboardMapping(display, start, end - start + 1, &skip);
    if (syms == nullptr) return;

    // grab keys
    for (int k = start; k <= end; k++) {
        for (int i = 0; i < keybinds.size(); i++) {
            if (keybinds[i].keysym == syms[(k - start) * skip]) {
                for (int j = 0; j < modifiers.size(); j++) {
                    XGrabKey(display, k, keybinds[i].mod | modifiers[j],
                         rootWindow, true, GrabModeAsync, GrabModeAsync);
                }
            }
        }
    }

    // free the array
    XFree(syms);
    ALogger::logMessageToFile(ALogger::Tag::DEBUG, "Grabbed keys");
}
