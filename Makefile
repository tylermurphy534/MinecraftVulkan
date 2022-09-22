CC = g++

INCFLAGS  = -Isrc
INCFLAGS += -Iengine
INCFLAGS += -Ilib/glfw/include
INCFLAGS += -Ilib/glm
INCFLAGS += -Ilib/stb
INCFLAGS += -I/usr/include/AL

CCFLAGS  = -std=c++17 -O2 -g
CCFLAGS += $(INCFLAGS)

LDFLAGS = -lm
LDFLAGS += $(INCFLAGS)
LDFLAGS += lib/glfw/src/libglfw3.a
LDFLAGS += lib/glm/glm/libglm_static.a
LDFLAGS += -lopenal
LDFLAGS += -lalut
LDFLAGS += -lvulkan

SRC  = $(shell find src -name "*.cpp")
SRC += $(shell find engine -name "*.cpp")
OBJ  = $(SRC:.cpp=.o)
BIN  = bin

VERTSRC = $(shell find ./res/shaders -type f -name "*.vert")
VERTOBJ = $(patsubst %.vert, %.vert.spv, $(VERTSRC))
FRAGSRC = $(shell find ./res/shaders -type f -name "*.frag")
FRAGOBJ = $(patsubst %.frag, %.frag.spv, $(FRAGSRC))

.PHONY: all clean

all: dirs libs shader build

libs:
	cd lib/glfw && cmake . && make
	cd lib/glm && cmake . && make

dirs:
	mkdir -p ./$(BIN)


shader: $(VERTOBJ) $(FRAGOBJ)

run: build
	$(RUN) $(BIN)/game

build: dirs shader ${OBJ}
	${CC} -o $(BIN)/game $(filter %.o,$^) $(LDFLAGS)

%.spv: %
	glslc -o $@ $<

%.o: %.cpp
	$(CC) -o $@ -c $< $(CCFLAGS)

clean:
	rm -rf app
	rm -rf $(BIN) $(OBJ)
	rm -rf res/shaders/*.spv
	rm -rf lib/glfw/CMakeCache.txt
