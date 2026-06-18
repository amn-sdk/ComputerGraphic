CXX      = g++
CXXFLAGS = -std=c++17
# macOS : utilise les frameworks natifs
LDFLAGS  = -framework OpenGL -framework GLUT

.PHONY: all clean

all: testgl triangle

testgl: testgl.cpp
	$(CXX) $(CXXFLAGS) -o testgl testgl.cpp -framework OpenGL

triangle: triangle.cpp
	$(CXX) $(CXXFLAGS) -o triangle triangle.cpp $(LDFLAGS)

clean:
	rm -f testgl triangle
