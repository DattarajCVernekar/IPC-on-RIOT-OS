#include<stdio.h>
#include<stdlib.h>
#include "msg.h"
#include "thread.h"

int board[10] = {2,2,2,2,2,2,2,2,2,2}; //The default value of cells in the tic-tac-toe board
int turn = 1,flag = 0;
int player,comp;
msg_t m,m1;  //The message structures used in the RIOT IPC API
kernel_pid_t pid, pid1,pid2;   //Thread identifiers

void menu(void);
void go(int n);
void start_game(void);
void check_draw(void);
void draw_board(void);
void player_first(void);
void put_X_O(char ch,int pos);
void* winprint(void*);
void* bgmusic(void*);
void* beeps(void*);
void gotoxy(int x,int y)
{
    printf("\033[%d;%dH",y+1,x+1);
}
//Declaration of thread stacks
//KERNEL_CONF_STACKSIZE_MAIN gives the highest stack size in RIOT 
char empty1[KERNEL_CONF_STACKSIZE_MAIN];
char empty2[KERNEL_CONF_STACKSIZE_MAIN];
char empty3[KERNEL_CONF_STACKSIZE_MAIN];
int main(void) //Standard RIOT OS main prototype
{
    //Thread creations
    pid1=thread_create(empty1,sizeof(empty1),PRIORITY_MAIN-1,CREATE_STACKTEST,bgmusic,NULL,"backgroundmusic");
    pid=thread_create(empty2,sizeof(empty2),PRIORITY_MAIN-1,CREATE_STACKTEST,winprint,NULL,"simply");
    pid2=thread_create(empty3,sizeof(empty3),PRIORITY_MAIN-1,CREATE_STACKTEST,beeps,NULL,"pings");
    system("clear");
    menu();
    return 0;

}
//Sound thread for playing music at certain events
void* beeps(void* arg)
{
   while(1)
   {
	msg_receive(&m1);
   	if(m1.content.value==0)
   	{
		system("gnome-terminal -x sh -c \"xdotool getactivewindow windowminimize; aplay draw.wav; exit; bash\"");
        }
   	else if(m1.content.value==1)
   	{
        	system("gnome-terminal -x sh -c \"xdotool getactivewindow windowminimize; aplay lose.wav; exit; bash\"");
   	}
   	else if(m1.content.value==2)
   	{
       		system("gnome-terminal -x sh -c \"xdotool getactivewindow windowminimize; aplay win.wav; exit; bash\"");
   	}
   	else if(m1.content.value==5)
   	{
       		system("gnome-terminal -x sh -c \"xdotool getactivewindow windowminimize; aplay coin.wav; exit; bash\"");
   	}
   }
   return NULL;
}
//Thread for playing background music	
void* bgmusic(void* arg)
{
   system("gnome-terminal -x sh -c \"xdotool getactivewindow windowminimize; aplay a.wav; exit; bash\"");
   return NULL;
}
//Thread for printing result of game
void* winprint(void* arg)
{
    msg_receive(&m);
    int i;
    gotoxy(30,20);
    if(m.content.value==0)
    {
        printf("Computer wins.\n");
	m1.content.value=1;
	msg_try_send(&m1,pid2);
    }
    else if(m.content.value==1)
    {
        printf("Player wins.\n");
	m1.content.value=2;
	msg_try_send(&m1,pid2);
    }
    else if(m.content.value==2)
    {
        printf("Game draw.\n");
	m1.content.value=0;
	msg_try_send(&m1,pid2);
    }
    msg_reply(&m,&m);
    return NULL;
}
//Function that prints the menu at the start    
void menu(void)
{
    int choice;
    system("clear");
    printf("\n--------MENU--------");
    printf("\n1 : Play with X");
    printf("\n2 : Play with O");
    printf("\n3 : Exit");
    printf("\nEnter your choice:>");
    scanf("%d",&choice);
    turn = 1;
    switch (choice)
    {
    case 1:
        player = 1;
        comp = 0;
        player_first();
        break;
    case 2:
        player = 0;
        comp = 1;
        start_game();
        break;
    case 3:
        exit(1);
    default:
        menu();
    }
}
//These functions are used by the computer to decide where to place its next cross or 'O'
int make2(void)
{
    if(board[5] == 2)
        return 5;
    if(board[2] == 2)
        return 2;
    if(board[4] == 2)
        return 4;
    if(board[6] == 2)
        return 6;
    if(board[8] == 2)
        return 8;
    return 0;
}

int make4(void)
{
    if(board[1] == 2)
        return 1;
    if(board[3] == 2)
        return 3;
    if(board[7] == 2)
        return 7;
    if(board[9] == 2)
        return 9;
    return 0;
}
//Checks whether the player or computer can or has won
int posswin(int p)
{
// p==1 then X   p==0  then  O
    int i;
    int check_val;

    if(p == 1)
        check_val = 18;
    else
        check_val = 50;

    i = 1;
    while(i<=9)//row check
    {
        if(board[i] * board[i+1] * board[i+2] == check_val)
        {
            if(board[i] == 2)
                return i;
            if(board[i+1] == 2)
                return i+1;
            if(board[i+2] == 2)
                return i+2;
        }
        i+=3;
    }

    i = 1;
    while(i<=3)//column check
    {
        if(board[i] * board[i+3] * board[i+6] == check_val)
        {
            if(board[i] == 2)
                return i;
            if(board[i+3] == 2)
                return i+3;
            if(board[i+6] == 2)
                return i+6;
        }
        i++;
    }

    if(board[1] * board[5] * board[9] == check_val)
    {
        if(board[1] == 2)
            return 1;
        if(board[5] == 2)
            return 5;
        if(board[9] == 2)
            return 9;
    }

    if(board[3] * board[5] * board[7] == check_val)
    {
        if(board[3] == 2)
            return 3;
        if(board[5] == 2)
            return 5;
        if(board[7] == 2)
            return 7;
    }
    return 0;
}

void go(int n)
{
    if(turn % 2)
        board[n] = 3;
    else
        board[n] = 5;
    turn++;
}
//Function for player's turn
void player_first(void)
{
    int pos;
    check_draw();
    draw_board();
    gotoxy(30,18);
    printf("Your Turn :> ");
    scanf("%d",&pos);
    if(board[pos] != 2)
        player_first();

    if(pos == posswin(player))
    {
        go(pos);
        draw_board();
	m.content.value=1;
        msg_send_receive(&m,&m,pid);
        return;
        exit(0);
    }
    m1.content.value=5;
    msg_try_send(&m1,pid2);
    go(pos);
    draw_board();
    start_game();
}
//Function for computer's turn
void start_game(void)
{
// p==1 then X   p==0  then  O
    if(posswin(comp))
    {
        go(posswin(comp));
        flag = 1;
    }
    else if(posswin(player))
        go(posswin(player));
    else if(make2())
        go(make2());
    else
        go(make4());
    draw_board();

    if(flag)
    {
	m.content.value=0;        
	msg_send_receive(&m,&m,pid);
        return;
    }
    else
        player_first();
}
//Function to check for end of game by draw
void check_draw(void)
{
    if(turn > 9)
    {
        m.content.value=2;
        msg_send_receive(&m,&m,pid);
        return;
        exit(0);
    }
}
//Main display function responsible for printing the board
void draw_board(void)
{
    int j;

    for(j=9; j<17; j++)
    {
        gotoxy(35,j);
        printf("|       |");
    }
    gotoxy(28,11);
    printf("-----------------------");
    gotoxy(28,14);
    printf("-----------------------");

    for(j=1; j<10; j++)
    {
        if(board[j] == 3)
            put_X_O('X',j);
        else if(board[j] == 5)
            put_X_O('O',j);
    }
}

void put_X_O(char ch,int pos)
{
    int m;
    int x = 31, y = 10;

    m = pos;

    if(m > 3)
    {
        while(m > 3)
        {
            y += 3;
            m -= 3;
        }
    }
    if(pos % 3 == 0)
        x += 16;
    else
    {
        pos %= 3;
        pos--;
        while(pos)
        {
            x+=8;
            pos--;
        }
    }
    gotoxy(x,y);
    printf("%c",ch);
}
