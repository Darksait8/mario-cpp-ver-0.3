#pragma once
#include "Animation.h"
#include "Object.h"
#include "Physics.h"
#include <Box2D/Dynamics/Body.hpp>

class Coin : public Object {
public:
  ~Coin();

  virtual void Begin() override;
  virtual void Update(float deltaTime) override;
  virtual void Render(Renderer &renderer) override;

private:
  FixtureData fixtureData;
  Animation animation;
  b2::Body *body;
};
