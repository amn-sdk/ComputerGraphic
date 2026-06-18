#!/bin/bash

# Compilation du projet TD2 OpenGL
clang++ -std=c++11 main.cpp common/GLShader.cpp -I/opt/homebrew/include -L/opt/homebrew/lib -lglfw -lGLEW -framework OpenGL -o app

echo "Compilation terminée. Lancement de l'application..."

# Lancement
./app
