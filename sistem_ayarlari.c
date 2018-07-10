/********************************************************
 *                                                      *
 * HASAN EREN KESKÝN                                    *
 * ELECTRICAL AND ELECTRONICS ENGINEERING STUDENT       *
 * herenkeskin@gmail.com                                *
 * eren.keskin@faydam.com.tr                            *
 *                                                      *
 * 25.04.2018 - FAYDAM INNOVATION                       *
 *                                                      *
 * Sistem Ayarlarý                                      *
 *                                                      *
 *******************************************************/

#include "SISTEM/sistem_ayarlari.h"

/*******************************************************
 * CC430F6137 CLOCK AYARLARINI YAPMAK ICIN KULLANILIR  *
 * 8MHZ ILE CALISITIRILMAKTA                           *
 *******************************************************/
void CLOCK_AYARI(void)
{

    UCSCTL3 |= SELREF_2;
    UCSCTL4 |= SELA_2;
    __bis_SR_register(SCG0);

    UCSCTL0 = 0x0000;
    UCSCTL1 = DCORSEL_5;
    UCSCTL2 = FLLD_1 + 255;

    __bic_SR_register(SCG0);
    __delay_cycles(600000);

    do
    {
        UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + XT1HFOFFG + DCOFFG);
        SFRIFG1 &= ~OFIFG;
    } while (SFRIFG1&OFIFG);

}

/************************************************************
 * DELAY FUNCTION                                           *
 * ISLEMLER ARASINA GECIKMELER EKLEMEK ICIN KULLANILIR      *
 ************************************************************/
void delay(unsigned int delay)
{

    while(delay)
    {
        __delay_cycles(8000);
        delay--;
    }

}

// Timer A Durdur Fonksiyonu
void DURDUR_TIMER0(void)
{
    TA0CTL |= ( MC_0 + TACLR);
    TA0CCTL0 &= ~CCIE;
}

// Timer A Baslatma Fonksiyonu
void BASLAT_TIMER0(void)
{
    TA0CCTL0 = CCIE;                        // Uyanmak için TA0'ý aç
    TA0CCR0 = 32768;                        // Birim Cycle -> 32768 (8 MHz için)
    TA0CTL = TASSEL_1 + MC_1 + TACLR;
}

// Sistemi Yeniden Baþlat
void SISTEM_RESET(void)
{
    WDTCTL = 0xDEAD;
}
