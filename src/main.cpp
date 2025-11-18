#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define MTK_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION

#include "engine/config.h"
#include "controller/Application.h"
#include "engine/core/LogManager.h"
#include <cstring>
#include <vector>
#include <cstdint>
#include <sstream>
#include <iomanip>

int main(int argc, char **argv)
{
    LOG_START("main starting");
    int exitCode = 0;
    try
    {
        {
            Application app;
            app.run();
        }
    }
    catch (const std::exception &e)
    {
        LOG_ERROR("Unhandled exception: {}", e.what());
        exitCode = 1;
    }
    catch (...)
    {
        LOG_ERROR("Unhandled unknown exception");
        exitCode = 1;
    }
    LOG_FINISH("main finished");
    return exitCode;
}