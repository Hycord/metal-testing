#include "utils/Path.h"

#include <filesystem>

namespace fs = std::filesystem;

namespace Path
{
    std::string dataPath(const std::string &relative)
    {
        static const fs::path base = fs::path("data");
        return (base / relative).string();
    }
}
