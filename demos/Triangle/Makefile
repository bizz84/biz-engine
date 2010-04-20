##########################################################################
# Shadows Makefile
##########################################################################
CC     = g++
CFLAGS = -c -O3 -ffast-math -Wall -DGL_GLEXT_PROTOTYPES
LFLAGS = -lSDL -lSDL_ttf -lGLU -lGL -lX11 -lm
 
ifeq ($(DEBUG), 1)
	CFLAGS += -g
endif

APPNAME = Font
SRCDIR = ../../

INCLUDES = -I../../ -I../../../../bizsdk/

#SRC = SDLShell.cpp Misc.cpp
#OBJ = $(SRC:.cpp=.o)

# Add relative path as a prefix
#SOURCES = $(addprefix $(SRCDIR),$(SRC))
#OBJECTS = $(addprefix $(SRCDIR),$(OBJ))


all: $(APPNAME)
	
$(APPNAME): $(APPNAME).o $(OBJECTS) 
	@echo "+l+ $@"
	$(CC) $(LFLAGS) $(APPNAME).o $(OBJECTS) -o $@

.cpp.o:
	@echo "+c+ $@"
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@


clean:
	rm -f $(APPNAME) *.o *~ *#
