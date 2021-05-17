# MCU Configuration
FREQ_SYS ?= 24000000
IRAM_SIZE ?= 0x100
XRAM_SIZE ?= 0x400
CODE_SIZE ?= 0x3800

SRC_DIR := ./src
OBJ_DIR := ./out
TMP_DIR := $(OBJ_DIR)/generated

# File containing "void main(void)" function
MAIN := $(SRC_DIR)/main.c
# Target file name, do not add an extension
TARGET := flashme

# Point to SDCC toolchain folder
# TOOLCHAIN := ../sdcc-4.0.0/bin
CC :=  sdcc
OBJCOPY = sdobjcopy
CFLAGS := \
	-mmcs51 \
	--model-large \
	--iram-size $(IRAM_SIZE) \
	--xram-size $(XRAM_SIZE) \
	--code-size $(CODE_SIZE) \
	-DFREQ_SYS=$(FREQ_SYS)
LDFLAGS := $(CFLAGS)

SRCS := $(shell find $(SRC_DIR)/ -name *.c ! -wholename $(MAIN))
RELS := $(patsubst $(SRC_DIR)/%.c,$(TMP_DIR)/%.rel,$(SRCS))
INC_FLAGS := $(addprefix -I,$(shell find $(SRC_DIRS) -type d ! -regex '\(.*\/\.git.*\|.\)'))

.PHONY: all
all: $(OBJ_DIR)/$(TARGET).bin

$(OBJ_DIR)/$(TARGET).bin: $(TMP_DIR)/$(TARGET).ihx
	$(OBJCOPY) -I ihex -O binary $< $@

$(TMP_DIR)/$(TARGET).ihx: $(MAIN) $(RELS) $(TMP_DIR)
	$(CC) $< $(RELS) -o $@ $(LDFLAGS)

$(TMP_DIR)/%.rel: $(SRC_DIR)/%.c $(TMP_DIR)
	$(CC) -c $(CFLAGS) $(INC_FLAGS) $< -o $(TMP_DIR)/

$(TMP_DIR):
	mkdir -p $@

$(OBJ_DIR):
	mkdir -p $@

.PHONY: clean
clean:
	rm -rf $(TMP_DIR)
	rm -rf $(OBJ_DIR)
