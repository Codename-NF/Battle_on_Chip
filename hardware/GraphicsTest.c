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

void blankscreen(int Colour)
{
    WAIT_FOR_GRAPHICS;              // is graphics ready for new command

    GraphicsX1Reg = 0;              // write coords to x1, y1
    GraphicsY1Reg = 0;
    GraphicsColourReg = Colour;
    GraphicsCommandReg = Blankboard; 
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

    printf("Done...\n");
    return 0 ;
}