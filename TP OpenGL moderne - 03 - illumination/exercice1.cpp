
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstddef>
#include <cmath>
#include "common/GLShader.h"
#include "mat4.h"
#include "dragondata.h"
#ifdef __APPLE__
#undef glGenVertexArrays
#undef glBindVertexArray
#undef glDeleteVertexArrays
#define glGenVertexArrays glGenVertexArraysAPPLE
#define glBindVertexArray glBindVertexArrayAPPLE
#define glDeleteVertexArrays glDeleteVertexArraysAPPLE
#endif
static const int NB_DRAGON_INDICES = 45000;
GLShader g_Shader;
GLuint   g_DragonVAO, g_DragonVBO, g_DragonEBO;
bool Initialise()
{
    g_Shader.LoadVertexShader("exo1.vs");
    g_Shader.LoadFragmentShader("exo1.fs");
    g_Shader.Create();
    auto program = g_Shader.GetProgram();
    glGenVertexArrays(1, &g_DragonVAO);
    glBindVertexArray(g_DragonVAO);
    glGenBuffers(1, &g_DragonVBO);
    glBindBuffer(GL_ARRAY_BUFFER, g_DragonVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(DragonVertices), DragonVertices, GL_STATIC_DRAW);
    glGenBuffers(1, &g_DragonEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_DragonEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(DragonIndices), DragonIndices, GL_STATIC_DRAW);
    int stride = 8 * sizeof(float); 
    int loc_pos = glGetAttribLocation(program, "a_position");
    glEnableVertexAttribArray(loc_pos);
    glVertexAttribPointer(loc_pos, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    int loc_nor = glGetAttribLocation(program, "a_normal");
    glEnableVertexAttribArray(loc_nor);
    glVertexAttribPointer(loc_nor, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glfwSwapInterval(1);
    return true;
}
void Terminate()
{
    glDeleteVertexArrays(1, &g_DragonVAO);
    glDeleteBuffers(1, &g_DragonVBO);
    glDeleteBuffers(1, &g_DragonEBO);
    g_Shader.Destroy();
}
void Render(GLFWwindow* window)
{
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glClearColor(0.12f, 0.12f, 0.18f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    float time = (float)glfwGetTime();
    Mat4 rotY  = makeRotationY(time * 0.8f);
    Mat4 trans = makeTranslation(0.f, -2.f, -10.f);
    Mat4 world = multiply(trans, rotY);
    float aspect = (height > 0) ? (float)width / height : 1.f;
    Mat4 proj = makePerspective(60.f, aspect, 0.1f, 1000.f);
    Mat4 mvp = multiply(proj, world);
    auto program = g_Shader.GetProgram();
    glUseProgram(program);
    int loc_mvp = glGetUniformLocation(program, "u_mvp");
    glUniformMatrix4fv(loc_mvp, 1, GL_FALSE, mvp.m);
    int loc_worldRot = glGetUniformLocation(program, "u_worldRot");
    glUniformMatrix4fv(loc_worldRot, 1, GL_FALSE, rotY.m);
    float lightDir[3] = { 1.f, -1.f, -1.f }; 
    int loc_lightDir = glGetUniformLocation(program, "u_lightDir");
    glUniform3fv(loc_lightDir, 1, lightDir);
    float lightColor[3] = { 1.f, 1.f, 1.f };
    int loc_lightColor = glGetUniformLocation(program, "u_lightColor");
    glUniform3fv(loc_lightColor, 1, lightColor);
    glBindVertexArray(g_DragonVAO);
    glDrawElements(GL_TRIANGLES, NB_DRAGON_INDICES, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
int main()
{
    if (!glfwInit())
        return -1;
    GLFWwindow* window = glfwCreateWindow(960, 540,
        "Exercice 1 - Diffuse Lambert (Dragon)", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
        return -1;
    if (!Initialise())
    {
        glfwTerminate();
        return -1;
    }
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
