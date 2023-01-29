#version 330 core
layout(triangles) in; //输入的是三角形
layout(triangle_strip, max_vertices = 3) out;//输出的也是三角形图元 并且最大定点数是3

in VS_OUT{ /*创建传入值*/
	vec2 texCoords;//纹理坐标
}gs_in[];
out vec2 TexCoords;//传出值纹理坐标
uniform float time;//从外面获取实际的uniform值

vec4 explode(vec4 position, vec3 normal) /*爆炸函数 传入片段的点位置 和片段的法向量*/
{
	float magnitude = 2.0;
	vec3 direction = normal * ((sin(time)+1.0)/2.0) * magnitude;//往外爆炸的方向距离
	return position + vec4(direction,0.0);
}

vec3 GetNormal() /*获取法向量的函数*/
{
	vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
	vec3 b = vec3(gl_in[1].gl_Position) - vec3(gl_in[2].gl_Position);
	return normalize(cross(a, b));  //返回叉乘之后的两个向量 并且单位化 从而得到法向量的方向
}

void main()
{
	vec3 normal = GetNormal();
	gl_Position = gl_in[0].gl_Position;  //explode(gl_in[0].gl_Position,normal);
	TexCoords = gs_in[0].texCoords;
	EmitVertex(); //把顶点向量添加到图元中
	gl_Position = gl_in[1].gl_Position;  //explode(gl_in[1].gl_Position,normal);
	TexCoords = gs_in[1].texCoords;
	EmitVertex();
	gl_Position = gl_in[2].gl_Position;  //explode(gl_in[2].gl_Position,normal);
	TexCoords = gs_in[2].texCoords;
	EmitVertex();
	EndPrimitive(); //把所有顶点合成为指定图元
}