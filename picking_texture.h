#pragma once
#include"all.h"
/*ʰȡ��֡������*/
class PickingTexture
{
public:
	PickingTexture();

	~PickingTexture();

	bool Init(unsigned int WindowWidth, unsigned int WindowHeight);//��ʼ������֡����

	void EnableWriting();//����д��

	void DisableWriting();//�ر�д��

	/*������Ԫ����*/
	struct PixelInfo
	{
		float objectID;//�������� �����е�ÿ�����󶼻���Ψһ��������
		float drawID;//��������  �����������¶���ʼʱ���á�Ҳ���ǻ����¶����ʱ��ÿ����һ�������+1
		float primID;//ԭʼ����  ����������ÿ�λ�ͼ���ÿ�ʼʱ���á�Ҳ����ÿ�ε�����ɫ�� ����ÿ��Ƭ�ε�ʱ���+1
		PixelInfo()
		{
			objectID = 0.0f;
			drawID = 0.0f;
			primID = 0.0f;
		}
	};
	PixelInfo ReadPixel(unsigned int x, unsigned int y);//��ȡ����������� ������Ӧ���ص���Ԫ����
private:
	GLuint m_fbo;//֡����Ķ�����
	GLuint m_pickingTexture;//������Ϣ���������
	GLuint m_depthTexture;//����������   
	
};