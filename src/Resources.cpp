#include "Resources.h"

std::unordered_map<std::string, sf::Texture> Resources::textures{};
std::unordered_map<std::string, sf::SoundBuffer> Resources::sounds{};
bool Resources::initialized = false;

void Resources::initialize() {
    if (initialized) return;

    // Загрузка текстур
    sf::Texture texture;
    
    // Загрузка текстур для меню
    if (texture.loadFromFile("./resources/textures/mainmenu.png")) {
        textures["mainmenu.png"] = texture;
    }
    if (texture.loadFromFile("./resources/textures/options.png")) {
        textures["options.png"] = texture;
    }
    if (texture.loadFromFile("./resources/textures/leaderboard.png")) {
        textures["leaderboard.png"] = texture;
    }
    if (texture.loadFromFile("./resources/textures/authorization.png")) {
        textures["authorization.png"] = texture;
    }
    if (texture.loadFromFile("./resources/textures/pause.png")) {
        textures["pause.png"] = texture;
    }
    if (texture.loadFromFile("./resources/textures/victory.png")) {
        textures["victory.png"] = texture;
    }
    
    // Загрузка текстур для Марио
    if (texture.loadFromFile("./resources/textures/run1.png")) {
        textures["run1.png"] = texture;
    }
    if (texture.loadFromFile("./resources/textures/run2.png")) {
        textures["run2.png"] = texture;
    }
    if (texture.loadFromFile("./resources/textures/run3.png")) {
        textures["run3.png"] = texture;
    }
    if (texture.loadFromFile("./resources/textures/idle.png")) {
        textures["idle.png"] = texture;
    }
    if (texture.loadFromFile("./resources/textures/jump.png")) {
        textures["jump.png"] = texture;
    }
    if (texture.loadFromFile("./resources/textures/marioDeath.png")) {
        textures["marioDeath.png"] = texture;
    }
    
    // Загрузка текстур для карты
    if (texture.loadFromFile("./resources/textures/sky.png")) {
        textures["sky.png"] = texture;
    }
    if (texture.loadFromFile("./resources/textures/brick.png")) {
        textures["brick.png"] = texture;
    }
    if (texture.loadFromFile("./resources/textures/kirpich.png")) {
        textures["kirpich.png"] = texture;
    }
    if (texture.loadFromFile("./resources/textures/monetki.png")) {
        textures["monetki.png"] = texture;
    }
    if (texture.loadFromFile("./resources/textures/pile1.png")) {
        textures["pile1.png"] = texture;
    }
    if (texture.loadFromFile("./resources/textures/pile1_2.png")) {
        textures["pile1_2.png"] = texture;
    }
    if (texture.loadFromFile("./resources/textures/pile2.png")) {
        textures["pile2.png"] = texture;
    }
    if (texture.loadFromFile("./resources/textures/pile2_2.png")) {
        textures["pile2_2.png"] = texture;
    }
    if (texture.loadFromFile("./resources/textures/flag.png")) {
        textures["flag.png"] = texture;
    }
    if (texture.loadFromFile("./resources/textures/flaghtock.png")) {
        textures["flaghtock.png"] = texture;
    }
    if (texture.loadFromFile("./resources/textures/nomoney.png")) {
        textures["nomoney.png"] = texture;
    }
    
    // Загрузка текстур для врагов
    if (texture.loadFromFile("./resources/textures/enemy1.png")) {
        textures["enemy1.png"] = texture;
    }
    if (texture.loadFromFile("./resources/textures/enemy2.png")) {
        textures["enemy2.png"] = texture;
    }
    
    // Загрузка текстур для монет
    for (int i = 4; i <= 14; i++) {
        std::string filename = "./resources/textures/coin" + std::to_string(i) + ".png";
        if (texture.loadFromFile(filename)) {
            textures["coin" + std::to_string(i) + ".png"] = texture;
        }
    }
    
    // Загрузка звуков
    sf::SoundBuffer sound;
    if (sound.loadFromFile("./resources/sounds/jump.wav")) {
        sounds["jump.wav"] = sound;
    }

    initialized = true;
}

sf::Texture& Resources::getTexture(const std::string& name) {
    if (!initialized) {
        throw std::runtime_error("Resources not initialized");
    }
    return textures[name];
}

sf::SoundBuffer& Resources::getSound(const std::string& name) {
    if (!initialized) {
        throw std::runtime_error("Resources not initialized");
    }
    return sounds[name];
}

void Resources::clear() {
    textures.clear();
    sounds.clear();
    initialized = false;
}
