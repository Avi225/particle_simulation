########################################
.RECIPEPREFIX = >
PROGRAM = particle_simulation
CXXSOURCES = $(wildcard src/*.cpp)
OBJECTS = $(CXXSOURCES:.cpp=.o)
DEPS = $(CXXSOURCES:.cpp=.d)
########################################
CXX ?= g++-14
CXXFLAGS ?= -Wall -g -std=c++17 -pedantic -MMD
CPPFLAGS ?= -I include
# something like
# sudo apt-get install libsdl2-dev libtiff5-dev zlib1g-dev libsdl2-image-dev libpng-dev libfreetype-dev libsdl2-ttf-dev
LIBS ?= SDL2 tiff z SDL2_image png freetype SDL2_ttf
RM ?= rm
########################################
.SUFFIXES:
.PHONY: all clean test tags
.SECONDARY:
all: $(PROGRAM)
ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif
$(PROGRAM): $(OBJECTS)
> $(CXX) $(OBJECTS) $(LDFLAGS) $(patsubst %,-l %,$(LIBS)) -o $@
src/%.o src/%.d: src/%.cpp Makefile
> $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $(patsubst %.cpp,%.o,$(filter %.cpp,$^)) $<
clean:
> $(RM) -f $(OBJECTS) $(DEPS) $(PROGRAM)
