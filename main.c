#include <GL/glut.h>
#include <math.h>
#include "model.h"
#include "control.h"
#include "objects.h"


#define DEFAULTZ -18
#define DEFAULTY -8 

struct object objects[256];
struct object *cube = &objects[0];

char coll = 0;

float buffer[3];
float lbuf[3], rbuf[3], sbuf[3];

float buf[SIZE*6];
float *wbuf[3], *wbuf1[3];

//camera
float xloc,yloc,zloc,anglex,angley;

int seed = 0xcafe;
float rand_color(){
	seed^=seed<<7;
	seed^=seed>>9;
	seed^=seed<<8;
	return seed/0xffff;
}
void generate_normal(float u1, float u2, float u3, 
	float v1, float v2, float v3, 
	float w1, float w2, float w3, 
	float *out) {
	float a[3], b[3];
	a[0] = v1 - u1;
	a[1] = v2 - u2;
	a[2] = v3 - u3;

	b[0] = w1 - u1;
	b[1] = w2 - u2;
	b[2] = w3 - u3;

	out[0] = a[1] * b[2] - a[2] * b[1];
	out[1] = a[2] * b[0] - a[0] * b[2];
	out[2] = a[0] * b[1] - a[1] * b[0];

	float len = sqrt(out[0]*out[0]+out[1]*out[1]+out[2]*out[2]);
	if (len > 0) {
		out[0] /= len;
		out[1] /= len;
		out[2] /= len;
	}
}
	
void setup_light() {
	//setup lights
	float lightpos[] = {0, 0, 1, 0};
	float lightcolor[] = {0.1, 0.1, 0.1};
	float lightspec[] = {1, 1, 1};
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightcolor);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightspec);
}

void clear_emission() {
	float color0[] = {0,0,0};
	glMaterialfv(GL_FRONT, GL_EMISSION, color0);
}

void set_material(int mode) {
	float matcolor[4], matspecular[3]={1,1,1}, matshininess[1]={128}, matdefaultamb[4]={0.2,0.2,0.2,1}; 
	glMaterialfv(GL_FRONT, GL_AMBIENT, matdefaultamb);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matspecular);
	glMaterialfv(GL_FRONT, GL_SHININESS, matshininess);
	switch (mode) {
		case WATER:
			matcolor[0]=0; matcolor[1]=0.05; matcolor[2]=0.2; matcolor[3]=0.5;
			glMaterialfv(GL_FRONT, GL_DIFFUSE, matcolor);
			break;
		case SYRUP:
			matcolor[0]=0.2; matcolor[1]=0.1; matcolor[2]=0; matcolor[3]=0.5;
			glMaterialfv(GL_FRONT, GL_DIFFUSE, matcolor);
			break;
		case CUBE:
			matcolor[0]=0; matcolor[1]=0.1; matcolor[2]=0.05; matcolor[3]=0.8;
			glMaterialfv(GL_FRONT, GL_DIFFUSE, matcolor);
			break;
		default:
			matcolor[0]=1; matcolor[1]=1; matcolor[2]=1; matcolor[3]=1;
			glMaterialfv(GL_FRONT, GL_DIFFUSE, matcolor);
			glMaterialfv(GL_FRONT, GL_AMBIENT, matcolor);
			break;
	}
}

//axis
void draw_ax() {
	glBegin(GL_LINES);
	glVertex3f(0,0,0);
	glVertex3f(0,0.5,0);
	glEnd();
}

void set_emission_rgb(unsigned char c) {
	if (!c) {
		float color[] = {1.0, 0, 0};
		glMaterialfv(GL_FRONT, GL_EMISSION, color);
	}
	else if (c == 1) {
		float color[] = {0, 1.0, 0};
		glMaterialfv(GL_FRONT, GL_EMISSION, color);
	}
	else if (c == 2) {
		float color[] = {0, 0, 1.0};
		glMaterialfv(GL_FRONT, GL_EMISSION, color);
	}
	else return;
}

void draw_axis(){
	glPushMatrix();
	glTranslatef(1,1,-4);
	glRotatef(anglex, 1, 0, 0);
	glRotatef(angley, 0, 1, 0);
	
	set_emission_rgb(0);
	glPushMatrix();
	draw_ax();
	glPopMatrix();
	set_emission_rgb(1);
	glPushMatrix();
	glRotatef(90, 1, 0, 0);
	draw_ax();
	glPopMatrix();
	set_emission_rgb(2);
	glPushMatrix();
	glRotatef(-90, 0, 0, 1);
	draw_ax();
	glPopMatrix();

	glPopMatrix();

	clear_emission();
}

//draw objects
void draw_cube(struct object *obj){
	int i,j,idx;
	glPushMatrix();
	glTranslatef(obj->loc[0],obj->loc[1],obj->loc[2]);
	glScalef(obj->sca[0],obj->sca[1],obj->sca[2]);
	glRotatef(obj->rot[0],1,0,0);
	glRotatef(obj->rot[1],0,1,0);
	glRotatef(obj->rot[2],0,0,1);
	glBegin(GL_QUADS);
	idx=0;
	for(i=0;i<6*3;i+=3){
		glNormal3fv(cube_normals+i);
		for(j=idx;j<idx+12;j+=3) glVertex3iv(cube_vertices+j);
		idx+=12;
	}
	glEnd();
	glPopMatrix();
}

void draw_water(struct object *obj){
	int i,j,idx0,idx1;
	glPushMatrix();
	glTranslatef(obj->loc[0],obj->loc[1],obj->loc[2]);
	glScalef(obj->sca[0],obj->sca[1],obj->sca[2]);
	glRotatef(obj->rot[0],1,0,0);
	glRotatef(obj->rot[1],0,1,0);
	glRotatef(obj->rot[2],0,0,1);
	
	glBegin(GL_TRIANGLES);
	idx0=0;
	for(i=0;i<H-1;i++){
		idx1=idx0;
		for(j=0;j<W-1;j++){
			//1st triangle
			generate_normal(j,obj->ybuf[1][idx1],i,
				j,obj->ybuf[1][idx1+W],i+1,
				j+1,obj->ybuf[1][idx1+1],i, buffer);
			
			glNormal3fv(buffer);
			glVertex3f(j,obj->ybuf[1][idx1],i);
			glVertex3f(j,obj->ybuf[1][idx1+W],i+1);
			glVertex3f(j+1,obj->ybuf[1][idx1+1],i);
			
			//2nd triangle
			generate_normal(j+1,obj->ybuf[1][idx1+1],i,
				j,obj->ybuf[1][idx1+W],i+1,
				j+1,obj->ybuf[1][idx1+1+W],i+1, buffer);
			glNormal3fv(buffer);
			glVertex3f(j+1,obj->ybuf[1][idx1+1],i);
			glVertex3f(j,obj->ybuf[1][idx1+W],i+1);
			glVertex3f(j+1,obj->ybuf[1][idx1+1+W],i+1);
			idx1++;
		}
		idx0+=W;
	}
	glEnd();
		
	glPopMatrix();
}

void draw_default(struct object *obj){
	int i;
	int vert[12]={0,0,0, W-1,0,0, W-1,0,H-1, 0,0,H-1};
	glPushMatrix();
	glTranslatef(obj->loc[0],obj->loc[1],obj->loc[2]);
	glScalef(obj->sca[0],obj->sca[1],obj->sca[2]);
	glRotatef(obj->rot[0],1,0,0);
	glRotatef(obj->rot[1],0,1,0);
	glRotatef(obj->rot[2],0,0,1);
	glBegin(GL_QUADS);
	glNormal3fv(&cube_normals[60]);
	for(i=0;i<12;i+=3)glVertex3iv(&vert[i]);
	glEnd();
	glPopMatrix();
}

void draw_objects() {
	int i,head;
	struct object *obj;

	head = get_head(); // ;-)
	for(i=0;i<head;i++){
		obj = &objects[i];
		set_material(obj->type);
		switch(obj->type){
			case CUBE:
				draw_cube(obj);
				break;
			case WATER:
			case SYRUP:
				draw_water(obj);
				break;
			case PLANE:
			default:
				draw_default(obj);
				break;
		}
	}
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	//gluLookAt(0, 0, 5, 0, 0, 0, 0, 1, 0);
	draw_axis();

	glPushMatrix();
	glRotatef(anglex, 1, 0, 0);
	glRotatef(angley, 0, 1, 0);
	glTranslatef(xloc,yloc,zloc);
	//glTranslatef(0,yloc,0);
	
	draw_objects();
	

	glPopMatrix();

	glutSwapBuffers();
}

void init_buffers(){
	int i;
	for(i=0;i<3;i++){
		wbuf[i]=buf+i*SIZE;
		wbuf1[i]=buf+(i+3)*SIZE;
	}
}

void init_objects(){
	rbuf[0]=rbuf[0]=rbuf[2]=0;
	sbuf[0]=sbuf[1]=sbuf[2]=1;
	
	lbuf[0]=-0.5f;
	lbuf[1]=4;
	lbuf[2]=-0.5f;
	add_object(lbuf,rbuf,sbuf,CUBE,NULL);
	
	lbuf[0]=-W2;

	lbuf[1]=0;
	lbuf[2]=-H2;
	add_object(lbuf,rbuf,sbuf,WATER,wbuf);

	lbuf[1]=0;
	lbuf[2]=-(H1+H2);
	add_object(lbuf,rbuf,sbuf,SYRUP,wbuf1);

	lbuf[1]=-8;
	lbuf[2]=-H2;
	add_object(lbuf,rbuf,sbuf,PLANE,NULL);
	
	lbuf[1]=-8;
	lbuf[2]=-(H1+H2);
	add_object(lbuf,rbuf,sbuf,PLANE,NULL);

	lbuf[1]=0;
	lbuf[2]=-((H1<<1)+H2);
	add_object(lbuf,rbuf,sbuf,PLANE,NULL);
	
	lbuf[1]=1;
	lbuf[2]=-(H1+(H1<<1)+H2);
	add_object(lbuf,rbuf,sbuf,PLANE,NULL);

	sbuf[0]=sbuf[2]=2;
	lbuf[1]=1;
	lbuf[2]=-(H1+(H1<<2)+H2);
	add_object(lbuf,rbuf,sbuf,PLANE,NULL);
	sbuf[0]=1;
	lbuf[1]=3;
	lbuf[2]=-((H1<<1)+(H1<<2)+H2);
	add_object(lbuf,rbuf,sbuf,PLANE,NULL);

	xloc=0;
	yloc=DEFAULTY;
	zloc=DEFAULTZ;
	anglex=15.0f;
	//angley=30.0f;
	angley=0.0f;
}

void init() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glLoadIdentity();
	gluPerspective(45, 1, 0.1f, 140.0f);
	//gluLookAt(1,0,1,0,0,0,0,1,0);
	
	setup_light();
	init_buffers();
	init_objects();
}

void resize(int w, int h) {
	float ratio = (float)w/(float)h;
	glViewport(0, 0, w, h);
	glLoadIdentity();
	gluPerspective(45, ratio, 0.1f, 140.0f);
}

void reset(){
	cube->loc[0]=-0.5f;
	cube->loc[1]=4;
	cube->loc[2]=-0.5f;
	xloc=0;
	yloc=DEFAULTY;
	zloc=DEFAULTZ;
	anglex=15.0f;
	angley=0.0f;
}

void update(int value) {
	int i;
	float *p;
	check_keys();
	/*do_wave_coll(&objects[1]);
	do_wave_coll(&objects[2]);*/
	
	collision_all();
	waves(&objects[1],W,SIZE);
	waves(&objects[2],W,SIZE);

	p = wbuf[0];
	wbuf[0] = wbuf[1];
	wbuf[1] = wbuf[2];
	wbuf[2] = p;
	p = wbuf1[0];
	wbuf1[0] = wbuf1[1];
	wbuf1[1] = wbuf1[2];
	wbuf1[2] = p;

	if(cube->loc[1]<=-0x20){
		reset();
	}

	glutPostRedisplay();
	glutTimerFunc(25, update, 0);
}


int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(400, 300);
	glutCreateWindow("Wave Demo");

	init();

	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutKeyboardFunc(key_down);
	glutKeyboardUpFunc(key_up);
	//glutSpecialFunc(special);
	
	glutTimerFunc(33, update, 0);
	//glutCloseFunc(close);
	glutMainLoop();
}
