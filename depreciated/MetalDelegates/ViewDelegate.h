#pragma once
#include "../../config.h"
#include "../View/Renderer.h"
class ViewDelegate : public MTK::ViewDelegate
{
    public:
        ViewDelegate(MTL::Device* device);
        virtual ~ViewDelegate() override;
        virtual void drawInMTKView(MTK::View* view) override;

    private:
        Renderer* renderer;
        float cameraX, cameraY, cameraZ, cameraYaw, cameraPitch;
};