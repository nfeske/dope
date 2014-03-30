SRC_CC = $(notdir $(wildcard $(REP_DIR)/src/lib/dope_widgets/*.cc))
SRC_C  = $(addprefix data/,$(notdir $(wildcard $(REP_DIR)/src/lib/dope_widgets/data/*.c)))

CC_OLEVEL = -O0

vpath % $(REP_DIR)/src/lib/dope_widgets
