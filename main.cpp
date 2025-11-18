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

    glm::vec3 red = glm::vec3(1.0f,0.0f,0.0f);
    glm::vec3 green = glm::vec3(0.0f,1.0f,0.0f);
    Circle Circles[2] = {Circle(red,shaderProgram), Circle(green,shaderProgram)};

    glm::vec3 vect(0.5f,0.0f,0.0f);
    Circles[0].Translate(vect);
    Circles[1].Translate(-vect);

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

        for (int i=0; i<2; i++){
            Circles[i].Draw(shaderProgram);
        }
        
        //swap buffers (apply changes)
        glfwSwapBuffers(window);
        glfwPollEvents(); // function that processes events (like resizing and moving the window)
    }

    //delete all created objects to keep it clean

    for (int i=0; i<2; i++){
        Circles[i].Delete();
    }
    shaderProgram.Delete();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
