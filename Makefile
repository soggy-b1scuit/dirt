# Alternative GNU Make workspace makefile autogenerated by Premake

ifndef config
  config=debug_x86_64
endif

ifndef verbose
  SILENT = @
endif

ifeq ($(config),debug_x86_64)
  dirt_config = debug_x86_64

else ifeq ($(config),debug_x64)
  dirt_config = debug_x64

else ifeq ($(config),release_x86_64)
  dirt_config = release_x86_64

else ifeq ($(config),release_x64)
  dirt_config = release_x64

else
  $(error "invalid configuration $(config)")
endif

PROJECTS := dirt

.PHONY: all clean help $(PROJECTS) 

all: $(PROJECTS)

dirt:
ifneq (,$(dirt_config))
	@echo "==== Building dirt ($(dirt_config)) ===="
	@${MAKE} --no-print-directory -C . -f dirt.make config=$(dirt_config)
endif

clean:
	@${MAKE} --no-print-directory -C . -f dirt.make clean

help:
	@echo "Usage: make [config=name] [target]"
	@echo ""
	@echo "CONFIGURATIONS:"
	@echo "  debug_x86_64"
	@echo "  debug_x64"
	@echo "  release_x86_64"
	@echo "  release_x64"
	@echo ""
	@echo "TARGETS:"
	@echo "   all (default)"
	@echo "   clean"
	@echo "   dirt"
	@echo ""
	@echo "For more information, see https://github.com/premake/premake-core/wiki"