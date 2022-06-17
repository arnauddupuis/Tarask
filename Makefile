CFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
DEBUG_FLAGS = -DNDEBUG

GLSLC = /usr/bin/glslc

vertSources = $(shell find shaders -type f -name "*.vert")
vertObjFiles = $(patsubst %.vert, %.vert.spv, $(vertSources))
fragSources = $(shell find shaders -type f -name "*.frag")
fragObjFiles = $(patsubst %.frag, %.frag.spv, $(fragSources))

TARGET = a.out
$(TARGET): $(vertObjFiles) $(fragObjFiles)
${TARGET}: *.cpp *.hpp
	g++ $(CFLAGS) $(DEBUG_FLAGS) -o ${TARGET} *.cpp $(LDFLAGS)

# a.out: *.cpp *.hpp
# 	g++ $(CFLAGS) -o a.out *.cpp $(LDFLAGS)

%.spv: %
	${GLSLC} $< -o $@

.PHONY: test clean

test: a.out
	./a.out

clean:
	rm -f a.out
	rm -f *.spv