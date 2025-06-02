#include "Game.h"
#include "Map.h"
#include "Mario.h"
#include "Physics.h"
#include "Resources.h"
#include "Enemy.h"
#include "Coin.h"

#include <filesystem>
#include <SFML/Audio.hpp>
#include <iostream>

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
  sf::Text scoreText{"Score", font};
  sf::Text livesText{"Lives", font};
  sf::Text timeText{"Time", font};
  sf::RectangleShape backgroundShape{sf::Vector2f(1.0f, 1.0f)};

  sf::Sound deathSound; // Добавляем объект sf::Sound для звука смерти

  int score = 0;
  int lives = 4;
  float timeElapsed = 0.0f;
  bool isGameOver = false;
};

static Data *data;

Camera &GetCamera() { return data->camera; }
bool &GetPaused() { return data->paused; }

void Restart() {
  // Проверяем количество жизней перед рестартом
  if (data->lives > 1) {
    data->lives--; // Уменьшаем количество жизней
    data->score = 0; // Сбрасываем очки
    data->mario.coins = 0; // Сбрасываем количество монет у Марио (предполагая, что coins это член Mario)
    Physics::Init();

    data->mario.position =
        data->map.CreateFromImage(data->mapImage, data->objects, &data->mario);
    data->mario.isDead = false;
    data->paused = false;

    data->mario.Begin();
    for (auto &object : data->objects) {
      object->Begin();
    }

    data->music.play();
  } else {
    // Жизни закончились, устанавливаем флаг Game Over
    if (Resources::sounds.count("mario-smert.mp3")) {
      data->deathSound.play(); // Воспроизводим звук смерти через член структуры Data
    }
    data->isGameOver = true;
    data->music.stop(); // Останавливаем фоновую музыку
  }
}

void Begin() {
  data = new Data();

  for (auto &file :
       std::filesystem::directory_iterator("resources/textures/")) {
    if (file.is_regular_file() && (file.path().extension() == ".png" ||
                                   file.path().extension() == ".jpg")) {
      std::string filename = file.path().filename().string();
      if (Resources::textures[filename].loadFromFile(file.path().string())) {
          std::cout << "Loaded texture: " << filename << std::endl;
      } else {
          std::cerr << "Failed to load texture: " << filename << std::endl;
      }
    }
  }

  // Загружаем текстуру Game Over (death.jpg)
  if (std::filesystem::exists("./resources/textures/death.jpg")) {
      Resources::textures["death.jpg"].loadFromFile("./resources/textures/death.jpg");
  } else {
      // Удаляем std::cerr << "Error: death.jpg not found in resources/textures/" << std::endl;
  }

  // std::cout << "Loaded " << Resources::textures.size() << " textures." << std::endl; // Комментируем вывод

  for (auto &file :
       std::filesystem::directory_iterator("./resources/sounds/")) {
    if (file.is_regular_file() && (file.path().extension() == ".ogg" ||
                                   file.path().extension() == ".wav" ||
                                   file.path().extension() == ".mp3")) {
      Resources::sounds[file.path().filename().string()].loadFromFile(
          file.path().string());
    }
  }

  // Debug: Check if the death sound is loaded
  if (Resources::sounds.count("mario-smert.mp3")) {
      // Удаляем std::cout << "Successfully loaded mario-smert.mp3" << std::endl;
      data->deathSound.setBuffer(Resources::sounds["mario-smert.mp3"]); // Устанавливаем буфер для deathSound
  } else {
      // Удаляем std::cerr << "Error: mario-smert.mp3 not found in Resources::sounds" << std::endl;
  }

  data->music.openFromFile("./resources/sounds/music.ogg");
  data->music.setLoop(true);
  data->music.setVolume(25);

  // data->font.loadFromFile("./resources/font.ttf"); // Закомментируем старую строку
  if (!data->font.loadFromFile("./resources/font.ttf")) {
      // Удаляем std::cerr << "Error: Failed to load font from ./resources/font.ttf" << std::endl;
      // Можно добавить какую-то обработку ошибки, например, завершение игры или использование шрифта по умолчанию
  }

  data->coinsText.setFillColor(sf::Color::White);
  data->coinsText.setOutlineColor(sf::Color::Black);
  data->coinsText.setOutlineThickness(1.0f);
  data->coinsText.setScale(0.1f, 0.1f);

  data->scoreText.setFont(data->font);
  data->scoreText.setFillColor(sf::Color::White);
  data->scoreText.setOutlineColor(sf::Color::Black);
  data->scoreText.setOutlineThickness(1.0f);
  data->scoreText.setScale(0.1f, 0.1f);

  data->livesText.setFont(data->font);
  data->livesText.setFillColor(sf::Color::White);
  data->livesText.setOutlineColor(sf::Color::Black);
  data->livesText.setOutlineThickness(1.0f);
  data->livesText.setScale(0.1f, 0.1f);

  data->timeText.setFont(data->font);
  data->timeText.setFillColor(sf::Color::White);
  data->timeText.setOutlineColor(sf::Color::Black);
  data->timeText.setOutlineThickness(1.0f);
  data->timeText.setScale(0.1f, 0.1f);

  data->backgroundShape.setFillColor(sf::Color(0, 0, 0, 150));
  data->backgroundShape.setOrigin(0.5f, 0.5f);

  data->mapImage.loadFromFile("./resources/level11.png");

  Restart();
}

void Update(float deltaTime) {
  // Если Марио мертв и игра еще не окончена, вызываем рестарт (или Game Over)
  if (data->mario.isDead && !data->isGameOver) // Добавляем проверку !data->isGameOver
    Restart();

  if (data->paused)
    return;

  // Если игра окончена, пропускаем остальную логику обновления
  if (data->isGameOver) {
      // Здесь можно добавить логику для Game Over экрана (например, проверка ввода для рестарта/выхода)
      return;
  }

  // Увеличиваем время
  data->timeElapsed += deltaTime;

  Physics::Update(deltaTime, 16, 6);

  data->mario.Update(deltaTime);
  data->camera.position = data->mario.position;

  // Проверяем, нужно ли разрушить блок
  if (data->mario.shouldBreakBlock) {
    // Вызываем метод разрушения блока у карты (меняем текстуру)
    data->map.BreakBlock(data->mario.blockToBreakX, data->mario.blockToBreakY, &Resources::textures["nomoney.png"]);
    
    // Изменяем тип физического тела блока на статический, чтобы он остался на месте, но не реагировал на физику
    if (data->mario.bodyToBreak) {
        data->mario.bodyToBreak->SetType(b2::BodyType::staticBody);

        // Находим фикстуру блока и помечаем ее как неразрушаемую
        b2::Fixture* blockFixture = data->mario.bodyToBreak->GetFixtureList(); // Предполагаем, что у блока одна фикстура
        if (blockFixture) {
            FixtureData* fixtureData = static_cast<FixtureData*>(blockFixture->GetUserData());
            if (fixtureData) {
                fixtureData->isBreakable = false; // Блок больше не разрушаемый
            }
        }
    }

    // Создаем новую монетку в позиции разрушенного блока
    Coin* newCoin = new Coin(&data->mario); // Pass Mario pointer
    newCoin->position = sf::Vector2f(
        data->mario.blockToBreakX * data->map.cellSize + data->map.cellSize / 2.0f,
        data->mario.blockToBreakY * data->map.cellSize + data->map.cellSize / 2.0f);
    newCoin->initialPosition = newCoin->position; // Сохраняем начальную позицию
    data->objects.push_back(newCoin);
    newCoin->Begin(); // Инициализируем монетку (создает физическое тело и запускает анимацию)

    // Даем монетке начальную скорость вверх и уменьшаем гравитацию
    if (newCoin->body) { // Убеждаемся, что физическое тело создано
        newCoin->body->SetLinearVelocity(b2::Vec2(0.0f, -7.0f)); // Уменьшаем скорость вверх
        newCoin->body->SetGravityScale(0.5f); // Оставляем уменьшенную гравитацию для плавности
    }

    // Сбрасываем флаг и поля у Марио
    data->mario.shouldBreakBlock = false;
    data->mario.blockToBreakX = -1;
    data->mario.blockToBreakY = -1;
    data->mario.bodyToBreak = nullptr;
  }

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
  // Если игра окончена, отображаем Game Over экран
  if (data->isGameOver) {
      renderer.target.clear(sf::Color::Black); // Очищаем экран

      // !!! Сохраняем текущий вид и устанавливаем вид по умолчанию для отрисовки UI !!!
      sf::View originalView = renderer.target.getView();
      renderer.target.setView(renderer.target.getDefaultView());

      // Выводим текстовое сообщение GAME OVER
      sf::Text gameOverText("GAME OVER", data->font, 36); // Размер шрифта
      gameOverText.setFillColor(sf::Color::White);

      // Получаем размеры окна (render target)
      sf::Vector2u windowSize = renderer.target.getSize();

      // Устанавливаем начало координат текста в его центр для правильного позиционирования
      gameOverText.setOrigin(gameOverText.getLocalBounds().width / 2.0f, gameOverText.getLocalBounds().height / 2.0f);

      // Устанавливаем позицию текста в центр окна
      gameOverText.setPosition(windowSize.x / 2.0f, windowSize.y / 2.0f);

      renderer.target.draw(gameOverText);

      // !!! Восстанавливаем оригинальный вид !!!
      renderer.target.setView(originalView);

      return; // Пропускаем обычную отрисовку уровня и объектов
  }

  // Обычная отрисовка игры
  renderer.Draw(Resources::textures["sky.png"], data->camera.position,
                data->camera.GetViewSize());

  data->map.Draw(renderer);

  for (auto &object : data->objects) {
    object->Render(renderer);
  }

  data->mario.Draw(renderer);

  Physics::DebugDraw(renderer);
}

void RenderUI(Renderer &renderer) {
  // Не отображаем UI, если игра окончена
  if (data->isGameOver) {
      return;
  }

  sf::Vector2f viewSize = data->camera.GetViewSize();
  // Начальная позиция UI блоков (верхний край видимой области камеры)
  sf::Vector2f uiTopLeft = -viewSize / 2.0f + sf::Vector2f(1.0f, 1.0f); // Небольшой отступ от края

  // Позиции для каждого элемента (центр секции)
  float horizontalPadding = 2.0f; // Добавляем горизонтальный отступ
  float sectionWidth = (viewSize.x - horizontalPadding * 5) / 4.0f; // Учитываем отступы между секциями и по краям

  float scoreX = uiTopLeft.x + horizontalPadding + sectionWidth * 0.5f;
  float coinX = uiTopLeft.x + horizontalPadding * 2 + sectionWidth * 1.5f;
  float timeX = uiTopLeft.x + horizontalPadding * 3 + sectionWidth * 2.5f;
  float livesX = uiTopLeft.x + horizontalPadding * 4 + sectionWidth * 3.5f;

  float verticalOffset = 2.5f; // Увеличиваем отступ для значений под надписями

  // Отображение Score
  data->scoreText.setString("SCORE");
  data->scoreText.setOrigin(data->scoreText.getLocalBounds().width / 2.0f, 0.0f); // Выравнивание по центру
  data->scoreText.setPosition(scoreX, uiTopLeft.y);
  renderer.target.draw(data->scoreText);

  data->scoreText.setString(std::to_string(data->score));
  data->scoreText.setOrigin(data->scoreText.getLocalBounds().width / 2.0f, 0.0f); // Выравнивание по центру
  data->scoreText.setPosition(scoreX, uiTopLeft.y + verticalOffset);
  renderer.target.draw(data->scoreText);

  // Отображение Coin
  data->coinsText.setString("COIN");
  data->coinsText.setOrigin(data->coinsText.getLocalBounds().width / 2.0f, 0.0f); // Выравнивание по центру
  data->coinsText.setPosition(coinX, uiTopLeft.y);
  renderer.target.draw(data->coinsText);

  data->coinsText.setString(std::to_string(data->mario.GetCoins()));
  data->coinsText.setOrigin(data->coinsText.getLocalBounds().width / 2.0f, 0.0f); // Выравнивание по центру
  data->coinsText.setPosition(coinX, uiTopLeft.y + verticalOffset);
  renderer.target.draw(data->coinsText);

  // Отображение Time
  data->timeText.setString("TIME");
  data->timeText.setOrigin(data->timeText.getLocalBounds().width / 2.0f, 0.0f); // Выравнивание по центру
  data->timeText.setPosition(timeX, uiTopLeft.y);
  renderer.target.draw(data->timeText);

  data->timeText.setString(std::to_string(static_cast<int>(data->timeElapsed)));
  data->timeText.setOrigin(data->timeText.getLocalBounds().width / 2.0f, 0.0f); // Выравнивание по центру
  data->timeText.setPosition(timeX, uiTopLeft.y + verticalOffset);
  renderer.target.draw(data->timeText);

  // Отображение Lives
  data->livesText.setString("LIVES");
  data->livesText.setOrigin(data->livesText.getLocalBounds().width / 2.0f, 0.0f); // Выравнивание по центру
  data->livesText.setPosition(livesX, uiTopLeft.y);
  renderer.target.draw(data->livesText);

  data->livesText.setString(std::to_string(data->lives));
  data->livesText.setOrigin(data->livesText.getLocalBounds().width / 2.0f, 0.0f); // Выравнивание по центру
  data->livesText.setPosition(livesX, uiTopLeft.y + verticalOffset);
  renderer.target.draw(data->livesText);

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

void AddScore(int scoreToAdd) {
    if (data) { // Проверяем, инициализирована ли data
        data->score += scoreToAdd;
    }
}

void End() {
  delete data;
  Resources::textures.clear();
  Resources::sounds.clear();
}
