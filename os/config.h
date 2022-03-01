/*
 * Copyright (c) 2022, Bernardo Yaser León Ávila <bernardoyla@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the idOS operating system.
 *
 */
/** 
 *  \file idos.h
 *  \author Bernardo Yaser León Ávila <bernardoyla@gmail.com>
 *  \copyright Todos los derechos reservados
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

/** Activa el modo de ahorro de energía */
#define SLEEP_MODE 0

/** Se utiliza el SPI */
#define SPI_ARCH

/** ------------------------ Debug Info ------------------------ */

#define NET_DEBUG_0 0
#define NET_DEBUG_1 1
#define NET_DEBUG_2 2
#define NET_DEBUG_3 3


#define NET_DEBUG   2


/** -------------------- Dispositivos (dev) -------------------- */
/** \brief Se defininen los dispositivos y forma de conectarse   */



/**                          Networkig                           */

/** \b ENC28J60  
 *  \brief      Microchip ENC28J60 Ethernet Interface
 *  \details    Se conecta por SPI y solo implementa el mecanismo de
 *              acceso al medio Ethernet. Deberá utilizarse uip
*/
//#define __ENC28J60__

/** Configuraciones por defecto */
#ifdef __ENC28J60__

#ifndef SPI_ARCH
#define SPI_ARCH
#endif

#endif /* __ENC28J60__ */


/** ------------------- Definiciones de red -------------------- */

#define NET_STACK_NULL  0
#define NET_STACK_HW    1
#define NET_STACK_UIP   2


/* Comente la linea a continuación si no tiene ninguna interface de red */
#define NET_STACK NET_STACK_UIP


#ifdef NET_STACK





/*      Pontencialmente esto no sea necesario       */

/** TRANSPORT 
 * \brief Habilita el uso de la capa de tansporte. Puede comletar con:
 * \b TRANSP_UDP  Su interface será configurada para utilizar IPv4 
 * \b TRANSP_TCP  Su interface será configurada para utilizar IPv6 
 * \note puede comentar esta linea para no cargar la capa de red */
#define CONF_TRANSP TRANSP_UDP

#define TRANSP_UDP 1
#define TRANSP_TCP 2

/** Routing: 
 *  \todo Aun no implementado
*/

/** NET 
 * \brief Habilita el uso de la capa de red. Puede comletar con:
 * \b NET_IPV4  Su interface será configurada para utilizar IPv4 
 * \b NET_IPV6  Su interface será configurada para utilizar IPv6 
 * \note puede comentar esta linea para no cargar la capa de red */
#define CONF_NET NET_IPV4

#define NET_IPV4 1
#define NET_IPV6 2

/** MAC 
 * \brief Habilita el uso de la capa de enlace. Puede comletar con:
 * \b MAC_ETH   Su interface será configurada para utilizar ETH
 * \todo Falta incluir los mecanismos de radio CSMA, TSCH y BLE
 * \note puede comentar esta linea para no cargar la capa de acceso al medio*/
//#define CONF_MAC MAC_ETH

//#define MAC_ETH 1

#endif /* NET_SATCK */


#endif /* _CONFIG_H_ */