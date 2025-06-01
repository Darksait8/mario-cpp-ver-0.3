#include "Game.h"
#include "Map.h"
#include "Mario.h"
#include "Physics.h"
#include "Resources.h"
#include "Enemy.h"
#include "Coin.h"

#include <filesystem>
#include <SFML/Audio.hpp>

class Coin; // Forward declaration

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
  sf::RectangleShape backgroundShape{sf::Vector2f(1.0f, 1.0f)};
};

static Data *data;

Camera &GetCamera() { return data->camera; }
bool &GetPaused() { return data->paused; }

void Restart() {
  Physics::Init();

  data->mario.position =
      data->map.CreateFromImage(data->mapImage, data->objects);
  data->mario.isDead = false;
  data->paused = false;

  data->mario.Begin();
  for (auto &object : data->objects) {
    object->Begin();
  }

  data->music.play();
}

void Begin() {
  data = new Data();

  for (auto &file :
       std::filesystem::directory_iterator("resources/textures/")) {
    if (file.is_regular_file() && (file.path().extension() == ".png" ||
                                   file.path().extension() == ".jpg")) {
      Resources::textures[file.path().filename().string()].loadFromFile(
          file.path().string());
    }
  }

  for (auto &file :
       std::filesystem::directory_iterator("./resources/sounds/")) {
    if (file.is_regular_file() && (file.path().extension() == ".ogg" ||
                                   file.path().extension() == ".wav")) {
      Resources::sounds[file.path().filename().string()].loadFromFile(
          file.path().string());
    }
  }

  data->music.openFromFile("./resources/sounds/music.ogg");
  data->music.setLoop(true);
  data->music.setVolume(25);

  data->font.loadFromFile("./resources/font.ttf");
  data->coinsText.setFillColor(sf::Color::White);
  data->coinsText.setOutlineColor(sf::Color::Black);
  data->coinsText.setOutlineThickness(1.0f);
  data->coinsText.setScale(0.1f, 0.1f);

  data->backgroundShape.setFillColor(sf::Color(0, 0, 0, 150));
  data->backgroundShape.setOrigin(0.5f, 0.5f);

  data->mapImage.loadFromFile("./resources/level1.png");

  Restart();
}

void Update(float deltaTime) {
  if (data->mario.isDead)
    Restart();

  if (data->paused)
    return;

  Physics::Update(deltaTime);
  data->mario.Update(deltaTime);
  data->camera.position = data->mario.position;

  // Удаляем объекты, помеченные как isDead, после шага физики
  data->objects.erase(
      std::remove_if(data->objects.begin(), data->objects.end(),
                     [](Object *obj) { 
                       // Удаляем врагов, помеченных как isDead
                       if (obj->tag == "enemy") {
                         return dynamic_cast<Enemy*>(obj)->IsDead();
                       }
                       // Удаляем монетки, помеченные как isCollected
                       if (obj->tag == "coin") {
                         return dynamic_cast<Coin*>(obj)->isCollected;
                       }
                       return false; // Не удаляем другие объекты
                     }),
      data->objects.end());

  for (auto &object : data->objects) {
    object->Update(deltaTime);
  }
}

void Render(Renderer &renderer) {
  renderer.Draw(Resources::textures["sky.png"], data->mario.position,
                data->camera.GetViewSize());

  data->map.Draw(renderer);

  for (auto &object : data->objects) {
    object->Render(renderer);
  }

  data->mario.Draw(renderer);

  Physics::DebugDraw(renderer);
}

void RenderUI(Renderer &renderer) {
  data->coinsText.setPosition(-data->camera.GetViewSize() / 2.0f +
                              sf::Vector2f(2.0f, 1.0f));
  data->coinsText.setString("Coins: " + std::to_string(data->mario.GetCoins()));
  renderer.target.draw(data->coinsText);

  if (data->paused) {
    data->backgroundShape.setScale(data->camera.GetViewSize());
    renderer.target.draw(data->backgroundShape);
  }
}

void DeleteObject(Object *object) {
  const auto &it =
      std::find(data->objects.begin(), data->objects.end(), object);
  if (it != data->objects.end()) {
    delete *it;
    data->objects.erase(it);
  }
}

void End() {
  delete data;
  Resources::textures.clear();
  Resources::sounds.clear();
}
