#version 330 core
layout(triangles) in; //�������������
layout(triangle_strip, max_vertices = 3) out;//�����Ҳ��������ͼԪ ������󶨵�����3

in VS_OUT{ /*��������ֵ*/
	vec2 texCoords;//��������
}gs_in[];
out vec2 TexCoords;//����ֵ��������
uniform float time;//�������ȡʵ�ʵ�uniformֵ

vec4 explode(vec4 position, vec3 normal) /*��ը���� ����Ƭ�εĵ�λ�� ��Ƭ�εķ�����*/
{
	float magnitude = 2.0;
	vec3 direction = normal * ((sin(time)+1.0)/2.0) * magnitude;//���ⱬը�ķ������
	return position + vec4(direction,0.0);
}

vec3 GetNormal() /*��ȡ�������ĺ���*/
{
	vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
	vec3 b = vec3(gl_in[1].gl_Position) - vec3(gl_in[2].gl_Position);
	return normalize(cross(a, b));  //���ز��֮����������� ���ҵ�λ�� �Ӷ��õ��������ķ���
}

void main()
{
	vec3 normal = GetNormal();
	gl_Position = gl_in[0].gl_Position;  //explode(gl_in[0].gl_Position,normal);
	TexCoords = gs_in[0].texCoords;
	EmitVertex(); //�Ѷ���������ӵ�ͼԪ��
	gl_Position = gl_in[1].gl_Position;  //explode(gl_in[1].gl_Position,normal);
	TexCoords = gs_in[1].texCoords;
	EmitVertex();
	gl_Position = gl_in[2].gl_Position;  //explode(gl_in[2].gl_Position,normal);
	TexCoords = gs_in[2].texCoords;
	EmitVertex();
	EndPrimitive(); //�����ж���ϳ�Ϊָ��ͼԪ
}