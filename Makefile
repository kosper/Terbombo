TARGET=terbombo

LIBS=ncurses -pthread
DIRS=src
IDIRS=src

#---------------------------------------------------
CC=gcc

#CFLAGS=-pedantic -O1 -std=c11 -Wall -g -MD -MP -Wextra

DEBUG ?= 0

ifeq ($(DEBUG), 1)
	CFLAGS=-pedantic -O1 -std=c11 -Wall -g -MD -MP -Wextra
else 
	CFLAGS=-pedantic -O3 -std=c11 -Wall -DNDEBUG -MD -MP -Wextra
endif

OUTDIR=bin
OBJDIR=obj

CFILES=$(foreach F, $(DIRS), $(wildcard $(F)/*.c))

OFILES=$(patsubst %.c, %.o, $(CFILES))
H_FILES=$(patsubst %.c, $(OBJDIR)/%.d, $(notdir $(CFILES)))
OBJ=$(patsubst %.o, $(OBJDIR)/%.o, $(notdir $(OFILES)))

.PHONY: ALL clean

ALL: $(TARGET) 

$(TARGET): $(OFILES) | $(OUTDIR)
	$(CC) -I$(IDIRS) $(OBJ) -o $(OUTDIR)/$@ -l$(LIBS)

%.o: %.c | $(OBJDIR) 
	$(CC) $(CFLAGS) -I$(IDIRS) -c $< -o $(OBJDIR)/$(notdir $@)

$(OUTDIR) $(OBJDIR):
	@mkdir -p $@
	@mkdir $@/Scores

clean:
	rm -rf $(OUTDIR)
	rm -rf $(OBJDIR)

-include $(H_FILES)
