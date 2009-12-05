###############################################################################
# Filename			Makefile
# 
# License			LGPL
#
# Author			Andrea Bizzotto (bizz84@gmail.com)
#
# Platform			LinuxX11 / OpenGL
# 
# Description		Makefile for Shadows project
#
###############################################################################

APP      = Triangle

SRCEXT   = cpp
SRCDIR   = ../..
SDKDIR   = ../../../../bizsdk
OBJDIR   = .
BINDIR   = .

SRCS    := $(shell find $(SRCDIR) -name '*.$(SRCEXT)')
SRCS    += $(shell find $(SDKDIR) -name '*.$(SRCEXT)')
SRCDIRS := $(shell find . -name '*.$(SRCEXT)' -exec dirname {} \; | uniq)
OBJS    := $(patsubst %.$(SRCEXT),$(OBJDIR)/%.o,$(SRCS))


INCLUDES = -I../../ -I../../../../bizsdk/

#-DGL_GLEXT_PROTOTYPES
CFLAGS = -c -O3 -ffast-math -Wall  $(INCLUDES)
LFLAGS = -lSDL -lGLU -lGL -lX11 -lm -lGLEW -l3ds
#

ifeq ($(DEBUG), 1)
	CFLAGS += -g
endif

#ifeq ($(SUPPORT_TTF), 1)
	CFLAGS += -DSUPPORT_TTF
	LFLAGS += -lSDL_ttf
#endif

.PHONY: all clean distclean

all: $(BINDIR)/$(APP)

$(BINDIR)/$(APP): buildrepo $(OBJS)
	@mkdir -p `dirname $@`
	@echo "+l+ $@..."
	@$(CC) $(OBJS) $(LFLAGS) -o $@

#@echo "+dep+ $<..."
#@$(call make-depend,$<,$@,$(subst .o,.d,$@))
$(OBJDIR)/%.o: %.$(SRCEXT)
	@echo "+c+ $<..."
	@$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) $(SRCDIR)/*.o $(SRCDIR)/*.d $(SRCDIR)/*~ $(SRCDIR)/*# $(SDKDIR)/*.o $(SDKDIR)/*.d $(SDKDIR)/*~ $(SDKDIR)/*#
	#$(RM) -r $(OBJDIR)

distclean: clean
	#$(RM) $(SRCDIR)/*.o $(SRCDIR)/*~ $(SRCDIR)/*# $(SDKDIR)/*.o $(SDKDIR)/*~ $(SDKDIR)/*#
	#$(RM) -r $(BINDIR)

buildrepo:
	@$(call make-repo)

define make-repo
   for dir in $(SRCDIRS); \
   do \
	mkdir -p $(OBJDIR)/$$dir; \
   done
endef


# usage: $(call make-depend,source-file,object-file,depend-file)
define make-depend
  $(CC) -MM       \
        -MF $3    \
        -MP       \
        -MT $2    \
        $(CFLAGS) \
        $1
endef
