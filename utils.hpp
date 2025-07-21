#pragma once

#include "cgl.hpp"

using namespace cgl;

constexpr u32 WINDOW_WIDTH = 320;
constexpr u32 WINDOW_HEIGHT = 160;

void drawWindowBorder(Framework &framework) {
    auto line = Drawable::create<drawables::Line>(
        Vector2<f32>{0.f, 0.f},
        Vector2<f32>{static_cast<f32>(WINDOW_WIDTH), 0.f}
    );
    auto filter = Filter::create(
        FilterType::Parallel,
        filters::singleColor,
        filters::SingleColorData{Color{255, 255, 255, 255}}
    );
    line->fragmentPipeline.addFilter(filter, 0);
    line->cloneOnDraw = true;

    framework.draw(line);

    line->end = Vector2<f32>{0.f, static_cast<f32>(WINDOW_HEIGHT)};
    framework.draw(line);

    line->start = Vector2<f32>{static_cast<f32>(WINDOW_WIDTH), static_cast<f32>(WINDOW_HEIGHT)};
    framework.draw(line);

    line->end = Vector2<f32>{static_cast<f32>(WINDOW_WIDTH), 0.f};
    framework.draw(line);
}

Vector2<f32> rotateVector(const Vector2<f32>& vec, f32 angle) {
    f32 rad = Transform::degreesToRadians(angle);
    f32 cosAngle = std::cos(rad);
    f32 sinAngle = std::sin(rad);
    return Vector2<f32>{
        vec.x * cosAngle - vec.y * sinAngle,
        vec.x * sinAngle + vec.y * cosAngle
    };
}