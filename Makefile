CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic
TARGET = comp2113_game
SOURCES = main.cpp game.cpp player.cpp pet.cpp

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
