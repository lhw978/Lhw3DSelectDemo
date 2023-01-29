#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include "glm/gtc/matrix_transform.hpp"

enum Camera_Movement {  // Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Default camera values
const float YAW = -90.0f;  //偏航角
const float PITCH = 0.0f;  //俯仰角
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera  // An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
{
public:
	// Camera Attributes
	glm::vec3 Position; //摄像机的位置
	glm::vec3 Front;  //摄像机的朝向向量
	glm::vec3 Up;  //摄像机的头顶向量
	glm::vec3 Right;  //摄像机的右侧向量
	glm::vec3 WorldUp;  //世界坐标系的上向量
	// Euler Angles
	float Yaw;  //偏航角
	float Pitch;  //俯仰角
	// Camera options
	float MovementSpeed;      //移动速度
	float MouseSensitivity;   //这个是灵敏度 再构造函数的时候被初始化了  如果忽略这个值，鼠标移动就会太大了
	float Zoom;

	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) 
		: Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)  //在构造函数这里初始化值
	{
		Position = position;   //设置了位置值
		WorldUp = up;  //设置了上向量
		Yaw = yaw;  //设置了偏航角
		Pitch = pitch;  //设置了俯仰角
		updateCameraVectors();  //这里是更新鼠标移动对上向量 右向量 和向前的方向向量的改变
	}

	// Constructor with scalar values
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) 
		: Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)  //这里可以手动设置
	{
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	glm::mat4 GetViewMatrix()  //设置观察矩阵： Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	{
		return glm::lookAt(Position, Position + Front, Up);//Position + Front目的是方向向量会跟着摄像机位置而改变
	}

	void ProcessKeyboard(Camera_Movement direction, float deltaTime)  /*键盘响应：按动wsad会移动  放入deltatime目的是消除每一帧画面时间不一样对移动速度的影响*/
	{
		float velocity = MovementSpeed * deltaTime;  //这里是应该移动的位置
		if (direction == FORWARD)
			Position += Front * velocity; //如果按动键盘 则会向相应方向移动画这一帧所要视角*速度的距离
		if (direction == BACKWARD)
			Position -= Front * velocity;
		if (direction == LEFT)
			Position -= Right * velocity;
		if (direction == RIGHT)
			Position += Right * velocity;
	}

	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)   	/*鼠标响应*/
	{
		xoffset *= MouseSensitivity;//xoffset  yoffset这两个值会在外面的鼠标响应函数里面回调回来 得到的是上一帧到这一帧鼠标再x  y方向的偏移量
		yoffset *= MouseSensitivity;  //目前还不知道灵敏度的作用有多大

		Yaw += xoffset;  //让偏航角和俯仰角+上偏移量（不知道为什么是角度+偏移量）
		Pitch += yoffset;
		if (constrainPitch)  /*确保视角的抬头低头不会过头 导致视角颠倒*/
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		updateCameraVectors();
	}

	void ProcessMouseScroll(float yoffset)	/*滚轮的响应函数*/
	{
		if (Zoom >= 1.0f && Zoom <= 45.0f)
			Zoom -= yoffset;
		if (Zoom <= 1.0f)
			Zoom = 1.0f;
		if (Zoom >= 45.0f)
			Zoom = 45.0f;
	}

private:
	void updateCameraVectors()   /*更新鼠标移动对视角的改变：Calculates the front vector from the Camera's (updated) Euler Angles */
	{		
		glm::vec3 front;  /*这里应该就是由于偏移量很小可以和角度类比 所以直接用偏移量=角度 然后放到这个里面来*/
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		// Also re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, Front));
	}
};
#endif