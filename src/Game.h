#pragma once
#include "Camera.h"
#include "Object.h"
#include "Renderer.h"
#include "Map.h"
#include "Mario.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>

Camera &GetCamera();
bool &GetPaused();

void Begin();
void Update(float deltaTime);
void End();
void Render(Renderer &renderer);
void RenderUI(Renderer &renderer);

void DeleteObject(Object *object);

void AddScore(int scoreToAdd);

struct Data {
  Map map{1.0f};
  Camera camera{20.0f};
  Mario mario{};
  std::vector<Object *> objects{};
  sf::Music music{};
  bool paused{};

  sf::Image mapImage{};
  sf::Font font{};
  sf::Text coinsText{"Coins", font};
  sf::Text scoreText{"Score", font};
  sf::Text livesText{"Lives", font};
  sf::Text timeText{"Time", font};
  sf::RectangleShape backgroundShape{sf::Vector2f(1.0f, 1.0f)};

  sf::Sound deathSound;

  int score = 0;
  int lives = 4;
  float timeElapsed = 0.0f;
  bool isGameOver = false;
  bool isVictory = false;
};

static Data *data;
