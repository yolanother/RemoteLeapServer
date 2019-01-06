OS := $(shell uname)
ARCH := $(shell uname -m)

ifeq ($(OS), Linux)
  ifeq ($(ARCH), x86_64)
    LEAP_LIBRARY := LeapSDK/Linux/lib/x64/libLeap.so -Wl,-rpath,LeapSDK/Linux/lib/x64
  else
    LEAP_LIBRARY := LeapSDK/Linux/lib/x86/libLeap.so -Wl,-rpath,LeapSDK/Linux/lib/x86
  endif
else
  # OS X
  LEAP_LIBRARY := LeapSDK/Mac/lib/libLeap.dylib
endif

OBJS := bin/basesocket.o bin/socket.o bin/server.o

CC			= $(CXX)
CXXFLAGS	= -std=c++14
CXXFLAGS	+= -Wall -pthread

$(shell mkdir -p bin)

all: sockets RemoteLeap

basesocket.o: src/sockets/basesocket.cpp
	$(CXX) $(CXXFLAGS) -c src/sockets/basesocket.cpp -o bin/basesocket.o
socket.o: src/sockets/socket.cpp
	$(CXX) $(CXXFLAGS) -c src/sockets/socket.cpp -o bin/socket.o
server.o: src/sockets/server.cpp
	$(CXX) $(CXXFLAGS) -c src/sockets/server.cpp -o bin/server.o

sockets: basesocket.o socket.o server.o

RemoteLeap: src/RemoteLeap.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) -g -ILeapSDK/Linux/include src/RemoteLeap.cpp -o bin/RemoteLeap $(LEAP_LIBRARY) $(OBJS)
ifeq ($(OS), Darwin)
	install_name_tool -change @loader_path/libLeap.dylib LeapSDK/Mac/lib/libLeap.dylib bin/RemoteLeap
endif

clean:
	rm -rf bin/RemoteLeap bin/RemoteLeap.dSYM bin/*.o
