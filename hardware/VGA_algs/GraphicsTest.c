#include <stdio.h>

// graphics register addresses

#define GraphicsCommandReg   		(*(volatile unsigned short int *)(0xFF210000))
#define GraphicsStatusReg   		(*(volatile unsigned short int *)(0xFF210000))
#define GraphicsX1Reg   			(*(volatile unsigned short int *)(0xFF210002))
#define GraphicsY1Reg   			(*(volatile unsigned short int *)(0xFF210004))
#define GraphicsX2Reg   			(*(volatile unsigned short int *)(0xFF210006))
#define GraphicsY2Reg   			(*(volatile unsigned short int *)(0xFF210008))
#define GraphicsColourReg   		(*(volatile unsigned short int *)(0xFF21000E))
#define GraphicsBackGroundColourReg   	(*(volatile unsigned short int *)(0xFF210010))

/************************************************************************************************
** This macro pauses until the graphics chip status register indicates that it is idle
***********************************************************************************************/

#define WAIT_FOR_GRAPHICS		while((GraphicsStatusReg & 0x0001) != 0x0001);

// #defined constants representing values we write to the graphics 'command' register to get
// it to draw something. You will add more values as you add hardware to the graphics chip
// Note DrawHLine, DrawVLine and DrawLine at the moment do nothing - you will modify these

#define DrawHLine		1
#define DrawVLine		2
#define DrawLine			3
#define Blankboard      4
#define Squaremapper    5
#define CrossBox        6
#define	PutAPixel		0xA
#define	GetAPixel		0xB
#define	ProgramPaletteColour    0x10

// defined constants representing colours pre-programmed into colour palette
// there are 256 colours but only 8 are shown below, we write these to the colour registers
//
// the header files "Colours.h" contains constants for all 256 colours
// while the course file "ColourPaletteData.c" contains the 24 bit RGB data
// that is pre-programmed into the palette

#define	BLACK			0
#define	WHITE			1
#define	RED			2
#define	LIME			3
#define	BLUE			4
#define	YELLOW			5
#define	CYAN			6
#define	MAGENTA			7

#define VERTICAL        1
#define HORIZONTAL      2

// Constants ideally put this in a header file and #include it
#define XRES 800
#define YRES 480
#define FONT2_XPIXELS   10              // width of Font2 characters in pixels (no spacing)
#define FONT2_YPIXELS   14              // height of Font2 characters in pixels (no spacing)

#define XBASE1 174
#define XBASE2 462
#define YBASE 160
#define SQUARESIZE 20
#define SPACESIZE 4

static const char P1WINS[] = "Player 1 Wins!";
static const char P2WINS[] = "Player 2 Wins!";
static const char COLCOOR[] = "ABCDEFGHIJ";
static const char ROWCOOR[] = "123456789";


const unsigned short int Font10x14[][14] = {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0},                                                              // ' '
    {0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0,0,0x30,0x30},                              // '!'
    {0xcc,0xcc,0xcc,0xcc,0,0,0,0,0,0,0,0,0,0},                                                  // '"'
    {0xcc,0xcc,0xcc,0xcc,0x3ff,0x3ff,0xcc,0xcc,0x3ff,0x3ff,0xcc,0xcc,0xcc,0xcc},                // '#'
    {0x30,0x30,0xfe,0x1ff,0x3b3,0x3b0,0x1fc,0xfe,0x37,0x337,0x3fe,0x1fc,0x30,0x30},             // '$'
    {0x186,0x3C6,0x3cc,0x18c,0x18,0x18,0x30,0x30,0x60,0x60,0xc6,0xcf,0x18f,0x186},              // '%'
    {0xf0,0x1f8,0x30c,0x31c,0x338,0x370,0x1e0,0x1e0,0x373,0x33b,0x31e,0x38e,0x1fb,0xf3},        // '&'
    {0x30,0x78,0x38,0x18,0x30,0x60,0,0,0,0,0,0,0,0},                                            // '''
    {0xc,0x3c,0x70,0x60,0xe0,0xc0,0xc0,0xc0,0xc0,0xe0,0x60,0x70,0x3c,0xc},                      // '('
    {0x60,0x78,0x1c,0xc,0xe,0x6,0x6,0x6,0x6,0xe,0xc,0x1c,0x78,0x60},            // ')'
    {0,0,0x30,0x30,0x333,0x3b7,0x1fe,0xfc,0x3b7,0x333,0x30,0x30,0,0},       // '*'
    {0,0,0,0x30,0x30,0x30,0x30,0x3ff,0x3ff,0x30,0x30,0x30,0x30,0},      // '+'
    {0,0,0,0,0,0,0,0,0x30,0x78,0x38,0x18,0x30,0x60},            // ','
    {0,0,0,0,0,0,0,0x3ff,0x3ff,0,0,0,0,0},          // '-'
    {0,0,0,0,0,0,0,0,0,0,0x30,0x78,0x78,0x30},                  // '.'
    {0xc,0xc,0x18,0x18,0x30,0x30,0x60,0x60,0xc0,0xc0,0x180,0x180,0x300,0x300} ,         // '/'
    {0xfc,0x1fe,0x387,0x307,0x30f,0x31f,0x33b,0x373,0x3e3,0x3c3,0x383,0x387,0x1fe,0xfc}, // '0'
    {0x30,0x70,0xf0,0xf0,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0xfc,0xfc},    // '1'
    {0xfc,0x1fe,0x387,0x303,0x3,0x7,0xe,0x1c,0x38,0x70,0xe0,0x1c0,0x3ff,0x3ff}, // '2'
    {0xfc,0x1fe,0x387,0x303,0x3,0x3,0x1e,0x1e,0x3,0x3,0x303,0x387,0x1fe,0xfc}, // '3'
    {0xc,0x1c,0x3c,0x7c,0xec,0x1cc,0x38c,0x30c,0x3ff,0x3ff,0xc,0xc,0xc,0xc}, // '4'
    {0x3ff,0x3ff,0x300,0x300,0x3fc,0x3fe,0x7,0x3,0x3,0x3,0x303,0x387,0x1fe,0xfc}, // '5'
    {0xfc,0x1fe,0x387,0x303,0x300,0x300,0x3fc,0x3fe,0x307,0x303,0x303,0x387,0x1fe,0xfc}, // '6'
    {0x3ff,0x3ff,0x3,0x7,0xe,0x1c,0x38,0x30,0x30,0x30,0x30,0x30,0x30,0x30}, // '7'
    {0xfc,0x1fe,0x387,0x303,0x303,0x387,0x1fe,0x1fe,0x387,0x303,0x303,0x387,0x1fe,0xfc}, // '8'
    {0xfc,0x1fe,0x387,0x303,0x303,0x387,0x1ff,0xff,0x7,0xe,0x1c,0x38,0xf0,0xe0}, // '9'
    {0,0,0x30,0x78,0x78,0x30,0,0,0x30,0x78,0x78,0x30,0,0}, // ':'
    {0,0,0x30,0x78,0x78,0x30,0,0,0x30,0x78,0x38,0x18,0x30,0x60}, // ';'
    {0xc,0x1c,0x38,0x70,0xe0,0x1c0,0x380,0x380,0x1c0,0xe0,0x70,0x38,0x1c,0xc}, // '<'
    {0,0,0,0,0x3ff,0x3ff,0,0,0x3ff,0x3ff,0,0,0,0}, // '='
    {0xc0,0xe0,0x70,0x38,0x1c,0xe,0x7,0x7,0xe,0x1c,0x38,0x70,0xe0,0xc0}, // '>'
    {0xfc,0x1fe,0x387,0x303,0x3,0x7,0x1e,0x38,0x30,0x30,0x30,0,0x30,0x30}, // '?'
    {0xfc,0x1fe,0x387,0x303,0x3,0x3,0xf3,0x1f3,0x3b3,0x333,0x333,0x3b7,0x1fe,0xfc}, // '@'
    {0xfc,0x1fe,0x387,0x303,0x303,0x303,0x303,0x3ff,0x3ff,0x303,0x303,0x303,0x303,0x303}, // 'A'
    {0x3fc,0x3fe,0x307,0x303,0x303,0x307,0x3fe,0x3fe,0x307,0x303,0x303,0x307,0x3fe,0x3fc}, // 'B'
    {0xfc,0x1fe,0x387,0x303,0x300,0x300,0x300,0x300,0x300,0x300,0x303,0x387,0x1fe,0xfc}, // 'C'
    {0x3fc,0x3fe,0x307,0x303,0x303,0x303,0x303,0x303,0x303,0x303,0x303,0x307,0x3fe,0x3fc}, // 'D'
    {0x3ff,0x3ff,0x300,0x300,0x300,0x300,0x3fc,0x3fc,0x300,0x300,0x300,0x300,0x3ff,0x3ff}, // 'E'
    {0x3ff,0x3ff,0x300,0x300,0x300,0x300,0x3fc,0x3fc,0x300,0x300,0x300,0x300,0x300,0x300}, // 'F'
    {0xfc,0x1fe,0x387,0x303,0x300,0x300,0x31f,0x31f,0x303,0x303,0x303,0x387,0x1fe,0xfc}, // 'G'
    {0x303,0x303,0x303,0x303,0x303,0x303,0x3ff,0x3ff,0x303,0x303,0x303,0x303,0x303,0x303}, // 'H'
    {0xfc,0xfc,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0xfc,0xfc}, // 'I'
    {0xff,0xff,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x30c,0x39c,0x1f8,0xf0}, // 'J'
    {0x303,0x307,0x30e,0x31c,0x338,0x3f0,0x3e0,0x3e0,0x3f0,0x338,0x31c,0x30e,0x307,0x303}, // 'K'
    {0x300,0x300,0x300,0x300,0x300,0x300,0x300,0x300,0x300,0x300,0x300,0x300,0x3ff,0x3ff}, // 'L'
    {0x303,0x387,0x3cf,0x3ff,0x37b,0x333,0x333,0x333,0x303,0x303,0x303,0x303,0x303,0x303}, // 'M'
    {0x303,0x303,0x303,0x383,0x3c3,0x3e3,0x373,0x33b,0x31f,0x30f,0x307,0x303,0x303,0x303}, // 'N'
    {0xfc,0x1fe,0x387,0x303,0x303,0x303,0x303,0x303,0x303,0x303,0x303,0x387,0x1fe,0xfc}, // 'O'
    {0x3fc,0x3fe,0x307,0x303,0x303,0x307,0x3fe,0x3fc,0x300,0x300,0x300,0x300,0x300,0x300}, // 'P'
    {0xfc,0x1fe,0x387,0x303,0x303,0x303,0x303,0x303,0x333,0x33b,0x31f,0x38e,0x1ff,0xfb} , // 'Q'
    {0x3fc,0x3fe,0x307,0x303,0x303,0x307,0x3fe,0x3fc,0x378,0x31c,0x306,0x307,0x303,0x303} , // R
    {0xfc,0x1ff,0x383,0x300,0x300,0x380, 0x1fc,0xfe,0x7,0x3,0x3,0x307,0x3fe,0x1fc}, // 'S'
    {0x3ff,0x3ff,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30}, // 'T'
    {0x303,0x303,0x303,0x303,0x303,0x303,0x303,0x303,0x303,0x303,0x303,0x387,0x1fe,0xfc}, // 'U'
    {0x303,0x303,0x303,0x303,0x303,0x303,0x303,0x303,0x303,0x387,0x1ce,0xfc,0x78,0x30}, // 'V'
    {0x303,0x303,0x303,0x303,0x303,0x303,0x303,0x303,0x333,0x333,0x37b,0x3ff,0x1fe,0xcc}, // 'W'
    {0x303,0x303,0x303,0x387,0x1ce,0xfc,0x78,0x78,0xfc,0x1ce,0x387,0x303,0x303,0x303}, // 'X'
    {0x303,0x303,0x303,0x303,0x303,0x387,0x1ce,0xfc,0x78,0x30,0x30,0x30,0x30,0x30}, // 'Y'
    {0x3ff,0x3ff,0x3,0x7,0xe,0x1c,0x38,0x70,0xe0,0x1c0,0x380,0x300,0x3ff,0x3ff}, // 'Z'
    {0x7c,0x7c,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x7c,0x7c}, // '['
    {0x180,0x180,0xc0,0xc0,0x60,0x60,0x30,0x30,0x18,0x18,0xc,0xc,0x6,0x6}, // '\'
    {0xf8,0xf8,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0xf8,0xf8}, // ']'
    {0x30,0x78,0xfc,0x1ce,0x387,0x303,0,0,0,0,0,0,0,0}, // '^'
    {0,0,0,0,0,0,0,0,0,0,0,0,0x3ff,0x3ff}, // '_'
    {0xc0,0xe0,0x70,0x38,0x18,0,0,0,0,0,0,0,0,0}, // '`'
    {0,0,0,0,0xfc,0x1fe,0x187,0x3,0xff,0x1ff,0x383,0x383,0x1ff,0xfe}, // 'a'
    {0x300,0x300,0x300,0x300,0x300,0x300,0x3fc,0x3fe,0x307,0x303,0x303,0x307,0x3fe,0x3fc}, // 'b'
    {0,0,0,0,0xfc,0x1fe,0x387,0x303,0x300,0x300,0x303,0x387,0x1fe,0xfc}, // 'c'
    {0x3,0x3,0x3,0x3,0x3,0x3,0xff,0x1ff,0x383,0x303,0x303,0x383,0x1ff,0xff}, // 'd'
    {0,0,0,0,0xfc,0x1fe,0x387,0x303,0x3ff,0x3fe,0x300,0x387,0x1fe,0xfc}, // 'e'
    {0x30,0x78,0xfc,0xcc,0xc0,0xc0,0x1f0,0x1f0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0}, // 'f'
    {0,0,0,0,0xfe,0x1ff,0x383,0x383,0x1ff,0xff,0x3,0x307,0x3fe,0x1fc} , // 'g'
    {0x180,0x180,0x180,0x180,0x180,0x180,0x1fc,0x1fe,0x187,0x183,0x183,0x183,0x183,0x183}, // 'h'
    {0,0,0,0x30,0x30,0,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30}, // 'i'
    {0xc,0xc,0,0,0xc,0xc,0xc,0xc,0xc,0xc,0x30c,0x39c,0x1f8,0xf0}, // 'j'
    {0x180,0x180,0x180,0x180,0x186,0x18e,0x19c,0x1b8,0x1f0,0x1f0,0x1b8,0x19c,0x18e,0x186}, // 'k'
    {0xf0,0xf0,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0xfc,0xfc}, // 'l'
    {0,0,0,0,0x387,0x3cf,0x3ff,0x37b,0x333,0x333,0x303,0x303,0x303,0x303}, // 'm'
    {0,0,0,0,0x33c,0x37e,0x3e7,0x3c3,0x303,0x303,0x303,0x303,0x303,0x303,}, // 'n'
    {0,0,0,0,0xfc,0x1fe,0x387,0x303,0x303,0x303,0x303,0x387,0x1fe,0xfc} , // 'o'
    {0,0,0,0,0x3fc,0x3fe,0x307,0x307,0x3fe,0x3fc,0x300,0x300,0x300,0x300}, // 'p'
    {0,0,0,0,0xff,0x1ff,0x383,0x383,0x1ff,0xff,0x3,0x3,0x3,0x3}, // 'q'
    {0,0,0,0,0x33c,0x37e,0x3e7,0x3c3,0x380,0x300,0x300,0x300,0x300,0x300}, // 'r'
    {0,0,0,0,0xfe,0x1ff,0x383,0x380,0x1fc,0xfe,0x7,0x307,0x3fe,0x1fc}, // 's'
    {0x60,0x60,0x60,0x60,0x1fe,0x1fe,0x60,0x60,0x60,0x60,0x66,0x7e,0x3c,0x18}, // 't'
    {0,0,0,0,0x303,0x303,0x303,0x303,0x303,0x303,0x303,0x387,0x1fe,0xfc}, // 'u'
    {0,0,0,0,0x303,0x303,0x303,0x303,0x303,0x387,0x1ce,0xfc,0x78,0x30}, // 'v'
    {0,0,0,0,0x303,0x303,0x303,0x303,0x333,0x333,0x333,0x3ff,0x1fe,0xcc}, // 'w'
    {0,0,0,0,0x303,0x387,0x1ce,0xfc,0x78,0x78,0xfc,0x1ce,0x387,0x303}, // 'x'
    {0,0,0,0,0x303,0x387,0x1ce,0xfc,0x78,0x30,0x30,0x70,0xe0,0xc0}, // 'y'
    {0,0,0,0,0x3ff,0x3ff,0xe,0x1c,0x38,0x70,0xe0,0x1c0,0x3ff,0x3ff}, // 'z'
    {0x1e,0x3e,0x70,0x60,0x60,0xe0,0x1c0,0x1c0,0xe0,0x60,0x60,0x70,0x3e,0x1e}, // '{'
    {0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30}, // '|'
    {0x1e0,0x1f0,0x38,0x18,0x18,0x1c,0xe,0xe,0x1c,0x18,0x18,0x38,0x1f0,0x1e0}, // '}'
    {0,0,0,0,0,0,0,0xc0,0x333,0xc,0,0,0,0},         // '~'
};
/*******************************************************************************************
* This function writes a single pixel to the x,y coords specified using the specified colour
* Note colour is a byte and represents a palette number (0-255) not a 24 bit RGB value
********************************************************************************************/
void WriteAPixel(int x, int y, int Colour)
{
	WAIT_FOR_GRAPHICS;				// is graphics ready for new command

	GraphicsX1Reg = x;				// write coords to x1, y1
	GraphicsY1Reg = y;
	GraphicsColourReg = Colour;			// set pixel colour
	GraphicsCommandReg = PutAPixel;			// give graphics "write pixel" command
}

void squaremapper(int x, int y, int player, int colour)
{
    WAIT_FOR_GRAPHICS;              // is graphics ready for new command
    GraphicsX1Reg = x;              // write coords to x1, y1
    GraphicsY1Reg = y;
    GraphicsX2Reg = player - 1;
    GraphicsColourReg = colour;
    GraphicsCommandReg = Squaremapper; 
}

void squaremappership(int player, int x, int y, int length, int dir, int done, int colour)
{
    //dir 1 is down, dir 2 is horizontal
    //left player is 1, right player is 2
    WAIT_FOR_GRAPHICS;              // is graphics ready for new command
    GraphicsX1Reg = x;              // write coords to x1, y1
    GraphicsY1Reg = y;
    GraphicsX2Reg = player - 1;
    GraphicsColourReg = colour;
    GraphicsCommandReg = CrossBox;
    int i = 1;
    for (i = 1; i < length; i++) {
        if (dir == VERTICAL) {
            WAIT_FOR_GRAPHICS;
            GraphicsY1Reg = y + i;
            GraphicsCommandReg = CrossBox;
        } else {
            WAIT_FOR_GRAPHICS;
            GraphicsX1Reg = x + i;
            GraphicsCommandReg = CrossBox;
        }
    }

}

void outgraphicschar(int x, int y, int colour, int backgroundcolour, int c, int Erase) {
    register int    row,
                    column,
                    theX = x,
                    theY = y ;
    register int    pixels ;
    register char   theColour = colour  ;
    register int    BitMask,
                    theCharacter = c,
                    j,
                    theRow, theColumn;


    if(((short)(x) > (short)(XRES-1)) || ((short)(y) > (short)(YRES-1)))  // if start off edge of screen don't bother
        return ;

    if(((short)(theCharacter) >= (short)(' ')) && ((short)(theCharacter) <= (short)('~'))) {            // if printable character
        theCharacter -= 0x20 ;                                                                          // subtract hex 20 to get index of first printable character (the space character)
        theRow = FONT2_YPIXELS;
        theColumn = FONT2_XPIXELS;

        for(row = 0; row < theRow ; row ++) {
            pixels = Font10x14[theCharacter][row] ;                                         // get the pixels for row 0 of the character to be displayed
            BitMask = 512 ;                                                                     // set of hex 200 i.e. bit 7-0 = 0010 0000 0000
            for(column = 0; column < theColumn;   )     {
                if((pixels & BitMask))                                                      // if valid pixel, then write it
                    WriteAPixel(theX+column, theY+row, theColour) ;
                else {                                                                      // if not a valid pixel, do we erase or leave it along (no erase)
                    if(Erase == 1)
                        WriteAPixel(theX+column, theY+row, backgroundcolour) ;
                    // else leave it alone
                }
                    column ++ ;
                BitMask = BitMask >> 1 ;
            }
        }
    }

}

void winnermessage(int winner, int colour, int background) {
    int current_x = 0;
    int current_y = YBASE - (2 * FONT2_YPIXELS);
    char current_letter = 'P';
    int i = 0;

    if (winner == 1) {
        current_x = XBASE1 + 2 * (SQUARESIZE + SPACESIZE);
        while (P1WINS[i] != '\0') {
            outgraphicschar(current_x, current_y, colour, background, P1WINS[i], 0);
            i++;
            current_x += FONT2_XPIXELS;
        }
    } else {
        current_x = XBASE2 + 2 * (SQUARESIZE + SPACESIZE);
        while (P2WINS[i] != '\0') {
            outgraphicschar(current_x, current_y, colour, background, P2WINS[i], 0);
            i++;
            current_x += FONT2_XPIXELS;
        }
    }
}

void writecoords() {
    int current_x = 0;
    int current_y = 0;
    int i = 0;
    int colour = LIME;
    int background = BLACK;

    //LEFT BOARD
    current_x = XBASE1 + 5;
    current_y = YBASE;
    while (COLCOOR[i] != '\0') {
        outgraphicschar(current_x, current_y, colour, background, COLCOOR[i], 0);
        i++;
        current_x += (SQUARESIZE + SPACESIZE);
    }
    current_x = XBASE1 - SQUARESIZE;
    current_y = YBASE + SQUARESIZE + 3;
    i = 0;
    while (ROWCOOR[i] != '\0') {
        outgraphicschar(current_x, current_y, colour, background, ROWCOOR[i], 0);
        i++;
        current_y += (SQUARESIZE + SPACESIZE);
    }
    outgraphicschar(current_x - FONT2_XPIXELS, current_y, colour, background, '1', 0);
    outgraphicschar(current_x, current_y, colour, background, '0', 0);

    //RIGHT BOARD
    i = 0;
    current_x = XBASE2 + 5;
    current_y = YBASE;
    while (COLCOOR[i] != '\0') {
        outgraphicschar(current_x, current_y, colour, background, COLCOOR[i], 0);
        i++;
        current_x += (SQUARESIZE + SPACESIZE);
    }
    current_x = XBASE2 - SQUARESIZE;
    current_y = YBASE + SQUARESIZE + 3;
    i = 0;
    while (ROWCOOR[i] != '\0') {
        outgraphicschar(current_x, current_y, colour, background, ROWCOOR[i], 0);
        i++;
        current_y += (SQUARESIZE + SPACESIZE);
    }
    outgraphicschar(current_x - FONT2_XPIXELS, current_y, colour, background, '1', 0);
    outgraphicschar(current_x, current_y, colour, background, '0', 0);


}

void blankscreen(int Colour)
{
    WAIT_FOR_GRAPHICS;              // is graphics ready for new command

    GraphicsX1Reg = 0;              // write coords to x1, y1
    GraphicsY1Reg = 0;
    GraphicsColourReg = Colour;
    GraphicsCommandReg = Blankboard;
    writecoords();
    winnermessage(1, BLACK, BLACK);
    winnermessage(2, BLACK, BLACK); 
}



/*********************************************************************************************
* This function read a single pixel from the x,y coords specified and returns its colour
* Note returned colour is a byte and represents a palette number (0-255) not a 24 bit RGB value
*********************************************************************************************/

int ReadAPixel(int x, int y)
{
	WAIT_FOR_GRAPHICS;			// is graphics ready for new command

	GraphicsX1Reg = x;			// write coords to x1, y1
	GraphicsY1Reg = y;
	GraphicsCommandReg = GetAPixel;		// give graphics a "get pixel" command

	WAIT_FOR_GRAPHICS;			// is graphics done reading pixel
	return (int)(GraphicsColourReg) ;	// return the palette number (colour)
}


/**********************************************************************************
** subroutine to program a hardware (graphics chip) palette number with an RGB value
** e.g. ProgramPalette(RED, 0x00FF0000) ;
**
************************************************************************************/

void ProgramPalette(int PaletteNumber, int RGB)
{
    WAIT_FOR_GRAPHICS;
    GraphicsColourReg = PaletteNumber;
    GraphicsX1Reg = RGB >> 16   ;        // program red value in ls.8 bit of X1 reg
    GraphicsY1Reg = RGB ;                // program green and blue into ls 16 bit of Y1 reg
    GraphicsCommandReg = ProgramPaletteColour; // issue command
}

/********************************************************************************************* This function draw a horizontal line, 1 pixel at a time starting at the x,y coords specified
*********************************************************************************************/

void HLine(int x1, int y1, int length, int Colour)
{
	int i;

	for(i = x1; i < x1+length; i++ )
		WriteAPixel(i, y1, Colour);
}

/********************************************************************************************* This function draw a vertical line, 1 pixel at a time starting at the x,y coords specified
*********************************************************************************************/

void VLine(int x1, int y1, int length, int Colour)
{
	int i;

	for(i = y1; i < y1+length; i++ )
		WriteAPixel(x1, i, Colour);
}

/*******************************************************************************
** Implementation of Bresenhams line drawing algorithm
*******************************************************************************/
int abs(int a)
{
    if(a < 0)
        return -a ;
    else
        return a ;
}

int sign(int a)
{
    if(a < 0)
        return -1 ;
    else if (a == 0)
        return 0 ;
    else
        return 1 ;
}



void Line(int x1, int y1, int x2, int y2, int Colour)
{
    int x = x1;
    int y = y1;
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);

    int s1 = sign(x2 - x1);
    int s2 = sign(y2 - y1);
    int i, temp, interchange = 0, error ;

// if x1=x2 and y1=y2 then it is a line of zero length so we are done

    if(dx == 0 && dy == 0)
        return ;

 // must be a complex line so use Bresenhams algorithm
    else    {

// swap delta x and delta y depending upon slop of line

        if(dy > dx) {
            temp = dx ;
            dx = dy ;
            dy = temp ;
            interchange = 1 ;
        }

// initialise the error term to compensate for non-zero intercept

        error = (dy << 1) - dx ;    // error = (2 * dy) - dx

// main loop
        for(i = 1; i <= dx; i++)    {
            WriteAPixel(x, y, Colour);

            while(error >= 0)   {
                if(interchange == 1)
                    x += s1 ;
                else
                    y += s2 ;

                error -= (dx << 1) ;    // error = error - (dx * 2)
            }

            if(interchange == 1)
                y += s2 ;
            else
                x += s1 ;

            error += (dy << 1) ;    // error = error + (dy * 2)

        }
    }
}

int main(void)
{
    int i;
    printf("Starting...\n");
    blankscreen( BLUE );
    squaremapper(0,0,1,RED);
    squaremapper(0,0,2,WHITE);
    squaremapper(1,1,1,MAGENTA);
    squaremapper(1,1,2,RED);
    squaremapper(3,3,1,RED);
    squaremapper(4,3,1,RED);
    squaremapper(5,3,1,RED);
    squaremapper(6,3,1,RED);
    squaremappership(1, 3, 3, 4, HORIZONTAL, 0, BLACK);
    winnermessage(1, LIME, BLACK);
    winnermessage(2, LIME, BLACK);
    for (i = 0; i < 20; i++) {
        printf("1\n");
    }
    printf("\n");
    blankscreen( BLUE );
    printf("Done...\n");
    return 0 ;
}