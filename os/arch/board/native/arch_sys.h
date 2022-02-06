/** 
 * 
 * \brief Definiciones del sistema base para todo native
 */

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>

#include <stdbool.h>
#include <stdint.h>



//#include <string.h>
//#include <sys/ioctl.h>
//#include <sys/socket.h>
//#include <sys/types.h>
//#include <sys/time.h>
//#include <sys/uio.h>

int sys_cmd(const char * cmd_buf, ...);