# Default to using all available CPU cores if MAKEFLAGS not already set
ifndef MAKEFLAGS
MAKEFLAGS += -j$(shell command -v nproc >/dev/null && nproc || sysctl -n hw.ncpu)
endif

CXX = g++
CXXFLAGS = -std=c++23 -Wall -Wconversion -Wshadow -Wextra -Werror -Wpedantic -g
LDFLAGS = -lncurses -lreadline
SRC = main.cpp handle_input.cpp serialization.cpp space.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = trekmush

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

run:
	./trekmush

clean:
	rm -f $(OBJ) $(TARGET)
