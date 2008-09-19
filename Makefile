RELEASE_TARGETS = nanopubsub-client libnanopubsub
DEBUG_TARGETS   = nanopubsub-client-debug libnanopubsub-debug

all: release
release: $(RELEASE_TARGETS)
debug: $(DEBUG_TARGETS)
.PHONY: all release debug $(RELEASE_TARGETS) $(DEBUG_TARGETS) clean


##############################################################################
# C compiler options

CFLAGS = -ansi -std=c99 -pedantic -Wall
$(RELEASE_TARGETS): CFLAGS += -O3 -DNDEBUG
$(DEBUG_TARGETS):   CFLAGS += -O0

export CFLAGS


##############################################################################
# build directory

BUILDDIR = ./build

$(RELEASE_TARGETS) $(DEBUG_TARGETS): $(BUILDDIR)

$(BUILDDIR):
	@mkdir $(BUILDDIR)


##############################################################################
# nanopubsub-client(-debug)

nanopubsub-client: libnanopubsub
nanopubsub-client-debug: libnanopubsub-debug

nanopubsub-client nanopubsub-client-debug:
	@$(MAKE) -C ./src/nanopubsub-client -w


##############################################################################
# libnanopubsub(-debug)

libnanopubsub libnanopubsub-debug:
	@$(MAKE) -C ./src/libnanopubsub -w


##############################################################################
# clean

clean:
	@$(MAKE) -C ./src/nanopubsub-client -w clean
	@$(MAKE) -C ./src/libnanopubsub -w clean
