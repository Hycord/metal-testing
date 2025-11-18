#pragma once

#include <Metal/Metal.hpp>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "engine/core/LogManager.h"


struct stbtt_fontinfo;

struct GlyphMetrics {
    float advanceX;
    float leftBearing;
    float x0, y0, x1, y1; 
};

struct BakedGlyph {
    float x0, y0, x1, y1;     
    float xoff, yoff;          
    float xoff2, yoff2;        
    float xadvance;            
    int width, height;         
};

class Font {
public:
    Font(MTL::Device* device, const std::string& fontPath, float fontSize);
    ~Font();
    
    bool isValid() const { return valid; }
    
    
    const BakedGlyph* getGlyph(char c) const;
    
    
    MTL::Texture* getTexture() const { return texture; }
    
    float getFontSize() const { return fontSize; }
    float getLineHeight() const { return lineHeight; }
    float getAscent() const { return ascent; }
    float getDescent() const { return descent; }
    
    void cleanup();
    
    void measureText(const std::string& text, float& width, float& height) const;

private:
    void bakeFont();
    
    MTL::Device* device;
    std::string fontPath;
    float fontSize;
    bool valid;
    bool cleanedUp = false;
    
    
    float lineHeight;
    float ascent;
    float descent;
    
    
    std::vector<unsigned char> fontBuffer;
    stbtt_fontinfo* fontInfo;
    
    
    static constexpr int ATLAS_WIDTH = 512;
    static constexpr int ATLAS_HEIGHT = 512;
    static constexpr int FIRST_CHAR = 32;  
    static constexpr int NUM_CHARS = 96;    
    
    std::vector<BakedGlyph> bakedGlyphs;
    std::vector<unsigned char> atlasData;
    MTL::Texture* texture;
};

class FontManager {
public:
    static FontManager& getInstance();
    
    
    void initialize(MTL::Device* device);
    
    void shutdown();
    
    std::shared_ptr<Font> loadFont(const std::string& fontPath, float fontSize);
    
    
    std::shared_ptr<Font> getFont(const std::string& fontPath, float fontSize);

private:
    FontManager() = default;
    ~FontManager() { LOG_DESTROY("FontManager (static)"); }
    FontManager(const FontManager&) = delete;
    FontManager& operator=(const FontManager&) = delete;
    
    MTL::Device* device = nullptr;
    std::map<std::string, std::shared_ptr<Font>> fontCache;
    
    std::string makeFontKey(const std::string& fontPath, float fontSize) const;
};
