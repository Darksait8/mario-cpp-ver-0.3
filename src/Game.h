#pragma once
#include "Camera.h"
#include "Object.h"
#include "Renderer.h"
#include "Map.h"
#include "Mario.h"
#include "GameMenu.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <map>

Camera &GetCamera();
bool &GetPaused();

void Begin(sf::RenderWindow& window);
void Update(float deltaTime);
void End();
void Render(Renderer &renderer, sf::RenderWindow& window);
void RenderUI(Renderer &renderer);
void HandleEvent(sf::Event event);
void StartNewGame();

void DeleteObject(Object *object);

void AddScore(int scoreToAdd);

sf::String GetLocalizedText(const std::string& key); // Declaration for localized text helper
void UpdateLocalizedStrings(); // Declaration for function to update all localized strings

enum class Language {
    ENGLISH,
    RUSSIAN
};

enum class Difficulty { EASY, NORMAL, HARD };

struct Data {
  Map map{1.0f};
  Camera camera{20.0f};
  Mario mario{};
  std::vector<Object *> objects{};
  sf::Music music{};
  bool paused{};
  GameMenu gameMenu{};

  sf::Image mapImage{};
  sf::Font font;
  sf::Text coinsText;
  sf::Text coinsValueText;
  sf::Text scoreText;
  sf::Text scoreValueText;
  sf::Text livesText;
  sf::Text livesValueText;
  sf::Text timeText;
  sf::Text timeValueText;
  sf::RectangleShape backgroundShape{sf::Vector2f(1.0f, 1.0f)};
  sf::RectangleShape backgroundDimmer;

  sf::Sound deathSound;

  int score = 0;
  int lives = 4;
  float timeElapsed = 0.0f;
  bool isGameOver = false;
  bool isVictory = false;
  bool isInMenu = true;
  bool isMusicOn = true; // Added for music toggle
  bool isGameStarted = false; // Added to track if game has started
  bool resetGame = false; // Added to signal game reset from menu
  int currentLevel = 0;
  float volume = 50.0f;
  float brightness = 1.0f;

  bool hasStartedWalking = false; // New flag to track if Mario has started walking
  float walkingStartTime = 0.0f; // New timer to record when Mario first starts walking

  float victoryScreenTimer = 0.0f; // Timer for victory screen delay
  bool startVictoryScreenTimer = false; // Flag to start the timer

  sf::RenderWindow *window;

  float enemySpeedMultiplier = 1.0f; // <--- Новая переменная для скорости врагов

  Difficulty difficulty = Difficulty::NORMAL;

  // Language currentLanguage = Language::ENGLISH; // Default to English
  // std::map<std::string, sf::String> englishStrings; // Map for English strings
  // std::map<std::string, sf::String> russianStrings; // Map for Russian strings

  // sf::Text languageButtonText; // Text for language toggle button
};

static Data *data;
