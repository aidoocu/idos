/** 
 *  \file mempool.h
 *  \brief Una implementación ligera y simple de un pool de memoria
 *  \note Esta es una modificación del archivo mempool de (2013)
 *        Norbert Truchsess <norbert.truchsess@t-online.de>
 *  \author Bernardo León Ávila <bernardoyla@gmail.com>
 *  \date noviembre 2021
 *  \copyright  
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *    
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _MEM_POOL_H_
#define _MEM_POOL_H_

#include <inttypes.h>
#include "mem_pool_conf.h"

#define POOLSTART 0
#define NOBLOCK 0

typedef uint16_t mem_address_t;
typedef uint8_t mem_handle_t;

/** \brief Estrtuctura de cada bloque de memoria */
struct mem_block_st {
  mem_address_t begin;
  mem_address_t size;
  mem_handle_t nextblock;
};

/** \brief Array de bloques creados */
static struct mem_block_st blocks[MEMPOOL_NUM_MEMBLOCKS + 1];

/** 
 * \brief Crear y asignar un bloque de memoria
 * \param size Tamaño del bloque
 * \return Manejador del bloque
*/
mem_handle_t mem_block_alloc(uint16_t size);

/** 
 * \brief Liberar este bloque de memoria
 * \param handle bloque a liberar
*/
void mem_block_free(mem_handle_t handle);

/** 
 *  \brief Modificar el tamaño del bloque
 *  \param handle Manejador del bloque
 *  \param position Posición del bloque
 *  \param size Nuevo tamaño del bloque, puede ser NULL
 */
void mem_block_resize(mem_handle_t handle, mem_address_t position, uint16_t size);

/** 
 *  \brief Obtener el tamaño de un bloque de memoria
 */
mem_handle_t mem_block_size(mem_handle_t handle);

#endif /* _MEM_POOL_H_ */