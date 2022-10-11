#pragma once
#include <cstdint>

#define BMP_CIRCLE_WIDTH  64
#define BMP_CIRCLE_HEIGHT 64

const uint8_t BMP_CIRCLE[] = 
{
	0x00,0x00,0x00,0x7F,0xFE,0x00,0x00,0x00, // .........................##############.........................
	0x00,0x00,0x03,0xFF,0xFF,0xC0,0x00,0x00, // ......................####################......................
	0x00,0x00,0x1F,0xFF,0xFF,0xF8,0x00,0x00, // ...................##########################...................
	0x00,0x00,0x7F,0xFF,0xFF,0xFE,0x00,0x00, // .................##############################.................
	0x00,0x01,0xFF,0xFF,0xFF,0xFF,0x80,0x00, // ...............##################################...............
	0x00,0x03,0xFF,0xFF,0xFF,0xFF,0xC0,0x00, // ..............####################################..............
	0x00,0x0F,0xFF,0xFF,0xFF,0xFF,0xF0,0x00, // ............########################################............
	0x00,0x1F,0xFF,0xFF,0xFF,0xFF,0xF8,0x00, // ...........##########################################...........
	0x00,0x3F,0xFF,0xFF,0xFF,0xFF,0xFC,0x00, // ..........############################################..........
	0x00,0x7F,0xFF,0xFF,0xFF,0xFF,0xFE,0x00, // .........##############################################.........
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00, // ........################################################........
	0x01,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x80, // .......##################################################.......
	0x03,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xC0, // ......####################################################......
	0x03,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xC0, // ......####################################################......
	0x07,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xE0, // .....######################################################.....
	0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF0, // ....########################################################....
	0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF0, // ....########################################################....
	0x1F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF8, // ...##########################################################...
	0x1F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF8, // ...##########################################################...
	0x3F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFC, // ..############################################################..
	0x3F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFC, // ..############################################################..
	0x3F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFC, // ..############################################################..
	0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE, // .##############################################################.
	0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE, // .##############################################################.
	0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE, // .##############################################################.
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ################################################################
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ################################################################
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ################################################################
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ################################################################
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ################################################################
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ################################################################
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ################################################################
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ################################################################
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ################################################################
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ################################################################
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ################################################################
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ################################################################
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ################################################################
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ################################################################
	0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE, // .##############################################################.
	0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE, // .##############################################################.
	0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE, // .##############################################################.
	0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE, // .##############################################################.
	0x3F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFC, // ..############################################################..
	0x3F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFC, // ..############################################################..
	0x1F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF8, // ...##########################################################...
	0x1F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF8, // ...##########################################################...
	0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF0, // ....########################################################....
	0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF0, // ....########################################################....
	0x07,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xE0, // .....######################################################.....
	0x03,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xC0, // ......####################################################......
	0x03,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xC0, // ......####################################################......
	0x01,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x80, // .......##################################################.......
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00, // ........################################################........
	0x00,0x7F,0xFF,0xFF,0xFF,0xFF,0xFE,0x00, // .........##############################################.........
	0x00,0x3F,0xFF,0xFF,0xFF,0xFF,0xFC,0x00, // ..........############################################..........
	0x00,0x1F,0xFF,0xFF,0xFF,0xFF,0xF8,0x00, // ...........##########################################...........
	0x00,0x0F,0xFF,0xFF,0xFF,0xFF,0xF0,0x00, // ............########################################............
	0x00,0x03,0xFF,0xFF,0xFF,0xFF,0xC0,0x00, // ..............####################################..............
	0x00,0x01,0xFF,0xFF,0xFF,0xFF,0x80,0x00, // ...............##################################...............
	0x00,0x00,0x7F,0xFF,0xFF,0xFE,0x00,0x00, // .................##############################.................
	0x00,0x00,0x1F,0xFF,0xFF,0xF8,0x00,0x00, // ...................##########################...................
	0x00,0x00,0x07,0xFF,0xFF,0xE0,0x00,0x00, // .....................######################.....................
	0x00,0x00,0x00,0x7F,0xFE,0x00,0x00,0x00  // .........................##############.........................
};