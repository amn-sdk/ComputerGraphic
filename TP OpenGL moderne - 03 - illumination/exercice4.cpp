
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
GLuint   g_Texture;
GLuint CreateCheckerTexture()
{
    const int SIZE   = 256;
    const int SQUARE = 16;
    unsigned char pixels[SIZE * SIZE * 3];
    for (int y = 0; y < SIZE; y++)
    for (int x = 0; x < SIZE; x++)
    {
        int checker = ((x / SQUARE) + (y / SQUARE)) % 2;
        int idx = (y * SIZE + x) * 3;
        if (checker == 0) { pixels[idx]=190; pixels[idx+1]=110; pixels[idx+2]=50; }
        else              { pixels[idx]=90;  pixels[idx+1]=50;  pixels[idx+2]=20; }
    }
    GLuint texId;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SIZE, SIZE, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texId;
}
bool Initialise()
{
    g_Shader.LoadVertexShader("exo4.vs");
    g_Shader.LoadFragmentShader("exo4.fs");
    g_Shader.Create();
    auto program = g_Shader.GetProgram();
    g_Texture = CreateCheckerTexture();
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
    int loc_uv = glGetAttribLocation(program, "a_texcoord");
    glEnableVertexAttribArray(loc_uv);
    glVertexAttribPointer(loc_uv, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
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
    glDeleteTextures(1, &g_Texture);
    g_Shader.Destroy();
}
void setUniform3f(GLuint program, const char* name, float x, float y, float z)
{
    int loc = glGetUniformLocation(program, name);
    glUniform3f(loc, x, y, z);
}
void setUniform1f(GLuint program, const char* name, float val)
{
    int loc = glGetUniformLocation(program, name);
    glUniform1f(loc, val);
}
void Render(GLFWwindow* window)
{
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glClearColor(0.1f, 0.1f, 0.15f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    float time = (float)glfwGetTime();
    Mat4 rotY  = makeRotationY(time * 0.8f);
    Mat4 trans = makeTranslation(0.f, -2.f, -10.f);
    Mat4 world = multiply(trans, rotY);
    float aspect = (height > 0) ? (float)width / height : 1.f;
    Mat4 proj    = makePerspective(60.f, aspect, 0.1f, 1000.f);
    Mat4 mvp     = multiply(proj, world);
    auto program = g_Shader.GetProgram();
    glUseProgram(program);
    glUniformMatrix4fv(glGetUniformLocation(program, "u_mvp"),      1, GL_FALSE, mvp.m);
    glUniformMatrix4fv(glGetUniformLocation(program, "u_world"),    1, GL_FALSE, world.m);
    glUniformMatrix4fv(glGetUniformLocation(program, "u_worldRot"), 1, GL_FALSE, rotY.m);
    setUniform3f(program, "u_light.direction",     0.5f, -0.5f, -1.f); 
    setUniform3f(program, "u_light.ambientColor",  0.3f, 0.3f,  0.3f); 
    setUniform3f(program, "u_light.diffuseColor",  1.f,  1.f,   1.f);  
    setUniform3f(program, "u_light.specularColor", 1.f,  1.f,   1.f);  
    setUniform3f(program, "u_material.ambientColor",  0.3f, 0.2f, 0.1f); 
    setUniform3f(program, "u_material.specularColor", 1.f,  1.f,  1.f);  
    setUniform1f(program, "u_material.shininess",     16.f);             
    setUniform3f(program, "u_cameraPos", 0.f, 0.f, 0.f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_Texture);
    glUniform1i(glGetUniformLocation(program, "u_texture"), 0);
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
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(960, 540,
        "Exercice 4 - Phong Complet (Ambient + Diffuse + Specular)", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
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
