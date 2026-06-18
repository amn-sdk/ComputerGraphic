#!/bin/bash

CXX="clang++"
FLAGS="-std=c++11 -I/opt/homebrew/include -L/opt/homebrew/lib -lglfw -lGLEW -framework OpenGL"
COMMON="common/GLShader.cpp"

echo "=== Compilation exercice 1 ==="
$CXX exercice1.cpp $COMMON $FLAGS -o exercice1 && echo "OK"

echo "=== Compilation exercice 2 ==="
$CXX exercice2.cpp $COMMON $FLAGS -o exercice2 && echo "OK"

echo "=== Compilation exercice 3 ==="
$CXX exercice3.cpp $COMMON $FLAGS -o exercice3 && echo "OK"

echo "=== Compilation exercice 4 ==="
$CXX exercice4.cpp $COMMON $FLAGS -o exercice4 && echo "OK"

echo "=== Compilation exercice 5 ==="
$CXX exercice5.cpp $COMMON $FLAGS -o exercice5 && echo "OK"

echo ""
echo "Pour lancer :"
echo "  ./exercice1"
echo "  ./exercice2"
echo "  ./exercice3"
echo "  ./exercice4"
echo "  ./exercice5"
