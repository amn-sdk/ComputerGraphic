
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstddef>
#include <cmath>
#include <cstring>   
#include "common/GLShader.h"
#include "mat4.h"
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
static Vertex g_smoothVertices[8];
static int g_smoothIndices[36];
void BuildSmoothNormals()
{
    for (int i = 0; i < 8; i++)
    {
        g_smoothVertices[i].x  = cubePos[i][0];
        g_smoothVertices[i].y  = cubePos[i][1];
        g_smoothVertices[i].z  = cubePos[i][2];
        g_smoothVertices[i].nx = 0.f;
        g_smoothVertices[i].ny = 0.f;
        g_smoothVertices[i].nz = 0.f;
    }
    for (int i = 0; i < 36; i += 3)
    {
        int i0 = cubeIdx[i];
        int i1 = cubeIdx[i + 1];
        int i2 = cubeIdx[i + 2];
        float* p0 = cubePos[i0];
        float* p1 = cubePos[i1];
        float* p2 = cubePos[i2];
        float ax = p1[0]-p0[0], ay = p1[1]-p0[1], az = p1[2]-p0[2];
        float bx = p2[0]-p0[0], by = p2[1]-p0[1], bz = p2[2]-p0[2];
        float nx = ay*bz - az*by;
        float ny = az*bx - ax*bz;
        float nz = ax*by - ay*bx;
        g_smoothVertices[i0].nx += nx;
        g_smoothVertices[i0].ny += ny;
        g_smoothVertices[i0].nz += nz;
        g_smoothVertices[i1].nx += nx;
        g_smoothVertices[i1].ny += ny;
        g_smoothVertices[i1].nz += nz;
        g_smoothVertices[i2].nx += nx;
        g_smoothVertices[i2].ny += ny;
        g_smoothVertices[i2].nz += nz;
    }
    for (int i = 0; i < 8; i++)
    {
        float len = sqrtf(
            g_smoothVertices[i].nx * g_smoothVertices[i].nx +
            g_smoothVertices[i].ny * g_smoothVertices[i].ny +
            g_smoothVertices[i].nz * g_smoothVertices[i].nz
        );
        if (len > 0.0001f)
        {
            g_smoothVertices[i].nx /= len;
            g_smoothVertices[i].ny /= len;
            g_smoothVertices[i].nz /= len;
        }
    }
    for (int i = 0; i < 36; i++)
        g_smoothIndices[i] = cubeIdx[i];
}
GLShader g_Shader;
GLuint g_VAO, g_VBO, g_EBO;
bool Initialise()
{
    BuildSmoothNormals();
    g_Shader.LoadVertexShader("lambert.vs");
    g_Shader.LoadFragmentShader("lambert.fs");
    g_Shader.Create();
    glGenVertexArrays(1, &g_VAO);
    glBindVertexArray(g_VAO);
    glGenBuffers(1, &g_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, g_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_smoothVertices), g_smoothVertices, GL_STATIC_DRAW);
    glGenBuffers(1, &g_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_smoothIndices), g_smoothIndices, GL_STATIC_DRAW);
    auto program = g_Shader.GetProgram();
    int stride = sizeof(Vertex);
    int loc_position = glGetAttribLocation(program, "a_position");
    glEnableVertexAttribArray(loc_position);
    glVertexAttribPointer(loc_position, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, x));
    int loc_normal = glGetAttribLocation(program, "a_normal");
    glEnableVertexAttribArray(loc_normal);
    glVertexAttribPointer(loc_normal, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, nx));
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
    glDeleteVertexArrays(1, &g_VAO);
    glDeleteBuffers(1, &g_VBO);
    glDeleteBuffers(1, &g_EBO);
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
    Mat4 rotY = makeRotationY(time * 1.0f);
    Mat4 rotX = makeRotationX(time * 0.4f);
    Mat4 rot  = multiply(rotX, rotY);
    Mat4 trans = makeTranslation(0.f, 0.f, -5.f);
    Mat4 world = multiply(trans, rot);
    float aspect = (height > 0) ? (float)width / height : 1.f;
    Mat4 proj = makePerspective(60.f, aspect, 0.1f, 1000.f);
    Mat4 mvp = multiply(proj, world);
    auto program = g_Shader.GetProgram();
    glUseProgram(program);
    int loc_mvp = glGetUniformLocation(program, "u_mvp");
    glUniformMatrix4fv(loc_mvp, 1, GL_FALSE, mvp.m);
    int loc_worldRot = glGetUniformLocation(program, "u_worldRot");
    glUniformMatrix4fv(loc_worldRot, 1, GL_FALSE, rot.m);
    glBindVertexArray(g_VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
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
    GLFWwindow* window = glfwCreateWindow(960, 540, "Exercice 2.2 - Normales lissees (smooth)", NULL, NULL);
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
