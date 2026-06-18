
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstddef>
#include <cmath>
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
static float positions[8][3] = {
    {-1.f,-1.f, 1.f}, { 1.f,-1.f, 1.f}, { 1.f, 1.f, 1.f}, {-1.f, 1.f, 1.f},
    {-1.f,-1.f,-1.f}, { 1.f,-1.f,-1.f}, { 1.f, 1.f,-1.f}, {-1.f, 1.f,-1.f}
};
static int indices[36] = {
    0,1,2, 2,3,0,   
    1,5,6, 6,2,1,   
    7,6,5, 5,4,7,   
    4,0,3, 3,7,4,   
    4,5,1, 1,0,4,   
    3,2,6, 6,7,3    
};
static Vertex g_cubeData[36];
void BuildCubeWithNormals()
{
    for (int i = 0; i < 36; i += 3)
    {
        float* p0 = positions[indices[i]];
        float* p1 = positions[indices[i + 1]];
        float* p2 = positions[indices[i + 2]];
        float ax = p1[0] - p0[0], ay = p1[1] - p0[1], az = p1[2] - p0[2];
        float bx = p2[0] - p0[0], by = p2[1] - p0[1], bz = p2[2] - p0[2];
        float nx = ay * bz - az * by;
        float ny = az * bx - ax * bz;
        float nz = ax * by - ay * bx;
        float len = sqrtf(nx * nx + ny * ny + nz * nz);
        nx /= len; ny /= len; nz /= len;
        for (int j = 0; j < 3; j++)
        {
            float* p = positions[indices[i + j]];
            g_cubeData[i + j] = { p[0], p[1], p[2], nx, ny, nz };
        }
    }
}
GLShader g_Shader;
GLuint g_VAO, g_VBO;
bool Initialise()
{
    BuildCubeWithNormals();
    g_Shader.LoadVertexShader("lambert.vs");
    g_Shader.LoadFragmentShader("lambert.fs");
    g_Shader.Create();
    glGenVertexArrays(1, &g_VAO);
    glBindVertexArray(g_VAO);
    glGenBuffers(1, &g_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, g_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_cubeData), g_cubeData, GL_STATIC_DRAW);
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
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glfwSwapInterval(1);
    return true;
}
void Terminate()
{
    glDeleteVertexArrays(1, &g_VAO);
    glDeleteBuffers(1, &g_VBO);
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
    Mat4 worldRot = rot;
    auto program = g_Shader.GetProgram();
    glUseProgram(program);
    int loc_mvp = glGetUniformLocation(program, "u_mvp");
    glUniformMatrix4fv(loc_mvp, 1, GL_FALSE, mvp.m);
    int loc_worldRot = glGetUniformLocation(program, "u_worldRot");
    glUniformMatrix4fv(loc_worldRot, 1, GL_FALSE, worldRot.m);
    glBindVertexArray(g_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
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
    GLFWwindow* window = glfwCreateWindow(960, 540, "Exercice 2.1 - Normales (hard edges)", NULL, NULL);
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
