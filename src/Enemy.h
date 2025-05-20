#pragma once

#include "Animation.h"
#include "Object.h"
#include "Physics.h"
#include <Box2D/Dynamics/Body.hpp>
#include <Box2D/Dynamics/Fixture.hpp>
#include <memory>

class Enemy : public Object {
public:
  virtual void Begin() override;
  virtual void Update(float deltaTime) override;
  virtual void Render(Renderer &renderer) override;

  void Die();
  bool IsDead();

private:
  Animation animation{};
  float movement = 5.0f;

  float destroyTimer = 0.0f;
  bool isDead = false;

  FixtureData fixtureData{};
  b2::Body *body{};
};
