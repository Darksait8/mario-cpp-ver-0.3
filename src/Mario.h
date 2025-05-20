#pragma once

#include "Animation.h"
#include "Physics.h"
#include "Renderer.h"
#include <Box2D/Collision/Shapes/PolygonShape.hpp>
#include <Box2D/Dynamics/Body.hpp>
#include <Box2D/Dynamics/Fixture.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

class Mario : public ContactListener {
public:
  void Begin();
  void Update(float deltaTime);
  void Draw(Renderer &renderer);

  virtual void OnBeginContact(b2::Fixture *self, b2::Fixture *other) override;
  virtual void OnEndContact(b2::Fixture *self, b2::Fixture *other) override;

  size_t GetCoins();

  sf::Vector2f position{};
  float angle{};
  bool isDead{};

private:
  Animation runAnimation{};
  sf::Texture textureToDraw{};
  sf::Sound jumpSound;

  FixtureData fixtureData{};
  b2::Body *body{};
  b2::Fixture *groundFixture{};

  size_t isGrounded{};
  bool facingLeft{};
  size_t coins{};
};
