
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstddef>
#include <cmath>
#include <string>

#include "common/GLShader.h"
#include "mat4.h"

// ----------- Macros Apple VAO -----------
#ifdef __APPLE__
#undef glGenVertexArrays
#undef glBindVertexArray
#undef glDeleteVertexArrays
#define glGenVertexArrays glGenVertexArraysAPPLE
#define glBindVertexArray glBindVertexArrayAPPLE
#define glDeleteVertexArrays glDeleteVertexArraysAPPLE
#endif

// ----------- STB Image -----------
// Deja defini dans exercice1 si on compile ensemble, mais ici on compile separement
#define STB_IMAGE_IMPLEMENTATION
#include "common/stb_image.h"

// ----------- Globals -----------
GLShader g_SkyboxShader;
GLuint   g_SkyboxVAO, g_SkyboxVBO;
GLuint   g_CubemapTexture;

GLShader g_DragonShader;
GLuint   g_DragonVAO, g_DragonVBO;
int      g_DragonVertexCount = 0;

// Caméra orbitale
float g_azimuth   = 0.0f;
float g_elevation = 0.0f;
float g_distance  = 12.0f;
bool  g_isMousePressed = false;
double g_lastMouseX = 0.0, g_lastMouseY = 0.0;

int g_RenderMode = 0; // 0 = reflection, 1 = refraction

// ---- Sommets du cube unité (skybox) ----
static const float skyboxVertices[] = {
    // +X face
    -1.f,  1.f, -1.f,
    -1.f, -1.f, -1.f,
     1.f, -1.f, -1.f,
     1.f, -1.f, -1.f,
     1.f,  1.f, -1.f,
    -1.f,  1.f, -1.f,
    // -X face
    -1.f, -1.f,  1.f,
    -1.f, -1.f, -1.f,
    -1.f,  1.f, -1.f,
    -1.f,  1.f, -1.f,
    -1.f,  1.f,  1.f,
    -1.f, -1.f,  1.f,
    // +Y face
     1.f, -1.f, -1.f,
     1.f, -1.f,  1.f,
     1.f,  1.f,  1.f,
     1.f,  1.f,  1.f,
     1.f,  1.f, -1.f,
     1.f, -1.f, -1.f,
    // -Y face
    -1.f, -1.f,  1.f,
    -1.f,  1.f,  1.f,
     1.f,  1.f,  1.f,
     1.f,  1.f,  1.f,
     1.f, -1.f,  1.f,
    -1.f, -1.f,  1.f,
    // +Z face
    -1.f,  1.f, -1.f,
     1.f,  1.f, -1.f,
     1.f,  1.f,  1.f,
     1.f,  1.f,  1.f,
    -1.f,  1.f,  1.f,
    -1.f,  1.f, -1.f,
    // -Z face
    -1.f, -1.f, -1.f,
    -1.f, -1.f,  1.f,
     1.f, -1.f, -1.f,
     1.f, -1.f, -1.f,
    -1.f, -1.f,  1.f,
     1.f, -1.f,  1.f
};

// ---- Sommets du Cube (pour la reflexion/refraction, avec normales) ----
static const float cubeVertices[] = {
    // positions          // normales
    // Back face
    -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
     1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
     1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
     1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,

    // Front face
    -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
     1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
     1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
     1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,

    // Left face
    -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,
    -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
    -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
    -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
    -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f,
    -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,

    // Right face
     1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
     1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
     1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
     1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
     1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
     1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,

    // Bottom face
    -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,
     1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,
     1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
     1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
    -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
    -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,

    // Top face
    -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
     1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
     1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
     1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
    -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
    -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f
};

GLuint LoadCubemap()
{
    const char* faces[6] = {
        "envmaps/pisa_posx.jpg", "envmaps/pisa_negx.jpg",
        "envmaps/pisa_posy.jpg", "envmaps/pisa_negy.jpg",
        "envmaps/pisa_posz.jpg", "envmaps/pisa_negz.jpg"
    };

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    for (int i = 0; i < 6; i++)
    {
        int w, h, nChannels;
        unsigned char* data = stbi_load(faces[i], &w, &h, &nChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else std::cerr << "Erreur cubemap : " << faces[i] << std::endl;
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    return textureID;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_R) { g_RenderMode = 0; std::cout << "Mode : Reflexion" << std::endl; }
        if (key == GLFW_KEY_F) { g_RenderMode = 1; std::cout << "Mode : Refraction" << std::endl; }
        if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            g_isMousePressed = true;
            glfwGetCursorPos(window, &g_lastMouseX, &g_lastMouseY);
        }
        else if (action == GLFW_RELEASE) g_isMousePressed = false;
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (g_isMousePressed)
    {
        double dx = xpos - g_lastMouseX;
        double dy = ypos - g_lastMouseY;
        g_azimuth   -= (float)dx * 0.01f;
        g_elevation += (float)dy * 0.01f;
        if (g_elevation >  1.5f) g_elevation =  1.5f;
        if (g_elevation < -1.5f) g_elevation = -1.5f;
        g_lastMouseX = xpos;
        g_lastMouseY = ypos;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    g_distance -= (float)yoffset * 0.5f;
    if (g_distance < 2.0f) g_distance = 2.0f;
    if (g_distance > 50.0f) g_distance = 50.0f;
}

bool Initialise()
{
    // --- Skybox ---
    g_SkyboxShader.LoadVertexShader("exo1_skybox.vs");
    g_SkyboxShader.LoadFragmentShader("exo1_skybox.fs");
    g_SkyboxShader.Create();

    glGenVertexArrays(1, &g_SkyboxVAO);
    glBindVertexArray(g_SkyboxVAO);
    glGenBuffers(1, &g_SkyboxVBO);
    glBindBuffer(GL_ARRAY_BUFFER, g_SkyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

    auto progSky = g_SkyboxShader.GetProgram();
    int locPos = glGetAttribLocation(progSky, "a_position");
    glEnableVertexAttribArray(locPos);
    glVertexAttribPointer(locPos, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // --- Cube Reflechissant ---
    g_DragonShader.LoadVertexShader("exo2_envmap.vs");
    g_DragonShader.LoadFragmentShader("exo2_envmap.fs");
    g_DragonShader.Create();

    g_DragonVertexCount = 36; // 6 faces * 6 vertices

    glGenVertexArrays(1, &g_DragonVAO);
    glBindVertexArray(g_DragonVAO);
    glGenBuffers(1, &g_DragonVBO);
    glBindBuffer(GL_ARRAY_BUFFER, g_DragonVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    auto progDrag = g_DragonShader.GetProgram();
    int locDragPos  = glGetAttribLocation(progDrag, "a_position");
    int locDragNorm = glGetAttribLocation(progDrag, "a_normal");

    glEnableVertexAttribArray(locDragPos);
    glVertexAttribPointer(locDragPos, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(locDragNorm);
    glVertexAttribPointer(locDragNorm, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);

    g_CubemapTexture = LoadCubemap();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL); 
    glfwSwapInterval(1);
    
    std::cout << "Commandes:" << std::endl;
    std::cout << "- Clic gauche + glisser : tourner la camera" << std::endl;
    std::cout << "- Molette : zoomer" << std::endl;
    std::cout << "- R : mode Reflexion (Chrome)" << std::endl;
    std::cout << "- F : mode Refraction (Verre)" << std::endl;

    return true;
}

void Terminate()
{
    glDeleteVertexArrays(1, &g_SkyboxVAO);
    glDeleteBuffers(1, &g_SkyboxVBO);
    glDeleteVertexArrays(1, &g_DragonVAO);
    glDeleteBuffers(1, &g_DragonVBO);
    glDeleteTextures(1, &g_CubemapTexture);
    g_SkyboxShader.Destroy();
    g_DragonShader.Destroy();
}

void Render(GLFWwindow* window)
{
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glClearColor(0.1f, 0.1f, 0.15f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float aspect = (height > 0) ? (float)width / height : 1.f;
    Mat4 proj = makePerspective(60.f, aspect, 0.1f, 1000.f);

    // Calcul des matrices de vue
    Mat4 viewRot = multiply(makeRotationX(g_elevation), makeRotationY(-g_azimuth));
    Mat4 viewTrans = makeTranslation(0.0f, 0.0f, -g_distance);
    Mat4 view = multiply(viewTrans, viewRot);

    // Calcul de la position de la camera (inverse de la vue, applique a 0,0,0)
    Mat4 invView = multiply(makeRotationY(g_azimuth), multiply(makeRotationX(-g_elevation), makeTranslation(0.f, 0.f, g_distance)));
    float camX = invView.m[12];
    float camY = invView.m[13];
    float camZ = invView.m[14];

    // ---- 1. Rendu Skybox ----
    glDepthMask(GL_FALSE);
    auto pSky = g_SkyboxShader.GetProgram();
    glUseProgram(pSky);

    glUniformMatrix4fv(glGetUniformLocation(pSky, "u_projection"), 1, GL_FALSE, proj.m);
    glUniformMatrix4fv(glGetUniformLocation(pSky, "u_view"),       1, GL_FALSE, viewRot.m); // pas de translation
    glUniform1i(glGetUniformLocation(pSky, "u_skybox"), 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, g_CubemapTexture);

    glBindVertexArray(g_SkyboxVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);

    // ---- 2. Rendu Objet (Environment Mapping) ----
    auto pDrag = g_DragonShader.GetProgram();
    glUseProgram(pDrag);

    // Placement du cube (scale le cube un peu pour qu'il soit bien visible)
    Mat4 world = multiply(makeTranslation(0.f, 0.f, 0.f), makeRotationY(0.f)); 
    Mat4 scale = identity();
    scale.m[0] = 3.f; scale.m[5] = 3.f; scale.m[10] = 3.f;
    world = multiply(world, scale);

    glUniformMatrix4fv(glGetUniformLocation(pDrag, "u_projection"), 1, GL_FALSE, proj.m);
    glUniformMatrix4fv(glGetUniformLocation(pDrag, "u_view"),       1, GL_FALSE, view.m);
    glUniformMatrix4fv(glGetUniformLocation(pDrag, "u_world"),      1, GL_FALSE, world.m);

    glUniform3f(glGetUniformLocation(pDrag, "u_cameraPos"), camX, camY, camZ);
    glUniform1i(glGetUniformLocation(pDrag, "u_skybox"), 0);
    glUniform1i(glGetUniformLocation(pDrag, "u_mode"), g_RenderMode);

    glBindVertexArray(g_DragonVAO);
    glDrawArrays(GL_TRIANGLES, 0, g_DragonVertexCount);
    glBindVertexArray(0);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{ glViewport(0, 0, width, height); }

int main()
{
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(960, 540,
        "TP Cube Mapping - Partie 2 : Env Map (Reflexion/Refraction)", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) return -1;

    if (!Initialise()) { glfwTerminate(); return -1; }

    while (!glfwWindowShouldClose(window))
    {
        Render(window);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    Terminate();
    glfwTerminate();
    return 0;
}
