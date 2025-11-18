#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "shaderClass.h"
#include "VBO.h"
#include "VAO.h"
#include "EBO.h"

class Circle{
private:
    const static int precision = 100; //amount of triangles, more equals more circle looking shape
    VAO vao;
    VBO vbo; // its lowercase for readability (the class has the same name)
    glm::mat4 trans = glm::mat4(1.0f); //transformation matrix set to be an identity matrix (i.e. does nothing)
    GLfloat circleVertices[precision*6];

public:
    float posX = 0.0f; 
    float posY = 0.0f;

    Circle(glm::vec3 color, Shader& shaderProgram){
        //set up vertex array   
        float angle;
        float radius = 0.2f;

        //the vertices' positions are calculated using sin and cos
        for (int i=0; i<precision; i++){
            angle=(2*M_PI*i)/precision;
            circleVertices[i*6] = radius*cos(angle) + posX; 
            circleVertices[i*6+1] = radius*sin(angle) + posY;
            circleVertices[i*6+2] = 1.0f;
            circleVertices[i*6+3] = color.x;
            circleVertices[i*6+4] = color.y;
            circleVertices[i*6+5] = color.z;
        }
        
        //set up buffers
        vao.Bind();
        vbo.AssignValue(circleVertices, sizeof(circleVertices));
        vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, 6*sizeof(float), (void*)0); //position
        vao.LinkAttrib(vbo, 1, 3, GL_FLOAT, 6*sizeof(float), (void*)(3*sizeof(float))); //color

        //put the identity matrix into the uniform so the ball actually renders
        shaderProgram.Activate();
        unsigned int transformLoc = glGetUniformLocation(shaderProgram.ID, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

        vao.Unbind();
        vbo.Unbind();
    }

    void Draw(Shader& shaderProgram){
        shaderProgram.Activate();
        unsigned int transformLoc = glGetUniformLocation(shaderProgram.ID, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

        vao.Bind();
        glDrawArrays(GL_TRIANGLE_FAN, 0, precision);
        vao.Unbind();
    }

    void Delete(){
        vao.Delete();
        vbo.Delete();
    }

    void Translate(glm::vec3 transVect){
        trans = glm::translate(trans, transVect); 
    }
};

// Vertices coordinates
GLfloat vertices[] =
{//     COORDINATES             COLORS     
    -0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,// Lower left corner
    -0.5f, 0.5f, 0.0f,    0.0f, 0.0f, 1.0f,// Upper left corner
    0.5f, 0.5f, 0.0f,     1.0f, 1.0f, 1.0f,// Upper right corner
    0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f// Lower right corner
};

// Indices for vertices order
GLuint indices[] =
{
    0, 2, 1, 
    0, 3, 2
};

int main(){

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    //initialization
    glfwInit();

    //tell glfw which version of opengl we using (in this case 3.3)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //tell glfw we are only using the CORE profile, so we only have the modern functions
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //create window with parameters width, height, name, fullscreen, some bullshit
    GLFWwindow* window =glfwCreateWindow(800,800,"Lorem Ipsum",NULL,NULL);
    //error checking
    if (window == NULL){
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    //introduce the window to the current context (make it active)
    glfwMakeContextCurrent(window);

    gladLoadGL(); //load GLAD so it configures

    glViewport(0,0,800,800); //tells opengl the area of the window to render in

    //make a shader program using the shader files
    Shader shaderProgram("shaders/default.vert","shaders/default.frag");    

    //make and bind vao
    VAO VAO1;
    VAO1.Bind();

    //make vbo and ebo and link them to vertices
    VBO VBO1;
    VBO1.AssignValue(vertices, sizeof(vertices));
    EBO EBO1(indices, sizeof(indices));

    //link vbo attriutes like coords and color to the vao
    //the n in n*sizeof(float) represents the amount of parameters within the vertex
    VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 6*sizeof(float), (void*)0);
    VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 6*sizeof(float), (void*)(3*sizeof(float)));
    
    //take a wild fucking guess
    VAO1.Unbind();
    VBO1.Unbind();  
    EBO1.Unbind();

    glm::vec3 red = glm::vec3(1.0f,0.0f,0.0f);
    glm::vec3 green = glm::vec3(0.0f,1.0f,0.0f);
    Circle Circle1(red,shaderProgram);
    Circle Circle2(green,shaderProgram);

    glm::vec3 vect(0.5f,0.0f,0.0f);
    Circle1.Translate(vect);
    Circle2.Translate(-vect);

    //main loop (while the window is active)
    while(!glfwWindowShouldClose(window)){
        //set bg color
        glClearColor(0.07f,0.13f,0.17f,1.0f);   
        glClear(GL_COLOR_BUFFER_BIT);
        //activate the shader!
        shaderProgram.Activate();

        /*//bind the VAO so opengl knows to use it
        VAO1.Bind();
        //draw
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        VAO1.Unbind();*/

        Circle1.Draw(shaderProgram);
        Circle2.Draw(shaderProgram);

        //swap buffers (apply changes)
        glfwSwapBuffers(window);
        glfwPollEvents(); // function that processes events (like resizing and moving the window)
    }

    //delete all created objects to keep it clean
    VAO1.Delete();
    VBO1.Delete();
    EBO1.Delete();
    Circle1.Delete();
    shaderProgram.Delete();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
