#pragma once
#include"all.h"
/*拾取的帧缓冲类*/
class PickingTexture
{
public:
	PickingTexture();

	~PickingTexture();

	bool Init(unsigned int WindowWidth, unsigned int WindowHeight);//初始化创建帧缓冲

	void EnableWriting();//开启写入

	void DisableWriting();//关闭写入

	/*像素三元索引*/
	struct PixelInfo
	{
		float objectID;//对象索引 场景中的每个对象都会获得唯一的索引。
		float drawID;//绘制索引  该索引将在新对象开始时重置。也就是绘制新对象的时候每绘制一个网格就+1
		float primID;//原始索引  该索引将在每次绘图调用开始时重置。也就是每次调用着色器 绘制每个片段的时候就+1
		PixelInfo()
		{
			objectID = 0.0f;
			drawID = 0.0f;
			primID = 0.0f;
		}
	};
	PixelInfo ReadPixel(unsigned int x, unsigned int y);//读取鼠标点击处像素 返回相应像素的三元索引
private:
	GLuint m_fbo;//帧缓冲的对象句柄
	GLuint m_pickingTexture;//索引信息的纹理对象
	GLuint m_depthTexture;//深度纹理对象   
	
};