#ifndef MVC_MODEL
#define MVC_MODEL

#define W 0x10
#define H 0x10
#define SIZE W*H
#define W2 (W>>1)
#define H2 (H>>1)
#define W1 0xf
#define H1 0xf

#define G 0.1

enum obj_type {
	NONE,
	WATER,
	SYRUP,
	CUBE,
	PLANE,
};

struct object {
	float loc[3], rot[3], sca[3];
	float **ybuf;
	enum obj_type type;
	char enabled;
	char flag;
	float grav;
};

extern struct object objects[256];
extern struct object *cube;

void waves(struct object *obj, int w, int size);
void add_object(float *loc, float *rot, float *sca, enum obj_type type, float **ybuf);
void remove_object(int id);
int get_head();
void do_wave(struct object *obj, int loc, float h);
void do_wave_coll(struct object *obj1);
void move_cube(float *loc);

void do_floor_coll(struct object *obj1);
void collision(struct object *obj);
void jump(void);
void collision_all(void);
#endif
