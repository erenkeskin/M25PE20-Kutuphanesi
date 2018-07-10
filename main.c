/********************************************************
 *                                                      *
 * HASAN EREN KESKIN                                    *
 * ELECTRICAL AND ELECTRONICS ENGINEERING STUDENT       *
 * herenkeskin@gmail.com                                *
 * eren.keskin@faydam.com.tr                            *
 *                                                      *
 * 10.07.2018 - FAYDAM INNOVATION                       *
 *                                                      *
 * RAF ETIKETI - Master                                 *
 * Versiyon: 0.1                                        *
 *                                                      *
 *******************************************************/

// Dahil edilmesi gereken kütüphaneler
#include <sistem_ayarlari.h>

unsigned char dataTable[25];

FLASH_SETTINGS flash_settings[1000];

int main(void)
{

	WDTCTL = WDTPW | WDTHOLD;

	CLOCK_AYARI();

	M25PE20_Init();

    _enable_interrupts();
    
    BASLAT_TIMER0();

    M25PE20_Flash_Configuration(&flash_settings[0]);
    int number_of_pending_data_in_flash = M25PE20_Flash_Total_Data_Size(&flash_settings[0]);
    dataTable[0] = 5;

	while(1)
	{

	    if(number_of_pending_data_in_flash > 0)
	    {
	        for(; number_of_pending_data_in_flash > 0 ;)
	        {
	            M25PE20_Flash_Read_Data(&flash_settings[0], dataTable, 15);
	            flash_settings[0].node_data_counter++;
	            M25PE20_Flash_Update(&flash_settings[0]);
	        }
	    } else {
	        M25PE20_Flash_Add_Data(&flash_settings[0], dataTable, 15);
	        number_of_pending_data_in_flash++;
	    }

	}

	return 0;
}

// Timer A0 Interrupt Service Routine
#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
{
    timer++;
    if( timer == 10)
    {
        timer = 0;
    }
}
