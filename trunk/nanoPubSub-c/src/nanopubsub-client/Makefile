BUILDDIR = ../../build

all: nanopubsub-client
.PHONY: all nanopubsub-client clean


##############################################################################
# C compiler options

CFLAGS += -I../libnanopubsub


##############################################################################
# linker options

LDFLAGS += -L$(BUILDDIR) -lnanopubsub -lc


##############################################################################
# objects

OBJECTS = $(BUILDDIR)/nanopubsub-client.o \
	$(BUILDDIR)/client_io.o

$(BUILDDIR)/%.o: defs.h
$(BUILDDIR)/nanopubsub-client.o: nanopubsub-client.h nanopubsub-client.c
$(BUILDDIR)/client_io.o: client_io.h client_io.c


##############################################################################
# executable program

nanopubsub-client: $(BUILDDIR)/nanopubsub-client

$(BUILDDIR)/nanopubsub-client: $(OBJECTS)


##############################################################################
# Implicit rules

$(BUILDDIR)/%.o: %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@


##############################################################################
# clean

clean:
	rm -rf $(BUILDDIR)/nanopubsub-client
	rm -rf $(OBJECTS)
