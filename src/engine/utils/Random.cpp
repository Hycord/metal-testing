#include "engine/utils/Random.h"
#include "engine/core/LogManager.h"
#include <random>
#include <chrono>
#include <mutex>

namespace {
    std::mt19937 &rng()
    {
        static std::mt19937 engine;
        static std::once_flag initFlag;
        std::call_once(initFlag, []() {
            std::random_device rd;
            auto now = std::chrono::steady_clock::now().time_since_epoch().count();
            auto seed = static_cast<unsigned int>(now ^ (static_cast<unsigned long long>(rd()) << 1));
            LOG_INFO("Seeding RNG (mt19937) with %u", seed);
            engine.seed(seed);
        });
        return engine;
    }
}

unsigned int randomUInt(unsigned int minInclusive, unsigned int maxInclusive)
{
    if (minInclusive > maxInclusive) {
        LOG_ERROR("randomUInt invalid range [%u,%u]", minInclusive, maxInclusive);
        return minInclusive;
    }
    std::uniform_int_distribution<unsigned int> dist(minInclusive, maxInclusive);
    auto value = dist(rng());
    LOG_DEBUG("randomUInt -> %u in [%u,%u]", value, minInclusive, maxInclusive);
    return value;
}
