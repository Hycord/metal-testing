#pragma once

#include <any>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>



class EngineIO {
public:
    EngineIO() = default;
    EngineIO(const EngineIO&) = delete;
    EngineIO& operator=(const EngineIO&) = delete;
    EngineIO(EngineIO&&) = default;
    EngineIO& operator=(EngineIO&&) = default;

    template <typename T>
    void set(const std::string &key, T &&value) {
        storage_[key] = std::forward<T>(value);
    }

    template <typename T>
    std::optional<T> get(const std::string &key) const {
        auto it = storage_.find(key);
        if (it == storage_.end()) {
            return std::nullopt;
        }
        try {
            return std::any_cast<T>(it->second);
        } catch (const std::bad_any_cast &) {
            return std::nullopt;
        }
    }

    bool has(const std::string &key) const { return storage_.contains(key); }

    void erase(const std::string &key) { storage_.erase(key); }

    void clear() { storage_.clear(); }

private:
    std::unordered_map<std::string, std::any> storage_;
};
