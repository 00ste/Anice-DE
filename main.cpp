#include <iostream>

#include "AWindowManager.h"
#include "AUtil.h"

#include <exception>

int main(int argc, char* argv[]) {
    freopen("/home/stefano/code/c-cpp/anice-cpp/logs/errorlog.txt", "a+", stderr);

    try {
        AWindowManager wm = AWindowManager();
        wm.eventLoop();
        return EXIT_SUCCESS;
    }
    catch (std::exception e) {
        ALogger::logMessageToFile(ALogger::Tag::ERROR, e.what());
        return EXIT_FAILURE;
    }
}