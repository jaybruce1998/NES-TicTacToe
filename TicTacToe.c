#include "LIB/neslib.h"
#include "LIB/nesdoug.h" 

#define BLACK 0x0f
#define DK_GY 0x00
#define LT_GY 0x1A
#define WHITE 0x30
// there's some oddities in the palette code, black must be 0x0f, white must be 0x30

#pragma bss-name(push, "ZEROPAGE")

// GLOBAL VARIABLES
// all variables should be global for speed
// zeropage global is even faster

unsigned char i, j;
unsigned char y_position=14;
unsigned char x_position=7;
unsigned char pad1, winner, turn, turns, r=0, c=0, moved, movedBefore;

unsigned char board[3][3];
const unsigned char palette[]={
	BLACK, DK_GY, LT_GY, WHITE,
	0,0,0,0,
	0,0,0,0,
	0,0,0,0
};

const unsigned char palette_sp[]={
	0x0f, 0x28, 0x30, 0x31, // black (bg), yellow, red, white	0,0,0,0,
	0,0,0,0,
	0,0,0,0
};

void showString(int n, unsigned char* s)
{
	vram_adr(NTADR_A(1, n));
	for(j=0; s[j]; ++j)
		vram_put(s[j]);
}

void showBoard() {
	ppu_off();//screen off
	for(i=0; i<3; ++i) {
		vram_adr(NTADR_A(1,i*2+2));
		vram_put(board[i][0]);
		vram_put('|');
		vram_put(board[i][1]);
		vram_put('|');
		vram_put(board[i][2]);
		if(i<2)
			showString(i*2+3, "-----");
	}
	switch(winner) {
	case 'T':
		showString(8, "Tie!");
		break;
	case 'X':
		showString(8, "X won!");
		break;
	case 'O':
		showString(8, "O won!");
		break;
	default:
		showString(8, "      ");
	}
	ppu_on_all();//turn on screen
}

void resetBoard() {
	winner=0;
	turn='X';
	turns=0;
	for(i=0; i<3; ++i)
		for(j=0; j<3; ++j)
			board[i][j]=' ';
	showBoard();
}

void main (void) {
	pal_bg(palette); //	load the BG palette
	pal_spr(palette_sp);
	bank_spr(1);
	resetBoard();
	while (1){
		ppu_wait_nmi();
		pad1 = pad_poll(0); // read the first controller
	    moved=0;
	    if (pad1 & PAD_UP)
	    {
	        moved=1;
	        if(!movedBefore) {
	            r=r==0?2:r-1;
	            y_position=r*16+14;
	        }
	    }
	    if (pad1 & PAD_DOWN)
	    {
	        moved=1;
	        if(!movedBefore) {
	            r=r==2?0:r+1;
	            y_position=r*16+14;
	        }
	    }
	    if (pad1 & PAD_LEFT)
	    {
	        moved=1;
	        if(!movedBefore) {
	            c=c==0?2:c-1;
	            x_position=c*16+7;
	        }
	    }
	    if (pad1 & PAD_RIGHT)
	    {
	        moved=1;
	        if(!movedBefore) {
	            c=c==2?0:c+1;
	            x_position=c*16+7;
	        }
	    }
		if(pad1 & PAD_A && winner==0 && board[r][c]==' '){
			board[r][c]=turn;
			if((board[0][c]==turn&&board[1][c]==turn&&board[2][c]==turn)||(board[r][0]==turn&&board[r][1]==turn&&board[r][2]==turn)||(board[1][1]==turn&&
				((board[0][0]==turn&&board[2][2]==turn)||(board[0][2]==turn&&board[2][0]==turn))))
				winner=turn;
			else if(++turns==9)
				winner='T';
			else
				turn=turn=='X'?'O':'X';
			showBoard();
		}
		if(pad1 & PAD_START && winner)
			resetBoard();
		oam_clear();
		oam_spr(x_position, y_position, 0, 0);
		movedBefore=moved;
	}
}
	
	