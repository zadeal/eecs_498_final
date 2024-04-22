#pragma once

#include "../vendor/glm/glm.hpp"
#include "../vendor/rapidjson/document.h"

const float PLAYER_INITIAL_STARTING_POSITION_X = 0.0f;
const float PLAYER_INITIAL_STARTING_POSITION_Y = 0.0f;

const float CAMERA_DEFAULT_WIDTH = 640.0f;
const float CAMERA_DEFAULT_HEIGHT = 360.0f;

// This class contains camera functionality which includes details such as
// the current position and viewport.
class Camera
{
public:
	Camera();

	const void setCameraPosition(const glm::fvec2& newPosition);
	inline const glm::fvec2& getPosition() const { return m_position; }
	const float getCameraWidthLowerBound() const;
	const float getCameraWidthUpperBound() const;
	const float getCameraHeightLowerBound() const;
	const float getCameraHeightUpperBound() const;
	const void setCameraWidth(float newCameraWidth);
	const void setCameraHeight(float newCameraHeight);
	const glm::fvec2 getRenderOffset() const;

	const glm::fvec2& getCameraOffset() const { return m_cameraOffset; }
	const void setCameraOffset(const glm::fvec2 & newOffset) { m_cameraOffset = newOffset; }

	const void setCameraZoom(float newZoom) { m_cameraZoom = newZoom; }

private:
	float m_cameraZoom;

	float m_cameraWidth;
	float m_cameraHeight;

	glm::fvec2 m_cameraOffset;

	glm::fvec2 m_position;
	glm::fvec2 m_viewport;
};
