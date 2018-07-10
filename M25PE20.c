/********************************************************
 *                                                      *
 * HASAN EREN KESKIN                                    *
 * ELECTRICAL AND ELECTRONICS ENGINEERING STUDENT       *
 * herenkeskin@gmail.com                                *
 * eren.keskin@faydam.com.tr                            *
 *                                                      *
 * 09.07.2018 - FAYDAM INNOVATION                       *
 *                                                      *
 * M25PE20.c                                            *
 *                                                      *
 *******************************************************/

#include "M25PE20.h"

M25PE20_STRUCT Read_Memory;
extern uint8_t CIHAZ_ID[6];

/* Clock sinyalini düsen kenara ayarla */
void M25PE20_Clock_Rising(void)
{
    M25PE20_PIN_CLOCK_HIGH;
    M25PE20_PIN_CLOCK_LOW;
}

/* Clock sinyalini yükselen kenara ayarla */
void M25PE20_Clock_Falling(void)
{
    M25PE20_PIN_CLOCK_LOW;
    M25PE20_PIN_CLOCK_HIGH;
}

/* SPI ile slave cihaza veri yazma */
void M25PE20_SPI_Write(uint8_t value)
{

    uint8_t i;

    for(i = 0; i < 8; i++)
    {

        // SERIAL_CLOCK = 0;
        M25PE20_PIN_CLOCK_LOW;
        if(value & 0x80)
        {
            // MOSI = 1;
            M25PE20_PIN_MOSI_HIGH;
        } else {
            // MOSI = 0;
            M25PE20_PIN_MOSI_LOW;
        }

        value = (value << 1);
        // SERIAL_CLOCK = 1;
        M25PE20_PIN_CLOCK_HIGH;

    }
}

/* SPI ile slave'den veri okuma */
uint8_t M25PE20_SPI_Read(void)
{

    int8_t i;
    uint8_t bitler = 0;

    // Bit by Bit oku
    for(i = 8; i >= 1; i--)
    {
        M25PE20_Clock_Falling();

        // Bitleri oku
        bitler |= (M25PE20_PIN_MOSI) << (i - 1);
    }

    return bitler;
}

/*
 *  1. Set UCSWRST (BIS.B #UCSWRST, &UCxCTL1).
 *  2. Initialize all USCI registers with UCSWRST = 1 (including UCxCTL1).
 *  3. Configure ports.
 *  4. Clear UCSWRST via software ( BIC.B #UCSWRST, &UCxCTL1 ).
 *  5. Enable interrupts (optional) via UCRXIE and/or UCTXIE.
 */
void M25PE20_Init(void)
{

    M25PE20_PIN_CLOCK_LOW;
    M25PE20_PIN_CHIP_SELECT_HIGH;
    P5DIR |=  M25PE20_PIN_CLOCK + M25PE20_PIN_MISO;
    P5DIR &= ~M25PE20_PIN_MOSI;
    P1DIR |=  M25PE20_PIN_CHIP_SELECT;

    delay(100);

}

void M25PE20_Chip_Select(void)
{
    M25PE20_PIN_CLOCK_LOW;
    M25PE20_PIN_CHIP_SELECT_LOW;
}

void M25PE20_Chip_Deselect(void)
{
    M25PE20_PIN_CHIP_SELECT_HIGH;
}

void M25PE20_Write_Enable(void)
{

    uint16_t timeout = 0;

    M25PE20_Chip_Select();
    M25PE20_SPI_Write(M25PE20_WRITE_ENABLE);
    M25PE20_Chip_Deselect();

    while((!STATUS_REGISTER_WEL()) && (timeout < 1000))
    {
        timeout++;
    }
}


void M25PE20_Write_Disable(void)
{
    M25PE20_Chip_Select();
    M25PE20_SPI_Write(M25PE20_WRITE_DISABLE);
    M25PE20_Chip_Deselect();
}

void M25PE20_Read_Identification(M25PE20_STRUCT * Read_Memory)
{
    uint8_t i;

    M25PE20_Chip_Select();
    M25PE20_SPI_Write(M25PE20_READ_IDENTIFICATION);

    Read_Memory->man_id     = M25PE20_SPI_Read();
    Read_Memory->mem_type   = M25PE20_SPI_Read(); /* Device ID MSB */
    Read_Memory->mem_size   = M25PE20_SPI_Read(); /* Device ID LSB */
    Read_Memory->uid_len    = M25PE20_SPI_Read();

    for(i = 0; i < Read_Memory->uid_len; i++)
    {
        Read_Memory->uid[i] = M25PE20_SPI_Read();
    }

    M25PE20_Chip_Deselect();
}


uint8_t M25PE20_Check(void)
{

    M25PE20_Read_Identification(&Read_Memory);

    if((Read_Memory.mem_type == 0x80) && (Read_Memory.mem_size == 0x12))
    {
        return 1;
    } else {
        return 0;
    }
}


uint8_t M25PE20_Read_Status_Register(void)
{

    uint8_t value;

    M25PE20_Chip_Select();
    M25PE20_SPI_Write(M25PE20_READ_STATUS_REGISTER);
    value = M25PE20_SPI_Read();
    M25PE20_Chip_Deselect();

    return value;

}

void M25PE20_Write_Status_Register(uint8_t value)
{

    // Write Enable
    M25PE20_Write_Enable();

    M25PE20_Chip_Select();
    M25PE20_SPI_Write(M25PE20_WRITE_STATUS_REGISTER);
    M25PE20_SPI_Write(value);

    M25PE20_Chip_Deselect();

}



void M25PE20_Read(uint8_t * addr, uint8_t * buff, uint8_t buff_len)
{
    uint8_t i;

    M25PE20_Chip_Select();

    #if M25PE20_READ_FAST
        M25PE20_SPI_Write(M25PE20_READ_HIGHER_SPEED);
    #else
        M25PE20_SPI_Write(M25PE20_READ_DATA_BYTES);
    #endif

    /* Write the address, MSB in addr[0], bits [7:5] of the MSB: 'don't care' */
    for(i = 0; i < 3; i++)
    {
        M25PE20_SPI_Write(addr[i]);
    }

    /* For FAST_READ, send the dummy byte */
    #if M25PE20_READ_FAST
        M25PE20_SPI_Write(M25PE20_DUMMY_BYTE);
    #endif

    for(i = 0; i < buff_len; i++) {
        buff[i] = ~M25PE20_SPI_Read();
    }

    M25PE20_Chip_Deselect();
}

void M25PE20_Page_Program (uint8_t * addr, uint8_t * buff, uint8_t buff_len)
{
    uint8_t i;

    M25PE20_Write_Enable(); // Write Enable

    M25PE20_Chip_Select();
    M25PE20_SPI_Write(M25PE20_PAGE_PROGRAM);

    /* Write the address, MSB in addr[0] */
    for(i = 0; i < 3; i++)
    {
        M25PE20_SPI_Write(addr[i]);
    }

    /* Write the bytes */
    for(i = 0; i < buff_len; i++)
    {
        M25PE20_SPI_Write(~buff[i]);
    }

    M25PE20_Chip_Deselect();

    delay(6);

}

//  PAGE_ERASE function
void M25PE20_Page_Erase(uint8_t s)
{

    M25PE20_Write_Enable(); // Write Enable

    M25PE20_Chip_Select();

    M25PE20_SPI_Write(M25PE20_PAGE_ERASE);
    M25PE20_SPI_Write(s);
    M25PE20_SPI_Write(0x00);
    M25PE20_SPI_Write(0x00);

    M25PE20_Chip_Deselect();

    delay(300);

}

void M25PE20_Sector_Erase(uint8_t s)
{

    M25PE20_Write_Enable(); // Write Enable

    M25PE20_Chip_Select();

    M25PE20_SPI_Write(M25PE20_SECTOR_ERASE);
    M25PE20_SPI_Write(s);
    M25PE20_SPI_Write(0x00);
    M25PE20_SPI_Write(0x00);

    M25PE20_Chip_Deselect();

    delay(6000);

}

//  Sector Erase function with Address
void M25PE20_Sector_Erase_With_Address(uint8_t* s)
{

    // Write Enable
    M25PE20_Write_Enable();

    M25PE20_Chip_Select();

    M25PE20_SPI_Write(M25PE20_SECTOR_ERASE);
    M25PE20_SPI_Write(s[0]);
    M25PE20_SPI_Write(s[1]);
    M25PE20_SPI_Write(s[2]);

    M25PE20_Chip_Deselect();

    delay(300);

}

// Subsector Erase
void M25PE20_Subsector_Erase (uint16_t subsector)
{

    uint8_t sec, subsec;

    sec = subsector / 16;
    subsec = subsector % 16;
    subsec = subsec << 4;

    M25PE20_Write_Enable();
    M25PE20_Chip_Select();

    M25PE20_SPI_Write(M25PE20_SUBSECTOR_ERASE);
    M25PE20_SPI_Write(sec);
    M25PE20_SPI_Write(subsec);
    M25PE20_SPI_Write(0x00);


    M25PE20_Chip_Deselect();
    delay(950);

}

/*
 * at maximum it takes 600 mSec
 */
//ERASE SUBSECTOR with WIP waiting
void M25PE20_Subsector_Erase_WIP (uint16_t subsector)
{
    uint16_t timeout = 120;
    uint8_t sec, subsec;

    sec = subsector / 16;
    subsec = subsector % 16;
    subsec = subsec << 4;

    M25PE20_Write_Enable();
    M25PE20_Chip_Select();

    M25PE20_SPI_Write(M25PE20_SUBSECTOR_ERASE);
    M25PE20_SPI_Write(sec);
    M25PE20_SPI_Write(subsec);
    M25PE20_SPI_Write(0x00);
    M25PE20_Chip_Deselect();

    while(STATUS_REGISTER_WIP())
    {
        if( !(--timeout))
        {
          break;
        }
        delay(10);
    }

}

void M25PE20_Bulk_Erase(void)
{

    uint16_t timeout = 64;

    M25PE20_Write_Enable(); // Write Enable

    M25PE20_Chip_Select();
    M25PE20_SPI_Write(M25PE20_BULK_ERASE);
    M25PE20_Chip_Deselect();

    while(STATUS_REGISTER_WIP())
    {
        if( !(--timeout))
        {
            break;
        }
        delay(1000);
    }
}

void M25PE20_Deep_Power_Down(void)
{
    M25PE20_Chip_Select();
    M25PE20_SPI_Write(M25PE20_DEEP_POWER_DOWN);
    M25PE20_Chip_Deselect();
}

/*
 * Release Deep Power Down. We do NOT read the Electronic Signature
 */
void M25PE20_Release_Deep_Power_Down(void)
{

    int32_t i = 0;

    M25PE20_Chip_Select();
    M25PE20_SPI_Write(M25PE20_RELEASE_DEEP_POWER_DOWN);
    M25PE20_Chip_Deselect();

    /* a few usec between RES and standby */
    while(STATUS_REGISTER_WIP())
    {
        i++;
        if(i > 50000)
        {
            break;
        }
    }
}

/**
 * Release Deep Power Down. Read and return the Electronic Signature
 * must return 0x14
 *
 * \return The old style Electronic Signature. This must be 0x14
 */
uint8_t M25PE20_Release(void)
{

    uint8_t value;

    M25PE20_Chip_Select();
    M25PE20_SPI_Write(M25PE20_RELEASE_DEEP_POWER_DOWN);
    M25PE20_SPI_Write(M25PE20_DUMMY_BYTE);
    M25PE20_SPI_Write(M25PE20_DUMMY_BYTE);
    M25PE20_SPI_Write(M25PE20_DUMMY_BYTE);

    value = M25PE20_SPI_Read();

    M25PE20_Chip_Deselect();

    /* a few usec between RES and standby */
    while(STATUS_REGISTER_WIP());

    return value;

}

void M25PE20_Flash_Read(uint32_t address, uint8_t * buff, int16_t buff_len)
{

    int16_t i;
    uint8_t  addr[3];

    addr[2] = (uint8_t) (address & 0x000000ff);
    addr[1] = (uint8_t) ((address & 0x0000ff00) >> 8);
    addr[0] = (uint8_t) ((address & 0x00ff0000) >> 16);

    M25PE20_Chip_Select();

    #if M25PE20_READ_FAST
        M25PE20_SPI_Write(M25PE20_READ_HIGHER_SPEED);
    #else
        M25PE20_SPI_Write(M25PE20_READ_DATA_BYTES);
    #endif

    /* Write the address, MSB in addr[0], bits [7:5] of the MSB: 'don't care' */
    for(i = 0; i < 3; i++)
    {
        M25PE20_SPI_Write(addr[i]);
    }

    /* For FAST_READ, send the dummy byte */
    #if M25PE20_READ_FAST
        M25PE20_SPI_Write(M25PE20_DUMMY_BYTE);
    #endif

    for(i = 0; i < buff_len; i++)
    {
        buff[i] = ~M25PE20_SPI_Read();
    }

    M25PE20_Chip_Deselect();

}

//all data must be in the same page, enkom panel programmer must send bytes with this in mind
//i.e 128 byte chunks or 256 byte chunks
void M25PE20_Flash_Write(uint32_t address, uint8_t * buff, int16_t buff_len)
{

    int16_t i;
    uint8_t  addr[3];

    addr[2] = (uint8_t) ( address & 0x000000ff);
    addr[1] = (uint8_t) ((address & 0x0000ff00) >> 8);
    addr[0] = (uint8_t) ((address & 0x00ff0000) >> 16);

    M25PE20_Write_Enable(); // Write Enable

    M25PE20_Chip_Select();
    M25PE20_SPI_Write(M25PE20_PAGE_PROGRAM);

    // Write the address, MSB in addr[0]
    for(i = 0; i < 3; i++)
    {
        M25PE20_SPI_Write(addr[i]);
    }

    // Write the bytes
    for(i = 0; i < buff_len; i++)
    {
        M25PE20_SPI_Write(~buff[i]);
    }

    M25PE20_Chip_Deselect();
    delay(6);

}

int16_t M25PE20_Test()
{
    return M25PE20_Check();
}

void M25PE20_Flash_Configuration(FLASH_SETTINGS * flash_struct)
{
    M25PE20_Init();

    M25PE20_Release_Deep_Power_Down();

    M25PE20_Read(flash_struct->flash_header_address ,  (uint8_t *) flash_struct, sizeof(FLASH_SETTINGS));

    if(flash_struct->magic_word == MAGIC_WORD)   //not first time
    {
        flash_struct->total_boot++;
        flash_struct->cihaz_id[0] = CIHAZ_ID[2];
        M25PE20_Flash_Update(flash_struct);
    } else {                                //first time

        flash_struct->magic_word = MAGIC_WORD;

        flash_struct->cihaz_id[0] = CIHAZ_ID[0];
        flash_struct->cihaz_id[1] = CIHAZ_ID[1];
        flash_struct->cihaz_id[2] = CIHAZ_ID[2];
        flash_struct->cihaz_id[3] = CIHAZ_ID[3];
        flash_struct->cihaz_id[4] = CIHAZ_ID[4];
        flash_struct->cihaz_id[5] = CIHAZ_ID[5];

        flash_struct->total_boot=1;
        flash_struct->node_data_counter = 8;
        flash_struct->flash_header_address[0] = 0x00;
        flash_struct->flash_header_address[1] = 0x00;
        flash_struct->flash_header_address[2] = 0x00;
        flash_struct->max_number_of_node_data = 6144;

        M25PE20_Flash_Update(flash_struct);

    }

    M25PE20_Deep_Power_Down();
}


unsigned char cbuffer[256];
void M25PE20_Flash_Update(FLASH_SETTINGS * flash_struct)
{
    M25PE20_Init();
    M25PE20_Release_Deep_Power_Down();
    M25PE20_Read((uint8_t *)flash_struct->flash_header_address, cbuffer, 255); //read whole page
    M25PE20_Page_Erase(0); //clear first page

    memcpy(cbuffer, flash_struct,  sizeof(FLASH_SETTINGS) );
    M25PE20_Page_Program((uint8_t *)flash_struct->flash_header_address, cbuffer, 255);

    M25PE20_Deep_Power_Down();
}

void M25PE20_Flash_Add_Data(FLASH_SETTINGS * flash_struct, uint8_t * data, int16_t data_size)
{
    M25PE20_Init();
    M25PE20_Release_Deep_Power_Down();
    M25PE20_Read((uint8_t *)flash_struct->flash_header_address, cbuffer, 255); //read whole page

    memcpy(cbuffer, flash_struct,  sizeof(FLASH_SETTINGS) );

    if((flash_struct->node_data_counter % flash_struct->max_number_of_node_data) == 0)
    {
        flash_struct->node_data_counter = flash_struct->node_data_counter + 8;
    }

    memcpy(cbuffer + ((flash_struct->node_data_counter % 8) * 32), data, data_size );

    flash_struct->node_data_counter++;

    M25PE20_Page_Erase(0); //clear header page
    memcpy(cbuffer, flash_struct,  sizeof(FLASH_SETTINGS) );

    M25PE20_Page_Program((uint8_t *)flash_struct->flash_header_address, cbuffer, 255);
    M25PE20_Deep_Power_Down();
}


uint8_t M25PE20_Flash_Total_Data_Size(FLASH_SETTINGS * flash_struct)
{

    if(flash_struct->node_data_counter > 8)
    {
        return (flash_struct->node_data_counter - 8);
    } else {
        return 0;
    }

}

void M25PE20_Flash_Read_Data(FLASH_SETTINGS * flash_struct, uint8_t * dataTable, int16_t dataTable_size)
{
    M25PE20_Init();
    M25PE20_Release_Deep_Power_Down();
    M25PE20_Read((uint8_t *) flash_struct->flash_header_address, cbuffer, 255); //read whole page

    memcpy(cbuffer, flash_struct, sizeof(FLASH_SETTINGS) );

    if(M25PE20_Flash_Total_Data_Size(flash_struct) > 0)
    {

        flash_struct->node_data_counter--;

        if(flash_struct->node_data_counter % flash_struct->max_number_of_node_data == 7)
        {
            flash_struct->node_data_counter = flash_struct->node_data_counter - 8;
        }

        memcpy(dataTable, cbuffer +((flash_struct->node_data_counter % 8) * 32), dataTable_size );

        M25PE20_Page_Erase(0); //clear header page
        memcpy(cbuffer, flash_struct,  sizeof(FLASH_SETTINGS) );

        M25PE20_Page_Program((uint8_t *)flash_struct->flash_header_address, cbuffer, 255);

    }

    M25PE20_Deep_Power_Down();
}

void M25PE20_Flash_Update_Settings(FLASH_SETTINGS * flash_struct)
{
    M25PE20_Init();
    M25PE20_Release_Deep_Power_Down();

    memcpy(cbuffer, flash_struct,  sizeof(FLASH_SETTINGS) );
    M25PE20_Page_Program((uint8_t *)flash_struct->flash_header_address, cbuffer, 255);

    M25PE20_Deep_Power_Down();
}
