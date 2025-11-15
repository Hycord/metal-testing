#pragma once

#include "components/engine/Renderable.h"
#include "components/engine/Shader.h"
#include "components/engine/Material.h"
#include "utils/Math.h"
#include <memory>


class RenderablePrimitive {
public:
    virtual ~RenderablePrimitive() = default;

    virtual void draw(MTL::RenderCommandEncoder *encoder,
                      const simd::float4x4 &projection,
                      const simd::float4x4 &view) = 0;

    void drawScreenSpace(MTL::RenderCommandEncoder *encoder,
                         const simd::float4x4 &projection) {
        draw(encoder, projection, MetalMath::identity());
    }

    void setPrimitiveType(MTL::PrimitiveType type) {
        if (primitiveType == type)
            return;
        primitiveType = type;
        if (auto renderable = registeredRenderable.lock())
            renderable->setPrimitiveType(type);
        onPrimitiveTypeChanged();
    }

    MTL::PrimitiveType getPrimitiveType() const { return primitiveType; }

    void setColor(const simd::float4 &c) {
        color = c;
        onColorChanged();
    }

    const simd::float4 &getColor() const { return color; }

    void setScreenSpace(bool value) {
        if (screenSpace == value)
            return;
        screenSpace = value;
        if (auto renderable = registeredRenderable.lock())
            renderable->setScreenSpace(screenSpace);
        onScreenSpaceChanged();
    }

    bool isScreenSpace() const { return screenSpace; }

    std::shared_ptr<Renderable> currentRenderable() const {
        return registeredRenderable.lock();
    }

    void setTransform(const simd::float4x4 &transform) {
        transformOverride = transform;
        hasTransformOverride = true;
        if (auto renderable = registeredRenderable.lock())
            renderable->setTransform(transformOverride);
        onTransformChanged();
    }

    void clearTransform() {
        transformOverride = MetalMath::identity();
        hasTransformOverride = false;
        if (auto renderable = registeredRenderable.lock())
            renderable->setTransform(transformOverride);
        onTransformChanged();
    }

protected:
    RenderablePrimitive() = default;

    std::shared_ptr<Renderable> makeRenderable(MTL::Device *device,
                                               const Mesh &mesh,
                                               const simd::float4 &col,
                                               MTL::PrimitiveType type) {
        Shader *shader = new Shader(device, "General", "vertexGeneral", "fragmentGeneral", mesh.vertexDescriptor);
        Material *material = new Material(shader);
        material->setColor(col);
        auto renderable = std::shared_ptr<Renderable>(new Renderable(mesh, material));
        registerRenderable(renderable);
        renderable->setPrimitiveType(type);
        renderable->setScreenSpace(screenSpace);
        if (hasTransformOverride)
            renderable->setTransform(transformOverride);
        return renderable;
    }

    void registerRenderable(const std::shared_ptr<Renderable> &renderable) {
        registeredRenderable = renderable;
        if (renderable) {
            renderable->setPrimitiveType(primitiveType);
            renderable->setScreenSpace(screenSpace);
            if (hasTransformOverride)
                renderable->setTransform(transformOverride);
        }
    }

    void unregisterRenderable() {
        registeredRenderable.reset();
    }

    void applyState(const std::shared_ptr<Renderable> &renderable) const {
        if (!renderable)
            return;
        renderable->setPrimitiveType(primitiveType);
        renderable->setScreenSpace(screenSpace);
        if (hasTransformOverride)
            renderable->setTransform(transformOverride);
    }

    virtual void onColorChanged() {}
    virtual void onPrimitiveTypeChanged() {}
    virtual void onScreenSpaceChanged() {}
    virtual void onTransformChanged() {}

    simd::float4 color{1.0f, 1.0f, 1.0f, 1.0f};

private:
    bool screenSpace = true;
    bool hasTransformOverride = false;
    simd::float4x4 transformOverride = MetalMath::identity();
    std::weak_ptr<Renderable> registeredRenderable;
    MTL::PrimitiveType primitiveType = MTL::PrimitiveType::PrimitiveTypeTriangle;
};
