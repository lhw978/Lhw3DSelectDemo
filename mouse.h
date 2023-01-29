#pragma once
#include "all.h"
#include <windows.h>
//#pragma comment(lib,"glu32.lib")
//#include "gl/glut.h"
#include "left_mouse.h"

void mouseMove(GLFWwindow* window, Camera *camera,HWND hwnd,LeftMouse &lf); //获取鼠标左键点击的位置，并且传出来给lm