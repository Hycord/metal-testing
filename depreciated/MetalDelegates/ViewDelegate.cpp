#include "ViewDelegate.h"
#include "../LogManager/LogManager.h"

ViewDelegate::ViewDelegate(MTL::Device *device)
    : MTK::ViewDelegate()
{
    LOG_CONSTRUCT("ViewDelegate");
    // renderer = new Renderer(device);

    cameraX = -5.0f;
    cameraY = 0.0f;
    cameraZ = 0.5f;

    cameraYaw = 0.0f;
    cameraPitch = 0.0f;
}

ViewDelegate::~ViewDelegate()
{
    LOG_DESTROY("ViewDelegate");
    // delete renderer;
}

void ViewDelegate::drawInMTKView(MTK::View *view)
{
    LOG_STEP("ViewDelegate: drawInMTKView");
    // renderer->draw(view);
}