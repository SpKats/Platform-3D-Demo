# Platformer Demo

This is a demo featuring a cube and 2D surfaces. 

### Requirements
* Operating System: Linux
* GCC
* OpenGL
* GLU (OpenGL Utility Library)
* FreeGLUT
* libm (C math library)

To install all dependencies, run: `sudo apt install build-essential libgl1-mesa-dev libglu1-mesa-dev freeglut3-dev`

### Compilation
First, you need to compile the model.c and control.c files into .o object files:
`gcc -c model.c control.c`
Then, compile the main.c file and link the object files and the libraries:
`gcc main.c *.o -lGLU -lGL -lglut -lm`

## Running the demonstration
To run the demonstration, simply run `./a.out`.
* Use the 'w', 'a', 's', 'd' keys to move the cube forward, left, backward and right. Press the 'x' key to jump.
* When the cube touches a liquid surface, you can sink the cube downwards with the 'z'  key. To move towards the surface, press 'x'.
* To control the camera, use 'i', 'j', 'k' and 'l' keys to pan down, right, up and left. Press the '+' and '-' keys to move the camera up or down.
