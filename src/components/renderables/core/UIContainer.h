#pragma once

#include "components/engine/Renderable.h"


class UIContainer : public Renderable {
public:
    UIContainer();
    virtual ~UIContainer();

    virtual void render(MTL::RenderCommandEncoder *encoder);
};
