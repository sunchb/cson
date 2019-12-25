#  Define the command of tool
CC := gcc
AR := ar
MD := mkdir -p
RM := rm -rf

# Define the JSON parsing library used (cjson or jansson supported)
JSON_LIB = cjson
#JSON_LIB = jansson

#		output files
OUT_DIR := ./output
TARGET := $(OUT_DIR)/libcson.a

#		source files
SRC_DIR = ./src
SRC_FILES += $(wildcard $(SRC_DIR)/*.c)

ADP_SRC_DIR = ./adapter/$(JSON_LIB)
ADP_SRC_FILES += $(wildcard $(ADP_SRC_DIR)/*.c)

#		*.o
OBJS += $(patsubst $(SRC_DIR)/%.c, $(OUT_DIR)/%.o, $(SRC_FILES))
OBJS += $(patsubst $(ADP_SRC_DIR)/%.c, $(OUT_DIR)/%.o, $(ADP_SRC_FILES))

#		include path
INC_PATH += -I./inc/
INC_PATH += -I./3rd/inc/

$(TARGET):$(OBJS)
	@$(MD) $(OUT_DIR)
	@$(AR) rc $(TARGET) $(OBJS)

$(OUT_DIR)/%.o:$(SRC_DIR)/%.c
	@$(MD) $(OUT_DIR)
	@$(CC) $(INC_PATH) -c $< -o $@

$(OUT_DIR)/%.o:$(ADP_SRC_DIR)/%.c
	@$(MD) $(OUT_DIR)
	@$(CC) $(INC_PATH) -c $< -o $@

.PHONY:clean
clean:
	@$(RM) $(OUT_DIR)
