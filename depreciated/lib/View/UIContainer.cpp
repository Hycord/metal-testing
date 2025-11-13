#include "UIContainer.h"

UIContainer::UIContainer() : Renderable(Mesh(), nullptr) {}

UIContainer::~UIContainer() {}

void UIContainer::render(MTL::RenderCommandEncoder *encoder) {
    // Default implementation for rendering UI elements
    // Derived classes can override this method for custom behavior
}