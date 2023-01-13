#pragma once
#include"all.h"
#include<windows.h>
#include<vector>
#pragma comment(lib,"glu32.lib")
#include"gl/glut.h"
#include"left_mouse.h"
//extern float lastX;
//extern float lastY;
//extern double xposition, yposition;
//extern bool firstMouse;
void mouseMove(GLFWwindow* window, Camera *camera,HWND hwnd,LeftMouse &lf);