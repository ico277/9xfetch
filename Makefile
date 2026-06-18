#.DEFAULT_GOAL := all

BACKEND ?= linux

BACKEND_MAKEFILE := backend_$(BACKEND).mk

.PHONY: all
all:
	@$(MAKE) -f $(BACKEND_MAKEFILE)

%:
	@$(MAKE) -f $(BACKEND_MAKEFILE) $@