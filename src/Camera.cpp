#include "Camera.h"

Camera::Camera(float zoomLevel)
	: zoomLevel(zoomLevel)
{
}

sf::Vector2f Camera::GetViewSize()
{
	return viewSize;
}

sf::View Camera::GetView(sf::Vector2u windowSize)
{
	float aspect = (float)windowSize.x / (float)windowSize.y;
	if (aspect < 1.0f)
		viewSize = sf::Vector2f(zoomLevel, zoomLevel / aspect);
	else
		viewSize = sf::Vector2f(zoomLevel * aspect, zoomLevel);

	// Clamp camera position within level bounds
	float halfWidth = viewSize.x / 2.0f;
	float halfHeight = viewSize.y / 2.0f;
	
	// Clamp X position (level width is 216)
	position.x = std::max(halfWidth, std::min(216.0f - halfWidth, position.x));
	// Clamp Y position (level height is 30)
	position.y = std::max(halfHeight, std::min(30.0f - halfHeight, position.y));

	return sf::View(position, viewSize);
}

sf::View Camera::GetUIView()
{
	float aspect = viewSize.x / viewSize.y;
	viewSize = sf::Vector2f(100.0f, 100.0f / aspect);
	return sf::View(sf::Vector2f(), viewSize);
}
