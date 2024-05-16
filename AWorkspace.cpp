#include "AWorkspace.h"

void AWorkspace::addWindow(AWindow *window) {
    windows.push_front(window);
}
