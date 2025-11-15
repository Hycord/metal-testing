#include "components/renderables/primitives/2d/TextPrimitive.h"
#include "core/FontManager.h"
#include "utils/Math.h"
#include "core/LogManager.h"

TextPrimitive::TextPrimitive(MTL::Device *device, 
                             const std::string &text, 
                             float x, float y,
                             const std::string &fontPath,
                             float fontSize, 
                             const simd::float4 &col)
    : device(device), text(text), fontPath(fontPath), x(x), y(y), fontSize(fontSize), dirty(true),
      hasBoxSize(false), boxWidth(0), boxHeight(0), alignment(TextAlign::Start), 
      justification(TextJustify::Start), wrapEnabled(false)
{
    setColor(col);
    
    
    font = FontManager::getInstance().getFont(fontPath, fontSize);
    if (!font) {
        LOG_ERROR("TextPrimitive: Failed to load font: %s", fontPath.c_str());
    }
}

void TextPrimitive::rebuild()
{
    if (!dirty || !font || text.empty())
        return;
    
    
    std::vector<std::string> lines;
    if (wrapEnabled && hasBoxSize) {
        lines = wrapText(text, boxWidth);
    } else {
        
        std::string line;
        for (char c : text) {
            if (c == '\n') {
                lines.push_back(line);
                line.clear();
            } else {
                line += c;
            }
        }
        if (!line.empty() || lines.empty()) {
            lines.push_back(line);
        }
    }
    
    
    int visibleChars = 0;
    for (const auto& line : lines) {
        for (char c : line) {
            if (c != ' ' && c != '\t') {
                visibleChars++;
            }
        }
    }
    
    if (visibleChars == 0) {
        dirty = false;
        return;
    }
    
    
    std::vector<Vertex> vertices;
    std::vector<ushort> indices;
    vertices.reserve(visibleChars * 4);
    indices.reserve(visibleChars * 6);
    
    float lineHeight = font->getLineHeight();
    
    
    float totalTextHeight = lines.size() * lineHeight;
    
    
    float startY = y;
    if (hasBoxSize) {
        switch (justification) {
            case TextJustify::Start:
                startY = y + boxHeight - lineHeight; 
                break;
            case TextJustify::Center:
                startY = y + (boxHeight + totalTextHeight) / 2.0f - lineHeight;
                break;
            case TextJustify::End:
                startY = y + totalTextHeight - lineHeight; 
                break;
        }
    }
    
    int vertexIndex = 0;
    float currentY = startY;
    
    for (const auto& line : lines) {
        
        float lineWidth = 0.0f;
        for (char c : line) {
            const BakedGlyph* glyph = font->getGlyph(c);
            if (glyph) {
                lineWidth += glyph->xadvance;
            }
        }
        
        
        float currentX = x;
        if (hasBoxSize) {
            switch (alignment) {
                case TextAlign::Start:
                    currentX = x;
                    break;
                case TextAlign::Center:
                    currentX = x + (boxWidth - lineWidth) / 2.0f;
                    break;
                case TextAlign::End:
                    currentX = x + boxWidth - lineWidth;
                    break;
            }
        }
        
        
        for (char c : line) {
            const BakedGlyph* glyph = font->getGlyph(c);
            if (!glyph) continue;
            
            
            
            
            float x0 = currentX + glyph->xoff;
            float y0 = currentY - glyph->yoff2;  
            float x1 = currentX + glyph->xoff2;
            float y1 = currentY - glyph->yoff;   
            
            
            float glyphWidth = glyph->xoff2 - glyph->xoff;
            float glyphHeight = glyph->yoff2 - glyph->yoff;
            if (glyphWidth > 0 && glyphHeight > 0) {
                
                
                vertices.push_back({{x0, y1, 0.0f}, {color.x, color.y, color.z}, {glyph->x0, glyph->y0}});
                vertices.push_back({{x1, y1, 0.0f}, {color.x, color.y, color.z}, {glyph->x1, glyph->y0}});
                vertices.push_back({{x1, y0, 0.0f}, {color.x, color.y, color.z}, {glyph->x1, glyph->y1}});
                vertices.push_back({{x0, y0, 0.0f}, {color.x, color.y, color.z}, {glyph->x0, glyph->y1}});
                
                
                indices.push_back(vertexIndex + 0);
                indices.push_back(vertexIndex + 1);
                indices.push_back(vertexIndex + 2);
                indices.push_back(vertexIndex + 2);
                indices.push_back(vertexIndex + 3);
                indices.push_back(vertexIndex + 0);
                
                vertexIndex += 4;
            }
            
            currentX += glyph->xadvance;
        }
        
        currentY -= lineHeight; 
    }
    
    if (vertices.empty()) {
        dirty = false;
        return;
    }
    
    
    size_t vbSize = vertices.size() * sizeof(Vertex);
    if (!mesh.vertexBuffer || mesh.vertexBuffer->length() < vbSize) {
        if (mesh.vertexBuffer) mesh.vertexBuffer->release();
        mesh.vertexBuffer = device->newBuffer(vbSize, MTL::ResourceStorageModeShared);
    }
    memcpy(mesh.vertexBuffer->contents(), vertices.data(), vbSize);
    mesh.vertexCount = vertices.size();
    
    
    size_t ibSize = indices.size() * sizeof(ushort);
    if (!mesh.indexBuffer || mesh.indexBuffer->length() < ibSize) {
        if (mesh.indexBuffer) mesh.indexBuffer->release();
        mesh.indexBuffer = device->newBuffer(ibSize, MTL::ResourceStorageModeShared);
    }
    memcpy(mesh.indexBuffer->contents(), indices.data(), ibSize);
    mesh.indexCount = indices.size();
    
    
    if (!mesh.vertexDescriptor) {
        MTL::VertexDescriptor *vertexDescriptor = MTL::VertexDescriptor::alloc()->init();
        auto attributes = vertexDescriptor->attributes();
        
        auto positionDescriptor = attributes->object(0);
        positionDescriptor->setFormat(MTL::VertexFormat::VertexFormatFloat3);
        positionDescriptor->setOffset(offsetof(Vertex, position));
        positionDescriptor->setBufferIndex(0);
        
        auto colorDescriptor = attributes->object(1);
        colorDescriptor->setFormat(MTL::VertexFormat::VertexFormatFloat3);
        colorDescriptor->setBufferIndex(0);
        colorDescriptor->setOffset(offsetof(Vertex, color));
        
        auto uvDescriptor = attributes->object(2);
        uvDescriptor->setFormat(MTL::VertexFormat::VertexFormatFloat2);
        uvDescriptor->setBufferIndex(0);
        uvDescriptor->setOffset(offsetof(Vertex, uv));
        
        auto layoutDescriptor = vertexDescriptor->layouts()->object(0);
        layoutDescriptor->setStride(sizeof(Vertex));
        mesh.vertexDescriptor = vertexDescriptor;
    }
    
    ensureMesh();
    dirty = false;
}

void TextPrimitive::ensureMesh()
{
    if (!renderable || !font) {
        
        Shader *shader = new Shader(device, "Text", "vertexText", "fragmentText", mesh.vertexDescriptor);
        Material *material = new Material(shader);
        material->setColor(color);
        renderable = std::shared_ptr<Renderable>(new Renderable(mesh, material));
        registerRenderable(renderable);
        renderable->setPrimitiveType(getPrimitiveType());
        renderable->setScreenSpace(isScreenSpace());
        
        if (font) {
            
            material->setTexture(font->getTexture());
        }
    }
}

void TextPrimitive::draw(MTL::RenderCommandEncoder *encoder,
                         const simd::float4x4 &projection,
                         const simd::float4x4 &view)
{
    if (!font) return;
    
    rebuild();
    
    if (!renderable) return;
    
    renderable->draw(encoder, projection, view);
}

void TextPrimitive::setText(const std::string &newText)
{
    if (text != newText) {
        text = newText;
        dirty = true;
    }
}

void TextPrimitive::setPosition(float newX, float newY)
{
    if (x != newX || y != newY) {
        x = newX;
        y = newY;
        dirty = true;
    }
}

void TextPrimitive::setFontSize(float size)
{
    if (fontSize != size) {
        fontSize = size;
        dirty = true;
        
        font = FontManager::getInstance().getFont(fontPath, fontSize);
    }
}

void TextPrimitive::setFont(const std::string &newFontPath)
{
    if (fontPath != newFontPath) {
        fontPath = newFontPath;
        dirty = true;
        font = FontManager::getInstance().getFont(fontPath, fontSize);
    }
}

void TextPrimitive::measureText(float& width, float& height) const
{
    if (font) {
        font->measureText(text, width, height);
    } else {
        width = 0;
        height = 0;
    }
}

void TextPrimitive::onColorChanged()
{
    dirty = true;
    if (renderable) {
        if (auto material = renderable->getMaterial()) {
            material->setColor(getColor());
        }
    }
}

void TextPrimitive::setBoxSize(float width, float height)
{
    hasBoxSize = true;
    boxWidth = width;
    boxHeight = height;
    dirty = true;
}

void TextPrimitive::clearBoxSize()
{
    hasBoxSize = false;
    dirty = true;
}

void TextPrimitive::setAlignment(TextAlign align)
{
    if (alignment != align) {
        alignment = align;
        dirty = true;
    }
}

void TextPrimitive::setJustification(TextJustify justify)
{
    if (justification != justify) {
        justification = justify;
        dirty = true;
    }
}

void TextPrimitive::setWrap(bool enabled)
{
    if (wrapEnabled != enabled) {
        wrapEnabled = enabled;
        dirty = true;
    }
}

std::vector<std::string> TextPrimitive::wrapText(const std::string& text, float maxWidth) const
{
    std::vector<std::string> lines;
    if (!font) return lines;
    
    std::string currentLine;
    float currentWidth = 0.0f;
    std::string word;
    float wordWidth = 0.0f;
    
    for (size_t i = 0; i < text.length(); ++i) {
        char c = text[i];
        
        
        if (c == '\n') {
            if (!word.empty()) {
                currentLine += word;
                word.clear();
                wordWidth = 0.0f;
            }
            lines.push_back(currentLine);
            currentLine.clear();
            currentWidth = 0.0f;
            continue;
        }
        
        const BakedGlyph* glyph = font->getGlyph(c);
        if (!glyph) continue;
        
        float charAdvance = glyph->xadvance;
        
        
        if (c == ' ' || c == '\t') {
            
            if (!word.empty()) {
                
                if (currentWidth + wordWidth > maxWidth && !currentLine.empty()) {
                    
                    lines.push_back(currentLine);
                    currentLine = word;
                    currentWidth = wordWidth;
                } else {
                    
                    currentLine += word;
                    currentWidth += wordWidth;
                }
                word.clear();
                wordWidth = 0.0f;
            }
            
            
            if (currentWidth + charAdvance <= maxWidth) {
                currentLine += c;
                currentWidth += charAdvance;
            }
        } else {
            
            word += c;
            wordWidth += charAdvance;
        }
    }
    
    
    if (!word.empty()) {
        if (currentWidth + wordWidth > maxWidth && !currentLine.empty()) {
            lines.push_back(currentLine);
            currentLine = word;
        } else {
            currentLine += word;
        }
    }
    
    
    if (!currentLine.empty()) {
        lines.push_back(currentLine);
    }
    
    
    if (lines.empty()) {
        lines.push_back("");
    }
    
    return lines;
}
