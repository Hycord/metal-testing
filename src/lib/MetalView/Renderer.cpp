#include "Renderer.h"
#include "../LogManager/LogManager.h"
#include "../FileReader/FileReader.h"

Renderer::Renderer(MTL::Device *device) : device(device->retain())
{
    commandQueue = device->newCommandQueue();
    buildPipeline();
}

Renderer::~Renderer()
{
    commandQueue->release();
    device->release();
}

void Renderer::buildPipeline()
{
    LogManager::Log("Reloading Shader File");
    std::string reader = ReadFile("data/Shaders/Triangle.metal");
    NS::String *shaderSource = NS::String::string(reader.c_str(), NS::StringEncoding::UTF8StringEncoding);

    NS::Error *error = nullptr;
    MTL::CompileOptions *options = nullptr;
    MTL::Library *library = device->newLibrary(shaderSource, options, &error);
    if (!library)
    {
        std::cout << error->localizedDescription()->utf8String() << std::endl;
    }

    NS::String *vertexName = NS::String::string("vertexMain", NS::StringEncoding::UTF8StringEncoding);
    MTL::Function *vertexMain = library->newFunction(vertexName);

    NS::String *fragmentName = NS::String::string("fragmentMain", NS::StringEncoding::UTF8StringEncoding);
    MTL::Function *fragmentMain = library->newFunction(fragmentName);

    MTL::RenderPipelineDescriptor *descriptor = MTL::RenderPipelineDescriptor::alloc()->init();
    descriptor->setVertexFunction(vertexMain);
    descriptor->setFragmentFunction(fragmentMain);
    descriptor->colorAttachments()->object(NS::UInteger(0))->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm);

    trianglePipeline = device->newRenderPipelineState(descriptor, &error);
    if (!trianglePipeline)
    {
        std::cout << error->localizedDescription()->utf8String() << std::endl;
    }

    library->release();
    vertexMain->release();
    fragmentMain->release();
    descriptor->release();
}

void Renderer::draw(MTK::View *view)
{

    NS::AutoreleasePool *pool = NS::AutoreleasePool::alloc()->init();

    MTL::CommandBuffer *commandBuffer = commandQueue->commandBuffer();
    MTL::RenderPassDescriptor *renderPass = view->currentRenderPassDescriptor();
    MTL::RenderCommandEncoder *encoder = commandBuffer->renderCommandEncoder(renderPass);

    encoder->setRenderPipelineState(trianglePipeline);
    encoder->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS::UInteger(0), NS::UInteger(3));

    encoder->endEncoding();
    commandBuffer->presentDrawable(view->currentDrawable());
    commandBuffer->commit();

    pool->release();
}