#pragma once
#include "Animation.h"
#include "Object.h"
#include "Physics.h"
#include <Box2D/Dynamics/Body.hpp>
#include "Mario.h"

class Coin : public Object {
public:
  Coin(Mario* marioPtr) : mario(marioPtr) {}
  ~Coin();

  virtual void Begin() override;
  virtual void Update(float deltaTime) override;
  virtual void Render(Renderer &renderer) override;

  bool isCollected = false; // Флаг для отложенного удаления

  b2::Body *body;
  sf::Vector2f initialPosition; // Добавляем поле для хранения начальной позиции

private:
  FixtureData fixtureData;
  Animation animation;
  Mario* mario;
};
