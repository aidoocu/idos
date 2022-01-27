# -*- makefile -*-

ifndef IDOS
  ${error IDOS not defined! You must specify the path to idOS}
endif

# Habilitar Werror por defecto. para desabilitar por cli use: make WERROR=0.
WERROR ?= 1

# Indentificando la plataforma. Si no se ha difinido un targer se asume native
### ToDo ## verificar que la plataforma sea una de las disponibles 
ifeq ($(BOARD),)
  BOARD = native
  ${info BOARD not defined, using board '$(BOARD)'}

else
  ${info Using board '$(BOARD)'}
endif

# Incluir los textos de ayuda
include $(IDOS)/tools/make/help.mk

# Incluir las definiciones globales
include $(IDOS)/tools/make/define.mk

## Tipos de plataforma
#define NATIVE = 0

ifeq ($(BOARD),native)
  CFLAGS += -DBOARD=1
  CFLAGS += -D$(OS_BASE)
endif


# Incluir los path globales y específicas de la plataforma

# IDOS : path relativo al repositorio de idOS. Se debe definir en 
# el Makefile del projecto para idicarle al proyecto donde está idOS
# IDOS

# IDOS_OS : path relativo al código de idOS independiente de la plataforma
IDOS_OS = $(IDOS)/os
# IDOS_KERNEL : path relativo al kernel de idOS
IDOS_KERNEL = $(IDOS_OS)/kernel
# IDOS_SYS : path relativo al sys de idOS
IDOS_SYS = $(IDOS_OS)/sys

# IDOS_NET : path relativo a las red de idOS
IDOS_NET = $(IDOS_SYS)/net

# IDOS_ARCH : path relativo al código de idOS dependiente de la plataforma
IDOS_ARCH = $(IDOS_OS)/arch/board
IDOS_BOARD = $(IDOS_ARCH)/$(BOARD)

# IDOS_APP: Camino a las aplicaciones
IDOS_APP = $(IDOS)/apps

# Incluir el makefile específico de la plataforma
include $(IDOS_BOARD)/$(BOARD).mk


# Organizando el resultado de la compilación

## Directorios donde se pondrán todo el resultado de build
BUILD_DIR = build
BUILD_DIR_BOARD = $(BUILD_DIR)/$(BOARD)
OBJECTDIR = $(BUILD_DIR_BOARD)/obj

## Crear OBJECTDIR si no existe o si ha sido eliminado por clean
$(OBJECTDIR):
	$(TRACE_MKDIR)
	@mkdir -p $@

## Función para la sustitución de las extenciones de los fuentes a objetos
define oname
${patsubst %.c,%.o, ${patsubst %.cpp,%.o, $(1)}}
endef

SOURCE_PATH += $(IDOS_OS) $(IDOS_SYS) $(IDOS_KERNEL) 
SOURCE_PATH += $(IDOS_NET) $(IDOS_NET)/ip $(IDOS_NET)/mac $(IDOS_NET)/transport 
SOURCE_PATH += $(IDOS_BOARD)
SOURCE_PATH += ${foreach app, $(APPS), $(IDOS_APP)/$(app)}

#${info -> Paths: $(SOURCE_PATH)}

# Especificando para VPATH los directorios de los fuentes que será utilizados
# como prerequisitos
vpath %.c $(SOURCE_PATH)
vpath %.cpp $(SOURCE_PATH)

## Incorporando los archivos fuentes
SOURCE_FILES = $(IDOS_PROJECT).cpp

SOURCE_FILES += ${notdir ${wildcard $(IDOS_OS)/*.cpp}}
SOURCE_FILES += ${notdir ${wildcard $(IDOS_SYS)/*.cpp}}
SOURCE_FILES += ${notdir ${wildcard $(IDOS_KERNEL)/*.cpp}}

SOURCE_FILES += ${notdir ${wildcard $(IDOS_NET)/*.cpp}}
SOURCE_FILES += ${notdir ${wildcard $(IDOS_NET)/ip/*.c}}
SOURCE_FILES += ${notdir ${wildcard $(IDOS_NET)/mac/*.cpp}}
SOURCE_FILES += ${notdir ${wildcard $(IDOS_NET)/transport/*.cpp}}

# El código dependiente del hardware será redactado en C
SOURCE_FILES += ${notdir ${wildcard $(IDOS_BOARD)/*.c}}

# Adicionamos las aplicaciones (apps)
SOURCE_FILES += ${notdir ${foreach app, $(APPS), ${wildcard $(IDOS_APP)/$(app)/*.cpp}}}


${info -> Sources: $(SOURCE_FILES)}

## Objetos
OBJECT_FILES += ${addprefix $(OBJECTDIR)/,${call oname, $(SOURCE_FILES)}}

## Resultado de la creación de la biblioteca
IDOS_BOARD_LIB = $(BUILD_DIR_BOARD)/idos_$(BOARD).a

# Banderas para c++
CXXFLAGS += $(CFLAGS)


# Obtener los archivos object

## A partir de los .cpp
$(OBJECTDIR)/%.o: %.cpp | $(OBJECTDIR)
	$(TRACE_CXX)
	@$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

%.o: %.cpp
	$(TRACE_CXX)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

## A partir de los .c
$(OBJECTDIR)/%.o: %.c | $(OBJECTDIR)
	$(TRACE_CC)
	@$(CC) $(CFLAGS) -MMD -c $< -o $@

%.o: %.c
	$(TRACE_CC)
	@$(CC) $(CFLAGS) -c $< -o $@

# .PHONY targets
.PHONY: help clean usage

## Limpiar todo el resultado de la compilación incluido el ejecutable
clean:
	-@rm -f *.d *.e *.o
	-@rm -rf $(BUILD_DIR)
	-@rm -f $(addsuffix .$(BOARD), $(IDOS_PROJECT))
	@echo Board $(BOARD) cleaned


# Cancel the predefined implict rule for compiling and linking
# a single C source into a binary to force GNU make to consider
# the match-anything rule below instead.
%: %.c

# Compilar el proyecto
$(IDOS_PROJECT): $(OBJECT_FILES)
	@echo Making project '$@' to $(BOARD) board
	$(CXX) $^ -o $(BUILD_DIR_BOARD)/$(IDOS_PROJECT).$(BOARD) 
