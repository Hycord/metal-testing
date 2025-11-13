#include "PipelineFactory.h"
#include "../Utils/FileReader.h"
#include "../LogManager/LogManager.h"

PipelineFactory::PipelineFactory(MTL::Device* device):
device(device->retain()) {}

PipelineFactory::~PipelineFactory() {
    device->release();
}

void PipelineFactory::set_filename(const char* filename) {
    this->fileName = filename;
}

void PipelineFactory::set_vertex_entry_point(const char* entryPoint) {
    this->vertexEntryPoint = entryPoint;
}

void PipelineFactory::set_fragment_entry_point(const char* entryPoint) {
    this->fragmentEntryPoint = entryPoint;
}

void PipelineFactory::set_vertex_descriptor(MTL::VertexDescriptor* descriptor) {
    if (this->vertexDescriptor) {
        this->vertexDescriptor->release();
    }
    this->vertexDescriptor = descriptor->retain();
}

MTL::RenderPipelineState* PipelineFactory::build() {
    std::string name = std::string("data/Shaders/") + fileName + ".metal";
    std::string reader = ReadFile(name);
    NS::String *shaderSource = NS::String::string(reader.c_str(), NS::StringEncoding::UTF8StringEncoding);

    LOG_START("PipelineFactory: building pipeline for %s", name.c_str());

    NS::Error* error = nullptr;
    MTL::CompileOptions* options = nullptr;
    LOG_DEBUG("PipelineFactory calling device->newLibrary()");
    MTL::Library* library = device->newLibrary(shaderSource, options, &error);
    LOG_DEBUG("PipelineFactory returned from newLibrary: %p error=%p", library, (void*)error);
    if (!library) {
        if (error) {
            LOG_ERROR("PipelineFactory newLibrary error: %s", error->localizedDescription()->utf8String());
        } else {
            LOG_ERROR("PipelineFactory newLibrary returned null and error is null");
        }
    }
    
    NS::String* vertexName = NS::String::string(
        vertexEntryPoint, NS::StringEncoding::UTF8StringEncoding);
    LOG_DEBUG("PipelineFactory creating vertex function: %s", vertexEntryPoint);
    MTL::Function* vertexMain = library->newFunction(vertexName);
    LOG_DEBUG("PipelineFactory vertexMain=%p", vertexMain);
    
    NS::String* fragmentName = NS::String::string(
        fragmentEntryPoint, NS::StringEncoding::UTF8StringEncoding);
    LOG_DEBUG("PipelineFactory creating fragment function: %s", fragmentEntryPoint);
    MTL::Function* fragmentMain = library->newFunction(fragmentName);
    LOG_DEBUG("PipelineFactory fragmentMain=%p", fragmentMain);
    
    MTL::RenderPipelineDescriptor* pipelineDescriptor = 
        MTL::RenderPipelineDescriptor::alloc()->init();
    pipelineDescriptor->setVertexFunction(vertexMain);
    pipelineDescriptor->setFragmentFunction(fragmentMain);
    pipelineDescriptor->colorAttachments()
                    ->object(0)
                    ->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);
    // enable a depth attachment format so the pipeline can use depth testing
    pipelineDescriptor->setDepthAttachmentPixelFormat(MTL::PixelFormat::PixelFormatDepth32Float);
    
    pipelineDescriptor->setVertexDescriptor(vertexDescriptor);
    
    LOG_DEBUG("PipelineFactory creating render pipeline state");
    MTL::RenderPipelineState* pipeline = device->newRenderPipelineState(pipelineDescriptor, &error);
    LOG_DEBUG("PipelineFactory newRenderPipelineState returned: %p error=%p", pipeline, (void*)error);
    if (!pipeline) {
        if (error) {
            LOG_ERROR("PipelineFactory pipeline error: %s", error->localizedDescription()->utf8String());
        } else {
            LOG_ERROR("PipelineFactory newRenderPipelineState returned null and error is null");
        }
    }
    else {
        LOG_FINISH("PipelineFactory: pipeline created %p for %s", pipeline, name.c_str());
    }
    vertexMain->release();
    fragmentMain->release();
    pipelineDescriptor->release();
    library->release();


    return pipeline;
}