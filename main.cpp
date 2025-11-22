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

GLsizei windowWidth = 800;
GLsizei windowHeight = 800; 


class Circle{
private:
    const static int precision = 20; //amount of triangles, more equals more circle looking shape
    VAO vao;
    VBO vbo; // its lowercase for readability (the class has the same name)
    glm::mat4 trans = glm::mat4(1.0f); //transformation matrix set to be an identity matrix (i.e. does nothing)
    GLfloat circleVertices[precision*6];

public:
    //physics variables
    glm::vec2 position = glm::vec2(500.0f,500.0f);
    glm::vec2 acceleration = glm::vec2(0.0f,0.0f);
    glm::vec2 velocity = glm::vec2(0.0f,0.0f);
    glm::vec2 gravForce = glm::vec2(0.0f,0.0f);
    glm::vec2 gravAcceleration = glm::vec2(0.0f,0.0f);
    float mass = 1e12f;


    Circle(glm::vec3 color, Shader& shaderProgram){
        //set up vertex array   
        float angle;
        float radius = 0.02f;

        //the vertices' positions are calculated using sin and cos
        for (int i=0; i<precision; i++){
            angle=(2*M_PI*i)/precision;
            circleVertices[i*6] = radius*cos(angle); 
            circleVertices[i*6+1] = radius*sin(angle);
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
        unsigned int transformLoc = glGetUniformLocation(shaderProgram.ID, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

        vao.Unbind();
        vbo.Unbind();
    }

    void Draw(Shader& shaderProgram){
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
        position = glm::vec2(position.x+transVect.x,position.y+transVect.y); 
    }

    void SetPosition(glm::vec2 newPosition){
        trans = glm::mat4(1.0f);    //reset the translation matrix
        //convert pixel coords to the ones between -1 and 1
        trans = glm::translate(trans,glm::vec3((newPosition.x-windowWidth/2)/(windowWidth/2),(newPosition.y-windowHeight/2)/(windowHeight/2),0.0f));
        position = newPosition;
    }

    void Update(float time){
        gravAcceleration= gravForce/mass;
        velocity += (acceleration+gravAcceleration)*time;
        SetPosition(position+velocity*time);
    }
};

const float G = 6.67430151515e-11;
void Gravity(Circle& circle1, Circle& circle2){
    glm::vec2 r21 = circle2.position - circle1.position;
    glm::vec2 unit_r21 = r21 / glm::length(r21);
    glm::vec2 F21 = (-G * circle1.mass * circle2.mass * unit_r21)/glm::length(r21*r21);

    //std::cout << r21.x << " " << r21.y << std::endl;
    //std::cout << unit_r21.x << unit_r21.y << std::endl;
    //std::cout << "Force applied to circle 2: [" << F21.x << "," << F21.y << "]" << std::endl;

    circle2.gravForce = F21;
    circle1.gravForce = -F21;
}


int main(){
    //initialization
    glfwInit();

    //tell glfw which version of opengl we using (in this case 3.3)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //tell glfw we are only using the CORE profile, so we only have the modern functions
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //create window with parameters width, height, name, fullscreen, some bullshit
    GLFWwindow* window =glfwCreateWindow(windowWidth,windowHeight,"Lorem Ipsum",NULL,NULL);
    //error checking
    if (window == NULL){
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    //introduce the window to the current context (make it active)
    glfwMakeContextCurrent(window);

    gladLoadGL(); //load OpenGL

    glViewport(0,0,windowWidth,windowHeight); //tells opengl the area of the window to render in

    //make a shader program using the shader files
    Shader shaderProgram("shaders/default.vert","shaders/default.frag"); 
    shaderProgram.Activate();   

    glm::vec3 red = glm::vec3(1.0f,0.0f,0.0f);
    glm::vec3 green = glm::vec3(0.0f,1.0f,0.0f);
    Circle Circles[2] = {Circle(red,shaderProgram), Circle(green,shaderProgram)};


    Circles[0].SetPosition(glm::vec2(200,600.0f));
    Circles[0].velocity.y = -2.0f;
    Circles[1].SetPosition(glm::vec2(500,500.0f));
    Circles[1].mass *= 100.0f;

    //Circles[0].acceleration = glm::vec2(1.0f,-1.0f);
    float stime = 0;
    //main loop (while the window is active)
    while(!glfwWindowShouldClose(window)){
        //set bg color
        glClearColor(0.07f,0.13f,0.17f,1.0f);   
        glClear(GL_COLOR_BUFFER_BIT);
        //activate the shader!
        shaderProgram.Activate();

        stime = glfwGetTime() / 10;

        Gravity(Circles[0],Circles[1]);
    
        for (int i=0; i<2; i++){
            Circles[i].Update(stime);
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
