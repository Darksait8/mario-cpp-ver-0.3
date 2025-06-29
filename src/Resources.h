#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <unordered_map>

class Resources
{
public:
	static void initialize();
	static sf::Texture& getTexture(const std::string& name);
	static sf::SoundBuffer& getSound(const std::string& name);
	static void clear();

private:
	static std::unordered_map<std::string, sf::Texture> textures;
	static std::unordered_map<std::string, sf::SoundBuffer> sounds;
	static bool initialized;
};
