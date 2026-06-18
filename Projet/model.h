#pragma once
#include <GL/glew.h>
#include <vector>
#include <string>
#include <iostream>
#include "tiny_obj_loader.h"
#include "common/stb_image.h"
#include "vec.h"
#include "mat4.h"

// Note: OpenGL 3.3 Core Profile on macOS provides standard VAO functions.
// No Apple-extension redefinitions needed.

struct OBJVertex {
    float x, y, z;
    float nx, ny, nz;
    float u, v;
};

struct OBJMesh {
    GLuint vao;
    GLuint vbo;
    int vertexCount;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    GLuint textureId;
};

class OBJModel {
private:
    GLuint LoadTextureFromFile(const std::string& filename, const std::string& baseDir) {
        std::string fullPath = baseDir + filename;
        int width, height, comp;
        uint8_t* data = stbi_load(fullPath.c_str(), &width, &height, &comp, STBI_rgb_alpha);
        if (!data) {
            std::cerr << "TinyOBJ Model loader: impossible de charger la texture " << fullPath << std::endl;
            return 0;
        }

        GLuint texId;
        glGenTextures(1, &texId);
        glBindTexture(GL_TEXTURE_2D, texId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
        glBindTexture(GL_TEXTURE_2D, 0);

        std::cout << "Texture chargee : " << fullPath << " (" << width << "x" << height << ")" << std::endl;
        return texId;
    }

public:
    std::vector<OBJMesh> meshes;

    bool Load(const std::string& filename, const std::string& baseDir) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.c_str(), baseDir.c_str(), true);
        if (!warn.empty()) {
            std::cout << "TinyOBJ Avertissement: " << warn << std::endl;
        }
        if (!err.empty()) {
            std::cerr << "TinyOBJ Erreur: " << err << std::endl;
        }
        if (!ret) {
            return false;
        }

        std::vector<GLuint> loadedTextures(materials.size(), 0);

        for (const auto& shape : shapes) {
            std::vector<OBJVertex> vertices;

            for (const auto& index : shape.mesh.indices) {
                OBJVertex vertex;
                vertex.x = attrib.vertices[3 * index.vertex_index + 0];
                vertex.y = attrib.vertices[3 * index.vertex_index + 1];
                vertex.z = attrib.vertices[3 * index.vertex_index + 2];

                if (index.normal_index >= 0) {
                    vertex.nx = attrib.normals[3 * index.normal_index + 0];
                    vertex.ny = attrib.normals[3 * index.normal_index + 1];
                    vertex.nz = attrib.normals[3 * index.normal_index + 2];
                } else {
                    vertex.nx = vertex.ny = vertex.nz = 0.0f;
                }

                if (index.texcoord_index >= 0) {
                    vertex.u = attrib.texcoords[2 * index.texcoord_index + 0];
                    vertex.v = attrib.texcoords[2 * index.texcoord_index + 1];
                } else {
                    vertex.u = vertex.v = 0.0f;
                }

                vertices.push_back(vertex);
            }

            if (vertices.empty()) continue;

            OBJMesh mesh;
            mesh.vertexCount = vertices.size();
            mesh.textureId = 0;

            // Assigner les proprietes de materiaux
            if (shape.mesh.material_ids.size() > 0 && shape.mesh.material_ids[0] >= 0 &&
                shape.mesh.material_ids[0] < (int)materials.size()) {
                int matId = shape.mesh.material_ids[0];
                const auto& mat = materials[matId];
                mesh.ambient   = vec3(mat.ambient[0], mat.ambient[1], mat.ambient[2]);
                mesh.diffuse   = vec3(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]);
                mesh.specular  = vec3(mat.specular[0], mat.specular[1], mat.specular[2]);
                mesh.shininess = mat.shininess;
                if (loadedTextures[matId] == 0 && !mat.diffuse_texname.empty()) {
                    loadedTextures[matId] = LoadTextureFromFile(mat.diffuse_texname, baseDir);
                }
                mesh.textureId = loadedTextures[matId];
            } else {
                // Fallback par defaut
                mesh.ambient   = vec3(0.2f, 0.2f, 0.2f);
                mesh.diffuse   = vec3(0.8f, 0.8f, 0.8f);
                mesh.specular  = vec3(1.0f, 1.0f, 1.0f);
                mesh.shininess = 16.0f;
                mesh.textureId = 0;
            }

            glGenVertexArrays(1, &mesh.vao);
            glBindVertexArray(mesh.vao);

            glGenBuffers(1, &mesh.vbo);
            glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(OBJVertex), vertices.data(), GL_STATIC_DRAW);

            int stride = sizeof(OBJVertex);
            // Attribut 0: Position
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);

            // Attribut 1: Normale
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));

            // Attribut 2: UV
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));

            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            meshes.push_back(mesh);
        }
        return true;
    }

    void Draw(GLuint program) {
        for (const auto& mesh : meshes) {
            // Liaison des uniforms de materiau
            glUniform3f(glGetUniformLocation(program, "u_material.ambientColor"),  mesh.ambient.x,  mesh.ambient.y,  mesh.ambient.z);
            glUniform3f(glGetUniformLocation(program, "u_material.diffuseColor"),  mesh.diffuse.x,  mesh.diffuse.y,  mesh.diffuse.z);
            glUniform3f(glGetUniformLocation(program, "u_material.specularColor"), mesh.specular.x, mesh.specular.y, mesh.specular.z);
            glUniform1f(glGetUniformLocation(program, "u_material.shininess"),     mesh.shininess);

            // Indiquer si on a une texture diffuse
            int hasTexLoc = glGetUniformLocation(program, "u_material.hasTexture");
            if (mesh.textureId != 0) {
                glUniform1i(hasTexLoc, 1);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, mesh.textureId);
                glUniform1i(glGetUniformLocation(program, "u_texture"), 0);
            } else {
                glUniform1i(hasTexLoc, 0);
            }

            glBindVertexArray(mesh.vao);
            glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount);
            glBindVertexArray(0);
        }
    }

    void DrawInstanced(GLuint program, int instanceCount) {
        for (const auto& mesh : meshes) {
            // Liaison des uniforms de materiau
            glUniform3f(glGetUniformLocation(program, "u_material.ambientColor"),  mesh.ambient.x,  mesh.ambient.y,  mesh.ambient.z);
            glUniform3f(glGetUniformLocation(program, "u_material.diffuseColor"),  mesh.diffuse.x,  mesh.diffuse.y,  mesh.diffuse.z);
            glUniform3f(glGetUniformLocation(program, "u_material.specularColor"), mesh.specular.x, mesh.specular.y, mesh.specular.z);
            glUniform1f(glGetUniformLocation(program, "u_material.shininess"),     mesh.shininess);

            // Indiquer si on a une texture diffuse
            int hasTexLoc = glGetUniformLocation(program, "u_material.hasTexture");
            if (mesh.textureId != 0) {
                glUniform1i(hasTexLoc, 1);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, mesh.textureId);
                glUniform1i(glGetUniformLocation(program, "u_texture"), 0);
            } else {
                glUniform1i(hasTexLoc, 0);
            }

            glBindVertexArray(mesh.vao);
            glDrawArraysInstanced(GL_TRIANGLES, 0, mesh.vertexCount, instanceCount);
            glBindVertexArray(0);
        }
    }

    void Destroy() {
        for (auto& mesh : meshes) {
            glDeleteVertexArrays(1, &mesh.vao);
            glDeleteBuffers(1, &mesh.vbo);
            if (mesh.textureId != 0) {
                glDeleteTextures(1, &mesh.textureId);
            }
        }
        meshes.clear();
    }
};
