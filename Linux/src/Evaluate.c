#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <assert.h>
#include "Evaluate.h"
#include "Search.h"


#define CONNECTEDPAWN   10
#define DOUBLEDPAWN     20
#define ISOLATEDPAWN    20
#define BACKWARDPAWN    8
#define ROOKOPENFILE    10
#define ROOKHALFFILE    5
#define ROOKQUEENFILE   7
#define BISHOPPAIR      30
#define BADBISHOP       5
#define OUTPOST         10
#define KNIGHTPAIR      8
#define ROOKPAIR        16
#define RETURNINGBISHOP 30 
#define TEMPO           10 


int PawnPassed_black[8] = { 0, 15, 20, 32, 56, 92, 140, 0 }; 
int PawnPassed_white[8] = { 0, 140, 92, 56, 32, 20, 15, 0 }; 
int knight_val[9] = { -20, -16, -12, -8, -4,  0,  4,  8, 10 };
int rook_val[9] = { 15, 12,  9,  6,  3,  0, -3, -6, -9 };

enum positions {
    a8, b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1
};

//piece-square tables:
//add bonus points based on position
int white_pawn[8][8] = {

                    {  0,   0,	 0,   0,   0,   0,   0,	  0 },
                    { 20,  20,	20,  30,  30,  20,  20,	 20 },
                    { 10,  10,	10,  20,  20,  10,  10,	 10 },
                    {  5,   5,	 5,  10,  10,	5,   5,	  5 },
                    {  0,   0,	10,  20,  20,  10,   0,	  0 },
                    {  5,   0,	 0,   5,   5,	0,   0,	  5 },
                    { 10,  10,	 5, -15, -15,   5,  10,	 10 },
                    {  0,   0,	 0,   0,   0,	0,   0,	  0 },	

                    };


int white_knight[8][8] = {

                    { -10, -10, -10, -10, -10, -10, -10, -10 },
                    { -10,   0,   0,   0,   0,   0,   0, -10 },
                    { -10,   0,   5,   5,   5,   5,   0, -10 },                  
                    { -10,   0,   5,  10,  10,   5,   0, -10 },
                    { -10,   0,   5,  10,  10,   5,   0, -10 },
                    { -10,   0,   5,   5,   5,   5,   0, -10 },
                    { -10,   0,   0,   0,   0,   0,   0, -10 },	
                    { -10, -30, -10, -10, -10, -10, -30, -10 },

                    };


int white_bishop[8][8] = {

                    { -10, -10, -10, -10, -10, -10, -10, -10 },
                    { -10,   0,   0,   0,   0,   0,   0, -10 },
                    { -10,   0,   5,   5,   5,   5,   0, -10 },                  
                    { -10,   0,   5,  10,  10,   5,   0, -10 },
                    { -10,   0,   5,  10,  10,   5,   0, -10 },
                    { -10,   0,   5,   5,   5,   5,   0, -10 },
                    { -10,   0,   0,   0,   0,   0,   0, -10 },	
                    { -10, -10, -20, -10, -10, -20, -10, -10 },

                    };

int white_rook[8][8] = {

                    {  5,   5,   5,   5,   5,   5,   5,   5 },
                    { 20,  20,  20,  20,  20,  20,  20,  20 },
                    { -5,   0,   0,   0,   0,   0,   0,  -5 },                    
                    { -5,   0,   0,   0,   0,   0,   0,  -5 },
                    { -5,   0,   0,   0,   0,   0,   0,  -5 },
                    { -5,   0,   0,   0,   0,   0,   0,  -5 },
                    { -5,   0,   0,   0,   0,   0,   0,  -5 },
                    {  0,   0,   0,   2,   2,   0,   0,   0 },	

                    };

int white_queen[8][8] = {

                    { -20, -10, -10,  -5,  -5, -10, -10, -20 },
                    { -10,   0,   0,   0,   0,   0,   0, -10 },
                    { -10,   0,   5,   5,   5,   5,   0, -10 },
                    {  -5,   0,   5,   5,   5,   5,   0,  -5 },
                    {   0,   0,   5,   5,   5,   5,   0,  -5 },
                    { -10,   5,   5,   5,   5,   5,   0, -10 },
                    { -10,   0,   5,   0,   0,   0,   0, -10 },
                    { -20, -10, -10,  -5,  -5, -10, -10, -20 },

                    };

int white_king_midgame[8][8] = {

                    { -30, -40, -40, -50, -50, -40, -40, -30 },
                    { -30, -40, -40, -50, -50, -40, -40, -30 },
                    { -30, -40, -40, -50, -50, -40, -40, -30 },
                    { -30, -40, -40, -50, -50, -40, -40, -30 },
                    { -20, -30, -30, -40, -40, -30, -30, -20 },
                    { -10, -20, -20, -20, -20, -20, -20, -10 },
                    {  20,  20,   0,   0,   0,   0,  20,  20 },
                    {  20,  40,  40,   0,   0,  10,  40,  20 },

                    };  

int white_king_endgame[8][8] = {

                    { -72, -48, -36, -24, -24, -36, -48, -72 },
                    { -48, -24, -12,   0,   0, -12, -24, -48 },
                    { -36, -12,   0,  12,  12,   0, -12, -36 },
                    { -24,   0,  12,  24,  24,  12,   0, -24 },
                    { -24,   0,  12,  24,  24,  12,   0, -24 },
                    { -36, -12,   0,  12,  12,   0, -12, -36 },
                    { -48, -24, -12,   0,   0, -12, -24, -48 },
                    { -72, -48, -36, -24, -24, -36, -48, -72 },
                    
                    };

//for black:
int black_pawn[8][8] = {

                    {  0,   0,	 0,   0,   0,	0,   0,	  0 },
                    { 10,  10,	 5, -15, -15,   5,  10,	 10 },
                    {  5,   0,	 0,   5,   5,	0,   0,	  5 },
                    {  0,   0,	10,  20,  20,  10,   0,	  0 },
                    {  5,   5,	 5,  10,  10,   5,   5,	  5 },
                    { 10,  10,	10,  20,  20,  10,  10,	 10 },
                    { 20,  20,	20,  30,  30,  20,  20,	 20 },
                    {  0,   0,   0,   0,   0,   0,   0,	  0 },	

                    };

int black_knight[8][8] = {

                    { -10, -30, -10, -10, -10, -10, -30, -10 },
                    { -10,   0,   0,   0,   0,   0,   0, -10 },
                    { -10,   0,   5,   5,   5,   5,   0, -10 },                  
                    { -10,   0,   5,  10,  10,   5,   0, -10 },
                    { -10,   0,   5,  10,  10,   5,   0, -10 },
                    { -10,   0,   5,   5,   5,   5,   0, -10 },
                    { -10,   0,   0,   0,   0,   0,   0, -10 },	
                    { -10, -10, -10, -10, -10, -10, -10, -10 },	
                    
                    };

int black_bishop[8][8] = {

                    { -10, -10, -20, -10, -10, -20, -10, -10 },
                    { -10,   0,   0,   0,   0,   0,   0, -10 },
                    { -10,   0,   5,   5,   5,   5,   0, -10 },                  
                    { -10,   0,   5,  10,  10,   5,   0, -10 },
                    { -10,   0,   5,  10,  10,   5,   0, -10 },
                    { -10,   0,   5,   5,   5,   5,   0, -10 },
                    { -10,   0,   0,   0,   0,   0,   0, -10 },	
                    { -10, -10, -10, -10, -10, -10, -10, -10 },

                    };

int black_rook[8][8] = {

                    {  0,   0,   0,   2,   2,   0,   0,   0 },	
                    { -5,   0,   0,   0,   0,   0,   0,  -5 },                       
                    { -5,   0,   0,   0,   0,   0,   0,  -5 },
                    { -5,   0,   0,   0,   0,   0,   0,  -5 },
                    { -5,   0,   0,   0,   0,   0,   0,  -5 },
                    { -5,   0,   0,   0,   0,   0,   0,  -5 }, 
                    { 20,  20,  20,  20,  20,  20,  20,  20 },
                    {  5,   5,   5,   5,   5,   5,   5,   5 },

                    };

int black_queen[8][8] = {

                    { -20, -10, -10,  -5,  -5, -10, -10, -20 },
                    { -10,   0,   5,   0,   0,   0,   0, -10 },
                    { -10,   5,   5,   5,   5,   5,   0, -10 },
                    {   0,   0,   5,   5,   5,   5,   0,  -5 },
                    {  -5,   0,   5,   5,   5,   5,   0,  -5 },
                    { -10,   0,   5,   5,   5,   5,   0, -10 },
                    { -10,   0,   0,   0,   0,   0,   0, -10 },
                    { -20, -10, -10,  -5,  -5, -10, -10, -20 },

                    };

int black_king_midgame[8][8] = {

                    {  20,  40,  40,   0,   0,  10,  40,  20 },
                    {  20,  20,   0,   0,   0,   0,  20,  20 },
                    { -10, -20, -20, -20, -20, -20, -20, -10 },     
                    { -20, -30, -30, -40, -40, -30, -30, -20 },
                    { -30, -40, -40, -50, -50, -40, -40, -30 },
                    { -30, -40, -40, -50, -50, -40, -40, -30 },
                    { -30, -40, -40, -50, -50, -40, -40, -30 },
                    { -30, -40, -40, -50, -50, -40, -40, -30 },

                    };      

int black_king_endgame[8][8] = {

                    { -72, -48, -36, -24, -24, -36, -48, -72 },
                    { -48, -24, -12,   0,   0, -12, -24, -48 },
                    { -36, -12,   0,  12,  12,   0, -12, -36 },
                    { -24,   0,  12,  24,  24,  12,   0, -24 },
                    { -24,   0,  12,  24,  24,  12,   0, -24 },
                    { -36, -12,   0,  12,  12,   0, -12, -36 },
                    { -48, -24, -12,   0,   0, -12, -24, -48 },
                    { -72, -48, -36, -24, -24, -36, -48, -72 },
                    
                    };


static inline bool passed_white(char board[8][8], int x, int y)
{
    switch(y)
    {
        case 0:
            for(int rank = 1; rank < x; rank++)
            {
                if(tolower(board[rank][0]) == 'p')
                {
                    return false;
                }
                if(board[rank][1] == 'p')
                {
                    return false;
                }
            }
            break;
        case 7:
            for(int rank = 1; rank < x; rank++)
            {
                if(tolower(board[rank][7]) == 'p')
                {
                    return false;
                }
                if(board[rank][6] == 'p')
                {
                    return false;
                }
            }
            break;  
        default:
            for(int rank = 1; rank < x; rank++)
            {
                if(tolower(board[rank][y]) == 'p')
                {
                    return false;
                }
                if(board[rank][y-1] == 'p')
                {
                    return false;
                }
                if(board[rank][y+1] == 'p')
                {
                    return false;
                }
            } 
            break;
    }
    return true;    
}

static inline bool passed_black(char board[8][8], int x, int y)
{
    switch(y)
    {
        case 0:
            for(int rank = 6; rank > x; rank--)
            {
                if(toupper(board[rank][0]) == 'P')
                {
                    return false;
                }
                if(board[rank][1] == 'P')
                {
                    return false;
                }
        }
            break;
        case 7:
            for(int rank = 6; rank > x; rank--)
            {
                if(toupper(board[rank][7]) == 'P')
                {
                    return false;
                }
                if(board[rank][6] == 'P')
                {
                    return false;
                }
            }
            break;   
        default:
            for(int rank = 6; rank > x; rank--)
            {
                if(toupper(board[rank][y]) == 'P')
                {
                    return false;
                }
                if(board[rank][y-1] == 'P')
                {
                    return false;
                }
                if(board[rank][y+1] == 'P')
                {
                    return false;
                }
            }
            break;
    }
    return true;    
}

static inline bool connected_white(char board[8][8], int x, int y)
{
    switch(y)
    {
        case 0:
            if(board[x+1][y+1] == 'P')
            {
                return true;
            }
            break;
        case 7:
            if(board[x+1][y-1] == 'P')
            {
                return true;
            }
            break; 
        default:
            if(board[x+1][y-1] == 'P' || board[x+1][y+1] == 'P')
            {
                return true;
            }
            break;

    }
    return false;    
}

static inline bool connected_black(char board[8][8], int x, int y)
{
    switch(y)
    {
        case 0:
            if(board[x-1][y+1] == 'p')
            {
                return true;
            }
            break;
        case 7:
            if(board[x-1][y-1] == 'p')
            {
                return true;
            }
            break; 
        default:
            if(board[x-1][y+1] == 'p' || board[x-1][y-1] == 'p')
            {
                return true;
            }
            break;

    }
    return false;    
}

static inline bool phalanx_white(char board[8][8], int x, int y)
{
    switch(y)
    {
        case 0:
            if(board[x][y+1] == 'P')
            {
                return true;
            }
            break;
        case 7:
            if(board[x][y-1] == 'P')
            {
                return true;
            }
            break; 
        default:
            if(board[x][y-1] == 'P' || board[x][y+1] == 'P')
            {
                return true;
            }
            break;

    }
    return false;    
}

static inline bool phalanx_black(char board[8][8], int x, int y)
{
    switch(y)
    {
        case 0:
            if(board[x][y+1] == 'p')
            {
                return true;
            }
            break;
        case 7:
            if(board[x][y-1] == 'p')
            {
                return true;
            }
            break; 
        default:
            if(board[x][y+1] == 'p' || board[x][y-1] == 'p')
            {
                return true;
            }
            break;

    }
    return false;    
}

static inline bool isolated_white(char board[8][8], int y)
{
    switch(y)
    {
        case 0:
            for(int rank = 6; rank > 0; rank--)
            {
                if(board[rank][1] == 'P')
                {
                    return false;
                }
            }
            break;
        case 7:
            for(int rank = 6; rank > 0; rank--)
            {
                if(board[rank][6] == 'P')
                {
                    return false;
                }
            }
            break; 
        default:
            for(int rank = 6; rank > 0; rank--)
            {
                if(board[rank][y-1] == 'P')
                {
                    return false;
                }
                if(board[rank][y+1] == 'P')
                {
                    return false;
                }
            }
            break;
    }
    return true;    
}

static inline bool isolated_black(char board[8][8], int y)
{
    switch(y)
    {
        case 0:
            for(int rank = 1; rank < 7; rank++)
            {
                if(board[rank][1] == 'p')
                {
                    return false;
                }
            }
            break;
        case 7:
            for(int rank = 1; rank < 7; rank++)
            {
                if(board[rank][6] == 'p')
                {
                    return false;
                }
            }
            break;   
        default:
            for(int rank = 1; rank < 7; rank++)
            {
                if(board[rank][y-1] == 'p')
                {
                    return false;
                }
                if(board[rank][y+1] == 'p')
                {
                    return false;
                }
            }
            break;  
    }
    return true;    
}

static inline bool backward_white(char board[8][8], int x, int y)
{
    switch(y)
    {
        case 0:
            for(int rank = 6; rank >= x; rank--)
            {
                if(board[rank][1] == 'P')
                {
                    return false;
                }
            }
            break;
        case 7:
            for(int rank = 6; rank >= x; rank--)
            {
                if(board[rank][6] == 'P')
                {
                    return false;
                }
            }
            break;    
        default:
            for(int rank = 6; rank >= x; rank--)
            {
                if(board[rank][y-1] == 'P')
                {
                    return false;
                }
                if(board[rank][y+1] == 'P')
                {
                    return false;
                }
            }
            break; 
    }
    return true;    
}

static inline bool backward_black(char board[8][8], int x, int y)
{
    switch(y)
    {
        case 0:
            for(int rank = 1; rank <= x; rank++)
            {
                if(board[rank][1] == 'p')
                {
                    return false;
                }
            }
            break;
        case 7:
            for(int rank = 1; rank <= x; rank++)
            {
                if(board[rank][6] == 'p')
                {
                    return false;
                }
            }
            break;   
        default:
            for(int rank = 1; rank <= x; rank++)
            {
                if(board[rank][y-1] == 'p')
                {
                    return false;
                }
                if(board[rank][y+1] == 'p')
                {
                    return false;
                }
            }
            break;  
    }
    return true;    
}

static inline bool openFile(char board[8][8], int y)
{
    for(int x = 1; x < 7; x++)
    {
        if(tolower(board[x][y]) == 'p')
        {
            return false;
        }
    }
    return true;
}

static inline bool queenFile(char board[8][8], int y)
{
    for(int x = 0; x < 8; x++)
    {
        if(tolower(board[x][y]) == 'q')
        {
            return true;
        }
    }
    return false;
}

static inline bool semiOpenFile_white(char board[8][8], int y)
{
    for(int x = 6; x > 0; x--)
    {
        if(board[x][y] == 'P')
        {
            return false;
        }
    }
    return true;
}

static inline bool semiOpenFile_black(char board[8][8], int y)
{
    for(int x = 1; x < 7; x++)
    {
        if(board[x][y] == 'p')
        {
            return false;
        }
    }
    return true;
}

static inline bool outpost_white(char board[8][8], int x, int y)
{
    if(x < 5 && x > 1)
    {
        switch(y)
        {
            case 0:
                if(board[x+1][y+1] == 'P')
                {
                    return true;
                }
                break;
            case 7:
                if(board[x+1][y-1] == 'P')
                {
                    return true;
                }
                break;
            default:
                if(board[x+1][y-1] == 'P' || board[x+1][y+1] == 'P')
                {
                    return true;
                }
                break;
        }
    }
    
    return false;
}

static inline bool outpost_black(char board[8][8], int x, int y)
{
    if(x > 2 && x < 6)
    {
        switch(y)
        {
            case 0:
                if(board[x-1][y+1] == 'p')
                {
                    return true;
                }
                break;
            case 7:
                if(board[x-1][y-1] == 'p')
                {
                    return true;
                }
                break;
            default:
                if(board[x-1][y-1] == 'p' || board[x-1][y+1] == 'p')
                {
                    return true;
                }
                break;
        }
    }
    
    return false;
}

static inline bool badBishop_white(char board[8][8], int x, int y)
{
    if(x != 0)
    {
        switch(y)
        {
            case 0:
                if(board[x-1][y+1] == 'P')
                {
                    return true;
                }
                break;
            case 7:
                if(board[x-1][y-1] == 'P')
                {
                    return true;
                }
                break;
            default:
                if(board[x-1][y-1] == 'P' || board[x-1][y+1] == 'P')
                {
                    return true;
                }
                break;
        }
    }
    
    return false;
}

static inline bool badBishop_black(char board[8][8], int x, int y)
{
    if(x != 7)
    {
        switch(y)
        {
            case 0:
                if(board[x+1][y+1] == 'p')
                {
                    return true;
                }
                break;
            case 7:
                if(board[x+1][y-1] == 'p')
                {
                    return true;
                }
                break;
            default:
                if(board[x+1][y-1] == 'p' || board[x+1][y+1] == 'p')
                {
                    return true;
                }
                break;
        }
    }
    
    return false;
}

static inline bool defended_by_wpawn(char board[8][8], int x, int y)
{
    if(x != 7)
    {
        switch(y)
        {
            case 0:
                if(board[x+1][y+1] == 'P')
                {
                    return true;
                }
                break;
            case 7:
                if(board[x+1][y-1] == 'P')
                {
                    return true;
                }
                break;
            default:
                if(board[x+1][y-1] == 'P' || board[x+1][y+1] == 'P')
                {
                    return true;
                }
                break;
        }
    }

    return false;
}

static inline bool defended_by_bpawn(char board[8][8], int x, int y)
{
    if(x != 0)
    {
        switch(y)
        {
            case 0:
                if(board[x-1][y+1] == 'p')
                {
                    return true;
                }
                break;
            case 7:
                if(board[x-1][y-1] == 'p')
                {
                    return true;
                }
                break;
            default:
                if(board[x-1][y-1] == 'p' || board[x-1][y+1] == 'p')
                {
                    return true;
                }
                break;
        }
    }

    return false;
}

//mobility
//exclude space protected by enemy pawns
//exclude own queen for minor pieces
//exclude own blocked pawns and own pawns on rank 2 and 3
//exclude own king
static inline int wknight_mobility(char board[8][8], int index_x, int index_y)
{
    int move_count = 0;
    int x, y;
    int knight_moves_x[8] = {-2, -2, -1, -1,  1,  1,  2,  2};
    int knight_moves_y[8] = {-1,  1, -2,  2, -2,  2, -1,  1};
    for(int j = 0; j < 8; j++)
    {
        x = index_x + knight_moves_x[j];
        y = index_y + knight_moves_y[j];
        if(x & 8 || y & 8) //out of board
        {
            continue;
        }
        if(board[x][y] != 'K' && board[x][y] != 'Q' && (board[x][y] != 'P' || (x < 5 && board[x - 1][y] == ' ')))
        {  
            if(!defended_by_bpawn(board, x, y))
                move_count++;
        }
    }
    
    return 4 * (move_count - 4);
}

static inline int bknight_mobility(char board[8][8], int index_x, int index_y)
{
    int move_count = 0;
    int x, y;
    int knight_moves_x[8] = {-2, -2, -1, -1,  1,  1,  2,  2};
    int knight_moves_y[8] = {-1,  1, -2,  2, -2,  2, -1,  1};
    for(int j = 0; j < 8; j++)
    {
        x = index_x + knight_moves_x[j];
        y = index_y + knight_moves_y[j];
        if(x & 8 || y & 8) //out of board
        {
            continue;
        }
        if(board[x][y] != 'k' && board[x][y] != 'q' && (board[x][y] != 'p' || (x > 2 && board[x + 1][y] == ' ')))
        {  
            if(!defended_by_wpawn(board, x, y))
                move_count++;
        }
    }
    return 4 * (move_count - 4);
}

static inline int wbishop_mobility(char board[8][8], int index_x, int index_y)
{
    int move_count = 0;
    int x;
    int y;

    //up left
    for(int j = 1; j <= 7; j++)
    {
        if(index_x - j < 0 || index_y - j < 0)
        {
            break;
        }
        x = index_x - j;
        y = index_y - j;
        if(board[x][y] != 'K' && board[x][y] != 'Q' && (board[x][y] != 'P' || (x < 5 && board[x - 1][y] == ' ')))
        {  
            if(!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if(board[index_x - j][index_y - j] != ' ')
        {
            break;
        }
    }
    //up right
    for(int j = 1; j <= 7; j++)
    {
        if(index_x - j < 0 || index_y + j > 7)
        {
            break;
        }
        x = index_x - j;
        y = index_y + j;
        if(board[x][y] != 'K' && board[x][y] != 'Q' && (board[x][y] != 'P' || (x < 5 && board[x - 1][y] == ' ')))
        {  
            if(!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if(board[index_x - j][index_y + j] != ' ')
        {
            break;
        }
    }
    //down left
    for(int j = 1; j <= 7; j++)
    {
        if(index_x + j > 7 || index_y - j < 0)
        {
            break;
        }
        x = index_x + j;
        y = index_y - j;
        if(board[x][y] != 'K' && board[x][y] != 'Q' && (board[x][y] != 'P' || (x < 5 && board[x - 1][y] == ' ')))
        {  
            if(!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if(board[index_x + j][index_y - j] != ' ')
        {
            break;
        }
    }
    //down right
    for(int j = 1; j <= 7; j++)
    {
        if(index_x + j > 7 || index_y + j > 7)
        {
            break;
        }
        x = index_x + j;
        y = index_y + j;
        if(board[x][y] != 'K' && board[x][y] != 'Q' && (board[x][y] != 'P' || (x < 5 && board[x - 1][y] == ' ')))
        {  
            if(!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if(board[index_x + j][index_y + j] != ' ')
        {
            break;
        }
    }
    return 3 * (move_count - 7);
}

static inline int bbishop_mobility(char board[8][8], int index_x, int index_y)
{
    int move_count = 0;
    int x;
    int y;
    //up left
    for(int j = 1; j <= 7; j++)
    {
        if(index_x - j < 0 || index_y - j < 0)
        {
            break;
        }
        x = index_x - j;
        y = index_y - j;
        if(board[x][y] != 'k' && board[x][y] != 'q' && (board[x][y] != 'p' || (x > 2 && board[x + 1][y] == ' ')))
        {  
            if(!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if(board[index_x - j][index_y - j] != ' ')
        {
            break;
        }
    }
    //up right
    for(int j = 1; j <= 7; j++)
    {
        if(index_x - j < 0 || index_y + j > 7)
        {
            break;
        }
        x = index_x - j;
        y = index_y + j;
        if(board[x][y] != 'k' && board[x][y] != 'q' && (board[x][y] != 'p' || (x > 2 && board[x + 1][y] == ' ')))
        {  
            if(!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if(board[index_x - j][index_y + j] != ' ')
        {
            break;
        }
    }
    //down left
    for(int j = 1; j <= 7; j++)
    {
        if(index_x + j > 7 || index_y - j < 0)
        {
            break;
        }
        x = index_x + j;
        y = index_y - j;
        if(board[x][y] != 'k' && board[x][y] != 'q' && (board[x][y] != 'p' || (x > 2 && board[x + 1][y] == ' ')))
        {  
            if(!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if(board[index_x + j][index_y - j] != ' ')
        {
            break;
        }
    }
    //down right
    for(int j = 1; j <= 7; j++)
    {
        if(index_x + j > 7 || index_y + j > 7)
        {
            break;
        }
        x = index_x + j;
        y = index_y + j;
        if(board[x][y] != 'k' && board[x][y] != 'q' && (board[x][y] != 'p' || (x > 2 && board[x + 1][y] == ' ')))
        {  
            if(!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if(board[index_x + j][index_y + j] != ' ')
        {
            break;
        }
    }
    return 3 * (move_count - 7);
}

static inline int wrook_mobility(char board[8][8], int index_x, int index_y)
{
    int move_count = 0;
    int x;
    int y;
    //up
    for(int j = index_x-1; j >= 0; j--)
    {
        x = j;
        y = index_y;
        if(board[x][y] != 'K' && (board[x][y] != 'P' || (x < 5 && board[x - 1][y] == ' '))) 
        {  
            if(!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if(board[j][index_y] != ' ')
        {
            break;
        }
    }
    //left
    for(int j = index_y-1; j >= 0; j--)
    {
        x = index_x;
        y = j;
        if(board[x][y] != 'K' && (board[x][y] != 'P' || (x < 5 && board[x - 1][y] == ' '))) 
        {  
            if(!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if(board[index_x][j] != ' ')
        {
            break;
        }
    }
    //right
    for(int j = index_y+1; j <= 7; j++)
    {
        x = index_x;
        y = j;
        if(board[x][y] != 'K' && (board[x][y] != 'P' || (x < 5 && board[x - 1][y] == ' ')))   
        {  
            if(!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if(board[index_x][j] != ' ')
        {
            break;
        }
    }
    //down
    for(int j = index_x+1; j <= 7; j++)
    {
        x = j;
        y = index_y;
        if(board[x][y] != 'K' && (board[x][y] != 'P' || (x < 5 && board[x - 1][y] == ' '))) 
        {  
            if(!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if(board[j][index_y] != ' ')
        {
            break;
        }
    }
    return 2 * (move_count - 7);
}

static inline int brook_mobility(char board[8][8], int index_x, int index_y)
{
    int move_count = 0;
    int x;
    int y;
    //up
    for(int j = index_x-1; j >= 0; j--)
    {  
        x = j;
        y = index_y;
        if(board[x][y] != 'k' && (board[x][y] != 'p' || (x > 2 && board[x + 1][y] == ' ')))
        {  
            if(!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if(board[j][index_y] != ' ')
        {
            break;
        }
    }
    //left
    for(int j = index_y-1; j >= 0; j--)
    {
        x = index_x;
        y = j;
        if(board[x][y] != 'k' && (board[x][y] != 'p' || (x > 2 && board[x + 1][y] == ' ')))
        {  
            if(!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if(board[index_x][j] != ' ')
        {
            break;
        }
    }
    //right
    for(int j = index_y+1; j <= 7; j++)
    {
        x = index_x;
        y = j;
        if(board[x][y] != 'k' && (board[x][y] != 'p' || (x > 2 && board[x + 1][y] == ' ')))
        {  
            if(!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if(board[index_x][j] != ' ')
        {
            break;
        }
    }
    //down
    for(int j = index_x+1; j <= 7; j++)
    {
        x = j;
        y = index_y;
        if(board[x][y] != 'k' && (board[x][y] != 'p' || (x > 2 && board[x + 1][y] == ' ')))
        {  
            if(!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if(board[j][index_y] != ' ')
        {
            break;
        }
    }
    return 2 * (move_count - 7);
}

static inline int wqueen_mobility(char board[8][8], int index_x, int index_y)
{
    int move_count = 0;
    int x;
    int y;
    //up left
    for(int j = 1; j <= 7; j++)
    {
        if(index_x - j < 0 || index_y - j < 0)
        {
            break;
        }
        x = index_x - j;
        y = index_y - j;
        if(board[x][y] != 'K' && (board[x][y] != 'P' || (x < 5 && board[x - 1][y] == ' '))) 
        {  
            if(!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if(board[index_x - j][index_y - j] != ' ')
        {
            break;
        }
    }
    //up
    for(int j = index_x-1; j >= 0; j--)
    {
        x = j;
        y = index_y;
        if(board[x][y] != 'K' && (board[x][y] != 'P' || (x < 5 && board[x - 1][y] == ' '))) 
        {  
            if(!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if(board[j][index_y] != ' ')
        {
            break;
        }
    }
    //up right
    for(int j = 1; j <= 7; j++)
    {
        if(index_x - j < 0 || index_y + j > 7)
        {
            break;
        }
        x = index_x - j;
        y = index_y + j;
        if(board[x][y] != 'K' && (board[x][y] != 'P' || (x < 5 && board[x - 1][y] == ' '))) 
        {  
            if(!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if(board[index_x - j][index_y + j] != ' ')
        {
            break;
        }
    }
    //left
    for(int j = index_y-1; j >= 0; j--)
    {
        x = index_x;
        y = j;
        if(board[x][y] != 'K' && (board[x][y] != 'P' || (x < 5 && board[x - 1][y] == ' '))) 
        {  
            if(!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if(board[index_x][j] != ' ')
        {
            break;
        }
    }
    //right
    for(int j = index_y+1; j <= 7; j++)
    {
        x = index_x;
        y = j;
        if(board[x][y] != 'K' && (board[x][y] != 'P' || (x < 5 && board[x - 1][y] == ' ')))   
        {  
            if(!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if(board[index_x][j] != ' ')
        {
            break;
        }
    }
    //down left
    for(int j = 1; j <= 7; j++)
    {
        if(index_x + j > 7 || index_y - j < 0)
        {
            break;
        }
        x = index_x + j;
        y = index_y - j;
        if(board[x][y] != 'K' && (board[x][y] != 'P' || (x < 5 && board[x - 1][y] == ' '))) 
        {  
            if(!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if(board[index_x + j][index_y - j] != ' ')
        {
            break;
        }
    }
    //down
    for(int j = index_x+1; j <= 7; j++)
    {
        x = j;
        y = index_y;
        if(board[x][y] != 'K' && (board[x][y] != 'P' || (x < 5 && board[x - 1][y] == ' '))) 
        {  
            if(!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if(board[j][index_y] != ' ')
        {
            break;
        }
    }
    //down right
    for(int j = 1; j <= 7; j++)
    {
        if(index_x + j > 7 || index_y + j > 7)
        {
            break;
        }
        x = index_x + j;
        y = index_y + j;
        if(board[x][y] != 'K' && (board[x][y] != 'P' || (x < 5 && board[x - 1][y] == ' '))) 
        {  
            if(!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if(board[index_x + j][index_y + j] != ' ')
        {
            break;
        }
    }
    return 1 * (move_count - 14);
}

static inline int bqueen_mobility(char board[8][8], int index_x, int index_y)
{
    int move_count = 0;
    int x;
    int y;
    //up left
    for(int j = 1; j <= 7; j++)
    {
        if(index_x - j < 0 || index_y - j < 0)
        {
            break;
        }
        x = index_x - j;
        y = index_y - j;
        if(board[x][y] != 'k' && (board[x][y] != 'p' || (x > 2 && board[x + 1][y] == ' ')))
        {  
            if(!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if(board[index_x - j][index_y - j] != ' ')
        {
            break;
        }
    }
    //up
    for(int j = index_x-1; j >= 0; j--)
    {
        x = j;
        y = index_y;
        if(board[x][y] != 'k' && (board[x][y] != 'p' || (x > 2 && board[x + 1][y] == ' ')))
        {  
            if(!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if(board[j][index_y] != ' ')
        {
            break;
        }
    }
    //up right
    for(int j = 1; j <= 7; j++)
    {
        if(index_x - j < 0 || index_y + j > 7)
        {
            break;
        }
        x = index_x - j;
        y = index_y + j;
        if(board[x][y] != 'k' && (board[x][y] != 'p' || (x > 2 && board[x + 1][y] == ' ')))
        {  
            if(!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if(board[index_x - j][index_y + j] != ' ')
        {
            break;
        }
    }
    //left
    for(int j = index_y-1; j >= 0; j--)
    {
        x = index_x;
        y = j;
        if(board[x][y] != 'k' && (board[x][y] != 'p' || (x > 2 && board[x + 1][y] == ' ')))
        {  
            if(!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if(board[index_x][j] != ' ')
        {
            break;
        }
    }
    //right
    for(int j = index_y+1; j <= 7; j++)
    {
        x = index_x;
        y = j;
        if(board[x][y] != 'k' && (board[x][y] != 'p' || (x > 2 && board[x + 1][y] == ' ')))
        {  
            if(!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if(board[index_x][j] != ' ')
        {
            break;
        }
    }
    //down left
    for(int j = 1; j <= 7; j++)
    {
        if(index_x + j > 7 || index_y - j < 0)
        {
            break;
        }
        x = index_x + j;
        y = index_y - j;
        if(board[x][y] != 'k' && (board[x][y] != 'p' || (x > 2 && board[x + 1][y] == ' ')))
        {  
            if(!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if(board[index_x + j][index_y - j] != ' ')
        {
            break;
        }
    }
    //down
    for(int j = index_x+1; j <= 7; j++)
    {
        x = j;
        y = index_y;
        if(board[x][y] != 'k' && (board[x][y] != 'p' || (x > 2 && board[x + 1][y] == ' ')))
        {  
            if(!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if(board[j][index_y] != ' ')
        {
            break;
        }
    }
    //down right
    for(int j = 1; j <= 7; j++)
    {
        if(index_x + j > 7 || index_y + j > 7)
        {
            break;
        }
        x = index_x + j;
        y = index_y + j;
        if(board[x][y] != 'k' && (board[x][y] != 'p' || (x > 2 && board[x + 1][y] == ' ')))
        {  
            if(!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if(board[index_x + j][index_y + j] != ' ')
        {
            break;
        }
    }
    return 1 * (move_count - 14);
}

static inline unsigned long long int getEvalHash(char board[8][8], int color)
{
    unsigned long long int h = 0;
    int piece;
    for(int x = 0; x < 8; x++)
    {
        for(int y = 0; y < 8; y++)
        {
            if(board[x][y] != ' ')
            {
                piece = index(board[x][y]);
                h ^= table[x][y][piece];
            }
        }
    }
    //encode turn
    if(color == 1)
    {
        h ^= turn;
    }

    return h;
}

static inline struct Eval *probeEvalTT(unsigned long long int key)
{
    //get the hash 
    int hashIndex = key % EVALHASHSIZE;
    if(Evaltt[hashIndex].valid == true && Evaltt[hashIndex].key == key)
    {    
        return &Evaltt[hashIndex];
    }
    else
    {
        return NULL;
    }

}

static inline void storeEvalTT(unsigned long long int key, int evaluation)
{
    //get the hash 
    int hashIndex = key % EVALHASHSIZE;

    Evaltt[hashIndex].key = key;
    Evaltt[hashIndex].evaluation = evaluation;
    Evaltt[hashIndex].valid = true;
}

void clearEvalTT()
{
    for(int x = 0; x < EVALHASHSIZE; x++)
    {
        Evaltt[x].key = 0;
        Evaltt[x].evaluation = 0;
        Evaltt[x].valid = false;
    }
}

void EvalTT_usage()
{
    float count = 0;
    for(int x = 0; x < EVALHASHSIZE; x++)
    {
        if(Evaltt[x].valid == true)
            count++;
    }

    printf("Eval Table Usage: %.2f%%\n", count * 100 / EVALHASHSIZE);
}

//1: black
//-1: white
int evaluate(char board[8][8], int color)
{
    //tt probe
    unsigned long long int key = getEvalHash(board, color);
    struct Eval* entry = probeEvalTT(key);
    if(entry != NULL)
    { 
        return entry -> evaluation;
    }

    int P_count = 0, R_count = 0, N_count = 0, B_count = 0, Q_count = 0; // White Chess piece counts
    int p_count = 0, r_count = 0, n_count = 0, b_count = 0, q_count = 0; // Black Chess piece counts
    int points = 0;
    int position_bonus_white = 0;
    int position_bonus_black = 0;
    int other_bonus_white = 0;
    int other_bonus_black = 0;
    int white_king_x = -1;
    int white_king_y = -1;
    int black_king_x = -1;
    int black_king_y = -1;
    int wqueen[4] = {-1, -1, -1, -1};
    int bqueen[4] = {-1, -1, -1, -1};
    int wknight[4] = {-1, -1, -1, -1};
    int bknight[4] = {-1, -1, -1, -1};  
    int wrook[4] = {-1, -1, -1, -1};
    int brook[4] = {-1, -1, -1, -1};
    int wbishop[4] = {-1, -1, -1, -1};
    int bbishop[4] = {-1, -1, -1, -1};
    int tempo = 0;   
    int midgame_white = 0, midgame_black = 0;
    int endgame_white = 0, endgame_black = 0;
    int wqueen_mob = 0, bqueen_mob = 0, wrook_mob = 0, brook_mob = 0;
    bool passed;

    for(int x = 0; x < 8; x++)
 	{
		for(int y = 0; y < 8; y++)
		{
            switch(board[x][y])
            {
                case 'P':
                {
                    P_count++;
                    passed = false;
                    if(board[x-1][y] == 'P')
                    {
                        other_bonus_white -= DOUBLEDPAWN;     //doubled pawn penalty    
                        position_bonus_white += white_pawn[x][y];
                    }
                    else if(passed_white(board, x, y))
                    {
                        passed = true;
                        //blockage penalty
                        if(islower(board[x-1][y]))
                            position_bonus_white += PawnPassed_white[x+1];
                        else
                            position_bonus_white += PawnPassed_white[x];
                        if(phalanx_white(board, x, y))
                        {
                            other_bonus_white += 20;
                        }
                    }
                    else
                    {
                        position_bonus_white += white_pawn[x][y];
                    }

                    if(connected_white(board, x, y))
                    {
                        if(passed)
                            other_bonus_white += CONNECTEDPAWN*2;
                        else
                            other_bonus_white += CONNECTEDPAWN;    //connected pawn bonus
                    }
                    else if(isolated_white(board, y))
                    {
                        other_bonus_white -= ISOLATEDPAWN;
                    }
                    
                    if(backward_white(board, x, y))
                    {
                        other_bonus_white -= BACKWARDPAWN;                        
                    }      
                    break;
                }
                case 'R':
                {
                    R_count++;
                    position_bonus_white += white_rook[x][y];
                    switch(R_count)
                    {
                        case 1:
                            wrook[0] = x;
                            wrook[1] = y;
                            break;
                        case 2:
                            wrook[2] = x;
                            wrook[3] = y;
                            break;                        
                    }
                    if(openFile(board, y))
                    {
                        other_bonus_white += ROOKOPENFILE;
                    }
                    else if(semiOpenFile_white(board, y))
                    {
                        other_bonus_white += ROOKHALFFILE;
                    }

                    if(queenFile(board, y))
                    {
                        other_bonus_white += ROOKQUEENFILE;
                    }
                    //mobility
                    wrook_mob = wrook_mobility(board, x, y);
                    midgame_white += wrook_mob;
                    endgame_white += wrook_mob*2;
                    break;
                }
                case 'N':
                {
                    N_count++;
                    position_bonus_white += white_knight[x][y];
                    switch(N_count)
                    {
                        case 1:
                            wknight[0] = x;
                            wknight[1] = y;
                            break;
                        case 2:
                            wknight[2] = x;
                            wknight[3] = y;
                            break;                        
                    }
                    if(outpost_white(board, x, y))
                    {
                        other_bonus_white += OUTPOST; //outpost bonus
                    }
                    //mobility
                    other_bonus_white += wknight_mobility(board, x, y);
                    //trapped pieces penalty
                    switch(8*x + y)
                    {
                        case a8:
                            if(board[1][0] == 'p' || board[1][2] == 'p')
                            {
                                other_bonus_white -= 100;
                            }
                            break;
                        case h8:
                            if(board[1][7] == 'p' || board[1][5] == 'p')
                            {
                                other_bonus_white -= 100;
                            }
                            break;
                        case a7:
                            if(board[2][0] == 'p' && board[1][1] == 'p')
                            {
                                other_bonus_white -= 100;
                            }
                            break;
                        case h7:
                            if(board[2][7] == 'p' && board[1][6] == 'p')
                            {
                                other_bonus_white -= 100;
                            }
                            break;
                        case c3:
                            if(board[6][2] == 'P' && board[4][3] == 'P' && board[4][4] != 'P')
                            {
                                other_bonus_white -= 5; //c3 knight penalty
                            }
                            break;
                    }
                    break;
                }
                case 'B':
                {
                    B_count++;
                    position_bonus_white += white_bishop[x][y];
                    switch(B_count)
                    {
                        case 1:
                            wbishop[0] = x;
                            wbishop[1] = y;
                            break;
                        case 2:
                            wbishop[2] = x;
                            wbishop[3] = y;
                            break;                        
                    }
                    if(badBishop_white(board, x, y))
                    {
                        other_bonus_white -= BADBISHOP;    //bad bishop penalty
                    }
                    if(outpost_white(board, x, y))
                    {
                        other_bonus_white += OUTPOST; //outpost bonus
                    }
                    //mobility
                    other_bonus_white += wbishop_mobility(board, x, y);
                    //trapped pieces penalty
                    switch(8*x + y)
                    {
                        case a7:
                            if(board[2][1] == 'p' && board[1][2] == 'p')
                            {
                                other_bonus_white -= 100;
                            }
                            break;
                        case h7:
                            if(board[2][6] == 'p' && board[1][5] == 'p')
                            {
                                other_bonus_white -= 100;
                            }
                            break;
                        case a6:
                            if(board[3][1] == 'p' && board[2][2] == 'p')
                            {
                                other_bonus_white -= 50;
                            }
                            break;
                        case h6:
                            if(board[3][6] == 'p' && board[2][5] == 'p')
                            {
                                other_bonus_white -= 50;
                            }
                            break;
                        case f1:
                            if(board[7][6] == 'K')
                            {
                                other_bonus_white += RETURNINGBISHOP; 
                            }
                            if(board[6][4] == 'P' && board[5][4] != ' ')
                            {
                                other_bonus_white -= 20; //bishop no development penalty
                            }
                            break;
                        case c1:
                            if(board[7][1] == 'K')
                            {
                                other_bonus_white += RETURNINGBISHOP; 
                            }
                            if(board[6][3] == 'P' && board[5][3] != ' ')
                            {
                                other_bonus_white -= 20; //bishop no development penalty
                            }
                            break;
                    }
                    break;
                }
                case 'Q':
                {
                    Q_count++;
                    position_bonus_white += white_queen[x][y];
                    switch(Q_count)
                    {
                        case 1:
                            wqueen[0] = x;
                            wqueen[1] = y;
                            break;
                        case 2:
                            wqueen[2] = x;
                            wqueen[3] = y;
                            break;                        
                    }
                    //queen early development penalty
                    if(x < 6)
                    {
                        if(board[7][1] == 'N')
                        {
                            other_bonus_white -= 4;
                        }
                        if(board[7][2] == 'B')
                        {
                            other_bonus_white -= 4;
                        }
                        if(board[7][5] == 'B')
                        {
                            other_bonus_white -= 4;
                        }
                        if(board[7][6] == 'N')
                        {
                            other_bonus_white -= 4;
                        }
                    }
                    //mobility
                    wqueen_mob = wqueen_mobility(board, x, y);
                    midgame_white += wqueen_mob; 
                    endgame_white += wqueen_mob*2;
                    break;
                }
                case 'K':
                {
                    white_king_x = x;
                    white_king_y = y;
                    break;
                }
                case 'p':
                {
                    p_count++;
                    passed = false;
                    if(board[x+1][y] == 'p')
                    {
                        other_bonus_black -= DOUBLEDPAWN;     //doubled pawn penalty
                        position_bonus_black += black_pawn[x][y];
                    }
                    else if(passed_black(board, x, y))
                    {
                        passed = true;
                        //blockage penalty
                        if(isupper(board[x+1][y]))
                            position_bonus_black += PawnPassed_black[x-1];
                        else
                            position_bonus_black += PawnPassed_black[x];
                        if(phalanx_black(board, x, y))
                        {
                            other_bonus_black += 20;    //connected pawn bonus
                        }
                    }
                    else
                    {
                        position_bonus_black += black_pawn[x][y];
                    }
                    
                    if(connected_black(board, x, y))
                    {
                        if(passed)
                            other_bonus_black += CONNECTEDPAWN*2;    //connected pawn bonus
                        else
                            other_bonus_black += CONNECTEDPAWN; 
                    }
                    else if(isolated_black(board, y))
                    {
                        other_bonus_black -= ISOLATEDPAWN;
                    }

                    if(backward_black(board, x, y))
                    {
                        other_bonus_black -= BACKWARDPAWN;                        
                    }
                    break;
                }
                case 'r':
                {
                    r_count++;
                    position_bonus_black += black_rook[x][y];
                    switch(r_count)
                    {
                        case 1:
                            brook[0] = x;
                            brook[1] = y;
                            break;
                        case 2:
                            brook[2] = x;
                            brook[3] = y;
                            break;                        
                    }
                    if(openFile(board, y))
                    {
                        other_bonus_black += ROOKOPENFILE;
                    }
                    else if(semiOpenFile_black(board, y))
                    {
                        other_bonus_black += ROOKHALFFILE;
                    }

                    if(queenFile(board, y))
                    {
                        other_bonus_black += ROOKQUEENFILE;
                    }
                    //mobility
                    brook_mob = brook_mobility(board, x, y);
                    midgame_black += brook_mob;
                    endgame_black += brook_mob*2; 
                    break;
                }
                case 'n':
                {
                    n_count++;
                    position_bonus_black += black_knight[x][y];
                    switch(n_count)
                    {
                        case 1:
                            bknight[0] = x;
                            bknight[1] = y;
                            break;
                        case 2:
                            bknight[2] = x;
                            bknight[3] = y;
                            break;                        
                    }
                    if(outpost_black(board, x, y))
                    {
                        other_bonus_black += OUTPOST; //outpost bonus
                    }
                    //mobility
                    other_bonus_black += bknight_mobility(board, x, y);
                    //trapped pieces penalty
                    switch(8*x + y)
                    {
                        case a1:
                            if(board[6][0] == 'P' || board[6][2] == 'P')
                            {
                                other_bonus_black -= 100;
                            }
                            break;
                        case h1:
                            if(board[6][7] == 'P' || board[6][5] == 'P')
                            {
                                other_bonus_black -= 100;
                            }
                            break;
                        case a2:
                            if(board[5][0] == 'P' && board[6][1] == 'P')
                            {
                                other_bonus_black -= 100;
                            }
                            break;
                        case h2:
                            if(board[5][7] == 'P' && board[6][6] == 'P')
                            {
                                other_bonus_black -= 100;
                            }
                            break;
                        case c6:
                            if(board[1][2] == 'p' && board[3][3] == 'p' && board[3][4] != 'p')
                            {
                                other_bonus_black -= 5; //c6 knight penalty
                            }
                            break;
                    }
                    break;
                }
                case 'b':
                {
                    b_count++;
                    position_bonus_black += black_bishop[x][y];
                    switch(b_count)
                    {
                        case 1:
                            bbishop[0] = x;
                            bbishop[1] = y;
                            break;
                        case 2:
                            bbishop[2] = x;
                            bbishop[3] = y;
                            break;                        
                    }
                    if(badBishop_black(board, x, y))
                    {
                        other_bonus_black -= BADBISHOP;    //bad bishop penalty
                    }
                    if(outpost_black(board, x, y))
                    {
                        other_bonus_black += OUTPOST; //outpost bonus
                    }
                    //mobility
                    other_bonus_black += bbishop_mobility(board, x, y);
                    //trapped pieces penalty
                    switch(8*x + y)
                    {
                        case a2:
                            if(board[5][1] == 'P' && board[6][2] == 'P')
                            {
                                other_bonus_black -= 100;
                            }
                            break;
                        case h2:
                            if(board[5][6] == 'P' && board[6][5] == 'P')
                            {
                                other_bonus_black -= 100;
                            }
                            break;
                        case a3:
                            if(board[4][1] == 'P' && board[5][2] == 'P')
                            {
                                other_bonus_black -= 50;
                            }
                            break;
                        case h3:
                            if(board[4][6] == 'P' && board[5][5] == 'P')
                            {
                                other_bonus_black -= 50;
                            }
                            break;
                        case f8:
                            if(board[0][6] == 'k')
                            {
                                other_bonus_black += RETURNINGBISHOP; 
                            }
                            if(board[1][4] == 'p' && board[2][4] != ' ')
                            {
                                other_bonus_black -= 20; //bishop no development penalty
                            }
                            break;
                        case c8:
                            if(board[0][1] == 'k')
                            {
                                other_bonus_black += RETURNINGBISHOP; 
                            }
                            if(board[1][3] == 'p' && board[2][3] != ' ')
                            {
                                other_bonus_black -= 20; //bishop no development penalty
                            }
                            break;
                    }
                    break;
                }
                case 'q':
                {
                    q_count++;
                    position_bonus_black += black_queen[x][y];
                    switch(q_count)
                    {
                        case 1:
                            bqueen[0] = x;
                            bqueen[1] = y;
                            break;
                        case 2:
                            bqueen[2] = x;
                            bqueen[3] = y;
                            break;                        
                    }
                    //queen early development penalty
                    if(x > 1)
                    {
                        if(board[0][1] == 'n')
                        {
                            other_bonus_black -= 4;
                        }
                        if(board[0][2] == 'b')
                        {
                            other_bonus_black -= 4;
                        }
                        if(board[0][5] == 'b')
                        {
                            other_bonus_black -= 4;
                        }
                        if(board[0][6] == 'n')
                        {
                            other_bonus_black -= 4;
                        }
                    }
                    //mobility
                    bqueen_mob = bqueen_mobility(board, x, y);
                    midgame_black += bqueen_mob;
                    endgame_black += bqueen_mob*2;
                    break;
                } 
                case 'k':
                {
                    black_king_x = x;
                    black_king_y = y;
                    break;
                } 
                default:
                {
                    continue;
                    break;
                }
            }
        }
    }

    //tempo
    if((P_count + p_count + N_count + n_count + B_count + b_count + R_count + r_count + Q_count + q_count) > 12)
        tempo = color * TEMPO;

    //game phase based on non-pawn materials
    int phase = N_count + n_count + B_count + b_count + R_count * 2 + r_count * 2 + Q_count * 4 + q_count * 4;

    if(phase > 0)
    {
        //for kings' position bonus midgame
        midgame_white += white_king_midgame[white_king_x][white_king_y];
        midgame_black += black_king_midgame[black_king_x][black_king_y];
        //pawn shield bonus
        if(white_king_x > 5 && white_king_y > 4)
        {
            if(board[7][6] == 'R' || board[7][7] == 'R') //blocked rook penalty
                midgame_white -= 40;
            
            if(board[6][5] == 'P')  
                midgame_white += 15;
            else if(board[5][5] == 'P')
                midgame_white += 10;
            else if(semiOpenFile_white(board, 5)) //semi-open file penalty
                midgame_white -= 15;
            
            if(board[5][5] == 'p')  //pawn storm penalty
                midgame_white -= 10;
            else if(board[4][5] == 'p')
                midgame_white -= 5;
            
            if(board[6][6] == 'P')
                midgame_white += 15;
            else if(board[5][6] == 'P')
                midgame_white += 10;
            else if(semiOpenFile_white(board, 6))
                midgame_white -= 15;

            if(board[5][6] == 'p')
                midgame_white -= 10;
            else if(board[4][6] == 'p')
                midgame_white -= 5;    
            
            if(board[6][7] == 'P')
                midgame_white += 15;
            else if(board[5][7] == 'P')
                midgame_white += 10;
            else if(semiOpenFile_white(board, 7))
                midgame_white -= 15;
            
            if(board[5][7] == 'p')
                midgame_white -= 10;
            else if(board[4][7] == 'p')
                midgame_white -= 5;  
        }
        else if(white_king_x > 5 && white_king_y < 3)
        {
            if(board[7][0] == 'R' || board[7][1] == 'R')
                midgame_white -= 40;
            
            if(board[6][0] == 'P')
                midgame_white += 15;
            else if(board[5][0] == 'P')
                midgame_white += 10; 
            else if(semiOpenFile_white(board, 0))
                midgame_white -= 15;

            if(board[5][0] == 'p')
                midgame_white -= 10;
            else if(board[4][0] == 'p')
                midgame_white -= 5;  
            
            if(board[6][1] == 'P')
                midgame_white += 15;
            else if(board[5][1] == 'P')
                midgame_white += 10;
            else if(semiOpenFile_white(board, 1))
                midgame_white -= 15;

            if(board[5][1] == 'p')
                midgame_white -= 10;
            else if(board[4][1] == 'p')
                midgame_white -= 5;  
            
            if(board[6][2] == 'P')
                midgame_white += 15;
            else if(board[5][2] == 'P')
                midgame_white += 10;
            else if(semiOpenFile_white(board, 2))
                midgame_white -= 15;

            if(board[5][2] == 'p')
                midgame_white -= 10;
            else if(board[4][2] == 'p')
                midgame_white -= 5;  
        }

        if(black_king_x < 2 && black_king_y > 4)
        {
            if(board[0][6] == 'r' || board[0][7] == 'r')
                midgame_black -= 40;
            
            if(board[1][5] == 'p')
                midgame_black += 15;
            else if(board[2][5] == 'p')
                midgame_black += 10;
            else if(semiOpenFile_black(board, 5))
                midgame_black -= 15;

            if(board[2][5] == 'P')
                midgame_black -= 10;
            else if(board[3][5] == 'P')
                midgame_black -= 5;  
            
            if(board[1][6] == 'p')
                midgame_black += 15;
            else if(board[2][6] == 'p')
                midgame_black += 10;
            else if(semiOpenFile_black(board, 6))
                midgame_black -= 15;
            
            if(board[2][6] == 'P')
                midgame_black -= 10;
            else if(board[3][6] == 'P')
                midgame_black -= 5; 

            if(board[1][7] == 'p')
                midgame_black += 15;
            else if(board[2][7] == 'p')
                midgame_black += 10;
            else if(semiOpenFile_black(board, 7))
                midgame_black -= 15;
            
            if(board[2][7] == 'P')
                midgame_black -= 10;
            else if(board[3][7] == 'P')
                midgame_black -= 5; 
        }
        else if(black_king_x < 2 && black_king_y < 3)
        {
            if(board[0][0] == 'r' || board[0][1] == 'r')
                midgame_black -= 40;

            if(board[1][0] == 'p')
                midgame_black += 15;
            else if(board[2][0] == 'p')
                midgame_black += 10;
            else if(semiOpenFile_black(board, 0))
                midgame_black -= 15;
            
            if(board[2][0] == 'P')
                midgame_black -= 10;
            else if(board[3][0] == 'P')
                midgame_black -= 5; 
            
            if(board[1][1] == 'p')
                midgame_black += 15;
            else if(board[2][1] == 'p')
                midgame_black += 10;
            else if(semiOpenFile_black(board, 1))
                midgame_black -= 15;
            
            if(board[2][1] == 'P')
                midgame_black -= 10;
            else if(board[3][1] == 'P')
                midgame_black -= 5; 
            
            if(board[1][2] == 'p')
                midgame_black += 15;
            else if(board[2][2] == 'p')
                midgame_black += 10;
            else if(semiOpenFile_black(board, 2))
                midgame_black -= 15;
            
            if(board[2][2] == 'P')
                midgame_black -= 10;
            else if(board[3][2] == 'P')
                midgame_black -= 5; 
        }
    }

    //king position bonus endgame
    endgame_white += white_king_endgame[white_king_x][white_king_y];
    endgame_black += black_king_endgame[black_king_x][black_king_y];

    int wqueen_tropism = 0, bqueen_tropism = 0, wrook_tropism = 0, brook_tropism = 0, wknight_tropism = 0, bknight_tropism = 0, wbishop_tropism = 0, bbishop_tropism = 0;

    //king tropism
    switch(Q_count)
    {
        case 0:
            break;
        case 1:
            wqueen_tropism = abs(wqueen[0] - black_king_x) + abs(wqueen[1] - black_king_y) - 7;
            midgame_black += wqueen_tropism*2; 
            endgame_black += wqueen_tropism*4; 
            break;
        default:
            wqueen_tropism = abs(wqueen[0] - black_king_x) + abs(wqueen[1] - black_king_y) + abs(wqueen[2] - black_king_x) + abs(wqueen[3] - black_king_y) - 14;
            midgame_black += wqueen_tropism*2; 
            endgame_black += wqueen_tropism*4; 
            break;                 
    }
    switch(q_count)
    {
        case 0:
            break;
        case 1:
            bqueen_tropism = abs(bqueen[0] - white_king_x) + abs(bqueen[1] - white_king_y) - 7;
            midgame_white += bqueen_tropism*2; 
            endgame_white += bqueen_tropism*4; 
            break;
        default:
            bqueen_tropism = abs(bqueen[0] - white_king_x) + abs(bqueen[1] - white_king_y) + abs(bqueen[2] - white_king_x) + abs(bqueen[3] - white_king_y)- 14;
            midgame_white += bqueen_tropism*2; 
            endgame_white += bqueen_tropism*4; 
            break;                       
    }
    switch(N_count)
    {
        case 0:
            break;
        case 1:
            wknight_tropism = abs(wknight[0] - black_king_x) + abs(wknight[1] - black_king_y) - 7;
            midgame_black += wknight_tropism*3; 
            endgame_black += wknight_tropism*3; 
            break;
        default:
            wknight_tropism = abs(wknight[0] - black_king_x) + abs(wknight[1] - black_king_y) + abs(wknight[2] - black_king_x) + abs(wknight[3] - black_king_y) - 14;
            midgame_black += wknight_tropism*3; 
            endgame_black += wknight_tropism*3;
            break;                       
    }
    switch(n_count)
    {
        case 0:
            break;
        case 1:
            bknight_tropism = abs(bknight[0] - white_king_x) + abs(bknight[1] - white_king_y) - 7;
            midgame_white += bknight_tropism*3; 
            endgame_white += bknight_tropism*3;
            break;
        default:
            bknight_tropism = abs(bknight[0] - white_king_x) + abs(bknight[1] - white_king_y) + abs(bknight[2] - white_king_x) + abs(bknight[3] - white_king_y) - 14;
            midgame_white += bknight_tropism*3; 
            endgame_white += bknight_tropism*3;
            break;                      
    }
    switch(R_count)
    {
        case 0:
            break;
        case 1:
            wrook_tropism = abs(wrook[0] - black_king_x) + abs(wrook[1] - black_king_y) - 7;
            midgame_black += wrook_tropism*2; 
            endgame_black += wrook_tropism*1; 
            break;
        default:
            wrook_tropism = abs(wrook[0] - black_king_x) + abs(wrook[1] - black_king_y) + abs(wrook[2] - black_king_x) + abs(wrook[3] - black_king_y) - 14;
            midgame_black += wrook_tropism*2; 
            endgame_black += wrook_tropism*1; 
            break;                       
    }
    switch(r_count)
    {
        case 0:
            break;
        case 1:
            brook_tropism = abs(brook[0] - white_king_x) + abs(brook[1] - white_king_y) - 7;
            midgame_white += brook_tropism*2; 
            endgame_white += brook_tropism*1;
            break;
        default:
            brook_tropism = abs(brook[0] - white_king_x) + abs(brook[1] - white_king_y) + abs(brook[2] - white_king_x) + abs(brook[3] - white_king_y) - 14;
            midgame_white += brook_tropism*2; 
            endgame_white += brook_tropism*1; 
            break;                       
    }
    switch(B_count)
    {
        case 0:
            break;
        case 1:
            wbishop_tropism = abs(wbishop[0] - black_king_x) + abs(wbishop[1] - black_king_y) - 7;
            midgame_black += wbishop_tropism*2; 
            endgame_black += wbishop_tropism*1; 
            break;
        default:
            wbishop_tropism = abs(wbishop[0] - black_king_x) + abs(wbishop[1] - black_king_y) + abs(wbishop[2] - black_king_x) + abs(wbishop[3] - black_king_y) - 14;
            midgame_black += wbishop_tropism*2; 
            endgame_black += wbishop_tropism*1; 
            break;                       
    }
    switch(b_count)
    {
        case 0:
            break;
        case 1:
            bbishop_tropism = abs(bbishop[0] - white_king_x) + abs(bbishop[1] - white_king_y) - 7;
            midgame_white += bbishop_tropism*2; 
            endgame_white += bbishop_tropism*1;
            break;
        default:
            bbishop_tropism = abs(bbishop[0] - white_king_x) + abs(bbishop[1] - white_king_y) + abs(bbishop[2] - white_king_x) + abs(bbishop[3] - white_king_y) - 14;
            midgame_white += bbishop_tropism*2; 
            endgame_white += bbishop_tropism*1; 
            break;                        
    }
    
    points = p_count * 100 + r_count * (500 + rook_val[p_count]) + n_count * (320 + knight_val[p_count]) + b_count * 330 + q_count * 900  + position_bonus_black + ((b_count >= 2)? 1 : 0) * BISHOPPAIR + ((n_count >= 2)? 1 : 0) * KNIGHTPAIR + ((r_count >= 2)? 1 : 0) * ROOKPAIR + other_bonus_black - P_count * 100 - R_count * (500 + rook_val[P_count]) - N_count * (320 + knight_val[P_count]) - B_count * 330 - Q_count * 900 - position_bonus_white - ((B_count >= 2)? 1 : 0) * BISHOPPAIR - ((N_count >= 2)? 1 : 0) * KNIGHTPAIR - ((R_count >= 2)? 1 : 0) * ROOKPAIR - other_bonus_white + tempo;
    
    //adjust phase score based on materials
    if(phase > 24)
        phase = 24;
    int mg_weight = phase;
    int eg_weight = 24 - mg_weight;
    points += (((midgame_black - midgame_white) * mg_weight + (endgame_black - endgame_white) * eg_weight) / 24);
    
    //material draw
    if(!P_count && !p_count)
    {
        if(!R_count && !r_count && !Q_count && !q_count)
        {
            if(!B_count && !b_count)
            {
                if(N_count < 3 && n_count < 3)
                {
                    //tt store
                    storeEvalTT(key, 0);
                    return 0;
                }
            }
            else if(!N_count && !n_count)
            {
                if(abs(B_count - b_count) < 2)
                {
                    //tt store
                    storeEvalTT(key, 0);
                    return 0;
                }
            }
            else if((N_count < 3 && !B_count) || (B_count == 1 && !N_count))
            {
                if((n_count < 3 && !b_count) || (b_count == 1 && !n_count))
                {
                    //tt store
                    storeEvalTT(key, 0);
                    return 0;
                }
            }
        }
        else if(!Q_count && !q_count)
        {
            if(R_count == 1 && r_count == 1)
            {
                if(((N_count + B_count) < 2 && (n_count + b_count) == 0) || ((N_count + B_count) == 0 && (n_count + b_count) < 2))
                {
                    //tt store
                    storeEvalTT(key, color * points / 2);
                    return color * points / 2;
                }
            }
            else if(R_count == 1 && !r_count)
            {
                if((N_count + B_count == 0) && (((n_count + b_count) == 1) || ((n_count + b_count) == 2))) 
                { 
                    //tt store
                    storeEvalTT(key, color * points / 2);
                    return color * points / 2; 
                }
            }
            else if(r_count == 1 && !R_count)
            {
                if((n_count + b_count == 0) && (((N_count + B_count) == 1) || ((N_count + B_count) == 2))) 
                { 
                    //tt store
                    storeEvalTT(key, color * points / 2);
                    return color * points / 2; 
                }
            }
        }
    }

    //tt store
    storeEvalTT(key, color * points);
    return color * points;
}