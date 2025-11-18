#pragma once

#include <vector>
#include <cstddef>
#include <stdexcept>
#include <typeinfo>
#include "engine/core/LogManager.h"

namespace Utils
{
    template <typename T>
    class Grid
    {
    public:
        using size_type = std::size_t;
        using value_type = T;

        Grid() : w_(0), h_(0), data_() { LOG_CONSTRUCT("Grid<%s> default", typeid(T).name()); }

        Grid(size_type w, size_type h) : w_(w), h_(h), data_(w * h) { LOG_CONSTRUCT("Grid<%s> %zu x %zu", typeid(T).name(), w, h); }

        Grid(size_type w, size_type h, const T &fill) : w_(w), h_(h), data_(w * h, fill) { LOG_CONSTRUCT("Grid<%s> %zu x %zu filled", typeid(T).name(), w, h); }

        Grid(const Grid &other) : w_(other.w_), h_(other.h_), data_(other.data_) { LOG_CONSTRUCT("Grid<%s> copy", typeid(T).name()); }

        Grid(Grid &&other) noexcept : w_(other.w_), h_(other.h_), data_(std::move(other.data_)) { other.w_ = 0; other.h_ = 0; LOG_CONSTRUCT("Grid<%s> move", typeid(T).name()); }

        Grid &operator=(const Grid &other)
        {
            if (this != &other)
            {
                w_ = other.w_;
                h_ = other.h_;
                data_ = other.data_;
                LOG_DEBUG("Grid<%s> copy assign %zu x %zu", typeid(T).name(), w_, h_);
            }
            return *this;
        }

        Grid &operator=(Grid &&other) noexcept
        {
            if (this != &other)
            {
                w_ = other.w_;
                h_ = other.h_;
                data_ = std::move(other.data_);
                other.w_ = 0;
                other.h_ = 0;
                LOG_DEBUG("Grid<%s> move assign %zu x %zu", typeid(T).name(), w_, h_);
            }
            return *this;
        }

        ~Grid() { LOG_DESTROY("Grid<%s>", typeid(T).name()); }

        const T *operator[](size_type x) const
        {
            return &data_[offset(x)];
        }

        T *operator[](size_type x)
        {
            return &data_[offset(x)];
        }

        T &at(size_type x, size_type y)
        {
            checkBounds(x, y);
            return data_[index(x, y)];
        }

        const T &at(size_type x, size_type y) const
        {
            checkBounds(x, y);
            return data_[index(x, y)];
        }

        bool inBounds(size_type x, size_type y) const noexcept
        {
            return x < w_ && y < h_;
        }

        void fill(const T &value)
        {
            std::fill(data_.begin(), data_.end(), value);
            LOG_DEBUG("Grid<%s> fill", typeid(T).name());
        }

        void clear()
        {
            data_.clear();
            w_ = 0;
            h_ = 0;
            LOG_DEBUG("Grid<%s> clear", typeid(T).name());
        }

        void resize(size_type w, size_type h)
        {
            w_ = w;
            h_ = h;
            data_.assign(w * h, T{});
            LOG_DEBUG("Grid<%s> resize %zu x %zu", typeid(T).name(), w_, h_);
        }

        void resize(size_type w, size_type h, const T &fill)
        {
            w_ = w;
            h_ = h;
            data_.assign(w * h, fill);
            LOG_DEBUG("Grid<%s> resize filled %zu x %zu", typeid(T).name(), w_, h_);
        }

        size_type width() const noexcept { return w_; }
        size_type height() const noexcept { return h_; }
        size_type size() const noexcept { return data_.size(); }
        bool empty() const noexcept { return data_.empty(); }

        const std::vector<T> &data() const noexcept { return data_; }
        std::vector<T> &data() noexcept { return data_; }

        T &operator()(size_type x, size_type y) { return data_[index(x, y)]; }
        const T &operator()(size_type x, size_type y) const { return data_[index(x, y)]; }

    private:
        size_type index(size_type x, size_type y) const noexcept { return x * h_ + y; }

        size_type offset(size_type x) const
        {
            if (x >= w_) throw std::out_of_range("x out of range");
            return x * h_;
        }

        void checkBounds(size_type x, size_type y) const
        {
            if (!inBounds(x, y)) throw std::out_of_range("grid index out of range");
        }

        size_type w_;
        size_type h_;
        std::vector<T> data_;
    };
}
