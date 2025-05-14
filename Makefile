# Compiler settings
CXX = g++
CXXFLAGS = -Iinclude -Wall

# Directories
SRCDIR = .
OBJDIR = obj
BINDIR = .

# SDL2 libraries
LIBS = -lSDL2 -lSDL2main -lSDL2_image -lSDL2_mixer -lSDL2_ttf
LIBDIRS = -Llib

# Source and object files
SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
TARGET = $(BINDIR)/game.exe

# Create directories
$(shell mkdir -p $(OBJDIR))

# Main target
all: $(TARGET)

# Link
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) icon.o -o $(TARGET) $(LIBDIRS) $(LIBS)

# Compile
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean
clean:
	rm -f $(OBJDIR)/*.o $(TARGET)

.PHONY: all clean
