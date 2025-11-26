// Public forwarding header to expose <idos.h>
#ifndef IDOS_PUBLIC_H
#define IDOS_PUBLIC_H

/* Forward to internal os header. The path uses a relative include so that
   PlatformIO's include path (lib/idos/include) finds this file and then it
   pulls the real implementation from lib/idos/os/idos.h. */
#include "idos/idos.h"

#endif // IDOS_PUBLIC_H
