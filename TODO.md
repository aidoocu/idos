# TODO: idos library cleanup

Objetivo: convertir `lib/idos` en una librería bien formada con un API público mínimo.

Tareas pendientes (resumen):

- Extraer API pública en `lib/idos/include/idos/idos.h` (solo prototipos públicos, typedefs y `#include "idos_conf.h"`).
- Renombrar `os/config.h` o `src/os/config.h` -> `lib/idos/include/idos/idos_conf.h` y actualizar referencias.
- Mantener `lib/idos/os/idos.h` como header interno (implementación). Los `.cpp` de la librería deben incluir los headers internos.
- Limpiar includes internos para que `#include <idos.h>` no arrastre dependencias no inicializadas (ej. tipos y macros de red).
- Revisar ejemplos y apps para usar `#include <idos/idos.h>` o `#include <idos.h>` según convenga.
- Ejecutar `pio run` y corregir errores hasta que la librería compile en entornos soportados.

Notas:
- Actualmente existen forwarders para compatibilidad (`lib/idos/idos.h` y `lib/idos/include/idos/idos.h`). Mantenerlos, pero apuntando al header público mínimo.
- Este archivo es un recordatorio temporal; una vez implementada la extracción de la API, eliminar o actualizar esta lista.

Fecha: 2025-11-24
Autor: equipo de mantenimiento
