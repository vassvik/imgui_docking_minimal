SHELL=/bin/bash

CFLAGS = -c -DGLEW_STATIC -std=c++11 #-Wall -Wextra -Wpedantic
IFLAGS = -Iimgui
SRCS = imgui/imgui.cpp imgui/imgui_draw.cpp imgui/imgui_demo.cpp imgui/imgui_dock.cpp imgui/imgui_impl_glfw_gl3.cpp main.cpp
OBJS = $(SRCS:.cpp=.o)
CC = g++

ifeq ($(OS),Windows_NT)
	LFLAGS = -lglfw3 -lglew32 -lopengl32 -lgdi32
	EXE=a.exe
else
	LFLAGS = -lGL -lglfw -lGLEW
	EXE=a.out
endif

all: $(SRCS) $(EXE)

$(EXE): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $(IFLAGS) $< -o $@

clean:
	rm -v	 $(OBJS) $(EXE)