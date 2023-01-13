#pragma once
#include "glad/glad.h"
//GLAS是用来管理OpenGL的函数指针，在调用任何OpenGL函数之前我们都需要初始化GLAD
#include "GLFW/glfw3.h"
#include <iostream>
#include <cmath>
#include "openglshader.h"
#include "camera.h"
//#define STB_IMAGE_IMPLEMENTATION  //加这个是因为加了这个下面那个头文件才能使用 要不然他的函数就是 无法解析的 外部命令
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp" 
