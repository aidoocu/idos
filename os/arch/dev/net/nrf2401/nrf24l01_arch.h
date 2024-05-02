/**
 * @file nrf24l01_arch.h
 * @brief Header file for NRF24L01 driver architecture.
 *
 * This file contains the declarations and definitions for the NRF24L01 driver architecture.
 * It provides functions for initializing the NRF24L01, setting MAC address, sending and receiving frames,
 * and controlling power state of the interface.
 */

#ifndef _NRF24L01_H_
#define _NRF24L01_H_

#include "../../../arch.h"

#include "RF24/nRF24L01.h"
#include "RF24/RF24.h"
#include "RF24/RF24_config.h"

/* Pone el pin CS del SPI por defecto. Cambiar esta línea en caso de utilizar otro */
#define NRF24L01_CSN SS
#define NRF24L01_CE 9

/* Open Pipe */
#define NRF_PIPE_0 0
#define NRF_PIPE_1 1
#define NRF_PIPE_2 2
#define NRF_PIPE_3 3
#define NRF_PIPE_4 4
#define NRF_PIPE_5 5

/* MAC address */
#define NRF_ADDR_WIDTH 5
#define NRF_ADDR_0 0x49     /* I */
#define NRF_ADDR_1 0x44     /* D */
#define NRF_ADDR_2 0x4F     /* O */
#define NRF_ADDR_3 0x53     /* S */
#define NRF_ADDR_4 0x00     /* 0 */

#define NRF24L01_CHANNEL 90
#define NRF24L01_PAYLOAD_SIZE 32
#define NRF24L01_RETR_DELAY 5
#define NRF24L01_RETR_COUNT 3
#define NRF24L01_SPEED RF24_250KBPS
#define NRF24L01_POWER RF24_PA_HIGH


#define TX_COLLISION_RETRY_COUNT 3

/** Macros para SPI */

/** \brief Seleccionar el ENC */
#define nrf_select() digitalWrite(NRF24L01_CSN, LOW)

/** \brief Deseleccionar el ENC */
#define nrf_deselect() digitalWrite(NRF24L01_CSN, HIGH)


/* ------------------------------- arch block -------------------------------- */

/** 
 *  \brief Inicializar el NRF24L01 
 *  \param mac Dirección (máscara) MAC que deberá tener el NRF24L01
 *  \return True if initialization is successful, false otherwise.
 */
bool nrf_init(uint8_t * mac);

/** 
 *  \brief Obtener la dirección MAC del NRF24L01
 *  \param mac Puntero al buffer donde se copiará la dirección MAC
 */
void nrf_get_address(uint8_t * mac);

/** 
 *  \brief  Poll the NRF24L01 to check if a frame has been received
 *  \param frame    Buffer where the received frame will be copied to, if any.
 *  \return Size of the received frame. 0 if no frame has been received.
 */
uint16_t nrf_poll(uint8_t * frame);

/** 
 *  \brief Set the destination address for sending frames
 *  \param mac Pointer to the buffer containing the destination MAC address
 */
void set_nrf_dst_addr(uint8_t * mac);

/** 
 *  \brief Send the frame to the NRF24L01 for transmission
 *  \param frame Pointer to the buffer containing the frame in RAM
 *  \param len Size of the frame
 *  \return true on success, false on failure
 */
bool nrf_send(uint8_t * frame, uint16_t len);

/** 
 *  \brief Get the active state of the interface
 *  \return 1 if the interface is active, 0 otherwise.
 */
uint8_t nrf_is_active(void);

/** 
 *  \brief Get the link status of the interface
 *  \return True if the link is active, false otherwise.
 */
bool nrf_link_status(void);

/** 
 *  \brief Power off the NRF24L01 interface
 */
void nrf_power_off(void);

/** 
 *  \brief Power on the NRF24L01 interface
 */
void nrf_power_on(void);


#endif /* _NRF24L01_H_ */