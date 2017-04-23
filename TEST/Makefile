TARGET = rfidd
CC = gcc
INCLUDE = ./include
CFLAGS = -g -Wall -ansi -std=gnu99
LDFLAGS = -lpthread
SRCDIR = ./src 
OBJDIR = ./obj

SRCS    = $(shell find $(SRCDIR) -name '*.c')
SRCDIRS = $(shell find . -name '*.c' -exec dirname {} \; | uniq)
OBJS    = $(patsubst %.c,$(OBJDIR)/%.o,$(SRCS))

.PHONY:clean

all: $(TARGET)

$(TARGET):buildrepo $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@
$(OBJDIR)/%.o:%.c
	$(CC) -I$(INCLUDE) $(CFLAGS) -c $< -o $@
clean:
	$(RM) -rf $(OBJS)
distclean: clean
	$(RM) $(TARGET)
buildrepo:
	@$(call make-repo)

define make-repo
  for dir in $(SRCDIRS); \
  do \
	mkdir -p $(OBJDIR)/$$dir; \
  done
endef

