# Definiciones globales


## Reconociendo la arquitectura en la que que va a compilar
## Aquí estamos suponiendo que no es Windows
UNAME := $(shell uname -m -s)
OS_BASE = $(word 1, $(UNAME))
OS_ARCH = $(word 2, $(UNAME))

# Control de salidas de mensajes, 

TRACE_CC    =    @echo "  CC       " $<
TRACE_CXX   =    @echo "  CXX      " $<
TRACE_LD    =    @echo "  LD       " $@
TRACE_AR    =    @echo "  AR       " $@
TRACE_AS    =    @echo "  AS       " $<
TRACE_CP    =    @echo "  CP       " $< "-->" $@
TRACE_OBJCOPY  = @echo "  OBJCOPY  " $< "-->" $@
TRACE_OBJDUMP  = @echo "  OBJDUMP  " $< "-->" $@
TRACE_MKDIR   =  @echo "  MKDIR    " $@


# Coloriar el texto de salidas
COLOR_GREEN  :=
COLOR_RED    :=
COLOR_PURPLE :=
COLOR_RESET  :=
#COLOR_ECHO   := /bin/echo

# Check if colored output is not disabled by user, i.e: CC_NOCOLOR unset
# or 0
ifneq ($(CC_NOCOLOR),1)
  IS_TERMINAL = $(if $(MAKE_TERMOUT),$(MAKE_TERMERR),)
  # Check if terminal support colored output
  #ifneq ($(IS_TERMINAL),)
    COLOR_GREEN  := \033[1;32m
    COLOR_RED    := \033[1;31m
    COLOR_YELLOW := \033[1;33m
    COLOR_PURPLE := \033[1;35m
    COLOR_RESET  := \033[0m
    ifeq ($(OS),Darwin)
      COLOR_ECHO   := echo -e
      SHELL=bash
    else
      COLOR_ECHO   := /bin/echo -e
    endif
  #endif
endif