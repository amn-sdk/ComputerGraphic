
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <cstddef>
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION
#include "common/GLShader.h"
#include "mat4.h"
#include "dragondata.h"
#include "camera.h"
#include "model.h"

GLShader g_SceneShader;
GLShader g_SkyboxShader;
GLShader g_ScreenShader;

GLuint g_SkyboxVAO, g_SkyboxVBO;
GLuint g_ScreenVAO, g_ScreenVBO;

GLuint g_CubemapTexture;
GLuint g_DragonVAO, g_DragonVBO, g_DragonEBO;

static const int NB_DRAGON_VERTICES = 16643;
static const int NB_DRAGON_INDICES  = 45000;

OBJModel g_PyramidModel;
OBJModel g_CubeModel;
OBJModel g_ConeModel;
GLuint g_WhiteTexture; 

GLuint g_CameraUBO;

GLuint g_ModelUBO;

const int NB_INSTANCES = 5;
GLuint g_InstanceVBOs[3]; 

struct InstanceData {
    Mat4 world;
    float color[4];
    float shadingMode; 
};
InstanceData g_InstanceMatrices[NB_INSTANCES];

GLuint g_FBO;
GLuint g_FBOTex;
GLuint g_FBODepthRB;
int    g_FBOWidth  = 960;
int    g_FBOHeight = 540;

Camera g_Camera;
float g_LastFrameTime = 0.0f;

int   g_PostProcessMode    = 0;
float g_BackLightStrength  = 0.4f;

int  g_CameraTargetIndex = 0;
vec3 g_CameraTargets[3] = {
    vec3(0.0f, -1.4f, -1.5f),  
    vec3(6.0f, -1.4f, -1.5f),  
    vec3(-6.0f, -1.4f, -1.5f)  
};

bool g_Keys[1024] = {};

void CheckGLError(const char* label)
{
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
        std::cerr << "GL Error [" << label << "]: 0x" << std::hex << err << std::dec << std::endl;
}

static const float skyboxVertices[] = {
    -1.f,  1.f, -1.f,  -1.f, -1.f, -1.f,   1.f, -1.f, -1.f,
     1.f, -1.f, -1.f,   1.f,  1.f, -1.f,  -1.f,  1.f, -1.f,
    -1.f, -1.f,  1.f,  -1.f, -1.f, -1.f,  -1.f,  1.f, -1.f,
    -1.f,  1.f, -1.f,  -1.f,  1.f,  1.f,  -1.f, -1.f,  1.f,
     1.f, -1.f, -1.f,   1.f, -1.f,  1.f,   1.f,  1.f,  1.f,
     1.f,  1.f,  1.f,   1.f,  1.f, -1.f,   1.f, -1.f, -1.f,
    -1.f, -1.f,  1.f,  -1.f,  1.f,  1.f,   1.f,  1.f,  1.f,
     1.f,  1.f,  1.f,   1.f, -1.f,  1.f,  -1.f, -1.f,  1.f,
    -1.f,  1.f, -1.f,   1.f,  1.f, -1.f,   1.f,  1.f,  1.f,
     1.f,  1.f,  1.f,  -1.f,  1.f,  1.f,  -1.f,  1.f, -1.f,
    -1.f, -1.f, -1.f,  -1.f, -1.f,  1.f,   1.f, -1.f, -1.f,
     1.f, -1.f, -1.f,  -1.f, -1.f,  1.f,   1.f, -1.f,  1.f
};

static const float screenQuadVertices[] = {
    
    -1.f, -1.f,   0.f, 0.f,
     1.f, -1.f,   1.f, 0.f,
     1.f,  1.f,   1.f, 1.f,
    -1.f, -1.f,   0.f, 0.f,
     1.f,  1.f,   1.f, 1.f,
    -1.f,  1.f,   0.f, 1.f
};

GLuint LoadCubemap()
{
    const char* faces[6] = {
        "envmaps/pisa_posx.jpg", "envmaps/pisa_negx.jpg",
        "envmaps/pisa_posy.jpg", "envmaps/pisa_negy.jpg",
        "envmaps/pisa_posz.jpg", "envmaps/pisa_negz.jpg"
    };
    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texID);
    for (int i = 0; i < 6; i++)
    {
        int w, h, comp;
        unsigned char* data = stbi_load(faces[i], &w, &h, &comp, STBI_rgb_alpha);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
            std::cout << "Cubemap face chargee : " << faces[i] << " (" << w << "x" << h << ")" << std::endl;
        }
        else
            std::cerr << "Erreur chargement cubemap : " << faces[i] << std::endl;
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    return texID;
}

bool CreateFBO(int w, int h)
{
    glGenFramebuffers(1, &g_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, g_FBO);

    glGenTextures(1, &g_FBOTex);
    glBindTexture(GL_TEXTURE_2D, g_FBOTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_FBOTex, 0);

    glGenRenderbuffers(1, &g_FBODepthRB);
    glBindRenderbuffer(GL_RENDERBUFFER, g_FBODepthRB);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, w, h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, g_FBODepthRB);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "FBO incomplet ! Status = 0x" << std::hex << status << std::dec << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    return true;
}

static void SetupInstancingAttributes(OBJModel& model, GLuint instanceVBO)
{
    for (auto& mesh : model.meshes) {
        glBindVertexArray(mesh.vao);
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        
        int stride = sizeof(InstanceData);
        
        for (int i = 0; i < 4; i++) {
            glEnableVertexAttribArray(3 + i);
            glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, stride, (void*)(i * sizeof(float) * 4));
            glVertexAttribDivisor(3 + i, 1);
        }
        
        glEnableVertexAttribArray(7);
        glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(Mat4)));
        glVertexAttribDivisor(7, 1);
        
        glEnableVertexAttribArray(8);
        glVertexAttribPointer(8, 1, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(Mat4) + 4 * sizeof(float)));
        glVertexAttribDivisor(8, 1);
        
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

bool Initialise()
{
    
    while (glGetError() != GL_NO_ERROR) {}

    if (!g_SceneShader.LoadVertexShader("shaders/scene.vs") ||
        !g_SceneShader.LoadFragmentShader("shaders/scene.fs") ||
        !g_SceneShader.Create())
    {
        std::cerr << "Erreur : Scene Shader" << std::endl;
        return false;
    }
    if (!g_SkyboxShader.LoadVertexShader("shaders/skybox.vs") ||
        !g_SkyboxShader.LoadFragmentShader("shaders/skybox.fs") ||
        !g_SkyboxShader.Create())
    {
        std::cerr << "Erreur : Skybox Shader" << std::endl;
        return false;
    }
    if (!g_ScreenShader.LoadVertexShader("shaders/screen.vs") ||
        !g_ScreenShader.LoadFragmentShader("shaders/screen.fs") ||
        !g_ScreenShader.Create())
    {
        std::cerr << "Erreur : Screen Shader" << std::endl;
        return false;
    }
    while (glGetError() != GL_NO_ERROR) {} 
    
    glGenBuffers(1, &g_CameraUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, g_CameraUBO);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(Mat4), NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, g_CameraUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    CheckGLError("UBO alloc");

    glGenBuffers(1, &g_ModelUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, g_ModelUBO);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(Mat4), NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, g_ModelUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    CheckGLError("Model UBO alloc");

    GLuint pScene  = g_SceneShader.GetProgram();
    GLuint pSkybox = g_SkyboxShader.GetProgram();
    GLuint idxS = glGetUniformBlockIndex(pScene,  "CameraData");
    GLuint idxK = glGetUniformBlockIndex(pSkybox, "CameraData");
    if (idxS != GL_INVALID_INDEX) glUniformBlockBinding(pScene,  idxS, 0);
    else std::cerr << "WARNING: CameraData non trouve dans Scene shader" << std::endl;
    if (idxK != GL_INVALID_INDEX) glUniformBlockBinding(pSkybox, idxK, 0);
    else std::cerr << "WARNING: CameraData non trouve dans Skybox shader" << std::endl;

    GLuint idxM = glGetUniformBlockIndex(pScene,  "ModelData");
    if (idxM != GL_INVALID_INDEX) glUniformBlockBinding(pScene,  idxM, 1);
    else std::cerr << "WARNING: ModelData non trouve dans Scene shader" << std::endl;
    CheckGLError("UBO binding");

    glGenVertexArrays(1, &g_SkyboxVAO);
    glBindVertexArray(g_SkyboxVAO);
    glGenBuffers(1, &g_SkyboxVBO);
    glBindBuffer(GL_ARRAY_BUFFER, g_SkyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    CheckGLError("Skybox VAO");

    glGenVertexArrays(1, &g_ScreenVAO);
    glBindVertexArray(g_ScreenVAO);
    glGenBuffers(1, &g_ScreenVBO);
    glBindBuffer(GL_ARRAY_BUFFER, g_ScreenVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screenQuadVertices), screenQuadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    CheckGLError("Screen VAO");

    glGenVertexArrays(1, &g_DragonVAO);
    glBindVertexArray(g_DragonVAO);
    glGenBuffers(1, &g_DragonVBO);
    glBindBuffer(GL_ARRAY_BUFFER, g_DragonVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(DragonVertices), DragonVertices, GL_STATIC_DRAW);
    glGenBuffers(1, &g_DragonEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_DragonEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(DragonIndices), DragonIndices, GL_STATIC_DRAW);
    int dstride = 8 * sizeof(float);
    glEnableVertexAttribArray(0); 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, dstride, (void*)0);
    glEnableVertexAttribArray(1); 
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, dstride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2); 
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, dstride, (void*)(6 * sizeof(float)));
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    CheckGLError("Dragon VAO+EBO");

    g_CubemapTexture = LoadCubemap();
    CheckGLError("Cubemap");

    {
        
        std::ofstream objP("pyramid.obj");
        objP << "mtllib pyramid.mtl\n"
             << "v 0.0 1.0 0.0\n"
             << "v -1.0 -1.0 1.0\n"
             << "v 1.0 -1.0 1.0\n"
             << "v 1.0 -1.0 -1.0\n"
             << "v -1.0 -1.0 -1.0\n"
             << "vn 0.0 0.707 0.707\n"
             << "vn 0.707 0.707 0.0\n"
             << "vn 0.0 0.707 -0.707\n"
             << "vn -0.707 0.707 0.0\n"
             << "vn 0.0 -1.0 0.0\n"
             << "vt 0.5 1.0\n"
             << "vt 0.0 0.0\n"
             << "vt 1.0 0.0\n"
             << "usemtl matte\n"
             << "f 1/1/1 2/2/1 3/3/1\n"
             << "f 1/1/2 3/2/2 4/3/2\n"
             << "f 1/1/3 4/2/3 5/3/3\n"
             << "f 1/1/4 5/2/4 2/3/4\n"
             << "f 5/2/5 4/3/5 3/1/5 2/2/5\n";
        objP.close();

        std::ofstream objC("cube.obj");
        objC << "mtllib pyramid.mtl\n"
             << "usemtl textured\n"
             << "v -1.0 -1.0 1.0\n"
             << "v 1.0 -1.0 1.0\n"
             << "v 1.0 1.0 1.0\n"
             << "v -1.0 1.0 1.0\n"
             << "v -1.0 -1.0 -1.0\n"
             << "v 1.0 -1.0 -1.0\n"
             << "v 1.0 1.0 -1.0\n"
             << "v -1.0 1.0 -1.0\n"
             << "vn 0.0 0.0 1.0\n"
             << "vn 0.0 0.0 -1.0\n"
             << "vn 1.0 0.0 0.0\n"
             << "vn -1.0 0.0 0.0\n"
             << "vn 0.0 1.0 0.0\n"
             << "vn 0.0 -1.0 0.0\n"
             << "vt 0.0 0.0\n"
             << "vt 1.0 0.0\n"
             << "vt 1.0 1.0\n"
             << "vt 0.0 1.0\n"
             << "f 1/1/1 2/2/1 3/3/1 4/4/1\n"
             << "f 6/1/2 5/2/2 8/3/2 7/4/2\n"
             << "f 2/1/3 6/2/3 7/3/3 3/4/3\n"
             << "f 5/1/4 1/2/4 4/3/4 8/4/4\n"
             << "f 4/1/5 3/2/5 7/3/5 8/4/5\n"
             << "f 5/1/6 6/2/6 2/3/6 1/4/6\n";
        objC.close();

        std::ofstream objCo("cone.obj");
        objCo << "mtllib pyramid.mtl\n"
              << "usemtl reflective\n"
              << "v 0.0 1.0 0.0\n"
              << "v 0.0 -1.0 0.0\n";
        const int slices = 8;
        for (int i = 0; i < slices; i++) {
            float angle = (float)i * (2.0f * 3.14159265f / slices);
            objCo << "v " << cosf(angle) << " -1.0 " << sinf(angle) << "\n";
        }
        for (int i = 0; i < slices; i++) {
            float angle = (float)i * (2.0f * 3.14159265f / slices);
            objCo << "vn " << cosf(angle)*0.707f << " 0.707 " << sinf(angle)*0.707f << "\n";
        }
        objCo << "vn 0.0 -1.0 0.0\n";
        objCo << "vt 0.5 1.0\n"
              << "vt 0.5 0.5\n";
        for (int i = 0; i <= slices; i++) {
            float u = (float)i / slices;
            objCo << "vt " << u << " 0.0\n";
        }
        for (int i = 0; i < slices; i++) {
            int next = (i + 1) % slices;
            objCo << "f 1/1/" << (1+i) << " " 
                  << (3+i) << "/" << (3+i) << "/" << (1+i) << " " 
                  << (3+next) << "/" << (3+next) << "/" << (1+next) << "\n";
        }
        for (int i = 0; i < slices; i++) {
            int next = (i + 1) % slices;
            objCo << "f 2/2/" << (slices+1) << " " 
                  << (3+next) << "/" << (3+next) << "/" << (slices+1) << " " 
                  << (3+i) << "/" << (3+i) << "/" << (slices+1) << "\n";
        }
        objCo.close();

        std::ofstream mtl("pyramid.mtl");
        mtl << "newmtl matte\n"
            << "Ka 0.1 0.1 0.1\n"
            << "Kd 0.85 0.85 0.85\n"
            << "Ks 0.04 0.04 0.04\n"
            << "Ns 32.0\n\n"
            << "newmtl textured\n"
            << "Ka 0.1 0.1 0.1\n"
            << "Kd 1.0 1.0 1.0\n"
            << "Ks 0.25 0.25 0.25\n"
            << "Ns 64.0\n"
            << "map_Kd texture2.png\n\n"
            << "newmtl reflective\n"
            << "Ka 0.02 0.02 0.02\n"
            << "Kd 0.45 0.45 0.45\n"
            << "Ks 0.85 0.85 0.85\n"
            << "Ns 128.0\n";
        mtl.close();
    }
    if (!g_PyramidModel.Load("pyramid.obj", "./") ||
        !g_CubeModel.Load("cube.obj", "./") ||
        !g_ConeModel.Load("cone.obj", "./"))
    {
        std::cerr << "Erreur : chargement des objets OBJ" << std::endl;
        return false;
    }
    CheckGLError("OBJ Loading");

    float colors[5][4] = {
        { 0.0f, 1.0f, 0.0f, 1.0f }, 
        { 0.0f, 0.4f, 1.0f, 1.0f }, 
        { 1.0f, 1.0f, 1.0f, 1.0f }, 
        { 1.0f, 0.8f, 0.0f, 1.0f }, 
        { 1.0f, 0.0f, 1.0f, 1.0f }  
    };
    
    float shadingModes[5] = { 0.0f, 3.0f, 1.0f, 4.0f, 2.0f };
    for (int i = 0; i < NB_INSTANCES; i++) {
        for (int c = 0; c < 4; c++) {
            g_InstanceMatrices[i].color[c] = colors[i][c];
        }
        g_InstanceMatrices[i].shadingMode = shadingModes[i];
    }

    glGenBuffers(3, g_InstanceVBOs);
    
    glBindBuffer(GL_ARRAY_BUFFER, g_InstanceVBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(InstanceData), NULL, GL_DYNAMIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, g_InstanceVBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(InstanceData), NULL, GL_DYNAMIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, g_InstanceVBOs[2]);
    glBufferData(GL_ARRAY_BUFFER, 1 * sizeof(InstanceData), NULL, GL_DYNAMIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    SetupInstancingAttributes(g_PyramidModel, g_InstanceVBOs[0]);
    SetupInstancingAttributes(g_CubeModel,    g_InstanceVBOs[1]);
    SetupInstancingAttributes(g_ConeModel,    g_InstanceVBOs[2]);
    
    CheckGLError("Instancing VBOs setup");

    {
        unsigned char white[4] = { 255, 255, 255, 255 };
        glGenTextures(1, &g_WhiteTexture);
        glBindTexture(GL_TEXTURE_2D, g_WhiteTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    CheckGLError("WhiteTexture");

    if (!CreateFBO(g_FBOWidth, g_FBOHeight))
        return false;
    CheckGLError("FBO");

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    glDisable(GL_CULL_FACE);

    std::cout << "Initialise() termine sans erreur." << std::endl;
    return true;
}

void Terminate()
{
    glDeleteVertexArrays(1, &g_SkyboxVAO);
    glDeleteBuffers(1,     &g_SkyboxVBO);
    glDeleteVertexArrays(1, &g_ScreenVAO);
    glDeleteBuffers(1,     &g_ScreenVBO);
    glDeleteVertexArrays(1, &g_DragonVAO);
    glDeleteBuffers(1,     &g_DragonVBO);
    glDeleteBuffers(1,     &g_DragonEBO);
    glDeleteTextures(1,    &g_WhiteTexture);
    glDeleteBuffers(1,     &g_CameraUBO);
    glDeleteBuffers(1,     &g_ModelUBO);
    glDeleteBuffers(3,     g_InstanceVBOs);
    glDeleteTextures(1,    &g_CubemapTexture);
    glDeleteFramebuffers(1,  &g_FBO);
    glDeleteTextures(1,      &g_FBOTex);
    glDeleteRenderbuffers(1, &g_FBODepthRB);
    g_PyramidModel.Destroy();
    g_CubeModel.Destroy();
    g_ConeModel.Destroy();
    g_SceneShader.Destroy();
    g_SkyboxShader.Destroy();
    g_ScreenShader.Destroy();
}

void ProcessCameraMovement(float dt)
{
    if (!g_Camera.freeMode)
    {
        
        float zoomSpeed = 15.0f * dt;
        if (g_Keys[GLFW_KEY_UP])
        {
            g_Camera.radius = std::max(2.0f, g_Camera.radius - zoomSpeed);
            g_Camera.Update();
        }
        if (g_Keys[GLFW_KEY_DOWN])
        {
            g_Camera.radius = std::min(80.0f, g_Camera.radius + zoomSpeed);
            g_Camera.Update();
        }
        return;
    }
    float speed = 5.0f * dt;
    float pitchRad = g_Camera.pitch * 3.14159265f / 180.0f;
    float yawRad   = g_Camera.yaw   * 3.14159265f / 180.0f;
    vec3 front;
    front.x = cosf(pitchRad) * cosf(yawRad);
    front.y = sinf(pitchRad);
    front.z = cosf(pitchRad) * sinf(yawRad);
    front = normalize(front);
    vec3 right = normalize(cross(front, vec3(0.0f, 1.0f, 0.0f)));
    if (g_Keys[GLFW_KEY_W] || g_Keys[GLFW_KEY_Z])
        g_Camera.freePos = add(g_Camera.freePos, mul(front, speed));
    if (g_Keys[GLFW_KEY_S])
        g_Camera.freePos = sub(g_Camera.freePos, mul(front, speed));
    if (g_Keys[GLFW_KEY_A] || g_Keys[GLFW_KEY_Q])
        g_Camera.freePos = sub(g_Camera.freePos, mul(right, speed));
    if (g_Keys[GLFW_KEY_D])
        g_Camera.freePos = add(g_Camera.freePos, mul(right, speed));
    g_Camera.Update();
}

static void SetSceneLightUniforms(GLuint prog, float currentTime)
{
    
    glUniform3f(glGetUniformLocation(prog, "u_light.direction"), 0.2f, -0.8f, -0.6f);
    glUniform3f(glGetUniformLocation(prog, "u_light.diffuseColor"),  1.2f, 1.1f, 1.0f);
    glUniform3f(glGetUniformLocation(prog, "u_light.specularColor"), 1.0f, 1.0f, 1.0f);
    
    glUniform3f(glGetUniformLocation(prog, "u_skyDirection"), 0.0f, 1.0f, 0.0f);
    glUniform3f(glGetUniformLocation(prog, "u_skyColor"),    0.3f, 0.5f, 0.9f);
    glUniform3f(glGetUniformLocation(prog, "u_groundColor"), 0.1f, 0.08f, 0.05f);
    glUniform3f(glGetUniformLocation(prog, "u_Ia"),          0.5f, 0.5f, 0.5f);
    
    glUniform1f(glGetUniformLocation(prog, "u_backLightStrength"), g_BackLightStrength);
}

void Render(GLFWwindow* window)
{
    float currentFrame = (float)glfwGetTime();
    float dt = currentFrame - g_LastFrameTime;
    g_LastFrameTime = currentFrame;

    ProcessCameraMovement(dt);

    Mat4 proj = makePerspective(60.f, (float)g_FBOWidth / (float)g_FBOHeight, 0.1f, 500.f);
    Mat4 view = g_Camera.GetViewMatrix();

    glBindFramebuffer(GL_FRAMEBUFFER, g_FBO);
    glViewport(0, 0, g_FBOWidth, g_FBOHeight);
    glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindBuffer(GL_UNIFORM_BUFFER, g_CameraUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0,            sizeof(Mat4), proj.m);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(Mat4), sizeof(Mat4), view.m);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);
    GLuint pSky = g_SkyboxShader.GetProgram();
    glUseProgram(pSky);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, g_CubemapTexture);
    glUniform1i(glGetUniformLocation(pSky, "u_skybox"), 0);
    glBindVertexArray(g_SkyboxVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);

    GLuint pScene = g_SceneShader.GetProgram();
    glUseProgram(pScene);

    glUniform1i(glGetUniformLocation(pScene, "u_useInstancing"), 0);

    glUniform3f(glGetUniformLocation(pScene, "u_cameraPos"),
                g_Camera.position.x, g_Camera.position.y, g_Camera.position.z);
    SetSceneLightUniforms(pScene, currentFrame);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_WhiteTexture);
    glUniform1i(glGetUniformLocation(pScene, "u_texture"), 0);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, g_CubemapTexture);
    glUniform1i(glGetUniformLocation(pScene, "u_skybox"), 2);

    {
        
        Mat4 rotY  = makeRotationY(currentFrame * 0.3f);
        
        Mat4 scale = makeScale(0.45f, 0.45f, 0.45f);
        
        Mat4 trans = makeTranslation(0.0f, -1.4f, -1.5f);
        
        Mat4 world = multiply(trans, multiply(rotY, scale));

        glBindBuffer(GL_UNIFORM_BUFFER, g_ModelUBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0,            sizeof(Mat4), world.m);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(Mat4), sizeof(Mat4), rotY.m);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glUniform3f(glGetUniformLocation(pScene, "u_material.ambientColor"),  0.15f, 0.18f, 0.25f);
        glUniform3f(glGetUniformLocation(pScene, "u_material.diffuseColor"),  0.35f, 0.40f, 0.55f);
        glUniform3f(glGetUniformLocation(pScene, "u_material.specularColor"), 0.85f, 0.88f, 1.00f);
        glUniform1f(glGetUniformLocation(pScene, "u_material.shininess"),     200.0f);
        glUniform1i(glGetUniformLocation(pScene, "u_material.hasTexture"),    0);

        glBindVertexArray(g_DragonVAO);
        
        glDrawElements(GL_TRIANGLES, NB_DRAGON_INDICES, GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);
    }

    {
        
        float orbitAngle = currentFrame * 0.15f;
        for (int i = 0; i < NB_INSTANCES; i++) {
            float angle = (float)i * (2.0f * 3.14159265f / (float)NB_INSTANCES) + orbitAngle;
            float px = 6.0f * cosf(angle);
            float pz = 6.0f * sinf(angle);
            
            float selfRotAngle = currentFrame * (1.0f + 0.3f * (float)i);
            Mat4 rotY = makeRotationY(selfRotAngle);
            
            float s = 0.5f + 0.08f * (float)i;
            Mat4 scale = makeScale(s, s, s);
            Mat4 trans = makeTranslation(px, -1.4f, pz - 1.5f);
            
            g_InstanceMatrices[i].world = multiply(trans, multiply(rotY, scale));

            if (i == 0) {
                g_CameraTargets[1] = vec3(px, -1.4f, pz - 1.5f);
            } else if (i == 2) {
                g_CameraTargets[2] = vec3(px, -1.4f, pz - 1.5f);
            }
        }

        if (!g_Camera.freeMode) {
            g_Camera.orbitTarget = g_CameraTargets[g_CameraTargetIndex];
            
            g_Camera.Update();
        }

        glBindBuffer(GL_ARRAY_BUFFER, g_InstanceVBOs[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, 2 * sizeof(InstanceData), &g_InstanceMatrices[0]);
        
        glBindBuffer(GL_ARRAY_BUFFER, g_InstanceVBOs[1]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, 2 * sizeof(InstanceData), &g_InstanceMatrices[2]);
        
        glBindBuffer(GL_ARRAY_BUFFER, g_InstanceVBOs[2]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, 1 * sizeof(InstanceData), &g_InstanceMatrices[4]);
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glUniform1i(glGetUniformLocation(pScene, "u_useInstancing"), 1);
        
        g_PyramidModel.DrawInstanced(pScene, 2);
        
        g_CubeModel.DrawInstanced(pScene, 2);
        
        g_ConeModel.DrawInstanced(pScene, 1);
        
        glUniform1i(glGetUniformLocation(pScene, "u_useInstancing"), 0);

        glBindTexture(GL_TEXTURE_2D, g_WhiteTexture);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    int ww, wh;
    glfwGetFramebufferSize(window, &ww, &wh);
    glViewport(0, 0, ww, wh);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    GLuint pScreen = g_ScreenShader.GetProgram();
    glUseProgram(pScreen);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_FBOTex);
    glUniform1i(glGetUniformLocation(pScreen, "u_screenTexture"), 0);
    glUniform1i(glGetUniformLocation(pScreen, "u_postProcessMode"), g_PostProcessMode);

    glBindVertexArray(g_ScreenVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glUseProgram(0);

    glEnable(GL_DEPTH_TEST);
}

bool g_FirstMouse  = true;
double g_LastX = 0.0, g_LastY = 0.0;
bool g_MousePressed = false;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)   g_Keys[key] = true;
        else if (action == GLFW_RELEASE) g_Keys[key] = false;
    }

    if (action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_C)
        {
            g_Camera.freeMode = !g_Camera.freeMode;
            g_Camera.freePos  = vec3(0.0f, 0.0f, 10.0f);
            g_Camera.yaw      = -90.0f;
            g_Camera.pitch    = 0.0f;
            g_Camera.Update();
            std::cout << "Camera : " << (g_Camera.freeMode ? "LIBRE" : "ORBITALE") << std::endl;
        }
        if (key == GLFW_KEY_T)
        {
            if (!g_Camera.freeMode)
            {
                g_CameraTargetIndex = (g_CameraTargetIndex + 1) % 3;
                g_Camera.orbitTarget = g_CameraTargets[g_CameraTargetIndex];
                g_Camera.Update();
                const char* targetName = (g_CameraTargetIndex == 0) ? "Dragon" :
                                         (g_CameraTargetIndex == 1) ? "Pyramide Est" : "Cube Ouest";
                std::cout << "Camera Target : " << targetName << std::endl;
            }
            else
            {
                std::cout << "Camera Target non configurable en mode LIBRE." << std::endl;
            }
        }
        if (key == GLFW_KEY_1) { g_PostProcessMode = 0; std::cout << "Post-process : Normal"     << std::endl; }
        if (key == GLFW_KEY_2) { g_PostProcessMode = 1; std::cout << "Post-process : Niveaux de gris" << std::endl; }
        if (key == GLFW_KEY_3) { g_PostProcessMode = 2; std::cout << "Post-process : Sepia"      << std::endl; }
        if (key == GLFW_KEY_4) { g_PostProcessMode = 3; std::cout << "Post-process : Inversion"  << std::endl; }
        if (key == GLFW_KEY_I) { g_BackLightStrength += 0.2f; std::cout << "Fresnel : " << g_BackLightStrength << std::endl; }
        if (key == GLFW_KEY_O) { g_BackLightStrength = std::max(0.0f, g_BackLightStrength - 0.2f); std::cout << "Fresnel : " << g_BackLightStrength << std::endl; }
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)  { g_MousePressed = true;  g_FirstMouse = true; }
        else if (action == GLFW_RELEASE) g_MousePressed = false;
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (g_FirstMouse) { g_LastX = xpos; g_LastY = ypos; g_FirstMouse = false; }
    float dx = (float)(xpos - g_LastX);
    float dy = (float)(g_LastY - ypos);
    g_LastX = xpos; g_LastY = ypos;

    if (g_Camera.freeMode)
    {
        g_Camera.yaw   += dx * 0.1f;
        g_Camera.pitch += dy * 0.1f;
        if (g_Camera.pitch >  89.0f) g_Camera.pitch =  89.0f;
        if (g_Camera.pitch < -89.0f) g_Camera.pitch = -89.0f;
        g_Camera.Update();
    }
    else if (g_MousePressed)
    {
        g_Camera.azimuth   -= dx * 0.007f;
        g_Camera.elevation += dy * 0.007f;
        float lim = 1.5f;
        if (g_Camera.elevation >  lim) g_Camera.elevation =  lim;
        if (g_Camera.elevation < -lim) g_Camera.elevation = -lim;
        g_Camera.Update();
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (!g_Camera.freeMode)
    {
        g_Camera.radius -= (float)yoffset * 0.5f;
        if (g_Camera.radius < 2.0f)  g_Camera.radius = 2.0f;
        if (g_Camera.radius > 80.0f) g_Camera.radius = 80.0f;
        g_Camera.Update();
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    
    glViewport(0, 0, width, height);
}

int main()
{
    if (!glfwInit()) return -1;

    glfwSetErrorCallback([](int error, const char* desc) {
        std::cerr << "GLFW Error " << error << ": " << desc << std::endl;
    });

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(960, 540, "Projet OpenGL – Rendu interactif", NULL, NULL);
    if (!window)
    {
        const char* desc; glfwGetError(&desc);
        std::cerr << "Impossible de creer la fenetre GLFW : " << desc << std::endl;
        glfwTerminate(); return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); 
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glewExperimental = GL_TRUE;
    GLenum glewErr = glewInit();
    if (glewErr != GLEW_OK)
    {
        std::cerr << "GLEW init error: " << glewGetErrorString(glewErr) << std::endl;
        glfwTerminate(); return -1;
    }
    
    while (glGetError() != GL_NO_ERROR) {}

    std::cout << "OpenGL " << glGetString(GL_VERSION)
              << "  Renderer: " << glGetString(GL_RENDERER) << std::endl;

    if (!Initialise())
    {
        glfwTerminate(); return -1;
    }

    std::cout << "\n=======================================" << std::endl;
    std::cout << "  CONTROLES :" << std::endl;
    std::cout << "  C       : Basculer camera (ORBITALE / LIBRE)" << std::endl;
    std::cout << "  T       : Changer la cible orbitale (Dragon / Pyramides)" << std::endl;
    std::cout << "  Orbitale: clic gauche + glisser | molette zoom ou fleches HAUT/BAS" << std::endl;
    std::cout << "  1/2/3/4 : Filtres post-process FBO (Normal / Gris / Sepia / Inversion)" << std::endl;
    std::cout << "  I / O   : Ajuster l'effet Fresnel (retro-eclairage)" << std::endl;
    std::cout << "  Scene   : Lambert couleur, Lambert hemispherique, Texture+Phong, Phong, Environment map" << std::endl;
    std::cout << "  ECHAP   : Quitter" << std::endl;
    std::cout << "=======================================" << std::endl;

    g_LastFrameTime = (float)glfwGetTime();

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
