#pragma once

#include "cgl.hpp"
#include "utils.hpp"
#include "asteroid.hpp"

using namespace cgl;

struct Bullet
{
    Bullet();

    f32 speed = 300.f;

    std::shared_ptr<drawables::Ellipse> ellipse;
    Vector2<f32> direction;
};

class Ship
{
public:

    Ship();

    void update(Framework &framework);
    bool checkForLoss(Framework &framework);
    void draw(Framework &framework);

    u32 getScore() const;

private:

    f32 movementSpeed = 150.f;
    f32 rotationSpeed = 180.f;
    f32 hitboxRadius = 10.f;

    u32 score = 0;

    std::shared_ptr<drawables::Rectangle> rect;
    std::shared_ptr<Texture> texture;
    std::shared_ptr<Filter> filter;

    Vector2<f32> direction = {0.f, -1.f};

    f32 shootCooldown = 0.5f;
    f32 shootTimer = 0.f;
    std::vector<Bullet> bullets;

    f32 asteroidSpawnCooldown = 1.5f;
    f32 asteroidSpawnTimer = 0.f;
    std::vector<Asteroid> asteroids;
};

Bullet::Bullet() {
    ellipse = std::make_shared<drawables::Ellipse>(Vector2<f32>{0.f, 0.f}, Vector2<f32>{2.f, 2.f});
    ellipse->transform.setOrigin(ellipse->radius / 2.0f);
    auto filter = Filter::create(
        FilterType::Parallel,
        filters::singleColor,
        filters::SingleColorData{Color{255, 255, 0, 255}}
    );
    ellipse->fragmentPipeline.addFilter(filter, 0);
}

Ship::Ship() {
    texture = Texture::create("assets/ship.png");
    rect = std::make_shared<drawables::Rectangle>(texture->getSize());
    filters::TextureData textureData { texture, Texture::SamplingMode::Bilinear };
    filter = Filter::create<filters::TextureData>(FilterType::Parallel, filters::texture, textureData);
    rect->fragmentPipeline.addFilter(filter, 0);
    rect->transform.setOrigin(texture->getSize() / 2.0f);
    rect->transform.setPosition({WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f + 40.f});
}

void Ship::update(Framework &framework) {
    if (framework.eventManager.isKeyPressed(KeyCode::A)) {
        rect->transform.rotate(-rotationSpeed * getDurationInSeconds(framework.getLastFrameTime()));
        direction = rotateVector(direction, -rotationSpeed * getDurationInSeconds(framework.getLastFrameTime()));
    }
    if (framework.eventManager.isKeyPressed(KeyCode::D)) {
        rect->transform.rotate(rotationSpeed * getDurationInSeconds(framework.getLastFrameTime()));
        direction = rotateVector(direction, rotationSpeed * getDurationInSeconds(framework.getLastFrameTime()));
    }
    if (framework.eventManager.isKeyPressed(KeyCode::W)) {
        rect->transform.move(direction * movementSpeed * getDurationInSeconds(framework.getLastFrameTime()));
    }

    /*****************/

    shootTimer += getDurationInSeconds(framework.getLastFrameTime());

    if (framework.eventManager.isKeyPressed(KeyCode::Spacebar) && shootTimer >= shootCooldown) {
        shootTimer = 0.f;
        Bullet bullet;
        bullet.ellipse->transform.setPosition(
            rect->transform.getPosition() + Vector2<f32>{
                direction.x * rect->transform.getScale().x * texture->getSize().x * 0.5f,
                direction.y * rect->transform.getScale().y * texture->getSize().x * 0.5f
            }
        );
        bullet.direction = direction;
        bullets.push_back(bullet);
    }

    /*****************/

    for (int i = bullets.size() - 1; i >= 0; --i) {
        auto &bullet = bullets[i];
        bullet.ellipse->transform.move(bullet.direction * bullet.speed * getDurationInSeconds(framework.getLastFrameTime()));

        if (
            bullet.ellipse->transform.getPosition().x + bullet.ellipse->radius.x < 0 ||
            bullet.ellipse->transform.getPosition().x - bullet.ellipse->radius.x > framework.console.getSize().x ||
            bullet.ellipse->transform.getPosition().y + bullet.ellipse->radius.y < 0 ||
            bullet.ellipse->transform.getPosition().y - bullet.ellipse->radius.y > framework.console.getSize().y
        ) {
            bullets.erase(bullets.begin() + i);
        }
    }

    /*****************/

    asteroidSpawnTimer += getDurationInSeconds(framework.getLastFrameTime());

    if (asteroidSpawnTimer >= asteroidSpawnCooldown) {
        asteroidSpawnTimer = 0.f;

        Asteroid asteroid(
            6, // Number of sides
            5.f, // Min radius
            30.f, // Max radius
            50.f, // Min speed
            100.f, // Max speed
            -180.f, // Min rotation
            180.f, // Max rotation
            {WINDOW_WIDTH, WINDOW_HEIGHT},
            rect->transform.getPosition()
        );
        asteroids.push_back(asteroid);
    }

    /*****************/

    for (int i = asteroids.size() - 1; i >= 0; --i) {
        auto &asteroid = asteroids[i];
        asteroid.polygon->transform.move(
            asteroid.direction * asteroid.movementSpeed * getDurationInSeconds(framework.getLastFrameTime())
        );

        asteroid.polygon->transform.rotate(
            asteroid.rotationSpeed * getDurationInSeconds(framework.getLastFrameTime())
        );

        if (
            asteroid.polygon->transform.getPosition().x + asteroid.biggestRadius + 5.f < 0 ||
            asteroid.polygon->transform.getPosition().x - asteroid.biggestRadius - 5.f > framework.console.getSize().x ||
            asteroid.polygon->transform.getPosition().y + asteroid.biggestRadius + 5.f < 0 ||
            asteroid.polygon->transform.getPosition().y - asteroid.biggestRadius - 5.f > framework.console.getSize().y
        ) {
            if (asteroid.big) {
                score += 2;
            }
            asteroids.erase(asteroids.begin() + i);
            continue;
        }

        for (int j = bullets.size() - 1; j >= 0; --j) {
            f32 distance = (asteroid.polygon->transform.getPosition() - bullets[j].ellipse->transform.getPosition()).magnitudeSquared();
            if (distance < (asteroid.biggestRadius + bullets[j].ellipse->radius.x) * (asteroid.biggestRadius + bullets[j].ellipse->radius.x)) {
                score += 10;
                if (asteroid.big) {
                    Vector2<f32> targets[3] = {
                        {asteroid.polygon->transform.getPosition() + rotateVector(bullets[j].direction, -30.f)},
                        {asteroid.polygon->transform.getPosition() + rotateVector(bullets[j].direction, 0.f)},
                        {asteroid.polygon->transform.getPosition() + rotateVector(bullets[j].direction, 30.f)}
                    };

					Vector2<f32> asteroidPosition = asteroid.polygon->transform.getPosition();

                    for (int k = 0; k < 3; ++k) {
                        Asteroid newAsteroid(
                            4, // Number of sides
                            asteroid.smallestRadius * 0.5f, // Min radius
                            asteroid.biggestRadius * 0.5f, // Max radius
                            asteroid.movementSpeed * 0.5f, // Min speed
                            asteroid.movementSpeed * 1.5f, // Max speed
                            -270.f, // Min rotation
                            270.f, // Max rotation
                            {WINDOW_WIDTH, WINDOW_HEIGHT},
                            targets[k]
                        );

                        newAsteroid.polygon->transform.setPosition(asteroidPosition);
                        newAsteroid.direction = (targets[k] - newAsteroid.polygon->transform.getPosition()).normalized();
                        newAsteroid.big = false;

                        asteroids.push_back(newAsteroid);
                    }
                }

                bullets.erase(bullets.begin() + j);
                asteroids.erase(asteroids.begin() + i);
                break;
            }
        }
    }
}

bool Ship::checkForLoss(Framework &framework) {
    for (const auto &asteroid : asteroids) {
        f32 distance = (asteroid.polygon->transform.getPosition() - rect->transform.getPosition()).magnitudeSquared();
        if (distance < (asteroid.smallestRadius + hitboxRadius) * (asteroid.smallestRadius + hitboxRadius)) {
            return true;
        }
    }
    return false;
}

void Ship::draw(Framework &framework) {
    framework.draw(rect);

    for (const auto &bullet : bullets) {
        framework.draw(bullet.ellipse);
    }

    for (const auto &asteroid : asteroids) {
        framework.draw(asteroid.polygon);
    }
}

u32 Ship::getScore() const {
    return score;
}
