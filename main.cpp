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

//define physics constants and units
const double solarMass = 1;
const double earthMass = 0.000003003 * solarMass;
const double pixelsPerAU = 100; // 1 pixel = x meters
const double uMass = solarMass; // unit of mass  
const double G = 4*M_PI*M_PI ; // gravitational constant
const double SecondsPerYear = 31556926;

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


    Circle(glm::vec3 color, Shader& shaderProgram){
        //set up vertex array   
        float angle;
        float radius = 0.02f; //base render radius

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
        positionPixel = glm::vec2(positionPixel.x+transVect.x,positionPixel.y+transVect.y); 
    }

    void SetPixelPosition(glm::vec2 newPosition){
        //TODO: make this set physics position based on given pixel coords
    }

    void Update(double time){
        gravAcceleration += gravForce/mass;
        velocity += (acceleration+gravAcceleration) * time;
        positionReal += velocity * time;

        //convert real position to pixel position
        positionPixel = glm::vec<2, double>(positionReal.x*pixelsPerAU+windowWidth/2, positionReal.y*pixelsPerAU+windowHeight/2);
        trans = glm::mat4(1.0f);    //reset the translation matrix
        //convert pixel coords to the ones between -1 and 1
        trans = glm::translate(trans,glm::vec3((positionPixel.x-windowWidth/2)/(windowWidth/2),(positionPixel.y-windowHeight/2)/(windowHeight/2),0.0f));

        //update size based on mass
        //size = sqrt(mass);
        trans = glm::scale(trans, glm::vec3(size));  
    }
};

void Gravity(Circle& circle1, Circle& circle2, double time){
    glm::vec<2, double> r = circle2.positionReal - circle1.positionReal;
    glm::vec<2, double> unit_r = r / glm::length(r);
    glm::vec<2, double> F21 = (-G * circle1.mass * circle2.mass * unit_r)/(double)pow(glm::length(r),2);

    //std::cout << glm::length(r) << std::endl;
    //std::cout << unit_r.x << unit_r.y << std::endl;
    //std::cout << "Force applied to circle 2: [" << F21.x << "," << F21.y << "]" << std::endl;

    circle2.gravForce += F21;
    //circle1.gravForce -= F21;
}

void GravForceReset(Circle& Circle){
    Circle.gravForce=glm::vec<2, double>(0);
    Circle.gravAcceleration=glm::vec<2, double>(0);
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
    glm::vec3 blue = glm::vec3(0.0f,0.0f,1.0f);
    Circle Circles[2] = {Circle(red,shaderProgram), Circle(green,shaderProgram)};
    //Circle Circles[3] = {Circle(red,shaderProgram), Circle(green,shaderProgram), Circle(blue,shaderProgram)};
    const static uint nCircles = sizeof(Circles)/sizeof(Circles[0]);

    Circles[0].positionReal = glm::vec2(0,0);
    Circles[1].positionReal = glm::vec2(1,0);
    Circles[0].mass = solarMass;
    Circles[1].mass = solarMass*3;
    //Circles[0].acceleration.x = 16;
    double V = 2*M_PI; 
    Circles[1].velocity.y = V;

    //Circles[0].velocity.x=3;
    //Circles[1].velocity.x=3;

    /*Circles[2].mass = 3;
    Circles[2].positionReal = glm::vec2(-2,1);
    Circles[2].velocity.y = -1; 
    Circles[2].velocity.x = -1;*/  
    
    double lastTime = glfwGetTime();
    double speed = 3600*24*30;
    //double speed = 1;
    double totalYears = 0;

    //main loop (while the window is active)
    while(!glfwWindowShouldClose(window)){
        glfwGetFramebufferSize(window,&windowWidth,&windowHeight);
        glViewport((windowWidth-windowHeight)/2,0,windowHeight,windowHeight);
        //set bg color
        glClearColor(0.07f,0.13f,0.17f,1.0f);   
        glClear(GL_COLOR_BUFFER_BIT);
        //activate the shader!
        shaderProgram.Activate();

        double now = glfwGetTime();
        double dt_sec = now - lastTime;
        double speed_dt_sec = dt_sec * speed; 
        double speed_dt_years = speed_dt_sec / SecondsPerYear;
        totalYears += speed_dt_years; 
        lastTime = now;

        std::cout << "Total years passed: " << totalYears << std::endl;
        //std::cout << Circles[1].positionReal.x << " " << Circles[1].positionReal.y << std::endl;


        //forgive me Omnissiah
        for (int i=0; i<nCircles; i++){
            for (int j=nCircles; j>=0, j--;){
                if (i!=j) Gravity(Circles[i],Circles[j],speed_dt_years);
            }
        }

        //Gravity(Circles[0],Circles[1],speed_dt_years);
        //Gravity(Circles[1],Circles[2],speed_dt_years);
        //Gravity(Circles[0],Circles[2],speed_dt_years);

    
        for (int i=0; i<nCircles; i++){
            Circles[i].Update(speed_dt_years);
            //if (windowWidth >= 1000) {
            //    std::cout << "dayum brug" << std::endl;
            //}
            Circles[i].Draw(shaderProgram);
        }

        //std::cout << Circles[1].positionReal.x << " " << Circles[1].positionReal.y << " " << dt_sec << std::endl;

        //reset forces so they dont overlap eachother or do weird shit
        for (int i=0; i<nCircles; i++){
            GravForceReset(Circles[i]);    
        }

        //swap buffers (apply changes)
        glfwSwapBuffers(window);
        glfwPollEvents(); // function that processes events (like resizing and moving the window)
    }

    //delete all created objects to keep it clean

    for (int i=0; i<nCircles; i++){
        Circles[i].Delete();
    }
    shaderProgram.Delete();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
