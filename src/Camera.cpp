// ─────────────────────────────────────────────────────────────────────────────
// Camera.cpp  –  FPS camera implementation
// ─────────────────────────────────────────────────────────────────────────────

#include "Camera.h"
#include <algorithm>   // std::clamp

// ── Constructor ───────────────────────────────────────────────────────────────
Camera::Camera(glm::vec3 position, glm::vec3 worldUp, float yaw, float pitch)
    : Position(position)
    , WorldUp(worldUp)
    , Yaw(yaw)
    , Pitch(pitch)
    , Front(glm::vec3(0.0f, 0.0f, -1.0f))
    , MovementSpeed(CAM_SPEED)
    , MouseSensitivity(CAM_SENSITIVITY)
    , Zoom(CAM_ZOOM)
    , BoundaryMin(-5.0f, -7.0f)
    , BoundaryMax( 5.0f,  7.0f)
    , EyeHeight(position.y)
{
    updateCameraVectors();
}

// ── View matrix ───────────────────────────────────────────────────────────────
glm::mat4 Camera::GetViewMatrix() const
{
    // lookAt(eye, target, up)
    return glm::lookAt(Position, Position + Front, Up);
}

// ── Keyboard movement ─────────────────────────────────────────────────────────
void Camera::ProcessKeyboard(CameraMovement direction, float deltaTime)
{
    float velocity = MovementSpeed * deltaTime;

    // Project Front onto the XZ plane so the player doesn't fly up/down
    glm::vec3 flatFront = glm::normalize(glm::vec3(Front.x, 0.0f, Front.z));
    glm::vec3 flatRight = glm::normalize(glm::vec3(Right.x, 0.0f, Right.z));

    switch(direction)
    {
        case CAM_FORWARD:  Position += flatFront *  velocity; break;
        case CAM_BACKWARD: Position += flatFront * -velocity; break;
        case CAM_LEFT:     Position += flatRight * -velocity; break;
        case CAM_RIGHT:    Position += flatRight *  velocity; break;
    }

    // Keep the player at eye level (FPS: no jumping)
    Position.y = EyeHeight;

    // Clamp to room boundaries
    applyBoundaries();
}

// ── Mouse look ────────────────────────────────────────────────────────────────
void Camera::ProcessMouseMovement(float xOffset, float yOffset,
                                  bool constrainPitch)
{
    xOffset *= MouseSensitivity;
    yOffset *= MouseSensitivity;

    Yaw   += xOffset;
    Pitch += yOffset;

    if(constrainPitch) {
        // Clamp pitch: looking straight up/down breaks the look-at
        Pitch = std::clamp(Pitch, -89.0f, 89.0f);
    }

    updateCameraVectors();
}

// ── Scroll zoom ───────────────────────────────────────────────────────────────
void Camera::ProcessMouseScroll(float yOffset)
{
    Zoom = std::clamp(Zoom - yOffset, 10.0f, 90.0f);
}

// ── Set collision bounds ──────────────────────────────────────────────────────
void Camera::SetBoundaries(glm::vec2 bMin, glm::vec2 bMax, float eyeHeight)
{
    BoundaryMin = bMin;
    BoundaryMax = bMax;
    EyeHeight   = eyeHeight;
    Position.y  = eyeHeight;
}

// ── Private: recalculate camera axes ─────────────────────────────────────────
void Camera::updateCameraVectors()
{
    // Spherical to Cartesian conversion
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

    Front = glm::normalize(front);
    // Right is perpendicular to Front in the XZ plane
    Right = glm::normalize(glm::cross(Front, WorldUp));
    // Up is perpendicular to both
    Up    = glm::normalize(glm::cross(Right, Front));
}

// ── Private: clamp to room ────────────────────────────────────────────────────
void Camera::applyBoundaries()
{
    Position.x = std::clamp(Position.x, BoundaryMin.x, BoundaryMax.x);
    Position.z = std::clamp(Position.z, BoundaryMin.y, BoundaryMax.y);
}
