#include "Circle.hpp"
#include "main.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace objects;

Circle::Circle(glm::vec3 color, Shader& shaderProgram){
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
    unsigned int viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(trans));

    vao.Unbind();
    vbo.Unbind();
}

void Circle::Draw(Shader& shaderProgram){
    unsigned int viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(trans));

    vao.Bind();
    glDrawArrays(GL_TRIANGLE_FAN, 0, precision);
    vao.Unbind();
}

//void Delete(){
//   vao.Delete();
//   vbo.Delete();
//}


void Circle::Translate(glm::vec3 transVect){
    trans = glm::translate(trans, transVect); 
    positionPixel = glm::vec2(positionPixel.x+transVect.x,positionPixel.y+transVect.y); 
}

void Circle::SetPixelPosition(glm::vec2 newPosition){
    //TODO: make this set physics position based on given pixel coords
}

void Circle::Update(double time){
    gravAcceleration += gravForce/mass;
    velocity += (acceleration+gravAcceleration) * time;
    positionReal += velocity * time;

    //convert real position to pixel position
    positionPixel = glm::vec<2, double>(positionReal.x*pixelsPerAU+windowWidth/2, positionReal.y*pixelsPerAU+windowHeight/2);
    trans = glm::mat4(1.0f);    //reset the translation matrix
    //convert pixel coords to the ones between -1 and 1
    trans = glm::translate(trans,glm::vec3((positionPixel.x-windowWidth/2)/(windowWidth/2),(positionPixel.y-windowHeight/2)/(windowHeight/2),0.0f));
    trans = glm::translate(trans,cameraPos);

    //update size based on mass
    //size = sqrt(mass);
    trans = glm::scale(trans, glm::vec3(size));  
}

Circle::~Circle(){
    vao.Delete();
    vbo.Delete();
}
