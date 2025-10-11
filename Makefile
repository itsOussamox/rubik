NAME	:= rubik
CXX	:= c++
CXXFLAGS := -std=c++11 -Wall -Wextra -Wpedantic -O2
CPPFLAGS := -Iinclude
LDFLAGS :=
GRAPHICS ?= 1

UNAME_S := $(shell uname 2>/dev/null)

ifeq ($(UNAME_S),)
SHELL := cmd.exe
MKDIR_P = if not exist "$(1)" mkdir "$(1)"
RMDIR_R = if exist "$(1)" rmdir /S /Q "$(1)"
RM_F = if exist "$(1)" del /Q "$(1)"
else
MKDIR_P = mkdir -p "$(1)"
RMDIR_R = rm -rf "$(1)"
RM_F = rm -f "$(1)"
endif

ifeq ($(GRAPHICS),1)
CPPFLAGS += -DRUBIK_WITH_GRAPHICS
ifeq ($(UNAME_S),)
LDFLAGS += -lglfw3 -lglu32 -lopengl32 -lgdi32
else
ifeq ($(UNAME_S),Darwin)
LDFLAGS += -lglfw -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
else
LDFLAGS += -lglfw -lGL -lGLU
endif
endif
endif

SRC_DIR := src
OBJ_DIR := build

SRCS := \
	$(SRC_DIR)/main.cpp \
	$(SRC_DIR)/CubeSolver.cpp \
	$(SRC_DIR)/Viewer.cpp

OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

.PHONY: all clean fclean re debug dirs

all: $(NAME)

$(NAME): dirs $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

dirs:
	@$(call MKDIR_P,$(OBJ_DIR))

clean:
	@$(call RMDIR_R,$(OBJ_DIR))

fclean: clean
	@$(call RM_F,$(NAME).exe)
	@$(call RM_F,$(NAME))

re: fclean all

debug: CXXFLAGS := -std=c++11 -Wall -Wextra -Werror

debug: re
