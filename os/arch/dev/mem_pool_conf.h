/* 
 */

#ifndef _MEM_POOL_CONF_H_
#define _MEM_POOL_CONF_H_

#include "../../sys/net/ip/uipethernet-conf.h"


#include "net/enc28j60/enc28j60.h"


extern "C" {
  #include "../../sys/net/ip/uipopt.h"
}

#include <inttypes.h>

#if UIP_SOCKET_NUMPACKETS and UIP_CONNS
#define NUM_TCP_MEMBLOCKS (UIP_SOCKET_NUMPACKETS*2) * UIP_CONNS
#else
#define NUM_TCP_MEMBLOCKS 0
#endif

#if UIP_UDP and UIP_UDP_CONNS
#define NUM_UDP_MEMBLOCKS 3 * UIP_UDP_CONNS
#else
#define NUM_UDP_MEMBLOCKS 0
#endif

#define MEMPOOL_NUM_MEMBLOCKS (NUM_TCP_MEMBLOCKS + NUM_UDP_MEMBLOCKS)

#define MEMPOOL_STARTADDRESS TXSTART_INIT+1
#define MEMPOOL_SIZE TXSTOP_INIT-TXSTART_INIT


#define MEMPOOL_MEMBLOCK_MV

//Esta es la forma que está implementada en el archivo original, ver que ventaja porque no me funcionó correctamente
//void enc28J60_mempool_block_move_callback(memaddress,memaddress,memaddress);
//#define MEMPOOL_MEMBLOCK_MV(dest,src,size) enc28J60_mempool_block_move_callback(dest,src,size)

#endif /* _MEM_POOL_CONF_H_ */
