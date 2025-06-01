#include "Enemy.h"
#include "Game.h"
#include "Resources.h"
#include <Box2D/Collision/Shapes/CircleShape.hpp>
#include <Box2D/Dynamics/Body.hpp>
#include <Box2D/Dynamics/Fixture.hpp>
#include <memory>

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

  if (std::abs(velocity.x) <= 0.02f)
    movement *= -1.0f;

  velocity.x = movement;

  body->SetLinearVelocity(velocity);

  auto xf = body->GetTransform();
  position = sf::Vector2f(xf.p.x, xf.p.y);

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
  // Physics::world->DestroyBody(body); // Убираем немедленное удаление тела
}

bool Enemy::IsDead() { return isDead; }
