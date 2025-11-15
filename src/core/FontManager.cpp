#include "core/FontManager.h"
#include "core/LogManager.h"
#include "utils/Path.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include <fstream>
#include <cmath>
#include <algorithm>


Font::Font(MTL::Device* device, const std::string& fontPath, float fontSize)
    : device(device), fontPath(fontPath), fontSize(fontSize), valid(false),
      lineHeight(0), ascent(0), descent(0), fontInfo(nullptr), texture(nullptr)
{
    LOG_INFO("Font: Loading font from %s at size %.1f", fontPath.c_str(), fontSize);
    
    
    std::ifstream file(fontPath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        LOG_ERROR("Font: Failed to open font file: %s", fontPath.c_str());
        return;
    }
    
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    fontBuffer.resize(size);
    if (!file.read((char*)fontBuffer.data(), size)) {
        LOG_ERROR("Font: Failed to read font file: %s", fontPath.c_str());
        return;
    }
    file.close();
    
    
    fontInfo = new stbtt_fontinfo();
    if (!stbtt_InitFont(fontInfo, fontBuffer.data(), 0)) {
        LOG_ERROR("Font: Failed to initialize font: %s", fontPath.c_str());
        delete fontInfo;
        fontInfo = nullptr;
        return;
    }
    
    
    float scale = stbtt_ScaleForPixelHeight(fontInfo, fontSize);
    
    int ascent_i, descent_i, lineGap_i;
    stbtt_GetFontVMetrics(fontInfo, &ascent_i, &descent_i, &lineGap_i);
    
    ascent = ascent_i * scale;
    descent = descent_i * scale;
    lineHeight = (ascent_i - descent_i + lineGap_i) * scale;
    
    LOG_INFO("Font metrics - ascent: %.2f, descent: %.2f, lineHeight: %.2f", ascent, descent, lineHeight);
    
    
    bakeFont();
    
    valid = (texture != nullptr);
    
    if (valid) {
        LOG_INFO("Font: Successfully loaded %s", fontPath.c_str());
    }
}

Font::~Font()
{
    if (texture) {
        texture->release();
        texture = nullptr;
    }
    
    if (fontInfo) {
        delete fontInfo;
        fontInfo = nullptr;
    }
}

void Font::bakeFont()
{
    
    atlasData.resize(ATLAS_WIDTH * ATLAS_HEIGHT);
    bakedGlyphs.resize(NUM_CHARS);
    
    
    stbtt_pack_context packContext;
    if (!stbtt_PackBegin(&packContext, atlasData.data(), ATLAS_WIDTH, ATLAS_HEIGHT, 0, 1, nullptr)) {
        LOG_ERROR("Font: Failed to begin packing");
        return;
    }
    
    stbtt_PackSetOversampling(&packContext, 2, 2); 
    
    std::vector<stbtt_packedchar> packedChars(NUM_CHARS);
    if (!stbtt_PackFontRange(&packContext, fontBuffer.data(), 0, fontSize, FIRST_CHAR, NUM_CHARS, packedChars.data())) {
        LOG_ERROR("Font: Failed to pack font range");
        stbtt_PackEnd(&packContext);
        return;
    }
    
    stbtt_PackEnd(&packContext);
    
    
    for (int i = 0; i < NUM_CHARS; ++i) {
        const auto& pc = packedChars[i];
        auto& bg = bakedGlyphs[i];
        
        bg.x0 = pc.x0 / (float)ATLAS_WIDTH;
        bg.y0 = pc.y0 / (float)ATLAS_HEIGHT;
        bg.x1 = pc.x1 / (float)ATLAS_WIDTH;
        bg.y1 = pc.y1 / (float)ATLAS_HEIGHT;
        bg.xoff = pc.xoff;
        bg.yoff = pc.yoff;
        bg.xoff2 = pc.xoff2;
        bg.yoff2 = pc.yoff2;
        bg.xadvance = pc.xadvance;
        bg.width = (int)(pc.x1 - pc.x0);
        bg.height = (int)(pc.y1 - pc.y0);
    }
    
    
    MTL::TextureDescriptor* texDesc = MTL::TextureDescriptor::texture2DDescriptor(
        MTL::PixelFormatR8Unorm,
        ATLAS_WIDTH,
        ATLAS_HEIGHT,
        false
    );
    texDesc->setUsage(MTL::TextureUsageShaderRead);
    texDesc->setStorageMode(MTL::StorageModeShared);
    
    texture = device->newTexture(texDesc);
    texDesc->release();
    
    if (!texture) {
        LOG_ERROR("Font: Failed to create texture");
        return;
    }
    
    
    MTL::Region region = MTL::Region(0, 0, ATLAS_WIDTH, ATLAS_HEIGHT);
    texture->replaceRegion(region, 0, atlasData.data(), ATLAS_WIDTH);
    
    LOG_INFO("Font: Baked %d glyphs into %dx%d atlas", NUM_CHARS, ATLAS_WIDTH, ATLAS_HEIGHT);
}

const BakedGlyph* Font::getGlyph(char c) const
{
    int index = (int)c - FIRST_CHAR;
    if (index < 0 || index >= NUM_CHARS) {
        return nullptr;
    }
    return &bakedGlyphs[index];
}

void Font::measureText(const std::string& text, float& width, float& height) const
{
    width = 0.0f;
    height = lineHeight;
    
    for (char c : text) {
        const BakedGlyph* glyph = getGlyph(c);
        if (glyph) {
            width += glyph->xadvance;
        }
    }
}


FontManager& FontManager::getInstance()
{
    static FontManager instance;
    return instance;
}

void FontManager::initialize(MTL::Device* device)
{
    this->device = device;
    LOG_INFO("FontManager: Initialized");
}

std::string FontManager::makeFontKey(const std::string& fontPath, float fontSize) const
{
    return fontPath + "@" + std::to_string((int)fontSize);
}

std::shared_ptr<Font> FontManager::loadFont(const std::string& fontPath, float fontSize)
{
    if (!device) {
        LOG_ERROR("FontManager: Not initialized");
        return nullptr;
    }
    
    auto font = std::make_shared<Font>(device, fontPath, fontSize);
    if (!font->isValid()) {
        LOG_ERROR("FontManager: Failed to load font: %s", fontPath.c_str());
        return nullptr;
    }
    
    std::string key = makeFontKey(fontPath, fontSize);
    fontCache[key] = font;
    
    return font;
}

std::shared_ptr<Font> FontManager::getFont(const std::string& fontPath, float fontSize)
{
    std::string key = makeFontKey(fontPath, fontSize);
    
    auto it = fontCache.find(key);
    if (it != fontCache.end()) {
        return it->second;
    }
    
    return loadFont(fontPath, fontSize);
}
