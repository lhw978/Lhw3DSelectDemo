#include "glad/glad.h"
#include "glm/glm.hpp"
#include "openglshader.h"
#include "model.h"
#include "mouse.h"


const unsigned int SCR_WIDTH = 800; // settings
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f)); // camera

float deltaTime = 0.0f;  // timing
float lastFrame = 0.0f;

void processInput(GLFWwindow* window) // process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) // glfw: whenever the window size changed (by OS or user resize) this callback function executes
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) // glfw: whenever the mouse scroll wheel scrolls, this callback is called
{
    camera.ProcessMouseScroll(yoffset);
}

int main()
{  
    glfwInit();                                // glfw: initialize and configure
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LhwLearnOpenGL", NULL, NULL);  // glfw window creation
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);  
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  // tell GLFW to capture our mouse
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))  // glad: load all OpenGL function pointers
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);  // configure global opengl state 

    Shader pickShader("shader/pick.vs", "shader/pick.fs"); ///拾取着色器 build and compile shaders
    Shader ourShader("shader/modelShader.vs", "shader/modelShader.fs", "shader/modelShader.gs");  //用到了几何着色器
    //Shader simpleShader("shader/simple.vs", "shader/simple.fs");
    Model ourModel("model/nanosuit.obj2");  //load model: Model ourModel("C:/Users/Howard Wilson/source/repos/模型载入1/装甲/nanosuit.obj");
   
    glm::mat4 model = glm::mat4(1.0f); /*这里把相应数据存入帧缓冲*/
    model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	      // it's a bit too big for our scene, so scale it down
    LeftMouse* lm = (LeftMouse*)calloc(1, sizeof(LeftMouse)); //开辟空间, 用于存储左键点击坐标的结构体指针
    HWND hwnd = GetActiveWindow();  //获取窗口句柄
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);   // draw in wireframe  
    PickingTexture pickingtexture;
    pickingtexture.Init(SCR_WIDTH, SCR_HEIGHT);//拾取纹理初始化
    while (!glfwWindowShouldClose(window))   // render loop
    {
        float currentFrame = glfwGetTime();    // per-frame time logic
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);       // input
        mouseMove(window, &camera, hwnd,*lm);  //这里获取了鼠标左键点击的位置，并且传出来给lm

        glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix(); // view/projection transformations
        
        ourModel.DrawRenderTarget(pickingtexture, pickShader, model, view, projection);  // GPU拾取绘制
        if (lm->IsPress == true)    /*本来是点击的模型绘制成红色，现在变成把点击处的三元索引显示出来*/
        {
            PickingTexture::PixelInfo Pixel = pickingtexture.ReadPixel(lm->x, SCR_HEIGHT - lm->y - 1); //这里得到像素信息 （从拾取帧缓冲处获得像素）参数是鼠标在屏幕上的二维坐标
            printf("x:%d    y:%d    ob:%f   dr:%f   pr:%f \n", lm->x, SCR_HEIGHT - lm->y - 1, Pixel.objectID, Pixel.drawID, Pixel.primID);
        }
        
        ourModel.Draw(ourShader, model, view, projection); // render the loaded model: 绘制所有场景, 这里是绘制模型

        glfwSwapBuffers(window);   // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwPollEvents();
    }

    if (lm) //释放内存：左键点击坐标的结构体指针
    {
        free(lm);
        lm = NULL;
    }

    glfwTerminate();  // glfw: terminate, clearing all previously allocated GLFW resources.
    return 0;
}

