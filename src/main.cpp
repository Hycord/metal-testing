#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define MTK_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION

#include "config.h"
#include "lib/GLFW/Application.h"

int main()
{

    NS::AutoreleasePool *autoreleasePool = NS::AutoreleasePool::alloc()->init();

    Application *application = new Application();
    application->run();
    delete application;

    autoreleasePool->release();

    return 0;
}