#ifndef A_WORKSPACE_H
#define A_WORKSPACE_H


#include "AWindow.h"

#include <list>
#include <string>


class AWorkspace {
public:
    AWorkspace(const std::string& name_)
        : name{name_} {}
    ~AWorkspace() = default;

    std::string getName() const { return name; }
    void setName(const std::string& name) { this->name = name; }

    void addWindow(AWindow* window);

private:
    std::string name;
    std::list<AWindow*> windows;

};


#endif /* A_WORKSPACE_H */