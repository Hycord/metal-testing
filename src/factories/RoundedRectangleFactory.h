#pragma once

#include <simd/simd.h>
#include "factories/CircleFactory.h"

struct RoundedRectangleFactory
{
    
    simd::float2 const size;
    
    float const radius;
    
    int const perCorner;
    
    CircleFactory const factory;

    
    simd::float2 const centers[4];

    
    RoundedRectangleFactory(simd::float2 size_, float radius_, int qualityPerCorner)
        : size{size_},
          radius{radius_},
          perCorner{qualityPerCorner < 1 ? 1 : qualityPerCorner},
          factory{radius, 4 * (qualityPerCorner < 1 ? 1 : qualityPerCorner)},
          centers{
              simd::float2{size_.x - radius_, radius_},          
              simd::float2{size_.x - radius_, size_.y - radius_},
              simd::float2{radius_,          size_.y - radius_}, 
              simd::float2{radius_,          radius_}            
          }
    {}

    
    int pointCount() const { return 4 * perCorner; }

    
    
    simd::float2 getPoint(int index) const
    {
        int N = pointCount();
        if (N <= 0) return {0.0f, 0.0f};
        int i = index % N;
        if (i < 0) i += N;

        int corner = i / perCorner;            
        int inCorner = i % perCorner;          

        
        
        int base = ((corner + 3) % 4) * perCorner; 
        int circleIndex = base + inCorner;

        return centers[corner] + factory.getPoint(circleIndex);
    }
};