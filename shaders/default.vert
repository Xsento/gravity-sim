#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

//output the color for the fragment shader
out vec3 color;

//the view matrix
uniform mat4 view;

void main()
{
   //multiply the position by the transformation matrix
   gl_Position = view * vec4(aPos, 1.0);
   //assigns the color from the vertex data to "color"
   color = aColor;
}