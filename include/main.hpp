#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace objects { class Circle; }

extern GLsizei windowWidth;
extern GLsizei windowHeight;

extern const double solarMass;
extern const double earthMass;
extern double pixelsPerAU; //modifiable for zoom purposes
extern const double G; // gravitational constant
extern const double SecondsPerYear;
extern double speed;

extern glm::vec3 cameraPos;
extern glm::vec3 cameraUp;

void Gravity(objects::Circle& circle1, objects::Circle& circle2, double time);
void GravForceReset(objects::Circle& Circle);
void processInput(GLFWwindow *window, double deltaTime);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

