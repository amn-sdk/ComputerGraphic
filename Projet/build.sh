#!/bin/bash

CXX="clang++"
FLAGS="-std=c++11 -I/opt/homebrew/include -L/opt/homebrew/lib -lglfw -lGLEW -framework OpenGL"
COMMON="common/GLShader.cpp"

echo "=== Compilation du Projet ==="
$CXX main.cpp $COMMON $FLAGS -o app && echo "Compilation OK!"
