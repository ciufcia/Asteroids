#pragma once

#include "cgl.hpp"
#include <random>

using namespace cgl;

struct Asteroid
{
    Asteroid(
        u32 numberOfSides,
        f32 minRadius,
        f32 maxRadius,
        f32 minSpeed,
        f32 maxSpeed,
        f32 minRotation,
        f32 maxRotation,
        Vector2<f32> screenSize,
        Vector2<f32> aimTarget
    );
    
    std::shared_ptr<drawables::Polygon> polygon;

    Vector2<f32> direction;

    f32 movementSpeed;
    f32 rotationSpeed;

    f32 smallestRadius = std::numeric_limits<f32>::max();
    f32 biggestRadius = std::numeric_limits<f32>::min();

    bool big = true;
};

Asteroid::Asteroid(
    u32 numberOfSides,
    f32 minRadius,
    f32 maxRadius,
    f32 minSpeed,
    f32 maxSpeed,
    f32 minRotation,
    f32 maxRotation,
    Vector2<f32> screenSize,
    Vector2<f32> aimTarget
) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<f32> radiusDist(minRadius, maxRadius);

    std::vector<Vector2<f32>> vertices;
    Vector2<f32> centroid {0.f, 0.f};
    for (int i = 0; i < numberOfSides; ++i) {
        f32 angle = 2.0f * 3.14159265358979323846f * i / numberOfSides;
        f32 radius = radiusDist(gen);
        smallestRadius = std::min(smallestRadius, radius);
        biggestRadius = std::max(biggestRadius, radius);
        f32 x = std::cos(angle) * radius;
        f32 y = std::sin(angle) * radius;
        vertices.emplace_back(x, y);
        centroid = centroid + vertices.back();
    }
    centroid = centroid / static_cast<f32>(numberOfSides);

    polygon = std::make_shared<drawables::Polygon>(vertices);
    polygon->transform.setOrigin(centroid);

    auto filter = Filter::create(
        FilterType::Parallel,
        filters::singleColor,
        filters::SingleColorData{Color{255, 255, 255, 255}}
    );
    polygon->fragmentPipeline.addFilter(filter, 0);

    std::uniform_real_distribution<f32> speedDist(minSpeed, maxSpeed);
    std::uniform_real_distribution<f32> rotationDist(minRotation, maxRotation);
    movementSpeed = speedDist(gen);
    rotationSpeed = rotationDist(gen);

    std::uniform_int_distribution<int> edgeDist(0, 3);
    int edge = edgeDist(gen);
    Vector2<f32> pos;
    std::uniform_real_distribution<f32> xDist(0, screenSize.x);
    std::uniform_real_distribution<f32> yDist(0, screenSize.y);
    f32 margin = biggestRadius;
    switch (edge) {
        case 0: // Top
            pos = { xDist(gen), -margin };
            break;
        case 1: // Bottom
            pos = { xDist(gen), screenSize.y + margin };
            break;
        case 2: // Left
            pos = { -margin, yDist(gen) };
            break;
        case 3: // Right
            pos = { screenSize.x + margin, yDist(gen) };
            break;
    }
    polygon->transform.setPosition(pos);
  
    direction = (aimTarget - pos).normalized();
}
