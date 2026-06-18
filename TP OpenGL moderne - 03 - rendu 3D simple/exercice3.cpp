
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
struct Vertex
{
    float x, y, z;
    float nx, ny, nz;
};
static float cubePos[8][3] = {
    {-1.f,-1.f, 1.f}, { 1.f,-1.f, 1.f}, { 1.f, 1.f, 1.f}, {-1.f, 1.f, 1.f},
    {-1.f,-1.f,-1.f}, { 1.f,-1.f,-1.f}, { 1.f, 1.f,-1.f}, {-1.f, 1.f,-1.f}
};
static int cubeIdx[36] = {
    0,1,2, 2,3,0,  1,5,6, 6,2,1,  7,6,5, 5,4,7,
    4,0,3, 3,7,4,  4,5,1, 1,0,4,  3,2,6, 6,7,3
};
static Vertex g_cubeData[36];
void BuildCubeNormals()
{
    for (int i = 0; i < 36; i += 3)
    {
        float* p0 = cubePos[cubeIdx[i]];
        float* p1 = cubePos[cubeIdx[i + 1]];
        float* p2 = cubePos[cubeIdx[i + 2]];
        float ax = p1[0]-p0[0], ay = p1[1]-p0[1], az = p1[2]-p0[2];
        float bx = p2[0]-p0[0], by = p2[1]-p0[1], bz = p2[2]-p0[2];
        float nx = ay*bz - az*by;
        float ny = az*bx - ax*bz;
        float nz = ax*by - ay*bx;
        float len = sqrtf(nx*nx + ny*ny + nz*nz);
        nx /= len; ny /= len; nz /= len;
        for (int j = 0; j < 3; j++)
        {
            float* p = cubePos[cubeIdx[i + j]];
            g_cubeData[i + j] = { p[0], p[1], p[2], nx, ny, nz };
        }
    }
}
GLShader g_Shader;
GLuint g_CubeVAO, g_CubeVBO;
GLuint g_DragonVAO, g_DragonVBO, g_DragonEBO;
static const int NB_DRAGON_INDICES = 45000;
bool Initialise()
{
    BuildCubeNormals();
    g_Shader.LoadVertexShader("lambert.vs");
    g_Shader.LoadFragmentShader("lambert.fs");
    g_Shader.Create();
    auto program = g_Shader.GetProgram();
    glGenVertexArrays(1, &g_CubeVAO);
    glBindVertexArray(g_CubeVAO);
    glGenBuffers(1, &g_CubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, g_CubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_cubeData), g_cubeData, GL_STATIC_DRAW);
    int stride_cube = sizeof(Vertex);
    int loc_pos = glGetAttribLocation(program, "a_position");
    glEnableVertexAttribArray(loc_pos);
    glVertexAttribPointer(loc_pos, 3, GL_FLOAT, GL_FALSE, stride_cube, (void*)offsetof(Vertex, x));
    int loc_nor = glGetAttribLocation(program, "a_normal");
    glEnableVertexAttribArray(loc_nor);
    glVertexAttribPointer(loc_nor, 3, GL_FLOAT, GL_FALSE, stride_cube, (void*)offsetof(Vertex, nx));
    glBindVertexArray(0);
    glGenVertexArrays(1, &g_DragonVAO);
    glBindVertexArray(g_DragonVAO);
    glGenBuffers(1, &g_DragonVBO);
    glBindBuffer(GL_ARRAY_BUFFER, g_DragonVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(DragonVertices), DragonVertices, GL_STATIC_DRAW);
    glGenBuffers(1, &g_DragonEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_DragonEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(DragonIndices), DragonIndices, GL_STATIC_DRAW);
    int stride_dragon = 8 * sizeof(float);
    int loc_pos2 = glGetAttribLocation(program, "a_position");
    glEnableVertexAttribArray(loc_pos2);
    glVertexAttribPointer(loc_pos2, 3, GL_FLOAT, GL_FALSE, stride_dragon, (void*)0);
    int loc_nor2 = glGetAttribLocation(program, "a_normal");
    glEnableVertexAttribArray(loc_nor2);
    glVertexAttribPointer(loc_nor2, 3, GL_FLOAT, GL_FALSE, stride_dragon, (void*)(3 * sizeof(float)));
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
    glDeleteVertexArrays(1, &g_CubeVAO);
    glDeleteBuffers(1, &g_CubeVBO);
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
    glClearColor(0.1f, 0.1f, 0.15f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    float time = (float)glfwGetTime();
    float aspect = (height > 0) ? (float)width / height : 1.f;
    Mat4 proj = makePerspective(60.f, aspect, 0.1f, 1000.f);
    auto program = g_Shader.GetProgram();
    glUseProgram(program);
    int loc_mvp      = glGetUniformLocation(program, "u_mvp");
    int loc_worldRot = glGetUniformLocation(program, "u_worldRot");
    {
        Mat4 rotY  = makeRotationY(time * 1.0f);
        Mat4 rotX  = makeRotationX(time * 0.4f);
        Mat4 rot   = multiply(rotX, rotY);
        Mat4 trans = makeTranslation(-3.f, 0.f, -8.f);
        Mat4 world = multiply(trans, rot);
        Mat4 mvp   = multiply(proj, world);
        glUniformMatrix4fv(loc_mvp, 1, GL_FALSE, mvp.m);
        glUniformMatrix4fv(loc_worldRot, 1, GL_FALSE, rot.m);
        glBindVertexArray(g_CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }
    {
        Mat4 rotY  = makeRotationY(time * 0.8f);
        Mat4 trans = makeTranslation(2.f, -2.f, -12.f);
        Mat4 world = multiply(trans, rotY);
        Mat4 mvp   = multiply(proj, world);
        glUniformMatrix4fv(loc_mvp, 1, GL_FALSE, mvp.m);
        glUniformMatrix4fv(loc_worldRot, 1, GL_FALSE, rotY.m);
        glBindVertexArray(g_DragonVAO);
        glDrawElements(GL_TRIANGLES, NB_DRAGON_INDICES, GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);
    }
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
int main()
{
    if (!glfwInit())
        return -1;
    GLFWwindow* window = glfwCreateWindow(960, 540, "Exercice 3 - Cube + Dragon", NULL, NULL);
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
