#include "labs/shell.h"
#include "labs/vgatext.h"
#include "labs/coroutine.h"
#include "labs/fiber.h"

uint8_t strlen(const char* a){
  uint8_t l = 0, i;
  for(i=0; a[i]!=0; i++)
    l++;
  return l;
}

bool strcmp(const char a[], const char* b){
 uint8_t i;
 if(strlen(a) != strlen(b))
  return false;
 else{
  for(i=0; i<strlen(b); i++){
    if(a[i] != b[i])
      return false;
  }
  return true;
 }
}

void array_init(char* a){
  uint8_t i;
  for(i=0; i<50; i++)
    a[i] = 0;
}

void longtask(shellstate_t& stateinout){
  int i, sum = 0;
  for(i=1; i<100000; i++){
    hoh_debug(i);
    sum += i; 
  }
  stateinout.longtask = false;    //to stop this function from executing throughout
  stateinout.result[0] = '#'; // helps in rendering "Task completed" since we cannot check by the value of stateinout.longtask  
  stateinout.result[1] = sum;
}

//
// initialize shellstate
//

void shell_init(shellstate_t& state){
	state.row = 2;
	state.column = 0; 
	state.ch = 36;
	state.nkeys = 0;
  state.echo = false;
  state.longtask = false;
  state.corlong = false;
  state.fiblong = false;
  state.fibonacci = false;
  state.factorial = false;
  int i;
  for(i=0; i<10; i++)
    state.mult_result[i] = 0;
  state.top = 0;
  state.rear = 0;
  array_init(state.command);
  array_init(state.result);
  state.offset = 0;
}

static void writecharxy(int x, int y, uint8_t c, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);
static void fillrect(int x0, int y0, int x1, int y1, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);
static void drawrect(int x0, int y0, int x1, int y1, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);
static void drawtext(int x,int y, const char* str, int maxw, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);
static void drawnumberinhex(int x,int y, uint32_t number, int maxw, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);
static void drawnumberindec(int x,int y, uint32_t number, int maxw, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);

//
// handle keyboard event.
// key is in scancode format.

void shell_update(uint8_t scankey, shellstate_t& stateinout){
    hoh_debug("Got: "<<unsigned(scankey));
    switch(scankey){
      
      case 0x1c:                      //enter key
        stateinout.row += 2;
        stateinout.column = 0; 
        stateinout.ch = '$';
        stateinout.command[stateinout.offset] = ' ';
        stateinout.offset = 0;
        break;
      case 0x02:
        stateinout.column += 1; 
        stateinout.ch = '1';
        stateinout.command[stateinout.offset] = stateinout.ch; //copies every typed character in the command array 
        stateinout.offset++;
        break;
      case 0x03:
        stateinout.column += 1; 
        stateinout.ch = '2';
        stateinout.command[stateinout.offset] = stateinout.ch;
        stateinout.offset++;
        break;
      case 0x04:
        stateinout.column += 1; 
        stateinout.ch = '3';
        stateinout.command[stateinout.offset] = stateinout.ch;
        stateinout.offset++;
        break;
      case 0x05:
        stateinout.column += 1; 
        stateinout.ch = '4';
        stateinout.command[stateinout.offset] = stateinout.ch;
        stateinout.offset++;
        break;
      case 0x06:
        stateinout.column += 1; 
        stateinout.ch = '5';
        stateinout.command[stateinout.offset] = stateinout.ch;
        stateinout.offset++;
        break;
      case 0x07:
        stateinout.column += 1; 
        stateinout.ch = '6';
        stateinout.command[stateinout.offset] = stateinout.ch;
        stateinout.offset++;
        break;
      case 0x08:
        stateinout.column += 1; 
        stateinout.ch = '7';
        stateinout.command[stateinout.offset] = stateinout.ch;
        stateinout.offset++;
        break;
      case 0x09:
        stateinout.column += 1; 
        stateinout.ch = '8';
        stateinout.command[stateinout.offset] = stateinout.ch;
        stateinout.offset++;
        break;
      case 0x0a:
        stateinout.column += 1; 
        stateinout.ch = '9';
        stateinout.command[stateinout.offset] = stateinout.ch;
        stateinout.offset++;
        break;
      case 0x0b:
        stateinout.column += 1; 
        stateinout.ch = '0';
        stateinout.command[stateinout.offset] = stateinout.ch;
        stateinout.offset++;
        break;
      case 0x10:
        stateinout.column += 1; 
        stateinout.ch = 'q';
        stateinout.command[stateinout.offset] = stateinout.ch;
        stateinout.offset++;
        break;
      case 0x11:
        stateinout.column += 1; 
        stateinout.ch = 'w';
        stateinout.command[stateinout.offset] = stateinout.ch;
        stateinout.offset++;
        break;
      case 0x12:
        stateinout.column += 1; 
        stateinout.ch = 'e';
        stateinout.command[stateinout.offset] = stateinout.ch;
        stateinout.offset++;
        break;
      case 0x13:
        stateinout.column += 1; 
        stateinout.ch = 'r';
        stateinout.command[stateinout.offset] = stateinout.ch;
        stateinout.offset++;
        break;
      case 0x14:
        stateinout.column += 1; 
        stateinout.ch = 't';
        stateinout.command[stateinout.offset] = stateinout.ch;
        stateinout.offset++;
        break;
      case 0x15:
        stateinout.column += 1; 
        stateinout.ch = 'y';
        stateinout.command[stateinout.offset] = stateinout.ch;
        stateinout.offset++;
        break;
      case 0x16:
        stateinout.column += 1; 
        stateinout.ch = 'u';
        stateinout.command[stateinout.offset] = stateinout.ch;
        stateinout.offset++;
        break;
      case 0x17:
        stateinout.column += 1; 
        stateinout.ch = 'i';
        stateinout.command[stateinout.offset] = stateinout.ch;
        stateinout.offset++;
        break;
      case 0x18:
        stateinout.column += 1; 
        stateinout.ch = 'o';
        stateinout.command[stateinout.offset] = stateinout.ch;
        stateinout.offset++;
        break;
      case 0x19:
        stateinout.column += 1; 
        stateinout.ch = 'p';
        stateinout.command[stateinout.offset] = stateinout.ch;
        stateinout.offset++;
        break;
      case 0x1e:
        stateinout.column += 1; 
        stateinout.ch = 'a';
        stateinout.command[stateinout.offset] = stateinout.ch;
        stateinout.offset++;
        break;
      case 0x1f:
        stateinout.column += 1; 
        stateinout.ch = 's';
        stateinout.command[stateinout.offset] = stateinout.ch;
        stateinout.offset++;
        break;
      case 0x20:
        stateinout.column += 1; 
        stateinout.ch = 'd';
        stateinout.command[stateinout.offset] = stateinout.ch;
        stateinout.offset++;
        break;
      case 0x21:
        stateinout.column += 1; 
        stateinout.ch = 'f';
        stateinout.command[stateinout.offset] = stateinout.ch;
        stateinout.offset++;
        break;
      case 0x22:
        stateinout.column += 1; 
        stateinout.ch = 'g';
        stateinout.command[stateinout.offset] = stateinout.ch;
        stateinout.offset++;
        break;
      case 0x23:
        stateinout.column += 1; 
        stateinout.ch = 'h';
        stateinout.command[stateinout.offset] = stateinout.ch;
        stateinout.offset++;
        break;
      case 0x24:
        stateinout.column += 1; 
        stateinout.ch = 'j';
        stateinout.command[stateinout.offset] = stateinout.ch;
        stateinout.offset++;
        break;
      case 0x25:
        stateinout.column += 1; 
        stateinout.ch = 'k';
        stateinout.command[stateinout.offset] = stateinout.ch;
        stateinout.offset++;
        break;
      case 0x26:
        stateinout.column += 1; 
        stateinout.ch = 'l';
        stateinout.command[stateinout.offset] = stateinout.ch;
        stateinout.offset++;
        break;
      case 0x2c:
        stateinout.column += 1; 
        stateinout.ch = 'z';
        stateinout.command[stateinout.offset] = stateinout.ch;
        stateinout.offset++;
        break;
      case 0x2d:
        stateinout.column += 1; 
        stateinout.ch = 'x';
        stateinout.command[stateinout.offset] = stateinout.ch;
        stateinout.offset++;
        break;
      case 0x2e:
        stateinout.column += 1; 
        stateinout.ch = 'c';
        stateinout.command[stateinout.offset] = stateinout.ch;
        stateinout.offset++;
        break;
      case 0x2f:
        stateinout.column += 1; 
        stateinout.ch = 'v';
        stateinout.command[stateinout.offset] = stateinout.ch;
        stateinout.offset++;
        break;
      case 0x30:
        stateinout.column += 1; 
        stateinout.ch = 'b';
        stateinout.command[stateinout.offset] = stateinout.ch;
        stateinout.offset++;
        break;
      case 0x31:
        stateinout.column += 1; 
        stateinout.ch = 'n';
        stateinout.command[stateinout.offset] = stateinout.ch;
        stateinout.offset++;
        break;
      case 0x32:
        stateinout.column += 1; 
        stateinout.ch = 'm';
        stateinout.command[stateinout.offset] = stateinout.ch;
        stateinout.offset++;
        break;
      case 0x39:
        stateinout.column += 1; 
        stateinout.ch = ' ';
        stateinout.command[stateinout.offset] = stateinout.ch;
        stateinout.offset++;
        break;
    }
    stateinout.result[stateinout.offset] = 0; //changes result[0] from # to NULL so that 'Task completed' is not rendered everytime after longtask is called even once
    stateinout.nkeys++;   
}

//
// do computation
//
void shell_step(shellstate_t& stateinout){
  stateinout.echo = false;
  char function[50]; uint8_t i,j;
  for(i=0; i<50; i++)
    function[i]=0;

  if(stateinout.ch == 36){
    for(i=0; i<50; i++){
      if(stateinout.command[i] != ' ')
        function[i] = stateinout.command[i]; //stores the first word of the typed sentence 
      else
        break;
    }
    while(stateinout.command[i] == ' ') //bypasses extra spaces
      i++;

    for(j=i; stateinout.command[j] != 0; j++) //stores the remaining part of the typed sentence
      stateinout.result[j-i] = stateinout.command[j];

    array_init(stateinout.command); //clears the command array for storing next sentence
  }
  if(strcmp(function,"echo\0"))
    stateinout.echo = true;
  else if(strcmp(function,"longtask\0"))
    stateinout.longtask = true;
  else if(strcmp(function,"corlong\0"))
    stateinout.corlong = true;
  else if(strcmp(function,"fiblong\0"))
    stateinout.fiblong = true;

  else if(strcmp(function,"fibo\0")){
    for(i=0; i<10; i++){
      if(stateinout.mult_result[i] == 0)
        break;
    }
    stateinout.fibonacci = true;
    stateinout.rear = i;
    stateinout.mult_result[i] = '#';
    for(i=0;i<10;i++){
        stateinout.top = (stateinout.top + 1) % 10;
        if(stateinout.mult_result[stateinout.top] != 0){
          break;
        }
      }
    stateinout.top = i;
  }

  else if(strcmp(function,"fact\0")){
    for(i=0; i<10; i++){
      if(stateinout.mult_result[i] == 0)
        break;
    }
    stateinout.factorial = true;
    stateinout.rear = i;
    stateinout.mult_result[i] = '$';
    for(i=0;i<10;i++){
        stateinout.top = (stateinout.top + 1) % 10;
        if(stateinout.mult_result[stateinout.top] != 0){
          break;
        }
      }
    stateinout.top = i;
  }

  if(stateinout.longtask == true)
    longtask(stateinout);
}


//
// shellstate --> renderstate
//
void shell_render(const shellstate_t& shell, renderstate_t& render){
	render.row = shell.row;
	render.column = shell.column;
	render.ch = shell.ch;
	render.nkeys = shell.nkeys;
  render.echo = shell.echo;
  render.longtask = shell.longtask;
  render.offset = shell.offset;
  for(int i = 0; i<50;i++){
    render.command[i] = shell.command[i];
    render.result[i] = shell.result[i];
  }
}


//
// compare a and b
//
bool render_eq(const renderstate_t& a, const renderstate_t& b){
  if ((a.row == b.row) && (a.column == b.column) && (a.ch == b.ch) && (a.nkeys == b.nkeys) && (a.echo == b.echo) && (a.offset == b.offset) && (a.longtask == b.longtask) && (a.corlong == b.corlong) && (a.fiblong == b.fiblong))
    {
      for(int i=0;i<50;i++){
        if ((a.command[i]!=b.command[i]) || (a.result[i]!= b.result[i]))
          return false;
      }
      return true;
    }
  else
    return false;
}

//
// Given a render state, we need to write it into vgatext buffer
//
void render(const renderstate_t& state, int w, int h, addr_t vgatext_base){
  if((state.echo == true) && (state.ch == 36))
    drawtext(state.column, state.row - 1, state.result, 50, 0, 7, w, h, vgatext_base);
  else if((state.result[0] == '#') && (state.ch == 36))
    drawtext(state.column, state.row - 1, state.result, 50, 0, 7, w, h, vgatext_base);

  const char* p = "No. of keys pressed:";
  drawtext(40, 1, p, 20, 0, 7, w, h, vgatext_base);
  drawnumberindec(60, 1, state.nkeys, 8, 0, 7, w, h, vgatext_base);
  writecharxy(state.column, state.row, state.ch, 0, 7, w, h, vgatext_base);
}

//
//
// helper functions
//
//

static void writecharxy(int x, int y, uint8_t c, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base){
  vgatext::writechar(y*w+x,c,bg,fg,vgatext_base);
}

static void fillrect(int x0, int y0, int x1, int y1, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base){
  for(int y=y0;y<y1;y++){
    for(int x=x0;x<x1;x++){
      writecharxy(x,y,0,bg,fg,w,h,vgatext_base);
    }
  }
}

static void drawrect(int x0, int y0, int x1, int y1, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base){

  writecharxy(x0,  y0,  0xc9, bg,fg, w,h,vgatext_base);
  writecharxy(x1-1,y0,  0xbb, bg,fg, w,h,vgatext_base);
  writecharxy(x0,  y1-1,0xc8, bg,fg, w,h,vgatext_base);
  writecharxy(x1-1,y1-1,0xbc, bg,fg, w,h,vgatext_base);

  for(int x=x0+1; x+1 < x1; x++){
    writecharxy(x,y0, 0xcd, bg,fg, w,h,vgatext_base);
  }

  for(int x=x0+1; x+1 < x1; x++){
    writecharxy(x,y1-1, 0xcd, bg,fg, w,h,vgatext_base);
  }

  for(int y=y0+1; y+1 < y1; y++){
    writecharxy(x0,y, 0xba, bg,fg, w,h,vgatext_base);
  }

  for(int y=y0+1; y+1 < y1; y++){
    writecharxy(x1-1,y, 0xba, bg,fg, w,h,vgatext_base);
  }
}

static void drawtext(int x,int y, const char* str, int maxw, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base){
  for(int i=0;i<maxw;i++){
    writecharxy(x+i,y,str[i],bg,fg,w,h,vgatext_base);
    if(!str[i]){
      break;
    }
  }
}

static void drawnumberinhex(int x,int y, uint32_t number, int maxw, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base){
  enum {max=sizeof(uint32_t)*2+1};
  char a[max];
  for(int i=0;i<max-1;i++){
    a[max-1-i-1]=hex2char(number%16);
    number=number/16;
  }
  a[max-1]='\0';

  drawtext(x,y,a,maxw,bg,fg,w,h,vgatext_base);
}

static void drawnumberindec(int x,int y, uint32_t number, int maxw, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base){
  enum {max=sizeof(uint32_t)*2+1};
  char a[max];
  for(int i=0;i<max-1;i++){
    a[max-1-i-1]=dec2char(number%10);
    number=number/10;
  }
  a[max-1]='\0';

  drawtext(x,y,a,maxw,bg,fg,w,h,vgatext_base);
}

