#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "Renderer.h"
#include "Object.h"

class Map
{
public:
	Map(float cellSize = 32.0f);

	void CreateCheckerboard(size_t width, size_t height);
	sf::Vector2f CreateFromImage(const sf::Image& image, std::vector<Object*>& objects, class Mario* mario);

	void Draw(Renderer& renderer);

	// Метод для разрушения блока
	void BreakBlock(int x, int y, sf::Texture* newTexture);

	// Удаляем объявление RemoveBlock, если оно было добавлено как статическое
	// static void RemoveBlock(int x, int y);

	// Убеждаемся, что grid не статическая
	std::vector<std::vector<sf::Texture*>> grid;
	float cellSize;
};
