
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstddef>
#include "common/GLShader.h"
#ifdef __APPLE__
#undef glGenVertexArrays
#undef glBindVertexArray
#undef glDeleteVertexArrays
#define glGenVertexArrays glGenVertexArraysAPPLE
#define glBindVertexArray glBindVertexArrayAPPLE
#define glDeleteVertexArrays glDeleteVertexArraysAPPLE
#endif
GLShader g_Shader;
GLuint   g_VAO, g_VBO;
GLuint   g_Texture;
bool g_useNearest = false;
static float g_quadVertices[] = {
   -0.8f, -0.8f,  0.f, 0.f,  
    0.8f, -0.8f,  2.f, 0.f,  
    0.8f,  0.8f,  2.f, 2.f,  
   -0.8f, -0.8f,  0.f, 0.f,  
    0.8f,  0.8f,  2.f, 2.f,  
   -0.8f,  0.8f,  0.f, 2.f   
};
GLuint CreateTexture2x2(GLint filterMode)
{
    uint8_t data[] = {
        255, 0,   0,   255,  
        0,   255, 0,   255,  
        0,   0,   255, 255,  
        255, 255, 255, 255   
    };
    GLuint texId;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 2, 2, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texId;
}
bool Initialise()
{
    g_Shader.LoadVertexShader("exo1.vs");
    g_Shader.LoadFragmentShader("exo1.fs");
    g_Shader.Create();
    auto program = g_Shader.GetProgram();
    g_Texture = CreateTexture2x2(GL_LINEAR);
    glGenVertexArrays(1, &g_VAO);
    glBindVertexArray(g_VAO);
    glGenBuffers(1, &g_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, g_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_quadVertices), g_quadVertices, GL_STATIC_DRAW);
    int stride = 4 * sizeof(float); 
    int loc_pos = glGetAttribLocation(program, "a_position");
    glEnableVertexAttribArray(loc_pos);
    glVertexAttribPointer(loc_pos, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    int loc_uv = glGetAttribLocation(program, "a_texcoords");
    glEnableVertexAttribArray(loc_uv);
    glVertexAttribPointer(loc_uv, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glfwSwapInterval(1);
    return true;
}
void Terminate()
{
    glDeleteVertexArrays(1, &g_VAO);
    glDeleteBuffers(1, &g_VBO);
    glDeleteTextures(1, &g_Texture);
    g_Shader.Destroy();
}
void Render(GLFWwindow* window)
{
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glClearColor(0.2f, 0.2f, 0.25f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    auto program = g_Shader.GetProgram();
    glUseProgram(program);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_Texture);
    glUniform1i(glGetUniformLocation(program, "u_sampler"), 0);
    glBindVertexArray(g_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        g_useNearest = !g_useNearest;
        GLint mode = g_useNearest ? GL_NEAREST : GL_LINEAR;
        glDeleteTextures(1, &g_Texture);
        g_Texture = CreateTexture2x2(mode);
        if (g_useNearest)
            std::cout << "Filtre : GL_NEAREST (pixelise)" << std::endl;
        else
            std::cout << "Filtre : GL_LINEAR (lisse)" << std::endl;
    }
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
int main()
{
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(800, 600,
        "Exercice 1 - Texture 2x2 (ESPACE = changer filtre)", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) return -1;
    if (!Initialise()) { glfwTerminate(); return -1; }
    std::cout << "ESPACE = basculer entre GL_NEAREST et GL_LINEAR" << std::endl;
    std::cout << "Debut : GL_LINEAR (bords lisses)" << std::endl;
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
