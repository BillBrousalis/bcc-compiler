SRC_DIR := src
OBJ_DIR := src/obj

EXE := bcc
SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CFLAGS   := -g -Wall

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJ) | $(BIN_DIR)
		$(CC) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
		$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
		mkdir -p $@

clean:
		@$(RM) -rv $(OBJ_DIR)

-include $(OBJ:.o=.d)
