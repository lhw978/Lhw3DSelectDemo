#version 330 core
/*将信息存储进帧缓冲*/
uniform int gDrawIndex;
uniform int gobjectIndex;
out vec3 FragColor;
void main()
{
	FragColor = vec3(gobjectIndex,gDrawIndex,gl_PrimitiveID+1);
	//FragColor = vec3(2.0,2.0,3.0);
}