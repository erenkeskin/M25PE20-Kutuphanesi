/********************************************************
 *                                                      *
 * HASAN EREN KESKIN                                    *
 * ELECTRICAL AND ELECTRONICS ENGINEERING STUDENT       *
 * herenkeskin@gmail.com                                *
 * eren.keskin@faydam.com.tr                            *
 *                                                      *
 * 09.07.2018 - FAYDAM INNOVATION                       *
 *                                                      *
 * M25PE20.h                                            *
 *                                                      *
 *******************************************************/

#ifndef EKLENTILER_M25PE20_H_
#define EKLENTILER_M25PE20_H_

#include "sistem_ayarlari.h"

// Komut Setleri
#define M25PE20_WRITE_ENABLE                0x06    // Write Enable
#define M25PE20_WRITE_DISABLE               0x04    // Write Disable
#define M25PE20_READ_IDENTIFICATION         0x9F    // Read Identification
#define M25PE20_READ_STATUS_REGISTER        0x05    // Read Status Register
#define M25PE20_WRITE_STATUS_REGISTER       0x01    // Write Status Register
#define M25PE20_WRITE_LOCK_REGISTER         0xE5    // Write Lock Register
#define M25PE20_READ_LOCK_REGISTER          0xE8    // Read Lock Register
#define M25PE20_READ_DATA_BYTES             0x03    // Read Data Bytes
#define M25PE20_READ_HIGHER_SPEED           0x0B    // Read Data Bytes at Higher Speed
#define M25PE20_PAGE_WRITE                  0x0A    // Page Write
#define M25PE20_PAGE_PROGRAM                0x02    // Page Program
#define M25PE20_PAGE_ERASE                  0xD8    // Page Erase
#define M25PE20_SUBSECTOR_ERASE             0x20    // Subsector Erase
#define M25PE20_SECTOR_ERASE                0xD8    // Sector Erase
#define M25PE20_BULK_ERASE                  0xC7    // Bulk Erase
#define M25PE20_DEEP_POWER_DOWN             0xB9    // Deep Power-Down
#define M25PE20_RELEASE_DEEP_POWER_DOWN     0xAB    // Release from Deep Power-Down
#define M25PE20_DUMMY_BYTE                  0x00    // Dummy Byte
#define M25PE20_READ_FAST                   0       // Hizli Oku

// Status Register Bitleri
#define STATUS_REGISTER_WRITE_IN_PROGRESS   0x01   // Write in Progress
#define STATUS_REGISTER_WRITE_ENABLE_LATCH  0x02   // Write Enable Latch
#define STATUS_REGISTER_BLOCK_PROTECT       0x1C   // All Block Protect Bits
#define STATUS_REGISTER_BLOCK_PROTECT_0     0x04   // Block Protect 0
#define STATUS_REGISTER_BLOCK_PROTECT_1     0x08   // Block Protect 1
#define STATUS_REGISTER_BLOCK_PROTECT_2     0x10   // Block Protect 2
#define STATUS_REGISTER_WRITE_PROTECT       0x80   // Status Register Write Protect
#define STATUS_REGISTER_WEL()               (M25PE20_Read_Status_Register() & STATUS_REGISTER_WRITE_ENABLE_LATCH)
#define STATUS_REGISTER_WIP()               (M25PE20_Read_Status_Register() & STATUS_REGISTER_WRITE_IN_PROGRESS)
#define STATUS_REGISTER_BP()                (M25PE20_Read_Status_Register() & STATUS_REGISTER_BLOCK_PROTECT)

// Giris - Cikis Pinleri
#define M25PE20_PORT_READ                   P5IN
#define M25PE20_PIN_CLOCK                   BIT7
#define M25PE20_PIN_MISO                    BIT6
#define M25PE20_PIN_MOSI                    BIT4
#define M25PE20_PIN_CHIP_SELECT             BIT7

// Ana Makrolar
#define M25PE20_PIN_CHIP_SELECT_HIGH        P1OUT |=  M25PE20_PIN_CHIP_SELECT       // Flash aktif degil
#define M25PE20_PIN_CHIP_SELECT_LOW         P1OUT &= ~M25PE20_PIN_CHIP_SELECT       // Flash aktif
#define M25PE20_PIN_CLOCK_HIGH              P5OUT |=  M25PE20_PIN_CLOCK             // Clock - Logic 1
#define M25PE20_PIN_CLOCK_LOW               P5OUT &= ~M25PE20_PIN_CLOCK             // Clock - Logic 0
#define M25PE20_PIN_MISO_HIGH               P5OUT |=  M25PE20_PIN_MISO              // Veri almasi aktif
#define M25PE20_PIN_MISO_LOW                P5OUT &= ~M25PE20_PIN_MISO              // Veri almasi aktif degil
#define M25PE20_PIN_MOSI_HIGH               P5OUT |=  M25PE20_PIN_MOSI              // Veri yazmasi aktif
#define M25PE20_PIN_MOSI_LOW                P5OUT &= ~M25PE20_PIN_MOSI              // Veri yazmasi aktif degil

typedef struct M25PE20_STRUCTURE
{
    uint8_t man_id;     // Manufacturer ID
    uint8_t mem_type;   // Memory Type
    uint8_t mem_size;   // Memory Size
    uint8_t uid_len;    // Unique ID length
    uint8_t uid[16];    // Unique ID
} M25PE20_STRUCT;

typedef struct FLASH_SETTINGS
{
    unsigned short  magic_word;
    unsigned char   cihaz_id[6];
    int16_t         node_data_counter;
    unsigned char   flash_header_address[3];
    int16_t         max_number_of_node_data;
    int16_t         total_boot;
} FLASH_SETTINGS;

extern FLASH_SETTINGS flash_settings[1000];

#define MAGIC_WORD 0xabcd

// Fonksiyon Prototipleri
void    M25PE20_SPI_Write(uint8_t value);
uint8_t M25PE20_SPI_Read(void);
void    M25PE20_Clock_Rising(void);
void    M25PE20_Clock_Falling(void);
void    M25PE20_Init(void);
void    M25PE20_Chip_Select(void);
void    M25PE20_Chip_Deselect(void);
void    M25PE20_Write_Enable(void);
void    M25PE20_Write_Disable(void);
void    M25PE20_Read_Identification(M25PE20_STRUCT * Read_Memory);
uint8_t M25PE20_Check(void);
uint8_t M25PE20_Read_Status_Register(void);
void    M25PE20_Write_Status_Register(uint8_t value);
void    M25PE20_Read(uint8_t * addr, uint8_t * buff, uint8_t buff_len);
void    M25PE20_Page_Program(uint8_t * addr, uint8_t * buff, uint8_t buff_len);
void    M25PE20_Page_Erase(uint8_t s);
void    M25PE20_Sector_Erase(uint8_t s);
void    M25PE20_Sector_Erase_With_Address(uint8_t * s);
void    M25PE20_Subsector_Erase (uint16_t subsector);
void    M25PE20_Subsector_Erase_WIP (uint16_t subsector);
void    M25PE20_Bulk_Erase(void);
void    M25PE20_Deep_Power_Down(void);
void    M25PE20_Release_Deep_Power_Down(void);
uint8_t M25PE20_Release(void);
void    M25PE20_Flash_Read(uint32_t address, uint8_t * buff, int16_t buff_len);
void    M25PE20_Flash_Write(uint32_t address, uint8_t * buff, int16_t buff_len);
int16_t M25PE20_Test(void);
void    M25PE20_Flash_Update(FLASH_SETTINGS * flash_struct);
void    M25PE20_Flash_Add_Data(FLASH_SETTINGS * flash_struct, uint8_t * data, int16_t data_size);
void    M25PE20_Flash_Configuration(FLASH_SETTINGS * flash_struct);
uint8_t M25PE20_Flash_Total_Data_Size(FLASH_SETTINGS * flash_struct);
void    M25PE20_Flash_Read_Data(FLASH_SETTINGS * flash_struct, uint8_t * dataTable, int16_t dataTable_size);
void    M25PE20_Update_Flash_Settings(FLASH_SETTINGS * flash_struct);

#endif /* EKLENTILER_M25PE20_H_ */
