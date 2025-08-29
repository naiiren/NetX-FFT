BUILD_DIR := build
TARGET := $(BUILD_DIR)/fft_test

.PHONY: all run clean

all: $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): main.cpp | $(BUILD_DIR)
	c++ -O3 -ffast-math -std=c++23 -o $@ $<

run: all
	nx compile _netx.toml --top fft8 --minimal | $(TARGET)

raw: all
	nx compile _netx.toml --top fft8 --minimal | $(TARGET) --no-native

clean:
	rm $(TARGET)
