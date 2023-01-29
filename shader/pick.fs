#version 330 core
out vec3 FragColor;
uniform int gDrawIndex;  /*将信息存储进帧缓冲*/
uniform int gobjectIndex;
void main()
{
    FragColor = vec3(gobjectIndex, gDrawIndex, gl_PrimitiveID + 1);
    //FragColor = vec3(2.0, 2.0, 3.0);
}