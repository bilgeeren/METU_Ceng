CFLAGS= -Wall -Wextra -pedantic
LDFLAGS=`pkg-config --static --libs glfw3 glew`

all: graph3

hw3_flat: genTex.cpp
	$(CXX) genTex.cpp -ljpeg -o hw3_flat $(LDFLAGS)

hw3_sphere: sphereGenTex.cpp
	$(CXX) sphereGenTex.cpp -ljpeg -o hw3_sphere $(LDFLAGS)


.PHONY=clean
clean:
	rm hw3_flat
	rm hw3_sphere
