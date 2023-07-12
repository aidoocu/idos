#Makefile para hello world

# Declarar el nombre del proyecto que será construido que tiene que
# ser igual que el archivo de código sin extensión
IDOS_PROJECT = stp_motor_coap

# Declarar dónde está IDOS
IDOS = ../..

# Cargamos el módulo de CoAP
APPS += coap

# Configuramos ip como estático
CFLAGS += -DNET_STATIC

# Regla por defecto
all: $(IDOS_PROJECT)

# Incorporar el resto de los makefile
include $(IDOS)/tools/make/idos.mk
