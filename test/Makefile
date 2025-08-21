# ref: https://github.com/samvrlewis/cmocka-embedded-example/blob/master/test/Makefile
# ref: https://github.com/artfulbytes/nsumo_video/blob/main/Makefile

.PHONY: all clean test cmocka

CMOCKA_TAR = cmocka-1.1.8.tar.xz

# Directories
BUILD_DIR = build
BIN_DIR = $(BUILD_DIR)/bin
OBJ_DIR = $(BUILD_DIR)/obj
CMOCKA_DIR = cmocka

TEST_SOURCES = \
  test_state_machine.c \

OBJECTS = $(patsubst %.c, $(OBJ_DIR)/%.o, $(TEST_SOURCES))

INCLUDE_DIRS = -I$(CMOCKA_DIR)/include

CFLAGS = $(INCLUDE_DIRS)
LDFLAGS = -L$(CMOCKA_DIR)/build/src -Wl,-rpath=$(CMOCKA_DIR)/build/src
LDFLAGS += -lcmocka

TARGET = $(BIN_DIR)/test_state_machine

# Build
all: test

test: $(TARGET)
	./$(TARGET)

## Linking
$(TARGET): $(OBJECTS)
	@echo Linking ...
	@mkdir -p $(dir $@)
	$(CC) -o $@ $^ $(LDFLAGS)

## Compiling
$(OBJ_DIR)/%.o: %.c
	@echo Compiling $< ...
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(CMOCKA_DIR):
	mkdir -p $@
	tar -xf $(CMOCKA_TAR) -C $@ --strip-components 1
	@(cd $@ && cmake -DWITH_EXAMPLES=OFF -DCMAKE_BUILD_TYPE=Release -Bbuild -S. && cmake --build build)

clean:
	@rm -rf $(BUILD_DIR) compile_commands.json
