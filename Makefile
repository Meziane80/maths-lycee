CC = arm-none-eabi-gcc
BUILD_DIR = output
NWLINK = npx --yes -- nwlink@0.0.16
LINK_GC = 1
LTO = 1

src = src/main.c
CFLAGS = -std=c99 $(shell $(NWLINK) eadk-cflags) -Os -Wall
CFLAGS += -fdata-sections -ffunction-sections
LDFLAGS = -Wl,--relocatable -nostartfiles --specs=nano.specs
LDFLAGS += -Wl,-e,main -Wl,-u,eadk_app_name -Wl,-u,eadk_app_icon -Wl,-u,eadk_api_level
LDFLAGS += -Wl,--gc-sections

.PHONY: build clean
build: $(BUILD_DIR)/MathsLycee.nwa

$(BUILD_DIR)/MathsLycee.nwa: $(BUILD_DIR)/main.o $(BUILD_DIR)/icon.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

$(BUILD_DIR)/main.o: src/main.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/icon.o: src/icon.png
	$(NWLINK) png-icon-o $< $@

clean:
	rm -rf $(BUILD_DIR)
