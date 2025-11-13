#include "Renderer.h"
#include "../LogManager/LogManager.h"
#include "../FileReader/FileReader.h"
#include "../MetalMath/MetalMath.h"
#include <cstddef>

Renderer::Renderer(MTL::Device *device) : device(device->retain())
{
    commandQueue = device->newCommandQueue();
    buildShaders();
    buildMeshes();
}

Renderer::~Renderer()
{
    quadMesh.indexBuffer->release();
    quadMesh.vertexBuffer->release();

    triangleMesh->release();
    trianglePipeline->release();
    generalPipeline->release();
    commandQueue->release();
    device->release();
}

void Renderer::buildMeshes()
{
    triangleMesh = MeshFactory::buildTriangle(device);
    quadMesh = MeshFactory::buildQuad(device);
}

void Renderer::buildShaders()
{
    LogManager::Log("Reloading Shader Files");
    generalPipeline = buildShader("General", "vertexGeneral", "fragmentGeneral")->retain();
    trianglePipeline = buildShader("Triangle", "vertexTriangle", "fragmentTriangle")->retain();
}

MTL::RenderPipelineState *Renderer::buildShader(const char *fileName, const char *vertexName, const char *fragmentName)
{
    MTL::RenderPipelineState *pipeline;
    std::string name = std::string("data/Shaders/") + fileName + ".metal";
    std::string reader = ReadFile(name);
    NS::String *shaderSource = NS::String::string(reader.c_str(), NS::StringEncoding::UTF8StringEncoding);

    NS::Error *error = nullptr;
    MTL::CompileOptions *options = nullptr;
    MTL::Library *library = device->newLibrary(shaderSource, options, &error);
    if (!library)
    {
        std::cout << error->localizedDescription()->utf8String() << std::endl;
    }

    NS::String *vertexNameNS = NS::String::string(vertexName, NS::StringEncoding::UTF8StringEncoding);
    MTL::Function *vertexMain = library->newFunction(vertexNameNS);

    NS::String *fragmentNameNS = NS::String::string(fragmentName, NS::StringEncoding::UTF8StringEncoding);
    MTL::Function *fragmentMain = library->newFunction(fragmentNameNS);

    MTL::RenderPipelineDescriptor *pipelineDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();
    pipelineDescriptor->setVertexFunction(vertexMain);
    pipelineDescriptor->setFragmentFunction(fragmentMain);
    pipelineDescriptor->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);

    MTL::VertexDescriptor *vertexDescriptor = MTL::VertexDescriptor::alloc()->init();
    auto attributes = vertexDescriptor->attributes();

    // attribute 0: Position
    auto positionDescriptor = attributes->object(0);
    positionDescriptor->setFormat(MTL::VertexFormat::VertexFormatFloat2);
    positionDescriptor->setOffset(offsetof(Vertex, position));
    positionDescriptor->setBufferIndex(0);
    // attribute 1: Color
    auto colorDescriptor = attributes->object(1);
    colorDescriptor->setFormat(MTL::VertexFormat::VertexFormatFloat3);
    colorDescriptor->setBufferIndex(0);
    colorDescriptor->setOffset(offsetof(Vertex, color));

    auto layoutDescriptor = vertexDescriptor->layouts()->object(0);
    layoutDescriptor->setStride(sizeof(Vertex));

    pipelineDescriptor->setVertexDescriptor(vertexDescriptor);

    pipeline = device->newRenderPipelineState(pipelineDescriptor, &error);
    if (!pipeline)
    {
        std::cout << error->localizedDescription()->utf8String() << std::endl;
    }

    vertexMain->release();
    fragmentMain->release();

    pipelineDescriptor->release();
    library->release();
    return pipeline;
}

void Renderer::draw(MTK::View *view)
{

    t += 1.0f;
    if (t > 360)
    {
        t -= 360.0f;
    }

    NS::AutoreleasePool *pool = NS::AutoreleasePool::alloc()->init();

    MTL::CommandBuffer *commandBuffer = commandQueue->commandBuffer();
    MTL::RenderPassDescriptor *renderPass = view->currentRenderPassDescriptor();
    MTL::RenderCommandEncoder *encoder = commandBuffer->renderCommandEncoder(renderPass);

    encoder->setRenderPipelineState(generalPipeline);

    simd::float4x4 transform = MetalMath::translate({0.5f, 0.5f, 0.0f}) * MetalMath::rotateZ(t) * MetalMath::scale(0.1f);
    encoder->setVertexBytes(&transform, sizeof(simd::float4x4), 1);

    encoder->setVertexBuffer(quadMesh.vertexBuffer, NS::UInteger(0), NS::UInteger(0));
    encoder->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS::UInteger(6), MTL::IndexType::IndexTypeUInt16, quadMesh.indexBuffer, NS::UInteger(0), NS::UInteger(1));

    encoder->endEncoding();
    commandBuffer->presentDrawable(view->currentDrawable());
    commandBuffer->commit();

    pool->release();
}