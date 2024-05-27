#include "AWindowManager.h"

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>
#include <csignal>
#include <sstream>


AWindowManager::AWindowManager() {
    if (!setlocale(LC_CTYPE, "") || !XSupportsLocale())
        fputs("warning: no locale support\n", stderr);

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
    activeWindow = nullptr;

    XSelectInput(display, DefaultRootWindow(display), SubstructureRedirectMask);
    XSync(display, false);
    XSetErrorHandler(xErrorHandler);
    XSync(display, false);

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
    //
    keybinds.push_back({ KeyPress, Mod4Mask, XK_Return,
        []() { system("/usr/bin/gnome-terminal"); }
    });
    keybinds.push_back({ KeyPress, Mod4Mask, XK_q,
        [this]() { running = false; }
    });
    keybinds.push_back({ KeyRelease, Mod4Mask, XK_Tab,
        [this]() {
        ALogger::logMessageToFile(ALogger::Tag::DEBUG, "Alt+Tab clicked");
        focusWindow(activeWorkspace->nextWindow(activeWindow));
        }
    });

    // Grab keys
    grabKeys();
    ALogger::logMessageToFile(ALogger::Tag::INFO, "Finished window manager setup");
}

void AWindowManager::eventLoop() {
    XEvent event;

    XSync(display, False);

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
    // TODO: set window title

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

    XRaiseWindow(display, window->getXWindow());

    // focus window
    focusWindow(window);

    // ALogger::logMessageToFile(ALogger::Tag::DEBUG, "(manage) Window mapped successfully!");
}

void AWindowManager::mapRequestHandler(XEvent* e) {
    // ALogger::logMessageToFile(ALogger::Tag::DEBUG, "Received map request");
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
    XKeyEvent* event = &e->xkey;
    KeySym keysym = XkbKeycodeToKeysym(display, (KeyCode)event->keycode, 0, 0);

    /*
    std::stringstream message;
    message << "Inside keyPressHandler with keybind" << modToString(cleanMask(event->state));
    message << " + " << XKeysymToString(keysym);
    ALogger::logMessageToFile(ALogger::Tag::DEBUG, message.str());
     */

    for (const AKeybind& keybind : keybinds) {
        if (keysym != keybind.keysym)
            continue;
        if (event->type != keybind.pressType)
            continue;
        if ((cleanMask(keybind.mod) & cleanMask(event->state)) == 0)
            continue;
        if (keybind.action != nullptr)
            keybind.action();
    }
}

AWindow* AWindowManager::getWindowFromXWindow(Window xWindow) {
    // TODO: Implement this
    /*
    for (AMonitor* monitor : monitors) {
        for (AWorkspace* workspace : monitor.getWorkspaces()) {
            for (AWindow* window : workspace->getWindows()) {
                if (window->getXWindow() == xWindow) return window->getXWindow();
            }
        }
    }
    */
    return nullptr;
}

void AWindowManager::focusWindow(AWindow* window) {
    ALogger::logMessageToFile(ALogger::Tag::DEBUG, "Inside focusWindow");
    if (window == nullptr) {
        // unfocus
        XSetInputFocus(display, rootWindow, RevertToPointerRoot, CurrentTime);
        XDeleteProperty(display, rootWindow, netAtoms[NET_ACTIVE_WINDOW]);
        activeWindow = nullptr;
        ALogger::logMessageToFile(ALogger::Tag::DEBUG, "Unfocused");
    }
    else {
        // focus new window
        XSetInputFocus(display, window->getXWindow(), RevertToPointerRoot, CurrentTime);
        XChangeProperty(display, rootWindow, netAtoms[NET_ACTIVE_WINDOW], XA_WINDOW, 32,
            PropModeReplace, (unsigned char*) &(window->getXWindow()), 1);
        activeWindow = window;

        // focus monitor and workspace where the window is contained
        // TODO: obtain monitor and workspace where a window is contained

        sendMessage(window, wmAtoms[WM_TAKE_FOCUS]);

        // redraw window on top of the stack
        XMapWindow(display, window->getXWindow());

        std::stringstream message;
        message << "Focused window " << window->getTitle();
        ALogger::logMessageToFile(ALogger::Tag::DEBUG, message.str());
    }

    grabKeys();
}

void AWindowManager::grabKeys() {
    // update num lock mask
    numLockMask = 0;

    XModifierKeymap* modmap = XGetModifierMapping(display);
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < modmap->max_keypermod; j++)
            if (modmap->modifiermap[i * modmap->max_keypermod + j]
                == XKeysymToKeycode(display, XK_Num_Lock))
                numLockMask = (1 << i);
    XFreeModifiermap(modmap);

    // ungrab keys
    XUngrabKey(display, AnyKey, AnyModifier, rootWindow);

    // grab keys
    std::vector<unsigned int> modifiers = { 0, LockMask, numLockMask, numLockMask | LockMask };
    for (const AKeybind& keybind : keybinds) {
        for (unsigned int modifier : modifiers) {
            XGrabKey(display, XKeysymToKeycode(display, keybind.keysym),
                keybind.mod | modifier, rootWindow, true,
                GrabModeAsync, GrabModeAsync);
        }
    }

    ALogger::logMessageToFile(ALogger::Tag::DEBUG, "Grabbed keys");
}

bool AWindowManager::sendMessage(AWindow* window, Atom protocol) {
    int n;
    Atom *protocols;
    int exists = false;

    if (XGetWMProtocols(display, window->getXWindow(), &protocols, &n)) {
        while (!exists && n--)
            exists = (protocols[n] == protocol);
        XFree(protocols);
    }

    if (exists) {
        XEvent ev;
        ev.type = ClientMessage;
        ev.xclient.window = window->getXWindow();
        ev.xclient.message_type = wmAtoms[WM_PROTOCOLS];
        ev.xclient.format = 32;
        ev.xclient.data.l[0] = protocol;
        ev.xclient.data.l[1] = CurrentTime;
        XSendEvent(display, window->getXWindow(), False, NoEventMask, &ev);
    }
    return exists;
}
