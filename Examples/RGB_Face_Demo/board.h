#pragma once

#define RGB_MAX_PIXEL_CLOCK_HZ  (8000000UL)

#define BOARD_TFT_WIDTH      (480)
#define BOARD_TFT_HEIGHT     (480)

#define BOARD_TFT_BL         (46)

#define BOARD_TFT_HSYNC      (47)
#define BOARD_TFT_VSYNC      (41)
#define BOARD_TFT_DE         (45)
#define BOARD_TFT_PCLK       (42)

#define XL_PWD                (2)
#define XL_CS                 (3)
#define XL_SCK                (5)
#define XL_MOSI               (4)

// T-RGB physical connection is RGB666, but ESP only supports RGB565

// RGB data signal(
// DB0:BlUE LSB;DB5:BIUE MSB;
// DB6:GREEN LSB;DB11:GREEN,MSB;
// DB12:RED LSB;DB17:RED MSB)
#define BOARD_TFT_B0      (43)   //B0
#define BOARD_TFT_B1      (7)   //B1
#define BOARD_TFT_B2      (6)   //B2
#define BOARD_TFT_B3      (5)   //B3
#define BOARD_TFT_B4      (3)   //B4
#define BOARD_TFT_B5      (2)   //B5    MSB

#define BOARD_TFT_G0      (14)   //G0
#define BOARD_TFT_G1      (13)   //G1
#define BOARD_TFT_G2      (12)   //G2
#define BOARD_TFT_G3      (11)   //G3
#define BOARD_TFT_G4      (10)   //G4
#define BOARD_TFT_G5      (9)    //G5    MSB

#define BOARD_TFT_R0      (44)   //R0
#define BOARD_TFT_R1      (21)    //R1
#define BOARD_TFT_R2      (18)    //R2
#define BOARD_TFT_R3      (17)    //R3
#define BOARD_TFT_R4      (16)    //R4
#define BOARD_TFT_R5      (15)    //R5    MSB

#define BOARD_TFT_RST        (6)
#define BOARD_TFT_CS         (3)
#define BOARD_TFT_MOSI       (4)
#define BOARD_TFT_SCLK       (5)

#define BOARD_I2C_SDA        (8)
#define BOARD_I2C_SCL        (48)

#define BOARD_TOUCH_IRQ      (1)
#define BOARD_TOUCH_RST      (1)

#define BOARD_SDMMC_EN       (7)
#define BOARD_SDMMC_SCK      (39)
#define BOARD_SDMMC_CMD      (40)
#define BOARD_SDMMC_DAT      (38)

#define BOARD_ADC_DET        (4)


// 2.8-inch initialization sequence
const uint8_t st7701_type9_init_operations2[297] = {
    BEGIN_WRITE,

    WRITE_COMMAND_8, 0xFF,
    WRITE_BYTES, 5, 0x77, 0x01, 0x00, 0x00, 0x13,

    WRITE_C8_D8, 0xEF, 0x08,

    WRITE_COMMAND_8, 0xFF,
    WRITE_BYTES, 5, 0x77, 0x01, 0x00, 0x00, 0x10,

    WRITE_C8_D16, 0xC0, 0x3B, 0x00,
    WRITE_C8_D16, 0xC1, 0x10, 0x0C,
    WRITE_C8_D16, 0xC2, 0x07, 0x0A,
    WRITE_C8_D8, 0xC7, 0x00,
    WRITE_C8_D8, 0xC7, 0x10,
    WRITE_C8_D8, 0xCD, 0x08,

    WRITE_COMMAND_8, 0xB0, // Positive Voltage Gamma Control
    WRITE_BYTES, 16,
    0x05, 0x12, 0x98, 0x0e, 0x0F,
    0x07, 0x07, 0x09, 0x09, 0x23,
    0x05, 0x52, 0x0F, 0x67, 0x2C,
    0x11,



    WRITE_COMMAND_8, 0xB1, // Negative Voltage Gamma Control
    WRITE_BYTES, 16,
    0x0B, 0x11, 0x97, 0x0C, 0x12,
    0x06, 0x06, 0x08, 0x08, 0x22,
    0x03, 0x51, 0x11, 0x66, 0x2B,
    0x0F,

    WRITE_COMMAND_8, 0xFF,
    WRITE_BYTES, 5, 0x77, 0x01, 0x00, 0x00, 0x11,

    WRITE_C8_D8, 0xB0, 0x5D,
    WRITE_C8_D8, 0xB1, 0x2D,
    WRITE_C8_D8, 0xB2, 0x81,
    WRITE_C8_D8, 0xB3, 0x80,
    WRITE_C8_D8, 0xB5, 0x4E,
    WRITE_C8_D8, 0xB7, 0x85,
    WRITE_C8_D8, 0xB8, 0x20,
    WRITE_C8_D8, 0xC1, 0x78,
    WRITE_C8_D8, 0xC2, 0x78,

    WRITE_C8_D8, 0xD0, 0x88,

    WRITE_COMMAND_8, 0xE0,
    WRITE_BYTES, 4,
    0x00, 0x00, 0x02,

    WRITE_COMMAND_8, 0xE1,
    WRITE_BYTES, 16,
    0x06, 0x30, 0x08, 0x30, 0x05,
    0x30, 0x07, 0x30, 0x00, 0x33,
    0x33,

    WRITE_COMMAND_8, 0xE2,
    WRITE_BYTES, 16,
    0x11, 0x11, 0x33, 0x33, 0xf4,
    0x00, 0x00, 0x00, 0xf4, 0x00,
    0x00, 0x00,

    WRITE_COMMAND_8, 0xE3,
    WRITE_BYTES, 4, 0x00, 0x00, 0x11, 0x11,

    WRITE_C8_D16, 0xE4, 0x44, 0x44,

    WRITE_COMMAND_8, 0xE5,
    WRITE_BYTES, 16,
    0x0d, 0xf5, 0x30, 0xf0, 0x0f,
    0xf7, 0x30, 0xf0, 0x09, 0xf1,
    0x30, 0xf0, 0x0b, 0xf3, 0x30, 0xf0,

    WRITE_COMMAND_8, 0xE6,
    WRITE_BYTES, 4, 0x00, 0x00, 0x11, 0x11,

    WRITE_C8_D16, 0xE7, 0x44, 0x44,

    WRITE_COMMAND_8, 0xE8,
    WRITE_BYTES, 16,
    0x0c, 0xf4, 0x30, 0xf0,
    0x0e, 0xf6, 0x30, 0xf0,
    0x08, 0xf0, 0x30, 0xf0,
    0x0a, 0xf2, 0x30, 0xf0,

    WRITE_C8_D8, 0xe9, 0x36,

    WRITE_COMMAND_8, 0xEB,
    WRITE_BYTES, 7,
    0x00, 0x01, 0xe4, 0xe4,
    0x44, 0x88, 0x40,

    WRITE_COMMAND_8, 0xED,
    WRITE_BYTES, 16,
    0xff, 0x10, 0xaf, 0x76,
    0x54, 0x2b, 0xcf, 0xff,
    0xff, 0xfc, 0xb2, 0x45,
    0x67, 0xfa, 0x01, 0xff,

    WRITE_COMMAND_8, 0xEF,
    WRITE_BYTES, 6,
    0x08, 0x08, 0x08, 0x45,
    0x3f, 0x54,

    WRITE_COMMAND_8, 0xFF,
    WRITE_BYTES, 5, 0x77, 0x01, 0x00, 0x00, 0x00,

    WRITE_COMMAND_8, 0x11, // Sleep Out
    END_WRITE,

    DELAY, 150,

    BEGIN_WRITE,
    WRITE_C8_D8, 0x3A, 0x66,
    WRITE_C8_D8, 0x36, 0x08,
    WRITE_C8_D8, 0x35, 0x00,
    WRITE_COMMAND_8, 0x29, // Display On
    END_WRITE,
    DELAY, 20
};