#include "actor.h"
#include "../utilities/helper.h"
#include "renderer.h"

#include "../vendor/glm/glm.hpp"

#if !__APPLE__
    #include "../vendor/SDL2/SDL.h"
    #include "../vendor/SDL2_image/SDL_image.h"
#else
    #include <SDL2/SDL.h>
    #include <SDL2_image/SDL_image.h>
#endif

#include <cmath>
#include <string>
#include <cstdlib>
#include <optional>

Actor::Actor(
	bool doesBounce,
	bool isBlocking,
	float rotationInDegrees,
	std::optional<int> renderOrder,
	glm::fvec2 position,
	glm::fvec2 scale,
	glm::fvec2 center,
	glm::fvec2 velocity,
	std::string nearbyDialogue,
	std::string contactDialogue,
	std::string name,
	std::string sprite, std::string backSprite) :
	m_doesBounce(doesBounce), m_facingUp(false),
	m_isBlocking(isBlocking), m_uuid(m_nextUuid++),
	m_rotationInDegrees(rotationInDegrees), m_renderOrder(renderOrder),
	m_position(position),
	m_scale(scale), m_center(center), m_velocity(velocity),
	m_nearbyDialogue(nearbyDialogue), m_contactDialogue(contactDialogue),
	m_name(name), m_sprite(sprite), m_backSprite(backSprite)
{
	// Set flip status.
	if (m_scale.x < 0.0f && m_scale.y < 0.0f)
	{
		m_flip = static_cast<SDL_RendererFlip>(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);
	}
	else if (m_scale.x < 0.0f)
	{
		m_flip = static_cast<SDL_RendererFlip>(SDL_FLIP_HORIZONTAL);
	}
	else if (m_scale.y < 0.0f)
	{
		m_flip = static_cast<SDL_RendererFlip>(SDL_FLIP_VERTICAL);
	}
	else
	{
		m_flip = static_cast<SDL_RendererFlip>(SDL_FLIP_NONE);
	}

	// Get texture.
	std::string filePrefix = "resources/images/";
	std::string fileSuffix = ".png";
	std::string spritePath = filePrefix + m_sprite + fileSuffix;
	std::string backSpritePath = filePrefix + m_backSprite + fileSuffix;

	m_texture = IMG_LoadTexture(Renderer::getRenderer(), spritePath.c_str());
	m_backTexture = IMG_LoadTexture(Renderer::getRenderer(), backSpritePath.c_str());

	SDL_QueryTexture(m_texture, nullptr, nullptr, &m_textureWidth, &m_textureHeight);
	SDL_QueryTexture(m_backTexture, nullptr, nullptr, &m_backTextureWidth, &m_backTextureHeight);

	//m_spritePixelDimensions = { m_textureWidth, m_textureHeight };

	m_pivotPoint.x = static_cast<int>(m_center.x * std::abs(m_scale.x));
	m_pivotPoint.y = static_cast<int>(m_center.y * std::abs(m_scale.y));

	m_destRectangleView.w = static_cast<int>(m_textureWidth * std::abs(m_scale.x));
	m_destRectangleView.h = static_cast<int>(m_textureHeight * std::abs(m_scale.y));

	calculateViewRectangleDimensions();

	/*m_srcRectangleView = { static_cast<int>(m_position.x), static_cast<int>(m_position.y),
		m_spritePixelDimensions.x, m_spritePixelDimensions.y };*/
}

const void Actor::calculateViewRectangleDimensions()
{
	glm::fvec2 offset;
	
	// Only apply bounce if specified in JSON file.
	if (!m_doesBounce)
	{
		offset = { 0.0f, 0.0f };
	}
	else
	{
		offset = glm::fvec2(0.0f, -glm::abs(glm::sin(Helper::GetFrameNumber() * 0.15f)) * 10.0f);
	}

	m_destRectangleView.x = static_cast<int>(-m_pivotPoint.x + (Renderer::getWindowXResolution() * 0.5f) + (m_position.x * 100.0f) + offset.x);
	m_destRectangleView.y = static_cast<int>(-m_pivotPoint.y + (Renderer::getWindowYResolution() * 0.5f) + (m_position.y * 100.0f) + offset.y);
}
