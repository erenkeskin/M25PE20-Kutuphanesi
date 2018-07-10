/********************************************************
 *                                                      *
 * HASAN EREN KESKIN                                    *
 * ELECTRICAL AND ELECTRONICS ENGINEERING STUDENT       *
 * herenkeskin@gmail.com                                *
 * eren.keskin@faydam.com.tr                            *
 *                                                      *
 * 25.04.2018 - FAYDAM INNOVATION                       *
 *                                                      *
 * Sistem Ayarlari                                      *
 *                                                      *
 *******************************************************/

#ifndef SISTEM_SISTEM_AYARLARI_H_
#define SISTEM_SISTEM_AYARLARI_H_

// Sabit Kütüphaneler
#include "cc430x613x.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define NOP                     asm ( nop ); asm ( nop );       // 2 cycle delay

void CLOCK_AYARI(void);
void delay(unsigned int delay);
void DURDUR_TIMER0(void);
void BASLAT_TIMER0(void);
void SISTEM_RESET(void);

#endif /* SISTEM_SISTEM_AYARLARI_H_ */
