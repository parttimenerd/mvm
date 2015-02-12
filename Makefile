CPP := clang++ 
DEBUGFLAGS := -ggdb -O1
RELEASEFLAGS := -O3
PROFILEFLAGS := -pg -ggdb -O3
COMMON := -std=c++1y -pedantic -W -Wall -Wextra

TARGET := mvm

# this looks for .cpp/.hpp-files in the current directory and all subdirectories. (recursive-depth = 1)
SOURCE_FILES := $(sort $(wildcard src/*.cpp)) $(sort $(wildcard src/**/*.cpp)) $(sort $(wildcard src/**/**/*.cpp))
HEADER_FILES := $(sort $(wildcard src/*.hpp)) $(sort $(wildcard src/**/*.hpp)) $(sort $(wildcard src/**/**/*.hpp))

all: $(TARGET)

#-include $(SOURCE_FILES:src/%.cpp=build/%.d)

#debug $(TARGET): $(SOURCE_FILES:src/%.cpp=build/%.o)
#	$(CPP) $(COMMON) $(DEBUGFLAGS) $^ $(LDFLAGS) -o $(TARGET)

# no .o files in release build
release: $(SOURCE_FILES)
	$(CPP) $(COMMON) $(CPPFLAGS) $(RELEASEFLAGS) $^ $(LDFLAGS) -o $(TARGET)

build:
	find src/ -type d | sed 's/^src/build/' | xargs mkdir

clean:
	rm -rf $(TARGET) build

build/%.o : src/%.cpp
	$(CPP) $(COMMON) $(CPPFLAGS) $(DEBUGFLAGS) -MMD -c $< -o $@

