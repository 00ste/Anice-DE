#include "AWorkspace.h"

void AWorkspace::addWindow(AWindow* window) {
    windows.push_front(window);
}

AWindow* AWorkspace::nextWindow(AWindow* window) {
    // if the window is nullptr or the list is empty return nullptr
    if (window == nullptr || windows.empty()) return nullptr;

    // if the window isn't present return nullptr
    auto target_it = std::find(windows.begin(), windows.end(), window);
    if (target_it == windows.end()) return nullptr;

    // return the next item (cyclic)
    ++target_it;
    if (target_it == windows.end()) return windows.front();
    else return *target_it;
}
