# En este archivo definimos los elementos para la plataforma native

# Compiladores
CC ?= gcc
CXX ?= gcc
ifdef LD_OVERRIDE
  LD     = $(LD_OVERRIDE)
else
  LD     = gcc
endif
AR       ?= ar
AS       ?= as
NM       ?= nm
OBJCOPY  ?= objcopy
STRIP    ?= strip

CFLAGS += -Wall -g

ifeq ($(WERROR),1)
  CFLAGS += -Werror
endif
