#pragma once
// ─────────────────────────────────────────────────────────────────────────────
// Camera.h  –  FPS-style camera with collision / boundary clipping
//
// Coordinate conventions:
//   X – right,  Y – up,  Z – out of screen (OpenGL right-hand)
// ─────────────────────────────────────────────────────────────────────────────

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Movement directions (bit-flags)
enum CameraMovement {
    CAM_FORWARD  = 0,
    CAM_BACKWARD = 1,
    CAM_LEFT     = 2,
    CAM_RIGHT    = 3
};

// Default values
constexpr float CAM_YAW         = -90.0f;  // face -Z by default
constexpr float CAM_PITCH       =   0.0f;
constexpr float CAM_SPEED       =   5.0f;  // units/second
constexpr float CAM_SENSITIVITY =   0.08f;
constexpr float CAM_ZOOM        =  45.0f;

class Camera
{
public:
    // ── State ────────────────────────────────────────────────────────────────
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Yaw;
    float Pitch;
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // ── Room boundaries for collision ────────────────────────────────────────
    // The camera's X and Z positions are clamped inside [min, max].
    glm::vec2 BoundaryMin;   // (xMin, zMin)
    glm::vec2 BoundaryMax;   // (xMax, zMax)
    float     EyeHeight;     // Y is locked to this value (FPS)

    // ── Constructor ──────────────────────────────────────────────────────────
    Camera(glm::vec3 position  = glm::vec3(0.0f, 1.7f, 0.0f),
           glm::vec3 worldUp   = glm::vec3(0.0f, 1.0f, 0.0f),
           float     yaw       = CAM_YAW,
           float     pitch     = CAM_PITCH);

    // ── View matrix ──────────────────────────────────────────────────────────
    glm::mat4 GetViewMatrix() const;

    // ── Input handlers ───────────────────────────────────────────────────────
    // Keyboard movement (deltaTime in seconds)
    void ProcessKeyboard(CameraMovement direction, float deltaTime);

    // Mouse look (xOffset, yOffset in pixels; constrainPitch prevents gimbal)
    void ProcessMouseMovement(float xOffset, float yOffset,
                              bool  constrainPitch = true);

    // Scroll wheel zooms the FOV
    void ProcessMouseScroll(float yOffset);

    // Set room collision box
    void SetBoundaries(glm::vec2 bMin, glm::vec2 bMax, float eyeHeight);

private:
    // Recalculate Front / Right / Up from Yaw and Pitch
    void updateCameraVectors();

    // Clamp position inside the room
    void applyBoundaries();
};
