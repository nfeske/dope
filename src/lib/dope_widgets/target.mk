TARGET = dope
SRC_CC = $(notdir $(wildcard $(PRG_DIR)/*.cc))
SRC_C  = $(addprefix data/,$(notdir $(wildcard $(PRG_DIR)/data/*.c)))
LIBS   = base
