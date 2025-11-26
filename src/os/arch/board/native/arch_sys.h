/** 
 * 
 * \brief Definiciones del sistema base para todo native
 */

#ifdef NATIVE

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>




//#include <string.h>

//
//#include <sys/types.h>
//#include <sys/uio.h>

int sys_cmd(const char * cmd_buf, ...);

#endif /* NATIVE */