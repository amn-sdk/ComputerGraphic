#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

int main() {
    glfwInit();
    GLFWwindow* w = glfwCreateWindow(100, 100, "t", NULL, NULL);
    glfwMakeContextCurrent(w);
    glewExperimental = GL_TRUE;
    glewInit();
    std::cout << "glGenVertexArrays: " << (void*)glGenVertexArrays << "\n";
    std::cout << "glGenVertexArraysAPPLE: " << (void*)glGenVertexArraysAPPLE << "\n";
    glfwTerminate();
    return 0;
}
