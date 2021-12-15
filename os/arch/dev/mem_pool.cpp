/** 
 * 
 * 
 *  \date noviembre 2021
 */

#include "mem_pool.h"
#include "../arch.h"

#define POOLOFFSET 1



void mem_pool_init() {

    memset(&blocks[0], 0, sizeof(blocks));
    blocks[POOLSTART].begin = MEMPOOL_STARTADDRESS;
    blocks[POOLSTART].size = 0;
    blocks[POOLSTART].nextblock = NOBLOCK;

}

mem_handle_t mem_block_alloc(uint16_t size) {

    mem_block_st * best = __null;
    mem_handle_t cur = POOLSTART;
    mem_block_st * block = &blocks[POOLSTART];
    mem_address_t bestsize = MEMPOOL_SIZE + 1;

    /* Aquí se recorre el pool de memoria buscando donde alojar el bloque nuevo */
    do {

        mem_handle_t next = block->nextblock;
        /* Aquí se determina cuanta memoria queda disponible */
        mem_address_t freesize = (next == NOBLOCK ? blocks[POOLSTART].begin + MEMPOOL_SIZE : blocks[next].begin) - block->begin - block->size;
        
        if (freesize == size) {
            best = &blocks[cur];
            goto found;
        }

        if (freesize > size && freesize < bestsize) {
            bestsize = freesize;
            best = &blocks[cur];
        }

        if (next == NOBLOCK) {
            if (best) 
                goto found;
            else
                goto collect;
        }

        block = &blocks[next];
        cur = next;

    } while (true);

    collect: {

        cur = POOLSTART;
        block = &blocks[POOLSTART];
        mem_handle_t next;
        while ((next = block->nextblock) != NOBLOCK) {

            mem_address_t dest = block->begin + block->size;
            mem_block_st * nextblock = &blocks[next];
            mem_address_t* src = &nextblock->begin;

            if (dest != * src) {
                #ifdef MEMPOOL_MEMBLOCK_MV
                mempool_block_move_callback_arch(dest, * src, nextblock->size);
                #endif
                * src = dest;
            }

            block = nextblock;

        }
        if (blocks[POOLSTART].begin + MEMPOOL_SIZE - block->begin - block->size >= size)
            best = block;
        else
            goto notfound;

    }

    found: {

        block = &blocks[POOLOFFSET];

        for (cur = POOLOFFSET; cur < MEMPOOL_NUM_MEMBLOCKS + POOLOFFSET; cur++) {
            if (block->size) {
                block++;
                continue;
            }

            mem_address_t address = best->begin + best->size;

            #ifdef MEMBLOCK_ALLOC
            MEMBLOCK_ALLOC(address,size);
            #endif

            block->begin = address;
            block->size = size;
            block->nextblock = best->nextblock;
            best->nextblock = cur;

          return cur;
        }
        
    }

    notfound: 
        return NOBLOCK;

}


void mem_block_free(mem_handle_t handle) {

    if (handle == NOBLOCK) {
        return;
    }

    mem_block_st * b = &blocks[POOLSTART];

    do {
        mem_handle_t next = b->nextblock;
        if (next == handle) {
            mem_block_st * f = &blocks[next];

            b->nextblock = f->nextblock;
            f->size = 0;
            f->nextblock = NOBLOCK;
            return;
        }
        if (next == NOBLOCK)
        return;
        b = &blocks[next];
    } while (true);   

}

void mem_block_resize(mem_handle_t handle, mem_address_t position, uint16_t size) {

    mem_block_st * block = &blocks[handle];
    block->begin += position;
    size == NULL ? block->size -= position : block->size = size;
   
}

/** 
 * 
 */
mem_handle_t mem_block_size(mem_handle_t handle) {

    return blocks[handle].size;

}

void mempool_block_move_callback(mem_address_t dest, mem_address_t src, uint16_t size) {

    mempool_block_move_callback_arch(dest, src, size);
    
}