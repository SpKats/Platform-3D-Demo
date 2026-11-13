#include <stdlib.h>

#include "control.h"
#include "model.h"

extern float xloc, yloc, zloc, anglex, angley;

char ascii[256];


void key_down(unsigned char c, int x, int y) {
	ascii[c]=1;
}
void key_up(unsigned char c, int x, int y) {
	ascii[c]=0;
}
void check_keys() {
	float loc[3];
	loc[0]=loc[1]=loc[2]=0;

	if(ascii['q']) exit(0);
	/*if(ascii['w'])zloc++;
	if(ascii['s'])zloc--;
	if(ascii['a'])xloc++;
	if(ascii['d'])xloc--;
	if(ascii['z'])yloc++;
	if(ascii['x'])yloc--;*/

	if(ascii['i'])anglex+=1;
	if(ascii['k'])anglex-=1;
	if(ascii['j'])angley+=1;
	if(ascii['l'])angley-=1;

	/*if(ascii['8']){loc[2]=-0.25f; zloc+=0.25f;}
	if(ascii['2']){loc[2]=0.25f; zloc-=0.25f;}
	if(ascii['4'])loc[0]=-0.25f;
	if(ascii['6'])loc[0]=0.25f;
	if(ascii['7'])loc[1]=-0.25f;
	if(ascii['9'])loc[1]=0.25f;*/
	
	if(ascii['w'])loc[2]=-0.25f;
	if(ascii['s'])loc[2]=0.25f;
	if(ascii['a'])loc[0]=-0.25f;
	if(ascii['d'])loc[0]=0.25f;
	if(ascii['z'])loc[1]=-0.25f;
	if(ascii['x'])loc[1]=0.25f;
	
	if(ascii['+'])yloc-=0.25;
	if(ascii['-'])yloc+=0.25;
	//if(ascii[' ']) do_wave(&objects[1], W2+H2*W, 4);
	if(ascii[' ']|ascii['x']) jump();
	
	move_cube(loc);
}

void special(int c, int x, int y) {
        /*if (c == GLUT_KEY_LEFT && !scrnmode) angley += 4.0f;
        else if (c == GLUT_KEY_RIGHT && !scrnmode) angley -= 4.0f;
        else if (c == GLUT_KEY_UP && anglex <= 45) anglex += 4.0f;
        else if (c == GLUT_KEY_DOWN && anglex >= -45) anglex -= 4.0f;
	*/
}

