#pragma once
#include <string>
#include <vector>
#include <functional>
#include <fstream> // For file operations
#include <algorithm> // For std::fill
#include <map>

struct Data; // Предварительное объявление Data для избежания циклической зависимости

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

enum class GameMenuState {
    MAIN_MENU,
    DIFFICULTY_SELECT,
    OPTIONS,
    CREDITS,
    AUTH,
    PAUSE,
    VICTORY,
    EXIT
};

enum class InputField {
    NONE,
    USERNAME,
    PASSWORD
};

enum class LeaderboardDisplayType {
    SCORE,
    FASTEST_TIME,
    MOST_COINS
};

// User structure for binary file storage
struct User {
    char username[50]; // Fixed-size array for simplicity in binary file
    char passwordHash[50]; // Storing a simple hash of the password
    int score;
    int coins; // Add coins member
    float fastestTime; // Add fastest time
    int mostCoins; // Add most coins

    // Default constructor for proper initialization
    User() : score(0), coins(0), fastestTime(0.0f), mostCoins(0) { // Initialize coins to 0
        std::fill(std::begin(username), std::end(username), 0);
        std::fill(std::begin(passwordHash), std::end(passwordHash), 0);
    }
};

class GameMenuItem {
public:
    sf::Text text;
    std::function<void()> action;

    GameMenuItem(const sf::String& textString, sf::Font& font, std::function<void()> action);
    void execute() const;
};

class GameMenu {
public:
    GameMenu();
    void initializeActions(Data* gameData);
    void update();
    void render(sf::RenderWindow& window);
    void handleInput(sf::Event event);
    void addMenuItem(const sf::String& textString, std::function<void()> action);
    void setState(GameMenuState newState);
    GameMenuState getCurrentState() const;

    void saveUsers();
    void renderDifficultySelect(sf::RenderWindow& window);

private:
    std::vector<GameMenuItem> items;
    GameMenuState currentState;
    size_t selectedIndex;
    GameMenuState previousState;

    sf::Sprite backgroundSprite;
    sf::Font font;
    sf::Text titleText;
    sf::Text instructionsText;

    LeaderboardDisplayType currentLeaderboardDisplay;

    // Переменные для полей ввода авторизации
    sf::Text usernameLabel;
    sf::Text passwordLabel;
    sf::RectangleShape usernameInputBox;
    sf::RectangleShape passwordInputBox;
    std::string usernameInput;
    std::string passwordInput;
    InputField activeInputField;

    // Переменные для ползунков опций
    enum class OptionSliderType {
        NONE,
        VOLUME,
        BRIGHTNESS
    };
    OptionSliderType activeSlider;

    sf::Text volumeLabel;
    sf::RectangleShape volumeTrack;
    sf::CircleShape volumeThumb;
    sf::Text volumeValueText;

    sf::Text brightnessLabel;
    sf::RectangleShape brightnessTrack;
    sf::CircleShape brightnessThumb;
    sf::Text brightnessValueText;

    // Переменные для кнопок авторизации
    sf::Text loginButtonText;
    sf::RectangleShape loginButtonRect;
    sf::Text registerButtonText;
    sf::RectangleShape registerButtonRect;

    sf::RectangleShape scoreLeaderboardRect; // Объявление для прямоугольника таблицы лидеров по очкам
    sf::RectangleShape timeLeaderboardRect; // Объявление для прямоугольника таблицы лидеров по времени
    sf::RectangleShape coinsLeaderboardRect; // Объявление для прямоугольника таблицы лидеров по монетам

    // User management related
    std::vector<User> users; // List of all registered users
    std::string loggedInUsername; // Stores the username of the logged-in user
    sf::Text messageText; // For displaying messages to the user

    float tempVolume = 100.0f;
    float tempBrightness = 1.0f;

    void renderMainMenu(sf::RenderWindow& window);
    void renderOptions(sf::RenderWindow& window);
    void renderCredits(sf::RenderWindow& window);
    void renderAuth(sf::RenderWindow& window);
    void renderPauseMenu(sf::RenderWindow& window);
    void renderVictoryScreen(sf::RenderWindow& window);

    // User management functions
public:
    std::string hashPassword(const std::string& password);
    bool registerUser(const std::string& username, const std::string& password);
    bool loginUser(const std::string& username, const std::string& password);
    void loadUsers();

private:
    Data* gameDataRef; // Ссылка на глобальные данные игры

    void initializePauseMenuActions(); // Объявление новой функции для настройки меню паузы
    void initializeOptionsActions(); // Объявление функции для настройки меню опций
    void setupMenuForState(GameMenuState state); // Новая функция для настройки пунктов меню в зависимости от состояния
}; 