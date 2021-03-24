CXX = g++

CXXFLAGS = -std=c++17 -flto -fPIC -W -Wall -Wextra -Os -s
LDFLAGS = -lncurses

TARGET = watte

all: $(TARGET)

$(TARGET): $(TARGET).cxx
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(TARGET).cxx $(LDFLAGS)

clean:
	$(RM) $(TARGET)
