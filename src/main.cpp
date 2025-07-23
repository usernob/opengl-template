#define GLM_FORCE_CXX20
#include "camera.h"
#include "model.h"
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

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 700

float g_deltaTime;
float g_lastFrame;

float g_lastCursorX = WINDOW_WIDTH / 2.0f;
float g_lastCursorY = WINDOW_HEIGHT / 2.0f;

bool g_firstMouse = true;
bool g_mouse_pressed = false;

Camera g_camera(glm::vec3(0.0f, 0.0f, 3.0f));
glm::mat4 g_projection = g_camera.getProjectionMatrix((float)WINDOW_WIDTH / (float)WINDOW_HEIGHT);

void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouseCallback(GLFWwindow *window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);

int main(int argc, char *argv[])
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    // glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
    // glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "opengl", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize glad" << std::endl;
        return -1;
    }

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

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
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
    stbi_set_flip_vertically_on_load(true);
    // clang-format off
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

    GLuint lightsource_VAO;
    glGenVertexArrays(1, &lightsource_VAO);
    glBindVertexArray(lightsource_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    Shader lightsource_shader(
        ASSETS_PATH "/shaders/light-source.vert",
        ASSETS_PATH "/shaders/light-source.frag"
    );

    Shader backpack_shader(ASSETS_PATH "/shaders/model.vert", ASSETS_PATH "/shaders/model.frag");
    Model backpack_model(ASSETS_PATH "/model/backpack/backpack.obj", backpack_shader);

    g_camera.processMouseMovement(0.2f, 0.2f);
    g_deltaTime = 0.0f; // Time between current frame and last frame
    g_lastFrame = 0.0f; // Time of last frame

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // light properties is used to adjust light behavior on the object
    struct LightProp
    {
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        float constant;
        float linear;
        float quadratic;
    };

    LightProp light_prop{
        glm::vec3(0.2f, 0.2f, 0.2f),
        glm::vec3(0.5f, 0.5f, 0.5f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        1.0f,
        0.09f,
        0.032f,
    };

    backpack_shader.use();
    backpack_shader.setFloat("material.shininess", 32.0f);
    backpack_shader.setFloat("light.constant", light_prop.constant);
    backpack_shader.setFloat("light.linear", light_prop.linear);
    backpack_shader.setFloat("light.quadratic", light_prop.quadratic);

    struct OrbitObject
    {
        float radius;
        float rotation_speed;
        glm::vec3 axis;
    };

    OrbitObject lightsource_orbit{
        4.0f,
        1.0f,
        glm::vec3(0.0f, 1.0f, 0.0f),
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
        {
            ImGui::Begin("Settings");

            if (ImGui::CollapsingHeader("light source"))
            {
                ImGui::SliderFloat("Radius", &lightsource_orbit.radius, 0.0f, 40.0f);
                ImGui::SliderFloat(
                    "Rotation speed",
                    &lightsource_orbit.rotation_speed,
                    0.0f,
                    30.0f
                );
                ImGui::SliderFloat3("Axis", &lightsource_orbit.axis[0], -1.0f, 1.0f);
            }

            if (ImGui::CollapsingHeader("light property"))
            {
                ImGui::SliderFloat3("Light ambient", &light_prop.ambient[0], 0.0f, 1.0f);

                ImGui::SliderFloat3("Light diffuse", &light_prop.diffuse[0], 0.0f, 1.0f);

                ImGui::SliderFloat3("Light specular", &light_prop.specular[0], 0.0f, 1.0f);
            }

            if (ImGui::CollapsingHeader("misc", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Checkbox("show demo window", &show_demo_window);
                ImGui::ColorEdit3("clear color", (float *)&clear_color);
                // Edit 3 floats representing a color
                ImGui::Text(
                    "Application average %.3f ms/frame (%.1f FPS)",
                    1000.0f / io.Framerate,
                    io.Framerate
                );
            }

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
        glm::mat4 view_projection = g_projection * view;

        glm::mat4 lightsource_pos = glm::mat4(1.0f);
        const float time = glfwGetTime() * 0.5f;

        lightsource_pos = glm::rotate(
            lightsource_pos,
            time * lightsource_orbit.rotation_speed,
            lightsource_orbit.axis
        );
        lightsource_pos =
            glm::translate(lightsource_pos, glm::vec3(lightsource_orbit.radius, 0.f, 0.f));
        lightsource_pos = glm::scale(lightsource_pos, glm::vec3(0.2f));

        glm::vec3 lightPos = glm::vec3(lightsource_pos * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

        lightsource_shader.use();
        lightsource_shader.setMat4("view", view);
        lightsource_shader.setMat4("model", lightsource_pos);
        lightsource_shader.setMat4("projection", g_projection);

        lightsource_shader.setVec3("light.ambient", light_prop.ambient);
        lightsource_shader.setVec3("light.diffuse", light_prop.diffuse);
        lightsource_shader.setVec3("light.specular", light_prop.specular);
        glBindVertexArray(lightsource_VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        backpack_shader.use();

        backpack_shader.setVec3("viewPos", g_camera.m_position);

        backpack_shader.setMat4("view", view);
        backpack_shader.setMat4("view_projection", view_projection);

        backpack_shader.setVec3("light.position", lightPos);
        backpack_shader.setVec3("light.ambient", light_prop.ambient);
        backpack_shader.setVec3("light.diffuse", light_prop.diffuse);
        backpack_shader.setVec3("light.specular", light_prop.specular);

        // render the loaded model
        glm::mat4 backpack_pos = glm::mat4(1.0f);
        backpack_pos = glm::translate(
            backpack_pos,
            glm::vec3(0.0f, 0.0f, 0.0f)
        ); // translate it down so it's at the center of the scene
        backpack_pos = glm::scale(
            backpack_pos,
            glm::vec3(1.0f, 1.0f, 1.0f)
        ); // it's a bit too big for our scene, so scale it down
        backpack_shader.setMat4("model", backpack_pos);

        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(backpack_pos)));
        backpack_shader.setMat3("normalMatrix", normalMatrix);

        backpack_model.draw();

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
    g_projection = g_camera.getProjectionMatrix((float)width / (float)height);
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
    g_mouse_pressed = (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) &&
                      !ImGui::GetIO().WantCaptureMouse;
}
