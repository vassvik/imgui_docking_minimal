ifeq ($(OS),Windows_NT)
	LFLAGS = -lglfw3 -lopengl32 -lgdi32
else
	LFLAGS = -lGL -lglfw
endif
CFLAGS = -g -std=c++11
IFLAGS = -Iimgui
SRCS = main.cpp imgui/{imgui,imgui_draw,imgui_demo,imgui_dock,imgui_impl_glfw}.cpp
CC = g++

all:
	$(CC) $(CFLAGS) $(IFLAGS) $(SRCS) $(LFLAGS)
