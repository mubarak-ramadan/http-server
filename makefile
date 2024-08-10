# Makefile

CXX = g++
CXXFLAGS = -Wall -std=c++11

# Define the target executable
TARGET = tcp_server

# List of source files
SRCS = server_linux.cpp http_tcpServer_linux.cpp

# List of object files
OBJS = $(SRCS:.cpp=.o)

# Default rule to build the target
all: $(TARGET)

# Rule to link the target executable
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

# Rule to compile source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up generated files
clean:
	rm -f $(OBJS) $(TARGET)
