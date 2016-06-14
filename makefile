# See LICENSE file for copyright and license details.

#
#   Project: bfst
#
#   Description:
#
#       Bitmap Font Simple Terminal
#

# Customize below to fit your system

# Select folder where is object files are created
BFST_DST_PATH = .

# Select folder where source code and header files are found
BFST_SRC_PATH = .

# Select compiler
BFST_CC = $(CC)

# Select compiler flags
BFST_CFLAGS = $(CFLAGS) -g -std=c99 -pedantic -Wall -Wextra -Wvariadic-macros -Os -D_XOPEN_SOURCE=600

# Uncomment the following line to enable debugging messages
# BFST_CFLAGS += -DBFST_CFG_DEBUG

# Select linker flags
BFST_LFLAGS = $(LDFLAGS) -lc -lm -lrt -lutil -lX11

# List of source files
BFST_SRCS = \
    $(BFST_SRC_PATH)/bfst_body.c \
    $(BFST_SRC_PATH)/bfst_child.c \
    $(BFST_SRC_PATH)/bfst_color.c \
    $(BFST_SRC_PATH)/bfst_display.c \
    $(BFST_SRC_PATH)/bfst_draw.c \
    $(BFST_SRC_PATH)/bfst_focus.c \
    $(BFST_SRC_PATH)/bfst_font.c \
    $(BFST_SRC_PATH)/bfst_font_list.c \
    $(BFST_SRC_PATH)/bfst_key.c \
    $(BFST_SRC_PATH)/bfst_log.c \
    $(BFST_SRC_PATH)/bfst_main.c \
    $(BFST_SRC_PATH)/bfst_opts.c \
    $(BFST_SRC_PATH)/bfst_os.c \
    $(BFST_SRC_PATH)/bfst_ptr.c \
    $(BFST_SRC_PATH)/bfst_ptr_show.c \
    $(BFST_SRC_PATH)/bfst_sel.c \
    $(BFST_SRC_PATH)/bfst_tab.c \
    $(BFST_SRC_PATH)/bfst_tools.c \
    $(BFST_SRC_PATH)/bfst_tty.c \
    $(BFST_SRC_PATH)/bfst_tty_list.c \
    $(BFST_SRC_PATH)/bfst_unique.c \
    $(BFST_SRC_PATH)/bfst_utf8.c \
    $(BFST_SRC_PATH)/bfst_view.c \
    $(BFST_SRC_PATH)/bfst_view_list.c \
    $(BFST_SRC_PATH)/bfst_window.c \
    $(BFST_SRC_PATH)/bfst_zombie.c

# List of header files
BFST_HDRS = \
    $(BFST_SRC_PATH)/bfst_body.h \
    $(BFST_SRC_PATH)/bfst_child.h \
    $(BFST_SRC_PATH)/bfst_color.h \
    $(BFST_SRC_PATH)/bfst_ctxt.h \
    $(BFST_SRC_PATH)/bfst_display.h \
    $(BFST_SRC_PATH)/bfst_draw.h \
    $(BFST_SRC_PATH)/bfst_focus.h \
    $(BFST_SRC_PATH)/bfst_font.h \
    $(BFST_SRC_PATH)/bfst_font_list.h \
    $(BFST_SRC_PATH)/bfst_key.h \
    $(BFST_SRC_PATH)/bfst_log.h \
    $(BFST_SRC_PATH)/bfst_opts.h \
    $(BFST_SRC_PATH)/bfst_ptr.h \
    $(BFST_SRC_PATH)/bfst_ptr_show.h \
    $(BFST_SRC_PATH)/bfst_sel.h \
    $(BFST_SRC_PATH)/bfst_tab.h \
    $(BFST_SRC_PATH)/bfst_tools.h \
    $(BFST_SRC_PATH)/bfst_tty.h \
    $(BFST_SRC_PATH)/bfst_tty_list.h \
    $(BFST_SRC_PATH)/bfst_unique.h \
    $(BFST_SRC_PATH)/bfst_utf8.h \
    $(BFST_SRC_PATH)/bfst_view.h \
    $(BFST_SRC_PATH)/bfst_view_list.h \
    $(BFST_SRC_PATH)/bfst_window.h \
    $(BFST_SRC_PATH)/bfst_zombie.h

# Default target
all: $(BFST_DST_PATH)/bfst

# Rule for building target
$(BFST_DST_PATH)/bfst: $(BFST_SRCS)
	@echo CC -o $@
	@$(BFST_CC) -o $@ $(BFST_CFLAGS) $(BFST_SRCS) $(BFST_LFLAGS)

# Extra dependencies of target
$(BFST_DST_PATH)/bfst: $(BFST_HDRS)

# Fake target used to clean source folder
clean:
	@echo cleaning
	@rm -f $(BFST_DST_PATH)/bfst

.PHONY: all clean
