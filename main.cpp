#include <iostream>

#include "AWindowManager.h"
#include "AUtil.h"

int main(int argc, char* argv[]) {
    AWindowManager wm = AWindowManager();
    ALogger::logMessageToFile(ALogger::Tag::DEBUG, "Starting event loop from main");
    wm.eventLoop();
}