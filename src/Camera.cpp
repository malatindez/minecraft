#include "Camera.h"
Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)),
      MovementSpeed(SPEED),
      MouseSensitivity(SENSITIVITY),
      Zoom(ZOOM) {
  Position = position;
  WorldUp = up;
  Yaw = yaw;
  Pitch = pitch;
  updateCameraVectors();
}
// Constructor with scalar values
Camera::Camera(float posX, float posY, float posZ, float upX, float upY,
               float upZ, float yaw, float pitch)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)),
      MovementSpeed(SPEED),
      MouseSensitivity(SENSITIVITY),
      Zoom(ZOOM) {
  Position = glm::vec3(posX, posY, posZ);
  WorldUp = glm::vec3(upX, upY, upZ);
  Yaw = yaw;
  Pitch = pitch;
  updateCameraVectors();
}

// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 Camera::GetViewMatrix() {
  return glm::lookAt(Position, Position + Front, Up);
}

// Processes input received from any keyboard-like input system. Accepts input
// parameter in the form of camera defined ENUM (to abstract it from windowing
// systems)
void Camera::ProcessKeyboard(Movement direction, float deltaTime) {
  auto prevPosition = Position;
  float velocity = (float)MovementSpeed * deltaTime;
  if (direction == Movement::FORWARD)
    Position += glm::vec3(Front.x / cos(glm::radians(Pitch)), 0,
                          Front.z / cos(glm::radians(Pitch))) *
                velocity;
  if (direction == Movement::BACKWARD)
    Position -= glm::vec3(Front.x / cos(glm::radians(Pitch)), 0,
                          Front.z / cos(glm::radians(Pitch))) *
                velocity;
  if (direction == Movement::LEFT) Position -= Right * velocity;
  if (direction == Movement::RIGHT) Position += Right * velocity;
  if (direction == Movement::DOWN) Position -= glm::vec3(0, 1, 0) * velocity;
  if (direction == Movement::UP) Position += glm::vec3(0, 1, 0) * velocity;
}

// Processes input received from a mouse input system. Expects the offset value
// in both the x and y direction.
void Camera::ProcessMouseMovement(double xoffset, double yoffset,
                                  GLboolean constrainPitch) {
  xoffset *= MouseSensitivity * Zoom / ZOOM;
  yoffset *= MouseSensitivity * Zoom / ZOOM;

  Yaw += xoffset;
  Pitch += yoffset;

  // Make sure that when pitch is out of bounds, screen doesn't get flipped
  if (constrainPitch) {
    if (Pitch > 89.9f) Pitch = 89.9f;
    if (Pitch < -89.9f) Pitch = -89.9f;
  }

  // Update Front, Right and Up Vectors using the updated Euler angles
  updateCameraVectors();
}

// Processes input received from a mouse scroll-wheel event. Only requires input
// on the vertical wheel-axis
void Camera::ProcessMouseScroll(float yoffset) {
  if (Zoom >= 1.0f && Zoom <= ZOOM) Zoom -= yoffset;
  if (Zoom <= 1.0f) Zoom = 1.0f;
  if (Zoom >= ZOOM) Zoom = ZOOM;
}
void Camera::updateCameraVectors() {
  // Calculate the new Front vector
  glm::vec3 front;
  front.x = cos(glm::radians((float)Yaw)) * cos(glm::radians((float)Pitch));
  front.y = sin(glm::radians((float)Pitch));
  front.z = sin(glm::radians((float)Yaw)) * cos(glm::radians((float)Pitch));
  Front = glm::normalize(front);
  // Also re-calculate the Right and Up vector
  Right = glm::normalize(glm::cross(
      Front, WorldUp));  // Normalize the vectors, because their length gets
                         // closer to 0 the more you look up or down which
                         // results in slower movement.
  Up = glm::normalize(glm::cross(Right, Front));
}