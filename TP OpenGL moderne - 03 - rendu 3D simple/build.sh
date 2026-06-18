#!/bin/bash

CXX="clang++"
FLAGS="-std=c++11 -I/opt/homebrew/include -L/opt/homebrew/lib -lglfw -lGLEW -framework OpenGL"
COMMON="common/GLShader.cpp"

echo "=== Compilation exercice 1 ==="
$CXX exercice1.cpp $COMMON $FLAGS -o exercice1 && echo "OK"

echo "=== Compilation exercice 1_2 ==="
$CXX exercice1_2.cpp $COMMON $FLAGS -o exercice1_2 && echo "OK"

echo "=== Compilation exercice 2_1 ==="
$CXX exercice2_1.cpp $COMMON $FLAGS -o exercice2_1 && echo "OK"

echo "=== Compilation exercice 3 ==="
$CXX exercice3.cpp $COMMON $FLAGS -o exercice3 && echo "OK"

echo ""
echo "Pour lancer :"
echo "  ./exercice1"
echo "  ./exercice1_2"
echo "  ./exercice2_1"
echo "  ./exercice3"
