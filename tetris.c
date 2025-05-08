#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <stdbool.h>
#include <fcntl.h>
#include <time.h>

#define screen_witdh 10
#define screen_height 18
/*Hola no se como se hacen las cosas con github (git clone)*/
/*

- [Check] Borrar lineas bien, a veces no se borran, a veces se borran de mas y a veces no son lasa correctas
- [Check] A veces las piezas pueden moverse despued de borrar las lineas
- [Check] A veces no spawnea nueva pieza

- Actualizar sistema de rotacion
- Color de piezas
- Buffer time al caer
- Refactorizar el codigo
- Puntuación
-Velocidades

-!!!  l475 shadpw usar temp, cambiar dos
*/
struct cube
{
  int x;
  int y;
};

int score = 0;

float centerx, centery;

int screen[screen_witdh][screen_height];
int color[screen_witdh][screen_height];
/*
0: null
1: red
2: blue
3: orange
4: green
5: yellow
6: indigo
7: pink
*/

char input = '0', rotationinput = '0'; int moveinput = 0;

bool gameover = false;
bool piecedown = false;
int framerule = 0, rotationbuffer = 0, fallbuffer = 0;
int pieceid;

float speed;

struct cube piece[4];

void sort(int arr[4])
{
  for  (int j = 0; j < 3; j++)
  {
  for (int i = j; i < 3; i++)
  {
    if(arr[i] > arr[i+1])
    {
      int temp = arr[i];
      arr[i] = arr[i+1];
      arr[i+1] = temp;
    }
  }
  }
}

void removeDuplicates(int arr[4]) {
    for (int i = 0; i < 3; i++)
    {
      if(arr[i] == arr[i+1])
      {
        arr[i] = -1;
      }
    }
}

void setcolor(){
  for (int i = 0; i < 4; i++)
  {
    color[piece[i].x][piece[i].y] = pieceid;
  }
}

void printscreen(int screen[screen_witdh][screen_height])
{
  usleep(1000*speed);
  printf("\x1b[H");
  printf("\x1b[2J");
  for (int j = 0; j < screen_height; j++)
  {
    putchar(124);

    for (int i = 0; i < screen_witdh; i++)
    {
      switch (color[i][j])
      {
      case 1:
        printf("\033[38;2;255;0;0m");
        break;
      case -1:
        printf("\033[38;2;80;0;0m");
        break;
      case 2:
        printf("\033[38;2;0;0;255m");
        break;
      case -2:
        printf("\033[38;2;0;0;80m");
        break;
      case 3:
        printf("\033[38;2;220;0;220m");
        break;
      case -3:
        printf("\033[38;2;100;0;100m");
        break;
      case 4:
        printf("\033[38;2;0;200;0m");
        break;
      case -4:
        printf("\033[38;2;0;80;0m");
        break;
      case 5:
        printf("\033[38;2;220;220;0m");
        break;
      case -5:
        printf("\033[38;2;70;70;0m");
        break;
      case 6:
        printf("\033[38;2;240;120;100m");
        break;
      case -6:
        printf("\033[38;2;100;50;70m");
        break;
      case 7:
        printf("\033[38;2;220;100;100m");
        break;
      case -7:
        printf("\033[38;2;70;30;30m");
        break;
      default:
        break;
      }
    putchar(screen[i][j] == 0 ? 32 : 91);
    putchar(screen[i][j] == 0 ? 32 : 93);
    printf("\033[0m");
    }

    putchar(124);
    putchar(10);

  }
  
  for (int i = 0; i < 2*screen_witdh+2; i++){
    printf("\u00AF");
  }
  printf("\nSCORE %d", score);
}

char getch() {
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    if (ch == EOF)
    {
      return '0';
    }
    else{return ch;}
}

void non_blocking_mode(int enable) {
    struct termios ttystate;
    int flags;

    tcgetattr(STDIN_FILENO, &ttystate);

    if (enable) {
        ttystate.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);

        flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
    } else {
        ttystate.c_lflag |= (ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);

        flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, flags & ~O_NONBLOCK);
    }
}

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != EOF && c != '\n'); // Read all characters until buffer is empty
}

int spawn(int id)
{
  int center = screen_witdh/2;
    struct cube tpiece[4];
    switch (id)
    {
    case 1://line
      {
      centerx = center + 1/2;
      centery = 3/2;
      int positionsx[] = {0,-1,1,2};
      int positionsy[] = {1,1,1,1};
      for (int i = 0; i < 4; i++) {
        tpiece[i].x = positionsx[i] + center;
        tpiece[i].y = positionsy[i];
      }
      break;
      }
    case 2://cube
      {
      centerx = center + 1/2;
      centery = 1/2;
      int positionsx[] = {0,1,0,1};
      int positionsy[] = {0,0,1,1};
      for (int i = 0; i < 4; i++) {
        tpiece[i].x = positionsx[i] + center;
        tpiece[i].y = positionsy[i];
      }
      break;
      }
    case 3://t
      {
      centerx = center + 0;
      centery = 0;
      int positionsx[] = {0,-1,0,1};
      int positionsy[] = {1,0,0,0};
      for (int i = 0; i < 4; i++) {
        tpiece[i].x = positionsx[i] + center;
        tpiece[i].y = positionsy[i];
      }
      break;
      }
    case 4://s
      {
      centerx = center + 0;
      centery = 1;
      int positionsx[] = {0,0,-1,1};
      int positionsy[] = {0,1,0,1};
      for (int i = 0; i < 4; i++) {
        tpiece[i].x = positionsx[i] + center;
        tpiece[i].y = positionsy[i];
      }
      break;
      }
    case 5://z
      {
      centerx = center + 0;
      centery = 1;
      int positionsx[] = {0,0,-1,1};
      int positionsy[] = {1,0,1,0};
      for (int i = 0; i < 4; i++) {
        tpiece[i].x = positionsx[i] + center;
        tpiece[i].y = positionsy[i];
      }
      break;
      }
    case 6://l
      {
      centerx = center + 0;
      centery = 0;
      int positionsx[] = {0,-1,-1,1};
      int positionsy[] = {0,1,0,0};
      for (int i = 0; i < 4; i++) {
        tpiece[i].x = positionsx[i] + center;
        tpiece[i].y = positionsy[i];
      }
      break;
      }
    case 7://rev_l
      {
      centerx = center + 0;
      centery = 0;
      int positionsx[] = {0,1,-1,1};
      int positionsy[] = {0,1,0,0};
      for (int i = 0; i < 4; i++) {
        tpiece[i].x = positionsx[i] + center;
        tpiece[i].y = positionsy[i];
      }
      break;
      }
    default:
    return 0;
      break;
    }
    if (screen[tpiece[0].x][tpiece[0].y] == 0 && screen[tpiece[1].x][tpiece[1].y] == 0 && screen[tpiece[2].x][tpiece[2].y] == 0 && screen[tpiece[3].x][tpiece[3].y] == 0)
    {
      for(int i = 0; i < 4; i++)
      {
        piece[i].x = tpiece[i].x;
        piece[i].y = tpiece[i].y;
      }
    screen[piece[0].x][piece[0].y] = 2;
    screen[piece[1].x][piece[1].y] = 2;
    screen[piece[2].x][piece[2].y] = 2;
    screen[piece[3].x][piece[3].y] = 2;
    }else {gameover = true;}
}

int islinecomplete(int completed_lines[4]){
  bool delete = true;
  for (int i = 0; i < 4; i++)
  {
    completed_lines[i] = piece[i].y;
  }

  for (int i = 0; i < 4; i++)
  {
    int j = 0;
    while (delete && j < screen_witdh)
    {
      if (screen[j][completed_lines[i]] == 0)
      {
        delete = false;
      }
      j++;
    }
    if (!delete)
    {
      completed_lines[i] = -1;
    }
    delete = true;
  }

  
  return 0;
}

int deletelines(int completed_lines[4]){
  int scorecount = 0;
  int temp_screen[screen_witdh][screen_height];
  bool deleted = false;
  sort(completed_lines);
  removeDuplicates(completed_lines);
  sort(completed_lines);
  for (int i = 0; i < screen_height; i++)
  {
    for (int j = 0; j < screen_witdh; j++)
    {
      temp_screen[j][i] = screen[j][i];
    }
  }
  for (int i = 0;  i < 4; i++)
  {
    if(completed_lines[i] != -1)
    {
      scorecount ++;
      deleted = true;
      for(int j = 0; j < screen_witdh; j ++)
      {
        screen[j][completed_lines[i]] = 0;
      }
    }
  }
  if (deleted)
  {
  for (int beep = 0; beep < 2; beep++)
  {
    printscreen(screen);
    usleep(10000*speed);
    printscreen(temp_screen);
    usleep(10000*speed);
  }
  }
  for (int i = 3;  i > -1; i--)
  {
    if(completed_lines[i] != -1)
    {
      for (int j = i; j > 0 ; j--)
      {
        if(completed_lines[j-1] != -1){
        completed_lines[j-1] ++;}
      }
      for(int j = completed_lines[i]; j > 0; j --)
      {
        for (int k = 0; k < screen_witdh; k++)
        {
          screen[k][j] = screen[k][j-1];
        }
        printscreen(screen);
        usleep(100*speed);
      }
      for (int l = 0; l < screen_witdh; l++)
      {
        screen[l][0] = 0;
      }
    }
  }
  score += (scorecount == 1 ? 40 : (scorecount == 2 ? 100 : (scorecount == 3 ? 300 : (scorecount == 4 ? 1200 : 0))));
}

bool collision(int direction){
  bool allow = false;
  switch (direction)
  {
  case 1 ... 2://abajo
    if(screen[piece[0].x][piece[0].y + 1] != 1 && screen[piece[1].x][piece[1].y + 1] != 1 && screen[piece[2].x][piece[2].y + 1] != 1 && screen[piece[3].x][piece[3].y + 1] != 1)
    {
      if(piece[0].y + 1 < screen_height && piece[1].y + 1 < screen_height && piece[2].y + 1 < screen_height && piece[3].y + 1 < screen_height)
      {
      allow = true;
      }
    }
    break;
  case 3://derecha
    if(piece[0].x + 1 < screen_witdh && piece[1].x + 1 < screen_witdh && piece[2].x + 1 < screen_witdh && piece[3].x + 1 < screen_witdh)
    {
      if(screen[piece[0].x + 1][piece[0].y] != 1 && screen[piece[1].x + 1][piece[1].y] != 1 && screen[piece[2].x + 1][piece[2].y] != 1 && screen[piece[3].x + 1][piece[3].y] != 1)
      {
      allow = true;
      }
    }
    break;
  case 4://izquierda
    if(piece[0].x > 0 && piece[1].x > 0 && piece[2].x > 0 && piece[3].x > 0)
    {
      if(screen[piece[0].x - 1][piece[0].y] != 1 && screen[piece[1].x - 1][piece[1].y] != 1 && screen[piece[2].x - 1][piece[2].y] != 1 && screen[piece[3].x - 1][piece[3].y] != 1)
      {
      allow = true;
      }
    }
    break;
  
  default:
    break;
  }
  return allow;
}



bool rotate(char direction){
  if(pieceid == 2)
  {
    return false;
  }
  float temp1, temp2;
  struct cube temp[4];
  int rot = (direction == 'a' ? -1 : 1);
  bool allow = false;

  for (int i = 0; i < 4; i++)
  {
    temp1 = piece[i].x - centerx;
    temp2 = piece[i].y - centery;
    temp[i].x = (centerx - rot*temp2);
    temp[i].y = (centery + rot*temp1);
  }

  if (temp[0].y >= 0 && temp[1].y >= 0 && temp[2].y >= 0 && temp[3].y >= 0 && temp[0].y < screen_height && temp[1].y < screen_height && temp[2].y < screen_height && temp[3].y < screen_height)
  {
    if(temp[0].x >= 0 && temp[1].x >= 0 && temp[2].x >= 0 && temp[3].x >= 0 && temp[0].x < screen_witdh && temp[1].x < screen_witdh && temp[2].x < screen_witdh && temp[3].x < screen_witdh)
    {
      if(screen[temp[0].x][temp[0].y] != 1 && screen[temp[1].x][temp[1].y] != 1 && screen[temp[2].x][temp[2].y] != 1 && screen[temp[3].x][temp[3].y] != 1)
      {
        allow = true;
      }
    }
  }

  if (allow)
  {
    for (int i = 0; i < 4; i++)
    {
      piece[i] = temp[i];
    }
  }
/*
      else
      {
        struct cube pivot[4];
        for (int i = 0; i < 4; i ++)
        {
          pivot[i] = piece[i];
          piece[i] = temp[i];
        }
        if (collision(3))
        {
          for (int i = 0; i < 4; i++)
          {
            piece[i].x++;
          }
          centerx++;
          allow = true;
        }else if (collision(4))
        {
          for (int i = 0; i < 4; i++)
          {
            piece[i].x--;
          }
          centerx--;
          allow = true;
        }else
        {
          for (int i = 0; i < 4; i++)
          {
            piece[i] = pivot[i];
          }
        }
      }
*/
  else
  {
    bool left = true, right = true; 
    for (int i = 0; i < 4; i++)
    {
      if (piece[i].x < 0 || screen[piece[i].x-1][piece[i].y] == 1)
      {
        left = false;
      }
      if (piece[i].x > screen_witdh || screen[piece[i].x+1][piece[i].y] == 1)
      {
        right = false;
      }
    }
    struct cube pivot[4];
    for (int i = 0; i < 4; i ++)
    {
      pivot[i] = piece[i];
      piece[i] = temp[i];
    }
    if (right)
    {
      for (int i = 0; i < 4; i++)
      {
        piece[i].x--;
      }
      centerx--;
      allow = true;
    }
    if (left)
    {
      for (int i = 0; i < 4; i++)
      {
        piece[i].x++;
      }
      centerx++;
      allow = true;
    } 
  }
  
  return allow;
}

void updateshadow(){

  for (int i = 0; i < screen_height; i++)
  {
    for (int j = 0; j < screen_witdh; j++)
    {
      if (screen[j][i] == 3) {screen[j][i] = 0;}
    }
  }

  int tempy[4];
  int iteration = 0;
  bool collision_move = collision(1);
  struct cube pivot[4];
  for (int i = 0; i < 4; i ++)
  {
    tempy[i] = piece[i].y;
    pivot[i] = piece[i];
  }
  while (collision_move)
    {
      iteration++;
      for (int i = 0; i < 4; i ++)
      {
        piece[i].y++;
      }
      for (int i = 0; i < iteration; i++)
      {
        collision_move = collision(1);
        if (!collision){break;}
      }
  }
  for (int i = 0; i < 4; i ++)
  {
    if (screen[piece[i].x][piece[i].y] != 2)
    {
    screen[piece[i].x][piece[i].y] = 3;
    color[piece[i].x][piece[i].y] = -pieceid;
    }
  }
  for (int i = 0; i < 4; i++)
  {
    piece[i].y = pivot[i].y;
  }
}

void updatepiece(char rotation, int move){
  int tempx[4], tempy[4];
  bool fastdrop = false;
  for (int i = 0; i < 4; i ++)
  {
    tempx[i] = piece[i].x;
    tempy[i] = piece[i].y;
  }

  updateshadow();


  if (framerule == 0)
  {
    bool collision_down = collision(2);
    if (collision_down)
     {   
      for (int i = 0; i < 4; i ++)
      {
        piece[i].y++;
      }
      centery++;
    }
  }
  framerule ++;
  framerule = framerule%48;
  bool collision_move = collision(move);

  if ((rotation == 'a'|| rotation == 's') && rotationbuffer == 0)
  {
    rotate(rotation);
    rotationbuffer = 1;
  }
  if (rotationbuffer != 0)
  {
    rotationbuffer ++;
    rotationbuffer = rotationbuffer%12;
  }

  if(collision_move)
  {
  switch (move)
  {
    
  case 1:
    while (collision_move)
    {
      for (int i = 0; i < 4; i ++)
      {
        piece[i].y++;
      }
      centery++;
      collision_move = collision(1);
    }
    fastdrop = true;

    break;
    
  case 2://abajo
  
    for (int i = 0; i < 4; i ++)
    {
      piece[i].y++;     
    }
    centery++;
  
    break;
  case 3://derecha
  
    for (int i = 0; i < 4; i ++)
    {
      piece[i].x++;      
    }
    centerx++;
  
    break;
  case 4://izquierda
  
    for (int i = 0; i < 4; i ++)
    {
      piece[i].x--;
    }
    centerx--;

  default:
    break;
  }
  }

  for (int i = 0; i < 4; i++)
  {
    screen[tempx[i]][tempy[i]] = 0;
    screen [piece[i].x][piece[i].y] = 2;
  }

  if((tempx[0] == piece[0].x && tempy[0] == piece[0].y && tempx[1] == piece[1].x && tempy[1] == piece[1].y && tempx[2] == piece[2].x && tempy[2] == piece[2].y && tempx[3] == piece[3].x && tempy[3] == piece[3].y)||fastdrop)
  {
    if (framerule == 1 || fastdrop)
    {
    piecedown = true;
    for (int i = 0; i < 4; i++)
    {
      screen [piece[i].x][piece[i].y] = 1;
    }
    }
  }
  setcolor();

}

void shuffle(int arr[14])
{
  int j;
  srand(time(0));
  for (int i = 0; i < 14; i++)
  {
    j = rand()%14 + 1;
    int temp = arr[i];
    arr[i] = arr[(i+j)%14 + 1];
    arr[(i+j)%14 + 1] = temp;
  }
  return;
}

void getinput(){
  input = getch();
    if (input == 27)
    {
      if (getch() == '[')
      {
        switch (getch()) {
          case 'A': moveinput = 1; break;//arriba
          case 'B': moveinput = 2; break;//abajo
          case 'C': moveinput = 3; break;//derecha
          case 'D': moveinput = 4; break;//izquierza
        }
      }
    }       
    rotationinput = input;
}

int main () {
    int completed_lines[] = {-1,-1,-1,-1};
    int whole;
    int bag[14] = {1,1,2,2,3,3,4,4,5,5,6,6,7,7}, bagindex = 0;

    for (int j = 0; j < screen_height; j++)
    {
      for (int i = 0; i < screen_witdh; i++)
      {
      screen[i][j] = 0;
      }
    }

    non_blocking_mode(1);

    shuffle(bag);
    pieceid = bag[bagindex];
    bagindex++;
    spawn(pieceid);


    while (!gameover)
    {
      speed = 10 - log(score/100 + 1);
    getinput();

    updatepiece(rotationinput, moveinput);
    input = '0', rotationinput = '0', moveinput = 0;
    clear_input_buffer();

    printscreen(screen);
    
    if (piecedown)
    {
      islinecomplete(completed_lines);
      deletelines(completed_lines);

      for (int i = 0; i < 4; i++)
      {
        completed_lines[i] = piece[i].y;
      }

      if(bagindex >= 14)
      {
        bagindex = 0;
        shuffle(bag);
      }
      pieceid = bag[bagindex];
      bagindex++;
      spawn(pieceid);
      piecedown = false;
    }

  }

  printf("\x1b[2J");
  printf("Game Over");
  non_blocking_mode(0);
  return 0;
}