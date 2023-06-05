#include <tron.h>

#define AMOUNT 2
#define MAX_LENGTH 1000
#define P1_COLOR 0x00ffff
#define P2_COLOR 0xffff00

#define BOARD_WIDTH 300
#define BOARD_HEIGHT 300

#define PLAYER1 1
#define PLAYER2 2


#define BOARD_TOPX (int)1024/2 - BOARD_WIDTH / 2 * AMOUNT
#define BOARD_TOPY (int)768/2 - BOARD_HEIGHT / 2 * AMOUNT
#define BOARD_BOTTOMX (int)1024/2 + BOARD_WIDTH / 2
#define BOARD_BOTTOMY (int)768/2 + BOARD_HEIGHT / 2

typedef struct BOARD_INFO
{
    int x;
    int y;
    int player;
}board_position;


board_position board_grid[BOARD_HEIGHT][BOARD_WIDTH];


typedef struct playerPos{
    unsigned int board_x;
    unsigned int board_y;
}playerPos;

void getDirs();
void board_init();
void updatePos();
int checkWinner();
void refresh();
int checkCollision(board_position, int player);
void updateGrid();


playerPos player2;
playerPos player1;

playerPos last1;
playerPos last2;

typedef enum directions{
    UP,
    DOWN,
    LEFT,
    RIGHT
}directions;

directions p1_Dir;
directions p2_Dir; 
directions p1_lastDir;
directions p2_lastDir; 

char actions[4];
int action_idx = 0;
int winner;

void tronRun(){
    sys_clear();
    
    board_init(); 
    winner = 0; //luego de mover va a una funcion que chequea por colision, si hubo unca carga 1 si gano 1 o 2 si gano 2.
    int quit = 0;
    sys_write(STDOUT, 1,"Press backspace to quit the game!",33);
    do{
        sys_read(STDIN, actions, 4);
        for(action_idx = 0; action_idx < 4 && winner == 0 && quit == 0; action_idx++) {
            if(actions[action_idx] != 0x7F) { 
                updatePos();
                refresh();
            } else{
                quit = 1;
            }
        }
    } while(quit == 0 && winner == 0);
    
    sys_clear();
    
    if(winner != 0) {
        sys_write(STDOUT, 1, "Player ", 7);
        sys_write(STDOUT, 1, winner == 1? "1 ":"2 ", 2);
        sys_write(STDOUT, 1, "won.", 4);
    }else{
        sys_write(STDOUT, 2,"NO ONE WON!!1!",14);
    }
    sys_beep();
    sys_write(STDOUT, 3,"\n\n\n\nPress P to play again or press Q to go back to the shell",60);
    char c = 0;
    do{
        sys_read(STDIN,&c,1);
    }while(c == 0);
    if(c == 'P'){
        tronRun();
    }
    
    sys_clear();
}

void updatePos() {
    p1_lastDir = p1_Dir;
    p2_lastDir = p2_Dir;
    last1.board_x = player1.board_x;
    last1.board_y = player1.board_y;
    last2.board_x = player2.board_x;
    last2.board_y = player2.board_y;
    
    getDirs();
    winner = checkWinner();
    if(winner != 0)
        return;
    switch(p1_Dir) {
        case UP:
            if(p1_lastDir != DOWN)
                player1.board_y --;
            break;
        case DOWN:
            if(p1_lastDir != UP)
                player1.board_y++;
            break;
        case LEFT:
            if(p1_lastDir != RIGHT)
                player1.board_x--;
            break;
        case RIGHT:
            if(p1_lastDir != LEFT)
                player1.board_x++;
            break;
    }
    switch(p2_Dir) {
        case UP:
            if(p2_lastDir != DOWN)
                player2.board_y --;
            break;
        case DOWN:
            if(p2_lastDir != UP)
                player2.board_y++;
            break;
        case LEFT:
            if(p2_lastDir != RIGHT)
                player2.board_x--;
            break;
        case RIGHT:
            if(p2_lastDir != LEFT)
                player2.board_x++;
            break;
    }
     
    updateGrid();
}

void updateGrid() {
    board_grid[player1.board_y][player1.board_x].player = PLAYER1;
    board_grid[player2.board_y][player2.board_x].player = PLAYER2;
}


void board_init() {
    
    //lleno los valoresde los pixeles
    for(int board_y = 0; board_y < BOARD_WIDTH; board_y++) {
        for(int board_x = 0; board_x < BOARD_HEIGHT; board_x++) {
            board_grid[board_y][board_x].x = BOARD_TOPX + AMOUNT * board_x;
            board_grid[board_y][board_x].y = BOARD_TOPY + AMOUNT * board_y; 
            board_grid[board_y][board_x].player =0;
        }
    }

    for(int board_x = 0; board_x < BOARD_WIDTH; board_x++) {
        board_grid[BOARD_HEIGHT-1][board_x].player = -1;
        board_grid[0][board_x].player = -1;
        sys_recto(board_grid[0][board_x].x, board_grid[0][board_x].y, 0XFFFFFF, AMOUNT, AMOUNT);
        sys_recto(board_grid[BOARD_HEIGHT-1][board_x].x, board_grid[BOARD_HEIGHT-1][board_x].y, 0XFFFFFF, AMOUNT, AMOUNT);
    }

    for(int board_y = 0; board_y < BOARD_HEIGHT; board_y++) {
        board_grid[board_y][0].player = -1;
        board_grid[board_y][BOARD_WIDTH-1].player = -1;
        sys_recto(board_grid[board_y][0].x, board_grid[board_y][0].y, 0XFFFFFF, AMOUNT, AMOUNT);
        sys_recto(board_grid[board_y][BOARD_WIDTH-1].x, board_grid[board_y][BOARD_WIDTH-1].y, 0XFFFFFF, AMOUNT, AMOUNT);
    }

    player1.board_x = BOARD_WIDTH/4;
    player1.board_y = BOARD_HEIGHT/2;

    player2.board_x = BOARD_WIDTH*3/4;
    player2.board_y = BOARD_HEIGHT/2;

    updateGrid();

    refresh();

    p1_Dir = RIGHT;
    p2_Dir = LEFT;
}


void getDirs() {
    
        switch(actions[action_idx]){
            case 'W':
                if(p1_lastDir != DOWN)
                    p1_Dir = UP;
                break;
            case 'A':
                if(p1_lastDir != RIGHT)
                   p1_Dir = LEFT;
                break;
            case 'S':
                if(p1_lastDir != UP)
                   p1_Dir = DOWN;
                break;
            case 'D':
                if(p1_lastDir != LEFT)
                   p1_Dir = RIGHT;
                break;
            case 'I':
                if(p2_lastDir != DOWN)
                   p2_Dir = UP;
                break;
            case 'J':
                if(p2_lastDir != RIGHT)
                   p2_Dir = LEFT;
                break;
            case 'K':
                if(p2_lastDir != UP)
                   p2_Dir = DOWN;
                break;
            case 'L':
                if(p2_lastDir != LEFT)
                   p2_Dir = RIGHT;
                break;
            default:
                break;
        }
}


void refresh() {
    sys_recto(board_grid[player1.board_y][player1.board_x].x, board_grid[player1.board_y][player1.board_x].y, P1_COLOR, AMOUNT, AMOUNT);
    sys_recto(board_grid[player2.board_y][player2.board_x].x, board_grid[player2.board_y][player2.board_x].y, P2_COLOR, AMOUNT, AMOUNT);
}

int checkWinner() {
    switch(p1_Dir) {
        case UP:
            if(board_grid[player1.board_y - 1][player1.board_x].player != 0)
                return 2;
            break;
        case DOWN:
            if(board_grid[player1.board_y + 1][player1.board_x].player != 0)
                return 2;
            break;
        case RIGHT:
            if(board_grid[player1.board_y][player1.board_x + 1].player != 0)
                return 2;
            break;
        case LEFT:
            if(board_grid[player1.board_y][player1.board_x - 1].player != 0)
                return 2;
            break;
        default:
            break;
    }
    switch(p2_Dir) {
        case UP:
            if(board_grid[player2.board_y - 1][player2.board_x].player != 0)
                return 1;
            break;
        case DOWN:
            if(board_grid[player2.board_y + 1][player2.board_x].player != 0)
                return 1;
            break;
        case LEFT:
            if(board_grid[player2.board_y][player2.board_x - 1].player != 0)
                return 1;
            break;
        case RIGHT:
            if(board_grid[player2.board_y][player2.board_x + 1].player != 0)
                return 1;
            break;
        default:
            break;
    }
    return 0;
}
