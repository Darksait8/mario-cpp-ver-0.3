#include "Mario.h"
#include "Object.h"
#include "Resources.h"

#include "Enemy.h"
#include "Game.h"
#include <Box2D/Collision/Shapes/CircleShape.hpp>
#include <Box2D/Collision/Shapes/PolygonShape.hpp>
#include <Box2D/Dynamics/Fixture.hpp>
#include <iostream>

const float movementSpeed = 7.0f;
const float jumpVelocity = 10.0f;

void Mario::Begin() {
  runAnimation =
      Animation(0.45f, {
                           AnimFrame(0.30f, Resources::textures["run3.png"]),
                           AnimFrame(0.15f, Resources::textures["run2.png"]),
                           AnimFrame(0.00f, Resources::textures["run1.png"]),
                       });

  jumpSound.setBuffer(Resources::sounds["jump.wav"]);
  jumpSound.setVolume(20);

  fixtureData.listener = this;
  fixtureData.mario = this;
  fixtureData.type = FixtureDataType::Mario;

  b2::BodyDef bodyDef{};
  bodyDef.type = b2::BodyType::dynamicBody;
  bodyDef.position = b2::Vec2(position.x, position.y);
  body = Physics::world->CreateBody(&bodyDef);

  b2::CircleShape circleShape;
  circleShape.m_radius = 0.5f;

  b2::FixtureDef fixtureDef{};
  fixtureDef.shape = &circleShape;
  fixtureDef.userData = &fixtureData;
  fixtureDef.density = 1.f;
  body->CreateFixture(&fixtureDef);

  b2::PolygonShape polygonShape;
  polygonShape.SetAsBox(0.5f, 0.5f);
  fixtureDef.shape = &polygonShape;
  body->CreateFixture(&fixtureDef);

  fixtureDef.isSensor = true;
  groundFixture = body->CreateFixture(&fixtureDef);
}

void Mario::Update(float deltaTime) {
  float move = movementSpeed;

  runAnimation.Update(deltaTime);

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
    move *= 2;

  auto velocity = body->GetLinearVelocity();
  velocity.x = 0.0f;

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    velocity.x += move;
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    velocity.x -= move;
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && isGrounded) {
    velocity.y = -jumpVelocity;
    jumpSound.play();
  }

  textureToDraw = runAnimation.GetTexture();

  if (velocity.x < -0.02f)
    facingLeft = true;
  else if (velocity.x > 0.02f)
    facingLeft = false;
  else
    textureToDraw = Resources::textures["idle.png"];

  if (!isGrounded)
    textureToDraw = Resources::textures["jump.png"];

  body->SetLinearVelocity(velocity);

  position = sf::Vector2f(body->GetPosition().x, body->GetPosition().y);
  angle = static_cast<float>(body->GetAngle() * (180.0f / M_PI));
}

void Mario::Draw(Renderer &renderer) {
  renderer.Draw(textureToDraw, position,
                sf::Vector2f(facingLeft ? -1.0f : 1.0f, 2.0f), angle);
}

void Mario::OnBeginContact(b2::Fixture *self, b2::Fixture *other) {
  FixtureData *data = static_cast<FixtureData *>(other->GetUserData());

  if (!data)
    return;

  if (groundFixture == self && data->type == FixtureDataType::MapTile)
    isGrounded++;
  else if (data->type == FixtureDataType::Object &&
           data->object->tag == "coin") {
    DeleteObject(data->object);
    std::cout << "coins = " << ++coins << "\n";
  } else if (data->type == FixtureDataType::Object &&
             data->object->tag == "enemy") {
    Enemy *enemy = dynamic_cast<Enemy *>(data->object);
    if (!enemy)
      return;

    if (groundFixture == self) {
      enemy->Die();
    } else if (!enemy->IsDead()) {
      isDead = true;
    }
  }
}

void Mario::OnEndContact(b2::Fixture *self, b2::Fixture *other) {
  FixtureData *data = static_cast<FixtureData *>(other->GetUserData());

  if (!data)
    return;

  if (groundFixture == self && data->type == FixtureDataType::MapTile &&
      isGrounded > 0)
    isGrounded--;
}

size_t Mario::GetCoins() { return coins; }
