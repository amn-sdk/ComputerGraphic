
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
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
GLuint   g_Texture0;  
GLuint   g_Texture1;  
int g_mode = 2;
static float g_quadVertices[] = {
   -0.8f, -0.8f,  0.f, 0.f,
    0.8f, -0.8f,  1.f, 0.f,
    0.8f,  0.8f,  1.f, 1.f,
   -0.8f, -0.8f,  0.f, 0.f,
    0.8f,  0.8f,  1.f, 1.f,
   -0.8f,  0.8f,  0.f, 1.f
};
GLuint LoadTexture(const char* filename)
{
    int w, h, comp;
    uint8_t* data = stbi_load(filename, &w, &h, &comp, STBI_rgb_alpha);
    if (!data)
    {
        std::cerr << "Erreur chargement : " << filename << std::endl;
        return 0;
    }
    std::cout << "Texture chargee : " << filename << " (" << w << "x" << h << ")" << std::endl;
    GLuint texId;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texId;
}
bool Initialise()
{
    g_Shader.LoadVertexShader("exo3.vs");
    g_Shader.LoadFragmentShader("exo3.fs");
    g_Shader.Create();
    auto program = g_Shader.GetProgram();
    g_Texture0 = LoadTexture("texture.png");   
    g_Texture1 = LoadTexture("texture2.png");  
    if (g_Texture0 == 0 || g_Texture1 == 0)
        return false;
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
    glDeleteTextures(1, &g_Texture0);
    glDeleteTextures(1, &g_Texture1);
    g_Shader.Destroy();
}
void Render(GLFWwindow* window)
{
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glClearColor(0.15f, 0.15f, 0.2f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    auto program = g_Shader.GetProgram();
    glUseProgram(program);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_Texture0);
    glUniform1i(glGetUniformLocation(program, "u_sampler0"), 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, g_Texture1);
    glUniform1i(glGetUniformLocation(program, "u_sampler1"), 1);
    glUniform1i(glGetUniformLocation(program, "u_mode"), g_mode);
    glBindVertexArray(g_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS) return;
    if (key == GLFW_KEY_1)
    {
        g_mode = 0;
        std::cout << "Mode : ADDITION (color0 + color1)" << std::endl;
    }
    else if (key == GLFW_KEY_2)
    {
        g_mode = 1;
        std::cout << "Mode : MULTIPLICATION (color0 * color1)" << std::endl;
    }
    else if (key == GLFW_KEY_3)
    {
        g_mode = 2;
        std::cout << "Mode : MIX 50/50 (mix(color0, color1, 0.5))" << std::endl;
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
        "Exercice 3 - 2 Textures (1=Add  2=Multiply  3=Mix)", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) return -1;
    if (!Initialise()) { glfwTerminate(); return -1; }
    std::cout << "Touches : 1 = Addition | 2 = Multiplication | 3 = Mix 50/50" << std::endl;
    std::cout << "Mode actuel : MIX 50/50" << std::endl;
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
