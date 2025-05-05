#define GLM_FORCE_CXX20

#include "camera.h"
#include "shader.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>
#include <stb_image.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

float g_deltaTime;
float g_lastFrame;

float g_lastCursorX = WINDOW_WIDTH / 2.0f;
float g_lastCursorY = WINDOW_HEIGHT / 2.0f;

bool g_firstMouse = true;

Camera g_camera;

void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouseCallback(GLFWwindow *window, double xpos, double ypos);

int main(int argc, char *argv[])
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "opengl", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize glad" << std::endl;
        return -1;
    }

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);

    // clang-format off
    // const float vertices[] = {
    //      0.5f,  0.5f, 0.0f,     4.0f, 4.0f,     // top right
    //      0.5f, -0.5f, 0.0f,     4.0f, 0.0f,     // bottom right
    //     -0.5f, -0.5f, 0.0f,     0.0f, 0.0f,     // bottom left
    //     -0.5f,  0.5f, 0.0f,     0.0f, 4.0f      // top left 
    // };
    // const unsigned int indices[] = {
    //     0, 1, 3,
    //     1, 2, 3
    // };
    // const float vertices[] = {
    //     0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
    //     -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
    //     0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f
    // };
    // const unsigned int indices[] = {
    //     0, 1, 2,
    // };

float vertices[] = {
    // positions        // UV
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // 0
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // 1
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // 2
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // 3

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // 4
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // 5
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // 6
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // 7

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // 8
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // 9
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // 10
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // 11

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // 12
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // 13
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // 14
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // 15

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // 16
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // 17
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // 18
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // 19

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // 20
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // 21
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // 22
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // 23
};

// unsigned int indices[] = {
//     // back face
//     0, 1, 2,  2, 3, 0,
//     // front face
//     4, 5, 6,  6, 7, 4,
//     // left face
//     8, 9,10, 10,11, 8,
//     // right face
//    12,13,14, 14,15,12,
//     // bottom face
//    16,17,18, 18,19,16,
//     // top face
//    20,21,22, 22,23,20
// };
unsigned int indices[] = {
    // back face
    0, 3, 2,  2, 1, 0,
    // front face
    4, 5, 6,  6, 7, 4,
    // left face
    8, 9,10, 10,11, 8,
    // right face
   12,15,14, 14,13,12,
    // bottom face
   16,17,18, 18,19,16,
    // top face
   20,23,22, 22,21,20
};
    // clang-format on

    GLuint VAO, VBO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    Shader shaderProgram(ASSETS_PATH "/shaders/vertex.glsl", ASSETS_PATH "/shaders/fragment.glsl");


    stbi_set_flip_vertically_on_load(true);

    GLuint texture1, texture2;
    glGenTextures(1, &texture1);

    glBindTexture(GL_TEXTURE_2D, texture1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int tex_width, tex_height, nr_channels;
    unsigned char *text_data1 =
        stbi_load(ASSETS_PATH "/textures/container.jpg", &tex_width, &tex_height, &nr_channels, 0);

    if (text_data1)
    {
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, text_data1
        );
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cerr << "Failed to load texture\n";
    }

    stbi_image_free(text_data1);

    glGenTextures(1, &texture2);

    glBindTexture(GL_TEXTURE_2D, texture2);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char *text_data2 = stbi_load(
        ASSETS_PATH "/textures/awesomeface.png", &tex_width, &tex_height, &nr_channels, 0
    );

    if (text_data2)
    {
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGB,
            tex_width,
            tex_height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            text_data2
        );
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cerr << "Failed to load texture\n";
    }

    stbi_image_free(text_data2);


    shaderProgram.use();
    shaderProgram.setInt("texture1", 0);
    shaderProgram.setInt("texture2", 1);
    shaderProgram.setFloat("blend", 0.5f);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glm::vec3 cubePositions[] = {
        glm::vec3(0.5f, 1.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)
    };

    glEnable(GL_DEPTH_TEST);

    glm::mat4 projection = g_camera.getProjectionMatrix((float)WINDOW_WIDTH / (float)WINDOW_HEIGHT);

    shaderProgram.setMat4("projection", projection);

    g_deltaTime = 0.0f; // Time between current frame and last frame
    g_lastFrame = 0.0f; // Time of last frame

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        g_deltaTime = currentFrame - g_lastFrame;
        g_lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        shaderProgram.use();

        // glm::mat4 model = glm::mat4(1.0f);
        // model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

        glm::mat4 view = g_camera.getViewMatrix();

        shaderProgram.setMat4("view", view);


        glBindVertexArray(VAO);
        for (unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 1.0f, 1.0f));

            shaderProgram.setMat4("model", model);

            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }
        // glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE))
    {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        g_camera.processKeyboard(CameraMovement::FORWARD, g_deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        g_camera.processKeyboard(CameraMovement::BACKWARD, g_deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        g_camera.processKeyboard(CameraMovement::LEFT, g_deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        g_camera.processKeyboard(CameraMovement::RIGHT, g_deltaTime);
    }
}

void mouseCallback(GLFWwindow *window, double xpos, double ypos)
{
    if (g_firstMouse)
    {
        g_lastCursorX = xpos;
        g_lastCursorY = ypos;
        g_firstMouse = false;
    }
    float xoffset = xpos - g_lastCursorX;
    float yoffset = g_lastCursorY - ypos; // reversed since y-coordinates go from bottom to top

    g_lastCursorX = xpos;
    g_lastCursorY = ypos;

    g_camera.processMouseMovement(xoffset, yoffset);
}
