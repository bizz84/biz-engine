###############################################################################
# Filename			Makefile
# 
# License			LGPL
#
# Author			Andrea Bizzotto (bizz84@gmail.com)
#
# Platform			LinuxX11 / OpenGL
# 
# Description		SDK Makefile
#
###############################################################################

SRCEXT   = cpp
#SRCDIR   = ../../
SDKDIR   = ../../../../bizsdk/
OBJDIR   = .
BINDIR   = .

SRCS    += $(shell find $(SDKDIR) -name '*.$(SRCEXT)')
SRCDIRS := $(shell find . -name '*.$(SRCEXT)' -exec dirname {} \; | uniq)
OBJS    := $(patsubst %.$(SRCEXT),$(OBJDIR)/%.o,$(SRCS))


INCLUDES = -I../../

CFLAGS = -c -O3 -ffast-math -Wall -DGL_GLEXT_PROTOTYPES $(INCLUDES)
LFLAGS = -lSDL -lGLU -lGL -lX11 -lm

.PHONY: all clean distclean

all: $(BINDIR)/$(APP)

$(BINDIR)/$(APP): buildrepo $(OBJS)
	@mkdir -p `dirname $@`
	@echo "+l+ $@..."
	@$(CC) $(OBJS) $(LFLAGS) -o $@

$(OBJDIR)/%.o: %.$(SRCEXT)
	@echo "+dep+ $<..."
	@$(call make-depend,$<,$@,$(subst .o,.d,$@))
	@echo "+c+ $<..."
	@$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) -r $(OBJDIR)

distclean: clean
	$(RM) -r $(BINDIR)

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
