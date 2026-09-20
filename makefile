# ============================================================
#  Makefile for TIMTIM_RENDER
#  Equivalent to the VS Code "C/C++: g++.exe build active file" task
#  Works with: mingw32-make -j8
# ============================================================

CXX      := g++
TARGET   := TIMTIM_RENDER
BUILD_DIR:= build
OBJ_DIR  := $(BUILD_DIR)/obj

# ---- Include / Lib paths (matches your tasks.json) ----
INCLUDES := -IC:/LIBS/include -I.
LIBDIRS  := -LC:/LIBS/lib
LIBS     := -lSDL3 -lSDL3_ttf -lopengl32

CXXFLAGS := -g -fdiagnostics-color=always $(INCLUDES)

# ---- Sources ----
# All .cpp files sitting directly in the project root (your app / main code)
APP_SRCS   := $(wildcard *.cpp)
# All ImGui source files
IMGUI_SRCS := $(wildcard imgui-master/*.cpp)

CPP_SRCS := $(APP_SRCS) $(IMGUI_SRCS)

OBJS := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(CPP_SRCS)) $(OBJ_DIR)/glad.o

EXE := $(BUILD_DIR)/$(TARGET).exe

# ---- Pre-create every output folder ONCE, at parse time ----
# (Doing this here instead of inside a recipe avoids race conditions
#  when running with -j8 / parallel jobs.)
NEEDED_DIRS := $(sort $(dir $(OBJS)) $(BUILD_DIR)/)
$(foreach d,$(NEEDED_DIRS),$(shell if not exist "$(subst /,\,$(d))" mkdir "$(subst /,\,$(d))"))

.PHONY: all clean run

all: $(EXE)

$(EXE): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LIBDIRS) $(LIBS)

$(OBJ_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/glad.o: glad.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: all
	"$(EXE)"

clean:
	if exist "$(BUILD_DIR)" rmdir /s /q "$(BUILD_DIR)"