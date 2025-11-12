#pragma once
#include "../../config.h"

class Renderer
{
    public:
        Renderer(MTL::Device* device);
        ~Renderer();
        void buildPipeline();
        void draw(MTK::View* view);

    private:
        MTL::Device* device;
        MTL::CommandQueue* commandQueue;
        MTL::RenderPipelineState* trianglePipeline;
};