# See LICENSE file for copyright and license details.

#
#   Module: bfst_project.mak
#
#   Description:
#
#       bfst (Bitmap Font Simple Terminal) build system.
#

# Select compiler flags
BFST_CFLAGS = $(CFLAGS) -g -std=c99 -pedantic -Wall -Wextra -Wvariadic-macros -Os -D_XOPEN_SOURCE=600 -I$(BFST_DST_PATH)

# Uncomment the following line to enable debugging messages
# BFST_CFLAGS += -DBFST_CFG_DEBUG

# Select linker flags
BFST_LFLAGS = $(LDFLAGS) -lc -lm -lrt -lutil -lX11

# List of source files
BFST_SRCS = \
    $(BFST_DST_PATH)/_obj_bfst_body.o \
    $(BFST_DST_PATH)/_obj_bfst_child.o \
    $(BFST_DST_PATH)/_obj_bfst_color.o \
    $(BFST_DST_PATH)/_obj_bfst_display.o \
    $(BFST_DST_PATH)/_obj_bfst_draw.o \
    $(BFST_DST_PATH)/_obj_bfst_focus.o \
    $(BFST_DST_PATH)/_obj_bfst_font.o \
    $(BFST_DST_PATH)/_obj_bfst_font_list.o \
    $(BFST_DST_PATH)/_obj_bfst_key.o \
    $(BFST_DST_PATH)/_obj_bfst_log.o \
    $(BFST_DST_PATH)/_obj_bfst_main.o \
    $(BFST_DST_PATH)/_obj_bfst_opts.o \
    $(BFST_DST_PATH)/_obj_bfst_os.o \
    $(BFST_DST_PATH)/_obj_bfst_ptr.o \
    $(BFST_DST_PATH)/_obj_bfst_ptr_show.o \
    $(BFST_DST_PATH)/_obj_bfst_sel.o \
    $(BFST_DST_PATH)/_obj_bfst_tab.o \
    $(BFST_DST_PATH)/_obj_bfst_tools.o \
    $(BFST_DST_PATH)/_obj_bfst_tty.o \
    $(BFST_DST_PATH)/_obj_bfst_tty_list.o \
    $(BFST_DST_PATH)/_obj_bfst_unique.o \
    $(BFST_DST_PATH)/_obj_bfst_utf8.o \
    $(BFST_DST_PATH)/_obj_bfst_view.o \
    $(BFST_DST_PATH)/_obj_bfst_view_list.o \
    $(BFST_DST_PATH)/_obj_bfst_window.o \
    $(BFST_DST_PATH)/_obj_bfst_zombie.o

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
    $(BFST_SRC_PATH)/bfst_main.h \
    $(BFST_SRC_PATH)/bfst_opts.h \
    $(BFST_SRC_PATH)/bfst_os.h \
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
$(BFST_DST_PATH)/bfst : $(BFST_SRCS)
	@echo LD -o $@
	@$(BFST_CC) -o $@ $(BFST_CFLAGS) $(BFST_SRCS) $(BFST_LFLAGS)

# Build each object file
$(BFST_DST_PATH)/_obj_%.o : $(BFST_SRC_PATH)/%.c
	@echo CC -o $@
	@$(BFST_CC) -c -o $@ $(BFST_CFLAGS) $<

# Build the precompiled header
$(BFST_DST_PATH)/bfst : $(BFST_DST_PATH)/bfst_os.h.gch

$(BFST_DST_PATH)/bfst_os.h.gch : $(BFST_SRC_PATH)/bfst_os.h
	$(BFST_CC) -c -o $@ $(BFST_CFLAGS) $(BFST_SRC_PATH)/bfst_os.h

# Extra dependencies of target
$(BFST_DST_PATH)/bfst: $(BFST_HDRS)

# Fake target used to clean source folder
clean:
	@echo cleaning
	@rm -f $(BFST_DST_PATH)/bfst
	@rm -f $(BFST_DST_PATH)/_obj_*.o

.PHONY: all clean
