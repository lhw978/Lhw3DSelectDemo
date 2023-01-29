#include"mouse.h"

float lastX; 
float lastY; 
double xposition, yposition;
bool firstMouse = true; //����Ҽ��յ��������Ϊtrue ��ֹ�ӽ�ͻȻ�ƶ�һ��  ���ǰ���������Ǹ�λ�����óɵ�ǰ�ƶ���ԭ�� ��ֹ����ϴ�������λ��

POINT lasPos;
GLdouble modelview1[16], projection1[16];
GLint viewport1[4];
GLdouble mvmatrix[16], projmatrix[16];
GLdouble wx, wy, wz, wwx, wwy, wwz;
POINT originalPos; //�����洢��갴��ʱ�Ǹ��������
POINT currentPos;
GLfloat pz, px, py;
GLint realy;
void mouseMove(GLFWwindow* window, Camera *camera,HWND hwnd,LeftMouse &lf) //��ȡ�����������λ�ã����Ҵ�������lf
{
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
	if (state == GLFW_PRESS)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  //��������óɲ���ʾ������
		glfwGetCursorPos(window, &xposition, &yposition);           //��ȡ���λ��
		if (firstMouse)
		{
			lastX = xposition;
			lastY = yposition;
			firstMouse = false;
		}
		float xoffset = xposition - lastX;
		float yoffset = lastY - yposition;

		lastX = xposition;
		lastY = yposition;
		camera->ProcessMouseMovement(xoffset, yoffset);   // reversed since y-coordinates go from bottom to top
	}
	else if (state == GLFW_RELEASE)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		firstMouse = true;
	}

	int stateL = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT); 	/*������»�ȡ�����λ��*/
	if (stateL == GLFW_PRESS)
	{
		glfwGetCursorPos(window, &xposition, &yposition);//��ȡ���λ��
		lf.x = xposition;
		lf.y = yposition;
		lf.IsPress = true;
	}
	else if (stateL == GLFW_RELEASE)
	{
		lf.IsPress = false;
	}
}