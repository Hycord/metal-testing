#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define MTK_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION

#include "config.h"
#include "controller/Application.h"
#include "core/LogManager.h"
#include <cstring>

int main(int argc, char **argv)
{
    NS::AutoreleasePool *autoreleasePool = NS::AutoreleasePool::alloc()->init();
    LOG_START("main: creating Application");
    Application *application = new Application();
    application->run();
    delete application;
    LOG_FINISH("main: Application destroyed");

    autoreleasePool->release();

    return 0;
}