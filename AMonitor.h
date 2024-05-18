#ifndef A_MONITOR_H
#define A_MONITOR_H


#include "AUtil.h"
#include "AWorkspace.h"

#include <vector>


class AMonitor {
public:
    AMonitor(int w, int h);

    ASize getSize() const { return size; }
    void setSize(int x, int y) { size = { x, y }; }
    void addWorkspace(const std::string& name);
    const std::vector<AWorkspace*>& getWorkspaces() const;

private:
    ASize size;

    std::vector<AWorkspace*> workspaces;
};

#endif /* A_MONITOR_H */