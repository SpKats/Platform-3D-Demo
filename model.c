#include <stdio.h>
#include "model.h"

extern float xloc,yloc,zloc;

int idx = 0;
float c_arr[3] = {0.4, 0.05, 0.2};
float damp_arr[3] = {0.98, 0.92, 0.98};
char flag = 0;
float limx[2], limy[2];


void waves(struct object *obj, int w, int size) {
        int i,j;
        float ds, dt, c, damp;
	float **buf = obj->ybuf;
	
	c = c_arr[obj->type-WATER];
	damp = damp_arr[obj->type-WATER];
	
        for(i=0;i<size;i+=w) buf[1][i]=buf[1][i+1];
        for(i=w-1;i<size;i+=w) buf[1][i]=buf[1][i-1];
        for(j=0;j<w;j++) buf[1][j]=buf[1][j+w];
        for(j=size-w;j<size;j++) buf[1][j]=buf[1][j-w];

        for(i=w;i<size-w;i+=w) {
                for(j=i+1;j<i+w-1;j++) {
                        ds = (buf[1][j]*4) - buf[1][j-1] - buf[1][j+1] - buf[1][j-w] - buf[1][j+w];
                        dt = buf[1][j] - buf[0][j];
                        buf[2][j] = buf[1][j] + dt - (ds * c);
                        buf[2][j] *= damp;
                }
        }
}

void add_object(float *loc, float *rot, float *sca, enum obj_type type, float **ybuf) {
	struct object *obj = &objects[idx];
	obj->loc[0] = loc[0];//*sca[0];
	obj->loc[1] = loc[1];//*sca[1];
	obj->loc[2] = loc[2];//*sca[2];
	obj->rot[0] = rot[0];
	obj->rot[1] = rot[1];
	obj->rot[2] = rot[2];
	obj->sca[0] = sca[0];
	obj->sca[1] = sca[1];
	obj->sca[2] = sca[2];
	obj->type = type;
	obj->ybuf = ybuf;
	obj->enabled = 1;
	obj->flag = 0;
	obj->grav = 0;
	idx++;
}

void remove_object(int id) {
	struct object *obj = &objects[id];
	obj->enabled = 0;
	idx -= (id == idx-1);
}

int get_head() {
	return idx;
}

void do_wave(struct object *obj, int loc, float h){
	if(loc<SIZE&&loc>=0) obj->ybuf[1][loc] = h;
}
void do_wave_coll(struct object *obj1) {
	char prev, f;
	int loc[2], a;
	float w1, h1;
	
	w1 = obj1->loc[0]+W*obj1->sca[0];
	h1 = obj1->loc[2]+H*obj1->sca[2];

	f = obj1->flag;

	prev=f;
	f = (cube->loc[1]<obj1->loc[1]) &&
		(cube->loc[0]>=obj1->loc[0]) && (cube->loc[0]<w1) &&
		(cube->loc[2]>=obj1->loc[2]) && (cube->loc[2]<h1);
	if(f){
		limx[0]=obj1->loc[0];
		limx[1]=w1-1;
		limy[0]=obj1->loc[2];
		limy[1]=h1-1;
	}

	prev^=f;
	if(prev){
		loc[0] = (int)(cube->loc[0]/obj1->sca[0]);
		loc[1] = (int)(cube->loc[2]/obj1->sca[2]);
		loc[0] -= obj1->loc[0];
		loc[1] -= obj1->loc[2];
		
		a = loc[0]+loc[1]*W;
		do_wave(obj1, a, 4);
	}
	obj1->flag = f;
	flag |= f;
}
void do_floor_coll(struct object *obj1) {
	char f;
	
	f = (cube->loc[1]<=obj1->loc[1]) && (cube->loc[1]>=obj1->loc[1]-0.8) &&
		(cube->loc[0]>=obj1->loc[0]) && (cube->loc[0]<obj1->loc[0]+W*obj1->sca[0]) &&
		(cube->loc[2]>=obj1->loc[2]) && (cube->loc[2]<obj1->loc[2]+H*obj1->sca[2]);
	if(f){
		cube->loc[1] = obj1->loc[1];
		cube->grav = 0;
	}
}

char clamp(float *loc){
	char c1,c2,c3,c4;
	c1=(loc[0]<limx[0]);
	c2=(loc[0]>limx[1]);
	c3=(loc[2]<limy[0]);
	c4=(loc[2]>limy[1]);
	if(c1)loc[0]=limx[0];
	if(c2)loc[0]=limx[1];
	if(c3)loc[2]=limy[0];
	if(c4)loc[2]=limy[1];
	return (c1|c2)|((c3|c4)<<1);
}

void move_cube(float *loc){
	float prev;
	char stop;
	
	stop = 0;

	cube->loc[0]+=loc[0];
	cube->loc[2]+=loc[2];
	
	if(!cube->flag){
		cube->loc[1]+=cube->grav;
		cube->grav -= G*(cube->grav>-1);
	}
	else {
		cube->grav = 0;
		cube->loc[1]+=loc[1];
		stop=clamp(cube->loc);
	}
	xloc-=loc[0]*(1^(stop&0b1));
	zloc-=loc[2]*(1^((stop>>1)&0b1));
}

void collision(struct object *obj) {
	switch(obj->type){
		case WATER:
		case SYRUP:
			do_wave_coll(obj);
			break;
		case PLANE:
			do_floor_coll(obj);
			break;
	}
}

void collision_all() {
	int i;
	flag = 0;
	for(i=0;i<idx;i++)collision(&objects[i]);
	cube->flag = flag;
	//cube->grav *= !flag;
}

void jump() {
	if(!cube->grav)cube->grav = 1;
}
