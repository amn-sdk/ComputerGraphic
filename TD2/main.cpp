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

struct Vector2f {
    float x, y;
};

struct Color3f {
    float r, g, b;
};

struct Vertex {
    Vector2f position;
    Color3f color;
};

GLShader g_BasicShader;
GLuint g_VBO, g_EBO, g_VAO;

bool Initialise()
{
    g_BasicShader.LoadVertexShader("basic.vs");
    g_BasicShader.LoadFragmentShader("basic.fs");
    g_BasicShader.Create();

    static const Vertex vertices[] = {
        { { 0.5f,  0.5f}, {1.0f, 0.0f, 0.0f} }, // Haut droite (Rouge)
        { { 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f} }, // Bas droite   (Vert)
        { {-0.5f, -0.5f}, {0.0f, 0.0f, 1.0f} }, // Bas gauche   (Bleu)
        { {-0.5f,  0.5f}, {1.0f, 1.0f, 0.0f} }  // Haut gauche  (Jaune)
    };

    static const unsigned int indices[] = {
        0, 1, 3, // Premier triangle
        1, 2, 3  // Deuxième triangle
    };

    glGenVertexArrays(1, &g_VAO);
    glBindVertexArray(g_VAO);

    glGenBuffers(1, &g_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, g_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &g_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    auto basicProgram = g_BasicShader.GetProgram();
    int loc_position = glGetAttribLocation(basicProgram, "a_position");
    int loc_color = glGetAttribLocation(basicProgram, "a_color");

    glEnableVertexAttribArray(loc_position);
    glEnableVertexAttribArray(loc_color);

    int stride = sizeof(Vertex);
    glVertexAttribPointer(loc_position, 2, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, position));
    glVertexAttribPointer(loc_color, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, color));

    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

#ifdef WIN32
    wglSwapIntervalEXT(1);
#else
    glfwSwapInterval(1);
#endif

    return true;
}

void Terminate() {
    glDeleteVertexArrays(1, &g_VAO);
    glDeleteBuffers(1, &g_EBO);
    glDeleteBuffers(1, &g_VBO);
    g_BasicShader.Destroy();
}

void Render(GLFWwindow* window) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    glViewport(0, 0, width, height);

    glClearColor(0.5f, 0.5f, 0.5f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    auto basicProgram = g_BasicShader.GetProgram();
    glUseProgram(basicProgram);

    glBindVertexArray(g_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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

    GLFWwindow* window = glfwCreateWindow(800, 600, "TD1", NULL, NULL);
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

    if (!Initialise()) {
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
