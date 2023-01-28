#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include"openglshader.h"
#include"camera.h"
#include "model.h"
#include"mouse.h"
#include"picking_texture.h"
//#include"stb_image.h"
//#include"assimp/anim.h"
//using namespace FileSystem
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
Camera* camerax = &camera;//主要是要一个这个对象的指针 在鼠标那个函数里面可以传进去

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

LeftMouse* lm;//用于存储左键点击坐标的结构体指针
HWND hwnd;
int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LhwLearnOpenGL", NULL, NULL);
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

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

 
    lm = (LeftMouse*)calloc(1, sizeof(LeftMouse));//开辟空间


    // build and compile shaders
    // -------------------------
    Shader ourShader("shader/modelShader.vs","shader/modelShader.fs","shader/modelShader.gs");
    Shader pickShader("shader/pick.vs", "shader/pick.fs"); ///拾取着色器
    Shader simpleShader("shader/simple.vs", "shader/simple.fs");
    PickingTexture pickingtexture;

    // load models
    // -----------
    //Model ourModel("C:/Users/Howard Wilson/source/repos/模型载入1/装甲/nanosuit.obj");
    Model ourModel("model/nanosuit.obj2");
    pickingtexture.Init(SCR_WIDTH, SCR_HEIGHT);//拾取纹理初始化

    hwnd = GetActiveWindow();//获取窗口句柄
    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);
        mouseMove(window, camerax, hwnd,*lm);//这里获取了鼠标左键点击的位置，并且传出来给lm
        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms

        // view/projection transformations
        /*这里把相应数据存入帧缓冲*/
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
        pickShader.use();//将复合矩阵传递给拾取着色器，为了在帧缓冲中绘制一次模型场景
        pickShader.setMat4("view", view);
        pickShader.setMat4("projection", projection);
        pickShader.setMat4("model", model);
        pickingtexture.EnableWriting();//开启帧缓存

        glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

// make sure we clear the framebuffer's content
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        /*绘制一次需要被点击的场景，但不是真正的绘制出来*/
        /*每绘制一个模型就传一个对象索引进去*/
        for (int i = 0; i < 1; i++)
        {
            pickShader.setInt("gobjectIndex", i);//传对象索引进着色器
            ourModel.Draw(pickShader);//传绘制索引和原始索引进着色器
        }
        glBindVertexArray(0);


        pickingtexture.DisableWriting();


        /*本来是点击的模型绘制成红色，现在变成把点击处的三元索引显示出来*/
        if (lm->IsPress == true)
        {
            //return 0;
            PickingTexture::PixelInfo Pixel = pickingtexture.ReadPixel(lm->x, SCR_HEIGHT - lm->y - 1);//这里得到像素信息 （从拾取帧缓冲处获得像素）参数是鼠标在屏幕上的二维坐标
            printf("x:%d    y:%d    ob:%f   dr:%f   pr:%f", lm->x, SCR_HEIGHT - lm->y - 1, Pixel.objectID, Pixel.drawID, Pixel.primID);
            printf("\n");
        }

        /*绘制所有场景*/
        ourShader.use();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // render the loaded model
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model);
           
        ourShader.setFloat("time", glfwGetTime());

        ourModel.Draw(ourShader);//这里是绘制模型


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
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

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}