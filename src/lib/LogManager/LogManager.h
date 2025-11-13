#pragma once

#include <atomic>
#include <cstdarg>
#include <string>
#include "../../config.h"

// Compile-time controls: logs are OFF by default. Define ENABLE_LOG_DEBUG / ENABLE_LOG_INFO / ENABLE_LOG_ERROR
// (for example via -DENABLE_LOG_DEBUG) to enable that logging layer at compile time.

namespace LogManager
{
    enum class Level { Info, Debug, Error };

    // Runtime control
    void setEnabled(Level level, bool enabled);
    bool isEnabled(Level level);

    // Low-level logging API (printf-style)
    void log(Level level, const char *fmt, ...);
    // Tagged logging: include a short tag (e.g. "CONSTRUCT", "DESTROY", "START", "FINISH")
    void logTagged(Level level, const char *tag, const char *fmt, ...);

} // namespace LogManager

// Convenience macros: logs are compiled out unless the corresponding ENABLE_* macro is defined.
#ifdef ENABLE_LOG_INFO
#define LOG_INFO(fmt, ...) LogManager::log(LogManager::Level::Info, fmt, ##__VA_ARGS__)
#else
#define LOG_INFO(fmt, ...) ((void)0)
#endif

// Tagged info macros (compile out if INFO disabled)
#ifdef ENABLE_LOG_INFO
#define LOG_TAG_INFO(tag, fmt, ...) LogManager::logTagged(LogManager::Level::Info, tag, fmt, ##__VA_ARGS__)
#define LOG_CONSTRUCT(fmt, ...) LOG_TAG_INFO("CONSTRUCT", fmt, ##__VA_ARGS__)
#define LOG_DESTROY(fmt, ...) LOG_TAG_INFO("DESTROY", fmt, ##__VA_ARGS__)
#define LOG_START(fmt, ...) LOG_TAG_INFO("START", fmt, ##__VA_ARGS__)
#define LOG_FINISH(fmt, ...) LOG_TAG_INFO("FINISH", fmt, ##__VA_ARGS__)
#define LOG_STEP(fmt, ...) LOG_TAG_INFO("STEP", fmt, ##__VA_ARGS__)
#else
#define LOG_TAG_INFO(tag, fmt, ...) ((void)0)
#define LOG_CONSTRUCT(fmt, ...) ((void)0)
#define LOG_DESTROY(fmt, ...) ((void)0)
#define LOG_START(fmt, ...) ((void)0)
#define LOG_FINISH(fmt, ...) ((void)0)
#define LOG_STEP(fmt, ...) ((void)0)
#endif

#ifdef ENABLE_LOG_DEBUG
#define LOG_DEBUG(fmt, ...) LogManager::log(LogManager::Level::Debug, fmt, ##__VA_ARGS__)
#else
#define LOG_DEBUG(fmt, ...) ((void)0)
#endif

#ifdef ENABLE_LOG_ERROR
#define LOG_ERROR(fmt, ...) LogManager::log(LogManager::Level::Error, fmt, ##__VA_ARGS__)
#else
#define LOG_ERROR(fmt, ...) ((void)0)
#endif
