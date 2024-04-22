#include "camera.h"
#include "renderer.h"
#include "../utilities/engineUtilities.h"

#include "../vendor/glm/glm.hpp"
#include "../vendor/rapidjson/document.h"
#include <iostream>
// The initial camera and player position is (19, 15)
// and the camera's viewport is 13 * 9 in size.
Camera::Camera() : m_cameraWidth(CAMERA_DEFAULT_WIDTH),
m_cameraHeight(CAMERA_DEFAULT_HEIGHT), m_cameraOffset({ 0.0f, 0.0f }),
m_position(PLAYER_INITIAL_STARTING_POSITION_X,
	PLAYER_INITIAL_STARTING_POSITION_Y),
m_viewport(CAMERA_DEFAULT_WIDTH, CAMERA_DEFAULT_HEIGHT),
m_cameraZoom(1.0f)
{}

const void Camera::setCameraPosition(const glm::fvec2& newPosition)
{
	m_position = newPosition * m_cameraZoom;
}

const float Camera::getCameraWidthLowerBound() const
{
	return m_position.x - (m_viewport.x / 2.0f);
}

const float Camera::getCameraWidthUpperBound() const
{
	return m_position.x + (m_viewport.x / 2.0f);
}

const float Camera::getCameraHeightLowerBound() const
{
	return m_position.y - (m_viewport.y / 2.0f);
}

const float Camera::getCameraHeightUpperBound() const
{
	return m_position.y + (m_viewport.y / 2.0f);
}

const void Camera::setCameraWidth(float newCameraWidth)
{
	m_cameraWidth = newCameraWidth;
	m_viewport = glm::fvec2(m_cameraWidth, m_cameraHeight);
}

const void Camera::setCameraHeight(float newCameraHeight)
{
	m_cameraHeight = newCameraHeight;
	m_viewport = glm::fvec2(m_cameraWidth, m_cameraHeight);
}

const glm::fvec2 Camera::getRenderOffset() const
{
	float x = m_position.x * 100.0f - m_viewport.x / 2.0f;
	float y = m_position.y * 100.0f - m_viewport.y / 2.0f;
	return { x, y };
}
