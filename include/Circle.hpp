#include <glad/glad.h>
#include <glm/glm.hpp>
#include <shaderClass.h>
#include "VBO.h"
#include "VAO.h"

namespace objects{
    
class Circle{
private:
    const static int precision = 20; //amount of triangles, more equals more circle looking shape
    VAO vao;
    VBO vbo; // its lowercase for readability (the class has the same name)
    glm::mat4 trans = glm::mat4(1.0f); //transformation matrix set to be an identity matrix (i.e. does nothing)
    GLfloat circleVertices[precision*6];

public:
    //physics variables
    glm::vec<2, double> positionReal = glm::vec<2, double>(0,0);
    glm::vec<2, double> positionPixel = glm::vec<2, double>(0.0,0.0);
    glm::vec<2, double> acceleration = glm::vec<2, double>(0.0,0.0);
    glm::vec<2, double> velocity = glm::vec<2, double>(0.0,0.0);
    glm::vec<2, double> gravForce = glm::vec<2, double>(0.0,0.0);
    glm::vec<2, double> gravAcceleration = glm::vec<2, double>(0.0,0.0);
    double mass = 0;
    double size = 1; 


    Circle(glm::vec3 color, Shader& shaderProgram);

    //renders the circle using the shader program
    void Draw(Shader& shaderProgram);

    //obsolete
    void Translate(glm::vec3 transVect);

    //todo
    void SetPixelPosition(glm::vec2 newPosition);

    //updates the position and velocity based on the current acceleration and velocity respectively
    void Update(double time);

    ~Circle();
};

}