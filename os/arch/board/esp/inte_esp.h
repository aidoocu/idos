/** 
 * 
 * 
 */

#ifndef _INTE_ESP_H_
#define _INTE_ESP_H_

/** 
 * \brief Levantar las interrupciones
 */
void inte_raise(uint8_t creg);

/** 
 * \brief Bajar las interrupciones
 */
uint8_t inte_fall(void);

#endif /* _INTE_ESP_H_ */