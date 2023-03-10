#include"mouse.h"

float lastX; 
float lastY; 
double xposition, yposition;
bool firstMouse = true; //如果右键刚点击就设置为true 防止视角突然移动一下  就是把鼠标点击的那个位置设置成当前移动的原点 防止变成上次松鼠标的位置

POINT lasPos;
GLdouble modelview1[16], projection1[16];
GLint viewport1[4];
GLdouble mvmatrix[16], projmatrix[16];
GLdouble wx, wy, wz, wwx, wwy, wwz;
POINT originalPos; //变量存储鼠标按下时那个点的坐标
POINT currentPos;
GLfloat pz, px, py;
GLint realy;
void mouseMove(GLFWwindow* window, Camera *camera,HWND hwnd,LeftMouse &lf) //获取鼠标左键点击的位置，并且传出来给lf
{
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
	if (state == GLFW_PRESS)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  //将鼠标设置成不显示和限制
		glfwGetCursorPos(window, &xposition, &yposition);           //获取鼠标位置
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

	int stateL = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT); 	/*左键按下获取左键的位置*/
	if (stateL == GLFW_PRESS)
	{
		glfwGetCursorPos(window, &xposition, &yposition);//获取鼠标位置
		lf.x = xposition;
		lf.y = yposition;
		lf.IsPress = true;
	}
	else if (stateL == GLFW_RELEASE)
	{
		lf.IsPress = false;
	}
}