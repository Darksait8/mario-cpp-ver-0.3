#include "Enemy.h"
#include "Game.h"
#include "Resources.h"
#include "Physics.h"
#include <Box2D/Dynamics/Body.hpp>
#include <Box2D/Dynamics/Fixture.hpp>
#include <Box2D/Dynamics/World.hpp>
#include <Box2D/Collision/Shapes/CircleShape.hpp>
#include <memory>
#include <cmath>
#include <random> // Включаем заголовочный файл для случайных чисел

// Custom query callback to find MapTile fixtures
struct GroundCheckCallback : public b2::QueryCallback {
    bool foundGround = false;

    bool ReportFixture(b2::Fixture* fixture) override {
        FixtureData* data = static_cast<FixtureData*>(fixture->GetUserData());
        if (data && data->type == FixtureDataType::MapTile) {
            foundGround = true;
            return false; // Stop at the first MapTile found
        }
        return true; // Continue searching
    }
};

void Enemy::Begin() {
  animation =
      Animation(0.5f, {
                          AnimFrame(0.25f, Resources::textures["enemy2.png"]),
                          AnimFrame(0.00f, Resources::textures["enemy1.png"]),
                      });

  tag = "enemy";

  fixtureData.object = this;
  fixtureData.type = FixtureDataType::Object;
  fixtureData.listener = nullptr;

  b2::BodyDef bodyDef{};
  bodyDef.type = b2::BodyType::dynamicBody;
  bodyDef.position = b2::Vec2(position.x, position.y);
  // bodyDef.fixedRotation = true; // если есть аналог
  body = Physics::world->CreateBody(&bodyDef);

  b2::CircleShape circleShape;
  circleShape.m_radius = 0.5f;

  b2::FixtureDef fixtureDef{};
  fixtureDef.shape = &circleShape;
  fixtureDef.userData = &fixtureData;
  body->CreateFixture(&fixtureDef);

  // Случайный выбор начального направления движения
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(-1.0, 1.0);

  // Устанавливаем movement в 5.0 или -5.0 случайным образом
  movement = (dis(gen) > 0) ? 5.0f : -5.0f;
}

void Enemy::Update(float deltaTime) {
  if (isDead) {
    destroyTimer += deltaTime;
    if (destroyTimer >= 2.0f)
      DeleteObject(this);

    return;
  }

  animation.Update(deltaTime);

  auto velocity = body->GetLinearVelocity();

  // Check for edge before moving
  b2::Vec2 bodyPosition = body->GetPosition();
  float checkDistance = 0.6f; // Check slightly ahead of the enemy
  float checkHeight = 0.6f; // Увеличиваем checkHeight, чтобы AABB был ниже низа тела
  
  b2::Vec2 checkPoint(bodyPosition.x + std::copysign(checkDistance, movement), bodyPosition.y + checkHeight);

  GroundCheckCallback callback;
  b2::AABB aabb;
  // Увеличиваем размер AABB
  aabb.lowerBound = checkPoint - b2::Vec2(0.1f, 0.1f);
  aabb.upperBound = checkPoint + b2::Vec2(0.1f, 0.1f);

  Physics::world->QueryAABB(&callback, aabb);

  // If no ground found ahead, reverse direction
  if (!callback.foundGround) {
      movement *= -1.0f;
  }

  // Reverse direction if hitting a wall (low horizontal velocity)
  if (std::abs(velocity.x) <= 0.02f && !isDead)
    movement *= -1.0f;

  velocity.x = movement;

  body->SetLinearVelocity(velocity);

  auto xf = body->GetTransform();
  this->position = sf::Vector2f(xf.p.x, xf.p.y);

  angle = static_cast<float>(body->GetAngle());
}

void Enemy::Render(Renderer &renderer) {
  renderer.Draw(animation.GetTexture(),
                !isDead ? position
                        : sf::Vector2f(position.x, position.y + 0.45f),
                sf::Vector2f(1.0f, isDead ? 0.2f : 1.0f), angle);
}

void Enemy::Die() {
  isDead = true;
  AddScore(100); // Начисляем 100 очков за убийство врага
  // Physics::world->DestroyBody(body); // Убираем немедленное удаление тела
}

bool Enemy::IsDead() { return isDead; }
