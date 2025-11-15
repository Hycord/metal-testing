#include "core/LogManager.h"
#include <chrono>
#include <ctime>
#include <iostream>
#include <cstring>
#include <mutex>
#include <vector>

namespace {
    static std::atomic<bool> g_info_enabled{true};
    static std::atomic<bool> g_debug_enabled{true};
    static std::atomic<bool> g_error_enabled{true};
    static std::mutex g_log_mutex;

    const char *kReset = "\033[0m";
    const char *kRed = "\033[0;31m";
    const char *kGreen = "\033[0;32m";
    const char *kYellow = "\033[1;33m";
    const char *kBlue = "\033[0;34m";
    const char *kMagenta = "\033[0;35m";
    const char *kCyan = "\033[0;36m";

    bool levelEnabled(LogManager::Level level)
    {
        switch (level)
        {
        case LogManager::Level::Info:
            return g_info_enabled.load();
        case LogManager::Level::Debug:
            return g_debug_enabled.load();
        case LogManager::Level::Error:
            return g_error_enabled.load();
        }
        return false;
    }

    const char *levelPrefix(LogManager::Level level)
    {
        switch (level)
        {
        case LogManager::Level::Info:
            return kGreen;
        case LogManager::Level::Debug:
            return kYellow;
        case LogManager::Level::Error:
            return kRed;
        }
        return kReset;
    }

    const char *levelName(LogManager::Level level)
    {
        switch (level)
        {
        case LogManager::Level::Info:
            return "INFO";
        case LogManager::Level::Debug:
            return "DEBUG";
        case LogManager::Level::Error:
            return "ERROR";
        }
        return "";
    }

    const char *tagColor(const char *tag)
    {
        
        if (!tag) return kReset;
        
        if (std::strcmp(tag, "CONSTRUCT") == 0) return kMagenta;
        if (std::strcmp(tag, "DESTROY") == 0) return kRed;
        if (std::strcmp(tag, "START") == 0) return kBlue;
        if (std::strcmp(tag, "FINISH") == 0) return kGreen;
        if (std::strcmp(tag, "SHADER") == 0) return kCyan;
        if (std::strcmp(tag, "STEP") == 0) return kYellow;
        return kReset;
    }
}

namespace LogManager
{

    void setEnabled(Level level, bool enabled)
    {
        switch (level)
        {
        case Level::Info:
            g_info_enabled.store(enabled);
            break;
        case Level::Debug:
            g_debug_enabled.store(enabled);
            break;
        case Level::Error:
            g_error_enabled.store(enabled);
            break;
        }
    }

    bool isEnabled(Level level)
    {
        return levelEnabled(level);
    }

    void log(Level level, const char *fmt, ...)
    {
        if (!levelEnabled(level))
            return;

        
        va_list args;
        va_start(args, fmt);

        
        char buffer[1024];
        vsnprintf(buffer, sizeof(buffer), fmt, args);
        va_end(args);

    
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    
    long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000;
    std::tm tm{};
#if defined(_POSIX_VERSION) || defined(__APPLE__)
    localtime_r(&t, &tm);
#else
    
    std::tm *tmp = std::localtime(&t);
    if (tmp) tm = *tmp;
#endif
    char timebuf[80];
    std::strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", &tm);
    char timestr[96];
    std::snprintf(timestr, sizeof(timestr), "%s.%03lld", timebuf, ms);

        std::lock_guard<std::mutex> lock(g_log_mutex);
    const char *prefixColor = levelPrefix(level);
    const char *name = levelName(level);

    
    std::cout << prefixColor << "[" << name << "] " << kReset << timestr << " - " << buffer << std::endl;
    }

    void logTagged(Level level, const char *tag, const char *fmt, ...)
    {
        if (!levelEnabled(level))
            return;

        va_list args;
        va_start(args, fmt);
        char buffer[1024];
        vsnprintf(buffer, sizeof(buffer), fmt, args);
        va_end(args);

    
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000;
    std::tm tm{};
#if defined(_POSIX_VERSION) || defined(__APPLE__)
    localtime_r(&t, &tm);
#else
    std::tm *tmp = std::localtime(&t);
    if (tmp) tm = *tmp;
#endif
    char timebuf[80];
    std::strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", &tm);
    char timestr[96];
    std::snprintf(timestr, sizeof(timestr), "%s.%03lld", timebuf, ms);

        std::lock_guard<std::mutex> lock(g_log_mutex);

        const char *levelC = levelPrefix(level);
        const char *name = levelName(level);
        const char *tcolor = tagColor(tag);

    
    std::cout << levelC << "[" << name << "] " << kReset
          << timestr << " "
                  << tcolor << "[" << (tag ? tag : "") << "] " << kReset
                  << "- " << buffer << std::endl;
    }

} 
