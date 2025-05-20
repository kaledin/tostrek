# Default to using all available CPU cores if MAKEFLAGS not already set
ifndef MAKEFLAGS
MAKEFLAGS += -j$(shell nproc)
endif

CXX = g++
CXXFLAGS = -std=c++23 -Wall -Wconversion -Wshadow -Wextra -Werror -Wpedantic -g
SRC = main.cpp handle_input.cpp serialization.cpp space.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = trekmush

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(OBJ) $(TARGET)
