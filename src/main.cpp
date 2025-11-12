#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define MTK_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION

#include <iostream>

#include "lib/LogManager/LogManager.h"
#include "lib/FileReader/FileReader.h"

#include "lib/GLFW/GLFWAdapter.h"

int main()
{

    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow *glfwWindow = glfwCreateWindow(500, 500, "Application", NULL, NULL);

    while (!glfwWindowShouldClose(glfwWindow))
    {
        glfwPollEvents();
    }

    glfwDestroyWindow(glfwWindow);
    glfwTerminate();

    std::cout << "Hello, world!" << std::endl;
    return 0;
}