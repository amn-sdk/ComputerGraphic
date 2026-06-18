
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstddef>
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
static const GLfloat cube_vertices[] = {
    -1.0f,-1.0f, 1.0f,  1.0f,-1.0f, 1.0f,  1.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,  1.0f,-1.0f,-1.0f,  1.0f, 1.0f,-1.0f,  -1.0f, 1.0f,-1.0f
};
static const GLushort cube_elements[] = {
    0,1,2, 2,3,0,   
    1,5,6, 6,2,1,   
    7,6,5, 5,4,7,   
    4,0,3, 3,7,4,   
    4,5,1, 1,0,4,   
    3,2,6, 6,7,3    
};
GLShader g_Shader;
GLuint g_VAO, g_VBO, g_EBO;
bool Initialise()
{
    g_Shader.LoadVertexShader("color.vs");
    g_Shader.LoadFragmentShader("color.fs");
    g_Shader.Create();
    glGenVertexArrays(1, &g_VAO);
    glBindVertexArray(g_VAO);
    glGenBuffers(1, &g_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, g_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
    glGenBuffers(1, &g_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);
    auto program = g_Shader.GetProgram();
    int loc_position = glGetAttribLocation(program, "a_position");
    glEnableVertexAttribArray(loc_position);
    glVertexAttribPointer(loc_position, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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
    glClear(GL_COLOR_BUFFER_BIT);
    float time = (float)glfwGetTime();
    Mat4 rotY = makeRotationY(time * 1.0f);
    Mat4 rotX = makeRotationX(time * 0.4f);
    Mat4 rotZ = makeRotationZ(0.f);
    Mat4 rot  = multiply(rotZ, multiply(rotX, rotY));
    Mat4 trans = makeTranslation(0.f, 0.f, -5.f);
    Mat4 world = multiply(trans, rot);
    float aspect = (height > 0) ? (float)width / height : 1.f;
    Mat4 proj = makePerspective(60.f, aspect, 0.1f, 1000.f);
    Mat4 mvp = multiply(proj, world);
    auto program = g_Shader.GetProgram();
    glUseProgram(program);
    int loc_mvp = glGetUniformLocation(program, "u_mvp");
    glUniformMatrix4fv(loc_mvp, 1, GL_FALSE, mvp.m);
    glBindVertexArray(g_VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
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
    GLFWwindow* window = glfwCreateWindow(960, 540, "Exercice 1 - Cube 3D perspective", NULL, NULL);
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
