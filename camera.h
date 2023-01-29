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
const float YAW = -90.0f;  //ƫ����
const float PITCH = 0.0f;  //������
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera  // An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
{
public:
	// Camera Attributes
	glm::vec3 Position; //�������λ��
	glm::vec3 Front;  //������ĳ�������
	glm::vec3 Up;  //�������ͷ������
	glm::vec3 Right;  //��������Ҳ�����
	glm::vec3 WorldUp;  //��������ϵ��������
	// Euler Angles
	float Yaw;  //ƫ����
	float Pitch;  //������
	// Camera options
	float MovementSpeed;      //�ƶ��ٶ�
	float MouseSensitivity;   //����������� �ٹ��캯����ʱ�򱻳�ʼ����  ����������ֵ������ƶ��ͻ�̫����
	float Zoom;

	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) 
		: Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)  //�ڹ��캯�������ʼ��ֵ
	{
		Position = position;   //������λ��ֵ
		WorldUp = up;  //������������
		Yaw = yaw;  //������ƫ����
		Pitch = pitch;  //�����˸�����
		updateCameraVectors();  //�����Ǹ�������ƶ��������� ������ ����ǰ�ķ��������ĸı�
	}

	// Constructor with scalar values
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) 
		: Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)  //��������ֶ�����
	{
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	glm::mat4 GetViewMatrix()  //���ù۲���� Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	{
		return glm::lookAt(Position, Position + Front, Up);//Position + FrontĿ���Ƿ�����������������λ�ö��ı�
	}

	void ProcessKeyboard(Camera_Movement direction, float deltaTime)  /*������Ӧ������wsad���ƶ�  ����deltatimeĿ��������ÿһ֡����ʱ�䲻һ�����ƶ��ٶȵ�Ӱ��*/
	{
		float velocity = MovementSpeed * deltaTime;  //������Ӧ���ƶ���λ��
		if (direction == FORWARD)
			Position += Front * velocity; //����������� �������Ӧ�����ƶ�����һ֡��Ҫ�ӽ�*�ٶȵľ���
		if (direction == BACKWARD)
			Position -= Front * velocity;
		if (direction == LEFT)
			Position -= Right * velocity;
		if (direction == RIGHT)
			Position += Right * velocity;
	}

	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)   	/*�����Ӧ*/
	{
		xoffset *= MouseSensitivity;//xoffset  yoffset������ֵ��������������Ӧ��������ص����� �õ�������һ֡����һ֡�����x  y�����ƫ����
		yoffset *= MouseSensitivity;  //Ŀǰ����֪�������ȵ������ж��

		Yaw += xoffset;  //��ƫ���Ǻ͸�����+��ƫ��������֪��Ϊʲô�ǽǶ�+ƫ������
		Pitch += yoffset;
		if (constrainPitch)  /*ȷ���ӽǵ�̧ͷ��ͷ�����ͷ �����ӽǵߵ�*/
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		updateCameraVectors();
	}

	void ProcessMouseScroll(float yoffset)	/*���ֵ���Ӧ����*/
	{
		if (Zoom >= 1.0f && Zoom <= 45.0f)
			Zoom -= yoffset;
		if (Zoom <= 1.0f)
			Zoom = 1.0f;
		if (Zoom >= 45.0f)
			Zoom = 45.0f;
	}

private:
	void updateCameraVectors()   /*��������ƶ����ӽǵĸı䣺Calculates the front vector from the Camera's (updated) Euler Angles */
	{		
		glm::vec3 front;  /*����Ӧ�þ�������ƫ������С���ԺͽǶ���� ����ֱ����ƫ����=�Ƕ� Ȼ��ŵ����������*/
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