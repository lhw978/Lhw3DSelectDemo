#include"picking_texture.h"

PickingTexture::PickingTexture()
{
	m_fbo = 0;
	m_pickingTexture = 0;
	m_depthTexture = 0;
}

PickingTexture::~PickingTexture()
{
    if (m_fbo != 0) {
        glDeleteFramebuffers(1, &m_fbo);
    }

    if (m_pickingTexture != 0) {
        glDeleteTextures(1, &m_pickingTexture);
    }

    if (m_depthTexture != 0) {
        glDeleteTextures(1, &m_depthTexture);
    }
}

bool PickingTexture::Init(unsigned int WindowWidth,unsigned int WindowHeight)
{ 
    glGenFramebuffers(1, &m_fbo);   /*创建fbo*/
    EnableWriting();
  
    glGenTextures(1,&m_pickingTexture);  /*创建颜色纹理*/
    glBindTexture(GL_TEXTURE_2D, m_pickingTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, NULL);  //给纹理开辟规范空间//纹理目标为2d纹理,无多级渐进纹理 ，纹理储存像素信息的纹理内部格式为RGB32f，纹理的宽度和高度设置为屏幕的宽高，后面这种设置就行
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pickingTexture, 0); //把颜色纹理附加上帧缓冲
  
    glGenTextures(1, &m_depthTexture);  /*创建深度纹理*/
    glBindTexture(GL_TEXTURE_2D, m_depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WindowHeight, WindowWidth, 0, GL_DEPTH_COMPONENT, GL_FLOAT , NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);

    glReadBuffer(GL_NONE);    /*选择写入缓冲区  为什么要在这选择还不清楚*/
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
  
    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);  // Verify that the FBO is correct
    if (Status != GL_FRAMEBUFFER_COMPLETE) {
        printf("FB error, status: 0x%x\n", Status);
        return false;
    }
  
    glBindTexture(GL_TEXTURE_2D, 0);  /*恢复默认缓冲*/
    DisableWriting();
    return true;
}

void PickingTexture::EnableWriting()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}

void PickingTexture::DisableWriting()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

PickingTexture::PixelInfo PickingTexture::ReadPixel(unsigned x, unsigned y) /*获取屏幕上鼠标点击处坐标的三元索引*/
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glReadBuffer(GL_COLOR_ATTACHMENT0);  //把这里注释掉可以读取到初始化的数据 获取到初始化的原因是 他没有读取到数据
    
    PixelInfo Pixel;
    glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, &Pixel);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return Pixel;
}