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

RemoteLeap: src/RemoteLeap.cpp
	mkdir -p bin
	$(CXX) -Wall -g -ILeapSDK/Linux/include src/RemoteLeap.cpp -o bin/RemoteLeap $(LEAP_LIBRARY)
ifeq ($(OS), Darwin)
	install_name_tool -change @loader_path/libLeap.dylib LeapSDK/Mac/lib/libLeap.dylib bin/RemoteLeap
endif

clean:
	rm -rf bin/RemoteLeap bin/RemoteLeap.dSYM
