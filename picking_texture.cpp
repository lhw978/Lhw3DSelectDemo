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
    glGenFramebuffers(1, &m_fbo);   /*����fbo*/
    EnableWriting();
  
    glGenTextures(1,&m_pickingTexture);  /*������ɫ����*/
    glBindTexture(GL_TEXTURE_2D, m_pickingTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, NULL);  //�������ٹ淶�ռ�//����Ŀ��Ϊ2d����,�޶༶�������� ��������������Ϣ�������ڲ���ʽΪRGB32f������Ŀ�Ⱥ͸߶�����Ϊ��Ļ�Ŀ�ߣ������������þ���
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pickingTexture, 0); //����ɫ��������֡����
  
    glGenTextures(1, &m_depthTexture);  /*�����������*/
    glBindTexture(GL_TEXTURE_2D, m_depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WindowHeight, WindowWidth, 0, GL_DEPTH_COMPONENT, GL_FLOAT , NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);

    glReadBuffer(GL_NONE);    /*ѡ��д�뻺����  ΪʲôҪ����ѡ�񻹲����*/
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
  
    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);  // Verify that the FBO is correct
    if (Status != GL_FRAMEBUFFER_COMPLETE) {
        printf("FB error, status: 0x%x\n", Status);
        return false;
    }
  
    glBindTexture(GL_TEXTURE_2D, 0);  /*�ָ�Ĭ�ϻ���*/
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

PickingTexture::PixelInfo PickingTexture::ReadPixel(unsigned x, unsigned y) /*��ȡ��Ļ����������������Ԫ����*/
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glReadBuffer(GL_COLOR_ATTACHMENT0);  //������ע�͵����Զ�ȡ����ʼ�������� ��ȡ����ʼ����ԭ���� ��û�ж�ȡ������
    
    PixelInfo Pixel;
    glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, &Pixel);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return Pixel;
}