BUILDDIR = ../../build

all: libnanopubsub
.PHONY: all libnanopubsub clean


##############################################################################
# objects

OBJECTS = $(BUILDDIR)/message.o \
	$(BUILDDIR)/network.o

$(BUILDDIR)/message.o: message.h message.c
$(BUILDDIR)/network.o: network.h network.c message.h


##############################################################################
# static library

libnanopubsub: $(BUILDDIR)/libnanopubsub.a


##############################################################################
# Implicit rules

$(BUILDDIR)/libnanopubsub.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

$(BUILDDIR)/%.o: %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@


##############################################################################
# clean

clean:
	rm -rf $(BUILDDIR)/libnanopubsub.a
	rm -rf $(OBJECTS)
