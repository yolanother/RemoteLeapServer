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

MODULES     := utilities utilities/bitpackdata sockets leapdata
SRC_DIR     := $(addprefix src/,$(MODULES))
BUILD_DIR   := $(addprefix obj/,$(MODULES)) bin obj

SRC         := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.cpp))
OBJ         := $(patsubst src/%.cpp,obj/%.o,$(SRC))
INCLUDES    := $(addprefix -I,$(SRC_DIR)) -ILeapSDK/Linux/include

vpath %.cpp $(SRC_DIR)

CC			= $(CXX)
LD          = $(CXX)
CXXFLAGS	= -std=c++14
CXXFLAGS	+= -Wall -pthread

define make-goal
$1/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $$< -o $$@
endef

.PHONY: all checkdirs clean

all: checkdirs cleanbin bin/RemoteLeap

cleanbin:
	@rm -rf bin/RemoteLeap

bin/RemoteLeap: $(OBJ)
	$(CXX) $(CXXFLAGS) -g -ILeapSDK/Linux/include  $^ src/RemoteLeap.cpp -o $@ $(LEAP_LIBRARY)
ifeq ($(OS), Darwin)
	install_name_tool -change @loader_path/libLeap.dylib LeapSDK/Mac/lib/libLeap.dylib bin/RemoteLeap
endif

test-udp-client:
	$(CXX) $(CXXFLAGS) src/sockets/testclient/test-udp-client.cpp -o bin/test-udp-client

test-udp-server:
	$(CXX) $(CXXFLAGS) src/sockets/testclient/test-udp-server.cpp -o bin/test-udp-server

checkdirs: $(BUILD_DIR)

$(BUILD_DIR):
	@mkdir -p $@

clean:
	@rm -rf $(BUILD_DIR)

$(foreach bdir,$(BUILD_DIR),$(eval $(call make-goal,$(bdir))))