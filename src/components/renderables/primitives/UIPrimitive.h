#pragma once

#include "components/engine/Renderable.h"
#include "components/engine/Material.h"
#include "factories/MeshFactory.h"
#include "components/engine/Shader.h"
#include <memory>
#include <vector>

// Base class for UI primitives that can be composed and drawn inside UIElements.
class UIPrimitive {
public:
    virtual ~UIPrimitive() {}
    virtual void draw(MTL::RenderCommandEncoder* encoder, const simd::float4x4 &ortho) = 0;
    void setPrimitiveType(MTL::PrimitiveType t) { primitiveType = t; }
    void setColor(const simd::float4 &c) { color = c; }
protected:
    // Helper to build a Renderable with provided mesh and color
    std::unique_ptr<Renderable> makeRenderable(MTL::Device* device, const Mesh &mesh, const simd::float4 &col, MTL::PrimitiveType primType) {
        Shader* shader = new Shader(device, "General", "vertexGeneral", "fragmentGeneral", mesh.vertexDescriptor);
        Material* material = new Material(shader);
        material->setColor(col);
        auto r = std::make_unique<Renderable>(mesh, material);
        r->setScreenSpace(true);
        r->setPrimitiveType(primType);
        return r;
    }
    MTL::PrimitiveType primitiveType = MTL::PrimitiveType::PrimitiveTypeTriangle;
    simd::float4 color {1,1,1,1};
};

