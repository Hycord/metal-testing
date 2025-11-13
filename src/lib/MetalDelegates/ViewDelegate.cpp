#include "ViewDelegate.h"

ViewDelegate::ViewDelegate(MTL::Device *device)
    : MTK::ViewDelegate()
{
    // renderer = new Renderer(device);

    cameraX = -5.0f;
    cameraY = 0.0f;
    cameraZ = 0.5f;

    cameraYaw = 0.0f;
    cameraPitch = 0.0f;
}

ViewDelegate::~ViewDelegate()
{
    // delete renderer;
}

void ViewDelegate::drawInMTKView(MTK::View *view)
{
    
    // renderer->draw(view);
}