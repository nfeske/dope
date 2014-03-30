TARGET = test-dope_widgets
SRC_CC = $(notdir $(wildcard $(PRG_DIR)/*.cc)) \
         $(addprefix data/,$(notdir $(wildcard $(PRG_DIR)/data/*.cc)))
LIBS   = base dope_widgets
