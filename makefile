# ============================================================
#  Makefile for TIMTIM_RENDER
# ============================================================

CXX       := g++
TARGET    := TIMTIM_RENDER
BUILD_DIR := build
WORKSPACE_DIR := $(CURDIR)
INCLUDE_PATH  := -I"$(WORKSPACE_DIR)"

# ---- Flags
LIBRARIES  := -L"$(WORKSPACE_DIR)/lib"
LINKER     := -lSDL3 -lSDL3_ttf -lopengl32

CXXFLAGS := -g -fdiagnostics-color=always

# ---- Sources
SRCS := $(wildcard src/code/*.cpp) $(wildcard imgui-master/*.cpp) glad.c

all:
	$(CXX) $(CXXFLAGS) $(INCLUDE_PATH) main.cpp $(SRCS) -o $(BUILD_DIR)/$(TARGET).exe $(LIBRARIES) $(LINKER)

clean:
	if exist "$(BUILD_DIR)" rmdir /s /q "$(BUILD_DIR)/$(TARGET)"