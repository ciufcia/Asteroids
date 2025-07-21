#include "cgl.hpp"
#include "ship.hpp"
#include "utils.hpp"

using namespace cgl;

void rainbowFilter(void *filterData, void *passData) {
    filter_pass_data::PixelPass *pixeldata = static_cast<filter_pass_data::PixelPass *>(passData);

    Color sampledColor = sampleUVColor(pixeldata->uv + Vector2<f32>(pixeldata->time, 0.f));

    pixeldata->color.r *= sampledColor.r / 256.f;
    pixeldata->color.g *= sampledColor.g / 256.f;
    pixeldata->color.b *= sampledColor.b / 256.f;
}

#include <iostream>

int main() {
    BitmapFont font;
    font.loadFromBDF("assets/font.bdf");
    auto titleTextTexture = Texture::create(Vector2<u32>{200, 100});
    font.renderToTexture("Asteroids", *titleTextTexture, Color{ 255, 255, 255, 255 }, Color{ 0, 0, 0, 0 });
    auto titleTextRect = Drawable::create<drawables::Rectangle>(Vector2<f32>{200.f, 100.f});
    titleTextRect->transform.setOrigin(titleTextRect->getSize() / 2.0f);
    titleTextRect->transform.setPosition({WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f - 40.f});

    auto promptTextTexture = Texture::create();
    font.renderToTexture("Press Space to Start", *promptTextTexture, Color{ 255, 255, 255, 255 }, Color{ 0, 0, 0, 0 });
    auto promptTextRect = Drawable::create<drawables::Rectangle>(Vector2<f32>{200.f, 50.f});
    promptTextRect->transform.setOrigin(promptTextRect->getSize() / 2.0f);
    promptTextRect->transform.setPosition({WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f + 5.f});

    auto scoreTexture = Texture::create();
    auto scoreTextRect = Drawable::create<drawables::Rectangle>(Vector2<f32>{50.f, 30.f});
    scoreTextRect->transform.setOrigin(scoreTextRect->getSize() / 2.0f);
    scoreTextRect->transform.setPosition({WINDOW_WIDTH / 2.f, scoreTextRect->getSize().y / 2.f});

    auto titleTextureFilter = Filter::create(
        FilterType::Parallel,
        filters::texture,
        filters::TextureData{titleTextTexture, Texture::SamplingMode::Bilinear}
    );

    auto promptTextureFilter = Filter::create(
        FilterType::Parallel,
        filters::texture,
        filters::TextureData{promptTextTexture, Texture::SamplingMode::Bilinear}
    );

    auto scoreTextureFilter = Filter::create(
        FilterType::Parallel,
        filters::texture,
        filters::TextureData{scoreTexture, Texture::SamplingMode::Bilinear}
    );

    auto Rainbowfilter = Filter::create(
        FilterType::Parallel,
        rainbowFilter,
        nullptr
    );

    titleTextRect->fragmentPipeline.addFilter(titleTextureFilter, 0);
    titleTextRect->fragmentPipeline.addFilter(Rainbowfilter, 1);

    promptTextRect->fragmentPipeline.addFilter(promptTextureFilter, 0);
    promptTextRect->fragmentPipeline.addFilter(Rainbowfilter, 1);

    scoreTextRect->fragmentPipeline.addFilter(scoreTextureFilter, 0);

    Framework framework;

    Ship ship;

    bool running = true;

    bool gameStarted = false;

    framework.initialize();

    framework.setFPSTarget(1000);

    while (running) {
        framework.eventManager.handleEvents(
            [&](KeyPressEvent, const Event &event) {
                if (event.key == KeyCode::Escape) {
                    running = false;
                }
                if (event.key == KeyCode::Spacebar) {
                    gameStarted = true;
                }
            }
        );


        if (gameStarted) {
            ship.update(framework);
            if (ship.checkForLoss(framework)) {
                gameStarted = false;
                ship = Ship();
            }
        }

        framework.clearDisplay(Color{0, 0, 0, 255});

        if (!gameStarted) {
            framework.draw(titleTextRect);
            framework.draw(promptTextRect);
        }

        drawWindowBorder(framework);
        ship.draw(framework);

        if (gameStarted) {
            std::string text = std::to_string(ship.getScore());
            scoreTextRect->setSize(Vector2<f32>{40.f * text.length(), 30.f});
            scoreTextRect->transform.setOrigin(scoreTextRect->getSize() / 2.0f);
            scoreTextRect->transform.setPosition({WINDOW_WIDTH / 2.f, scoreTextRect->getSize().y / 2.f});
            font.renderToTexture(
                text,
                *scoreTexture,
                Color{255, 255, 255, 255},
                Color{0, 0, 0, 0}
            );
            framework.draw(scoreTextRect);
        }

        framework.update();

        framework.console.setTitle(
            "Asteroids - FPS: " + std::to_string((int)(1.f / getDurationInSeconds(framework.getLastFrameTime()))) +
            ", Target FPS: " + std::to_string(framework.getFPSTarget())
        );
    }
}