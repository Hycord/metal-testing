#include "engine/components/renderables/core/WorldContainer.h"

WorldContainer::WorldContainer() : Renderable(Mesh(), nullptr) {}

WorldContainer::~WorldContainer() {}

void WorldContainer::render(MTL::RenderCommandEncoder* encoder,
                            const simd::float4x4& projection,
                            const simd::float4x4& view) {
}
