
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
#define STB_IMAGE_IMPLEMENTATION
#include "common/stb_image.h"

// ----------- Globals -----------
GLShader g_SkyboxShader;
GLuint   g_SkyboxVAO, g_SkyboxVBO;
GLuint   g_CubemapTexture;

// Caméra orbitale
float g_azimuth   = 0.0f;
float g_elevation = 0.0f;
bool  g_isMousePressed = false;
double g_lastMouseX = 0.0, g_lastMouseY = 0.0;

// ---- Sommets du cube unité (skybox) ----
// Un cube centré sur l'origine, 36 vertices (6 faces * 2 triangles * 3 vertices)
static const float skyboxVertices[] = {
    // +X face (droite)
    -1.f,  1.f, -1.f,
    -1.f, -1.f, -1.f,
     1.f, -1.f, -1.f,
     1.f, -1.f, -1.f,
     1.f,  1.f, -1.f,
    -1.f,  1.f, -1.f,
    // -X face (gauche)
    -1.f, -1.f,  1.f,
    -1.f, -1.f, -1.f,
    -1.f,  1.f, -1.f,
    -1.f,  1.f, -1.f,
    -1.f,  1.f,  1.f,
    -1.f, -1.f,  1.f,
    // +Y face (dessus)
     1.f, -1.f, -1.f,
     1.f, -1.f,  1.f,
     1.f,  1.f,  1.f,
     1.f,  1.f,  1.f,
     1.f,  1.f, -1.f,
     1.f, -1.f, -1.f,
    // -Y face (dessous)
    -1.f, -1.f,  1.f,
    -1.f,  1.f,  1.f,
     1.f,  1.f,  1.f,
     1.f,  1.f,  1.f,
     1.f, -1.f,  1.f,
    -1.f, -1.f,  1.f,
    // +Z face (avant)
    -1.f,  1.f, -1.f,
     1.f,  1.f, -1.f,
     1.f,  1.f,  1.f,
     1.f,  1.f,  1.f,
    -1.f,  1.f,  1.f,
    -1.f,  1.f, -1.f,
    // -Z face (arrière)
    -1.f, -1.f, -1.f,
    -1.f, -1.f,  1.f,
     1.f, -1.f, -1.f,
     1.f, -1.f, -1.f,
    -1.f, -1.f,  1.f,
     1.f, -1.f,  1.f
};

// ---- Chargement de la Cubemap (6 faces) ----
GLuint LoadCubemap()
{
    // Ordre : +X, -X, +Y, -Y, +Z, -Z (correspond aux enums OpenGL)
    const char* faces[6] = {
        "envmaps/pisa_posx.jpg",
        "envmaps/pisa_negx.jpg",
        "envmaps/pisa_posy.jpg",
        "envmaps/pisa_negy.jpg",
        "envmaps/pisa_posz.jpg",
        "envmaps/pisa_negz.jpg"
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
            std::cout << "Face " << i << " chargee : " << faces[i] << std::endl;
        }
        else
        {
            std::cerr << "Erreur chargement face " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    return textureID;
}

// ---- GLFW Callbacks souris (caméra orbitale) ----
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            g_isMousePressed = true;
            glfwGetCursorPos(window, &g_lastMouseX, &g_lastMouseY);
        }
        else if (action == GLFW_RELEASE)
            g_isMousePressed = false;
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

// ---- Initialisation ----
bool Initialise()
{
    g_SkyboxShader.LoadVertexShader("exo1_skybox.vs");
    g_SkyboxShader.LoadFragmentShader("exo1_skybox.fs");
    g_SkyboxShader.Create();

    // Création du VAO/VBO de la skybox
    glGenVertexArrays(1, &g_SkyboxVAO);
    glBindVertexArray(g_SkyboxVAO);

    glGenBuffers(1, &g_SkyboxVBO);
    glBindBuffer(GL_ARRAY_BUFFER, g_SkyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

    auto prog = g_SkyboxShader.GetProgram();
    int loc = glGetAttribLocation(prog, "a_position");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Chargement de la cubemap
    g_CubemapTexture = LoadCubemap();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL); // Important pour la skybox (depth = 1.0)
    glfwSwapInterval(1);
    return true;
}

// ---- Terminaison ----
void Terminate()
{
    glDeleteVertexArrays(1, &g_SkyboxVAO);
    glDeleteBuffers(1, &g_SkyboxVBO);
    glDeleteTextures(1, &g_CubemapTexture);
    g_SkyboxShader.Destroy();
}

// ---- Rendu ----
void Render(GLFWwindow* window)
{
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glClearColor(0.1f, 0.1f, 0.15f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float aspect = (height > 0) ? (float)width / height : 1.f;
    Mat4 proj = makePerspective(60.f, aspect, 0.1f, 1000.f);

    // Vue SANS translation : on supprime la colonne de translation en reconstruisant
    // la matrice de rotation seule à partir des angles orbitaux
    Mat4 viewRot = multiply(makeRotationX(g_elevation), makeRotationY(-g_azimuth));

    // ---- Rendu Skybox ----
    glDepthMask(GL_FALSE); // On n'écrit pas dans le depth buffer
    auto prog = g_SkyboxShader.GetProgram();
    glUseProgram(prog);

    glUniformMatrix4fv(glGetUniformLocation(prog, "u_projection"), 1, GL_FALSE, proj.m);
    glUniformMatrix4fv(glGetUniformLocation(prog, "u_view"),       1, GL_FALSE, viewRot.m);
    glUniform1i(glGetUniformLocation(prog, "u_skybox"), 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, g_CubemapTexture);

    glBindVertexArray(g_SkyboxVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE); // On réactive l'écriture depth
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{ glViewport(0, 0, width, height); }

int main()
{
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(960, 540,
        "TP Cube Mapping - Partie 1 : Skybox", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

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
