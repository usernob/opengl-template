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
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <stb_image.h>

#define WINDOW_WIDTH 900
#define WINDOW_HEIGHT 700

float g_deltaTime;
float g_lastFrame;

float g_lastCursorX = WINDOW_WIDTH / 2.0f;
float g_lastCursorY = WINDOW_HEIGHT / 2.0f;

bool g_firstMouse = true;
bool g_mouse_pressed = false;

Camera g_camera(glm::vec3(0.0f, 0.0f, 3.0f));

void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouseCallback(GLFWwindow *window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
int loadTexture(const char *path);

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
    // glfwSwapInterval(1);
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize glad" << std::endl;
        return -1;
    }

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // IF using Docking Branch

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(
        window, true
    ); // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();
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
    // positions            // UV           // normals
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,   0.0f,  0.0f, -1.0f, // 0
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,   0.0f,  0.0f, -1.0f, // 1
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   0.0f,  0.0f, -1.0f, // 2
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,   0.0f,  0.0f, -1.0f, // 3

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   0.0f,  0.0f,  1.0f, // 4
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,   0.0f,  0.0f,  1.0f, // 5
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,   0.0f,  0.0f,  1.0f, // 6
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,   0.0f,  0.0f,  1.0f, // 7

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  -1.0f,  0.0f,  0.0f, // 8
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  -1.0f,  0.0f,  0.0f, // 9
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  -1.0f,  0.0f,  0.0f, // 10
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  -1.0f,  0.0f,  0.0f, // 11

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   1.0f,  0.0f,  0.0f, // 12
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   1.0f,  0.0f,  0.0f, // 13
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   1.0f,  0.0f,  0.0f, // 14
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   1.0f,  0.0f,  0.0f, // 15

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   0.0f, -1.0f,  0.0f, // 16
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,   0.0f, -1.0f,  0.0f, // 17
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,   0.0f, -1.0f,  0.0f, // 18
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   0.0f, -1.0f,  0.0f, // 19

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,   0.0f,  1.0f,  0.0f, // 20
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   0.0f,  1.0f,  0.0f, // 21
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   0.0f,  1.0f,  0.0f, // 22
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,   0.0f,  1.0f,  0.0f  // 23
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

    GLuint VBO, EBO;

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    GLuint cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    GLuint lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    Shader cubeShader(ASSETS_PATH "/shaders/cube.vert", ASSETS_PATH "/shaders/cube.frag");
    Shader ligthShader(
        ASSETS_PATH "/shaders/light-source.vert", ASSETS_PATH "/shaders/light-source.frag"
    );

    glActiveTexture(GL_TEXTURE0);
    GLuint diffuse_map = loadTexture(ASSETS_PATH "/textures/container2.png");
    glActiveTexture(GL_TEXTURE1);
    GLuint specular_map = loadTexture(ASSETS_PATH "/textures/container2_specular.png");
    // GLuint texture;
    // glGenTextures(1, &texture);
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, texture);
    //
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    //
    // int tex_width, tex_height, tex_channel;
    // unsigned char *tex_data =
    //     stbi_load(ASSETS_PATH "/textures/container2.png", &tex_width, &tex_height, &tex_channel,
    //     0);
    //
    // if (!tex_data)
    // {
    //     std::cerr << "Failed to load texture" << std::endl;
    //     return -1;
    // }
    // glTexImage2D(
    //     GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data
    // );
    // glGenerateMipmap(GL_TEXTURE_2D);
    //
    // stbi_image_free(tex_data);

    glEnable(GL_DEPTH_TEST);

    glm::mat4 projection = g_camera.getProjectionMatrix((float)WINDOW_WIDTH / (float)WINDOW_HEIGHT);

    g_camera.processMouseMovement(0.2f, 0.2f);
    g_deltaTime = 0.0f; // Time between current frame and last frame
    g_lastFrame = 0.0f; // Time of last frame

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    struct LightProp
    {
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        float constant;
        float linear;
        float quadratic;
    };

    LightProp lightProp{
        glm::vec3(0.2f, 0.2f, 0.2f),
        glm::vec3(0.5f, 0.5f, 0.5f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        1.0f,
        0.09f,
        0.032f,
    };

    cubeShader.use();
    cubeShader.setInt("material.diffuse", 0);
    cubeShader.setInt("material.specular", 1);
    cubeShader.setFloat("material.shininess", 32.0f);
    cubeShader.setFloat("light.constant", lightProp.constant);
    cubeShader.setFloat("light.linear", lightProp.linear);
    cubeShader.setFloat("light.quadratic", lightProp.quadratic);

    cubeShader.setVec3("dirLight.direction", 0.0f, -2.0f, -3.0f);
    cubeShader.setVec3("dirLight.ambient", 0.2f, 0.2f, 0.2f);
    cubeShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.3f);
    cubeShader.setVec3("dirLight.specular", 0.3f, 0.3f, 0.3f);

    struct OrbitingCube
    {
        float orbitRadius;
        float orbitSpeed;
        float selfRotateSpeed;
    };

    OrbitingCube cubePositions[] = {
        {2.0f, 1.0f, 2.0f},
        {4.0f, 0.6f, 1.0f},
        {8.0f, 0.4f, 3.0f},
    };

    ImVec4 clear_color = ImVec4(0.059f, 0.059f, 0.059f, 1.00f);
    bool show_demo_window = false;
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        g_deltaTime = currentFrame - g_lastFrame;
        g_lastFrame = currentFrame;

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        if (show_demo_window)
        {
            ImGui::ShowDemoWindow(&show_demo_window); // Show demo window! :)
        }
        //        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to
        //        create a named window.
        {

            ImGui::Begin("Hello, world!"
            ); // Create a window called "Hello, world!" and append into it.

            ImGui::Text(
                "mouse state : %d", g_mouse_pressed
            ); // Display some text (you can use a format strings too)

            // ImGui::Text("Light ambient");
            ImGui::SliderFloat3("Light ambient", &lightProp.ambient[0], 0.0f, 1.0f);
            // ImGui::SliderFloat("y", &lightProp.ambient.y, 0.0f, 1.0f);
            // ImGui::SliderFloat("z", &lightProp.ambient.z, 0.0f, 1.0f);

            // ImGui::Text("Light diffuse");
            ImGui::SliderFloat3("Light diffuse", &lightProp.diffuse[0], 0.0f, 1.0f);
            // ImGui::SliderFloat("y", &lightProp.diffuse.y, 0.0f, 1.0f);
            // ImGui::SliderFloat("z", &lightProp.diffuse.z, 0.0f, 1.0f);

            // ImGui::Text("Light specular");
            ImGui::SliderFloat3("Light specular", &lightProp.specular[0], 0.0f, 1.0f);
            // ImGui::SliderFloat("y", &lightProp.specular.y, 0.0f, 1.0f);
            // ImGui::SliderFloat("z", &lightProp.specular.z, 0.0f, 1.0f);

            ImGui::ColorEdit3(
                "clear color", (float *)&clear_color
            ); // Edit 3 floats representing a color


            ImGui::Text(
                "Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate
            );
            ImGui::End();
        }

        processInput(window);

        glClearColor(
            clear_color.x * clear_color.w,
            clear_color.y * clear_color.w,
            clear_color.z * clear_color.w,
            clear_color.w
        );
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = g_camera.getViewMatrix();

        glBindVertexArray(cubeVAO);
        cubeShader.use();

        cubeShader.setVec3("viewPos", g_camera.m_position);

        cubeShader.setMat4("view", view);
        cubeShader.setMat4("projection", projection);

        cubeShader.setVec3("light.position", glm::vec3(0.0f, 0.0f, 0.0f));
        cubeShader.setVec3("light.ambient", lightProp.ambient);
        cubeShader.setVec3("light.diffuse", lightProp.diffuse);
        cubeShader.setVec3("light.specular", lightProp.specular);

        for (int i = 0; i < 3; i++)
        {
            const float time = glfwGetTime() * 0.5f;
            glm::mat4 model = glm::mat4(1.0f);
            const OrbitingCube &cubePos = cubePositions[i];

            model = glm::rotate(model, time * cubePos.orbitSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::translate(model, glm::vec3(cubePos.orbitRadius, 0.0f, 0.0f));
            model = glm::rotate(model, time * cubePos.selfRotateSpeed, glm::vec3(0.0f, 1.0f, 0.0f));

            glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));

            cubeShader.setMat4("model", model);
            cubeShader.setMat3("normalMatrix", normalMatrix);

            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }


        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.2f));

        ligthShader.use();
        ligthShader.setMat4("view", view);
        ligthShader.setMat4("model", model);
        ligthShader.setMat4("projection", projection);

        ligthShader.setVec3("light.ambient", lightProp.ambient);
        ligthShader.setVec3("light.diffuse", lightProp.diffuse);
        ligthShader.setVec3("light.specular", lightProp.specular);
        glBindVertexArray(lightVAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
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
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        g_camera.processKeyboard(CameraMovement::UP, g_deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        g_camera.processKeyboard(CameraMovement::DOWN, g_deltaTime);
    }
}

void mouseCallback(GLFWwindow *window, double xpos, double ypos)
{
    if (!g_mouse_pressed)
    {
        g_firstMouse = true;
        return;
    }
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

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    g_mouse_pressed = (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS);
}

int loadTexture(const char *path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format;
        if (nrChannels == 1) format = GL_RED;
        else if (nrChannels == 3) format = GL_RGB;
        else if (nrChannels == 4) format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
        return textureID;
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
        return -1;
    }
    stbi_image_free(data);
}
