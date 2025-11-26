/** 
 * 
 * 
 * 
 * 
 */

 #ifdef NATIVE

#include <idos.h>
#include "arch_sys.h"

/*-------------------------------- sys_cmd() --------------------------------*/

//esta funcion tal vez deba estar en otro archivo mas global
int sys_cmd(const char * cmd_buf, ...) {

    /* Memoria para el comando formateado y... */
    char cmd[250];
    /* ... lista de variables */
    va_list args;

    /* Cargando la lista de variables y formateando el comando */
    va_start(args, cmd_buf);
    vsnprintf(cmd, sizeof(cmd), cmd_buf, args);
    va_end(args);

    return system(cmd);

}

#endif /* NATIVE */