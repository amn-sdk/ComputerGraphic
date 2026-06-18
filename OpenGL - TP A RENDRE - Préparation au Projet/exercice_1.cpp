
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

// Variables pour la camera orbitale
float cam_radius = 15.0f;
float cam_theta = 0.3f; // elevation
float cam_phi = 0.0f;   // azimuth
double last_mouse_x = 0;
double last_mouse_y = 0;
bool mouse_pressed = false;

void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
    if (mouse_pressed) {
        float dx = (float)(xpos - last_mouse_x);
        float dy = (float)(ypos - last_mouse_y);
        cam_phi -= dx * 0.01f;
        cam_theta += dy * 0.01f;
        
        if (cam_theta > 1.57f) cam_theta = 1.57f;
        if (cam_theta < -1.57f) cam_theta = -1.57f;
    }
    last_mouse_x = xpos;
    last_mouse_y = ypos;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) mouse_pressed = true;
        else if (action == GLFW_RELEASE) mouse_pressed = false;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    cam_radius -= (float)yoffset * 0.5f;
    if (cam_radius < 1.0f) cam_radius = 1.0f;
    if (cam_radius > 50.0f) cam_radius = 50.0f;
}
GLuint CreateCheckerTexture()
{
    const int SIZE = 256, SQUARE = 16;
    unsigned char pixels[SIZE * SIZE * 3];
    for (int y = 0; y < SIZE; y++)
    for (int x = 0; x < SIZE; x++)
    {
        int c   = ((x / SQUARE) + (y / SQUARE)) % 2;
        int idx = (y * SIZE + x) * 3;
        if (c == 0) { pixels[idx]=190; pixels[idx+1]=110; pixels[idx+2]=50; }
        else        { pixels[idx]=90;  pixels[idx+1]=50;  pixels[idx+2]=20; }
    }
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SIZE, SIZE, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);
    return id;
}
bool Initialise()
{
    g_Shader.LoadVertexShader("exo.vs");
    g_Shader.LoadFragmentShader("exo.fs");
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
void setUniform3f(GLuint prog, const char* name, float x, float y, float z)
{ glUniform3f(glGetUniformLocation(prog, name), x, y, z); }
void setUniform1f(GLuint prog, const char* name, float v)
{ glUniform1f(glGetUniformLocation(prog, name), v); }
void Render(GLFWwindow* window)
{
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glClearColor(0.1f, 0.1f, 0.15f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    float time  = (float)glfwGetTime();
    
    // Calcul de la position de la camera en spheriques
    float Y = cam_radius * sinf(cam_theta);
    float X = cam_radius * cosf(cam_theta) * cosf(cam_phi);
    float Z = cam_radius * cosf(cam_theta) * sinf(cam_phi);
    
    vec3 camPos(X, Y, Z);
    vec3 target(0.0f, 0.0f, 0.0f); // Le target est maitenant au centre (le dragon est descendu via WorldMatrix)
    vec3 up(0.0f, 1.0f, 0.0f);
    
    Mat4 view = makeLookAt(camPos, target, up);
    
    // EXERCICE 1.2 : "World Matrix = Translation * Rotation * Scale"
    Mat4 scale = makeScale(1.0f, 1.0f, 1.0f);
    Mat4 rot   = makeRotationY(time * 0.5f);
    Mat4 trans = makeTranslation(0.0f, -2.0f, 0.0f); // On descend le dragon ici
    
    // L'ordre se lit de droite à gauche : trans * (rot * scale)
    Mat4 world = multiply(trans, multiply(rot, scale));
    
    Mat4 proj = makePerspective(60.f, (height > 0) ? (float)width/height : 1.f, 0.1f, 1000.f);
    
    auto program = g_Shader.GetProgram();
    glUseProgram(program);
    
    glUniformMatrix4fv(glGetUniformLocation(program, "u_proj"),  1, GL_FALSE, proj.m);
    glUniformMatrix4fv(glGetUniformLocation(program, "u_view"),  1, GL_FALSE, view.m);
    glUniformMatrix4fv(glGetUniformLocation(program, "u_world"), 1, GL_FALSE, world.m);
    
    setUniform3f(program, "u_light.direction",     0.5f, -0.5f, -1.f);
    setUniform3f(program, "u_light.diffuseColor",  1.f,  1.f,   1.f);
    setUniform3f(program, "u_light.specularColor", 1.f,  1.f,   1.f);
    setUniform3f(program, "u_material.specularColor", 1.f, 1.f, 1.f);
    setUniform1f(program, "u_material.shininess", 16.f);
    setUniform3f(program, "u_skyDirection", 0.f, 1.f, 0.f);
    setUniform3f(program, "u_skyColor", 0.3f, 0.5f, 0.9f);
    setUniform3f(program, "u_groundColor", 0.2f, 0.3f, 0.1f);
    setUniform3f(program, "u_Ia", 0.6f, 0.6f, 0.6f);
    
    setUniform3f(program, "u_cameraPos", camPos.x, camPos.y, camPos.z);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_Texture);
    glUniform1i(glGetUniformLocation(program, "u_texture"), 0);
    glBindVertexArray(g_DragonVAO);
    glDrawElements(GL_TRIANGLES, NB_DRAGON_INDICES, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{ glViewport(0, 0, width, height); }
int main()
{
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(960, 540,
        "TP Camera Orbitale", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
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
