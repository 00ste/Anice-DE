#include "AMonitor.h"

AMonitor::AMonitor(int w, int h)
    : size{ w, h } {
    workspaces.push_back(new AWorkspace("main"));
}

void AMonitor::addWorkspace(const std::string &name) {
    workspaces.push_back(new AWorkspace(name));
}
