RM			= rm -f
MKDIR		= mkdir -p

SOURCES		= $(shell find src -type f -iname "*.c")
OBJECTS		= $(patsubst src/%.c,out/%.o,$(SOURCES))
TARGET		= ./out/rsdoom

CXX			= gcc
CXXFLAGS	= 
#-Wall -Wextra -Wpedantic
CXXFLAGS	+= -DNORMALUNIX -DLINUX

LD			= $(CXX)
LDFLAGS		= -lm
LDFLAGS		+= `sdl2-config --cflags --libs`

CPPCHECK	= cppcheck
CLANGXX		= clang
VALGRIND	= valgrind

ifeq ($(origin DEBUG), environment)
	CXXFLAGS += -Og -g -DRSD_DEBUG
else
	CXXFLAGS += -O2
endif

all: rsdoom

clean:
	@$(RM) $(TARGET) $(OBJECTS)

check:
	@$(CPPCHECK) --language=c --std=c99 ./src/i_main.c
	@$(CLANGXX) --analyze -Xclang -analyzer-output=html $(CXXFLAGS) \
		-o ./out/analysis \
		./src/i_main.c \
		$(LDFLAGS)

rsdoom: $(OBJECTS)
	$(LD) -o $(TARGET) $^ $(LDFLAGS)

out/%.o: src/%.c | create_dirs
	@$(CXX) $(CXXFLAGS) -c $^ -o $@ $(LDFLAGS)

create_dirs:
	@$(MKDIR) $(sort $(dir $(OBJECTS)))

test:
	$(VALGRIND) \
		--leak-check=full \
		--show-leak-kinds=all \
		--track-origins=yes \
		$(TARGET)