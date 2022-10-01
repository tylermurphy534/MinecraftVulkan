CC = g++

INCFLAGS  = -Isrc
INCFLAGS += -Iengine
INCFLAGS += -Ilib/glfw/include
INCFLAGS += -Ilib/glm
INCFLAGS += -Ilib/stb
INCFLAGS += -Ilib/openal-soft/include
INCFLAGS += -Ilib/freealut/include

CCFLAGS  = -std=c++17 -O2 -g
CCFLAGS += $(INCFLAGS)

LDFLAGS = -lm
LDFLAGS += -lglfw
LDFLAGS += -ldl
LDFLAGS += -lopenal
LDFLAGS += -lalut
LDFLAGS += -lvulkan
LDFLAGS += $(INCFLAGS)

BIN  = bin
SRC  = $(shell find src -name "*.cpp")
SRC += $(shell find engine -name "*.cpp")
OBJ  = $(SRC:%.cpp=$(BIN)/%.o)

VERTSRC = $(shell find ./res/shaders -type f -name "*.vert")
VERTOBJ = $(patsubst %.vert, %.vert.spv, $(VERTSRC))
FRAGSRC = $(shell find ./res/shaders -type f -name "*.frag")
FRAGOBJ = $(patsubst %.frag, %.frag.spv, $(FRAGSRC))

.PHONY: all clean

all: dirs shader build

dirs:
	mkdir -p ./$(BIN)
	mkdir -p ./$(BIN)/src
	mkdir -p ./$(BIN)/engine

shader: $(VERTOBJ) $(FRAGOBJ)

run: build
	$(RUN) $(BIN)/game

build: dirs shader ${OBJ}
	${CC} -o $(BIN)/game $(filter %.o,$^) $(LDFLAGS)

%.spv: %
	glslc -o $@ $<

$(BIN)/%.o: %.cpp
	$(CC) -o $@ -c $< $(CCFLAGS)

clean:
	rm -rf app
	rm -rf $(BIN)
	rm -rf res/shaders/*.spv