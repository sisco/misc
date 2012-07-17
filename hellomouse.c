/**************************************************************************
File: hellomouse.c
Does: basic drawing and interaction in OpenGL/GLUT
Author: Brian Sisco (skeleton from gem, based on hwa's)
Date: 09/15/11
**************************************************************************/
/**********
notes:
VS users: include opengl/glut.h below instead of GL/glut.h
OSX users: include glut/glut.h below instead of GL/glut.h
**********/

#define _USE_MATH_DEFINES //this defines M_PI among other things

#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include "hellomouse.h"

//to change the accuracy of the circle approximation, change
//the number of vertices in both this line and in the struct definition
const int num_vertices = 36;

struct shape{
	int type; /*0 for rectangle, 1 for circle */
	float color[3];
	float radius;
	double x;
	double y;
	float circle_vertices[36][2];
	double force_x;
	double force_y;
};

struct shape shapes[25];
int num_shapes = 0; //how many shapes exist
int shapes_head = 0; //a "pointer" to the head of the array so it can be pseudo-circular
int next_shape = 0; /*0 for rectangle, 1 for circle */

/* define and initialize some global variables */
int window_w = 400, window_h = 300; /* window height and width */
//double my_rect_w = 0.1, my_rect_h = 0.1; /* rectangle width and height */
//the above two are no longer used

//loop counters
int i;
int j;

/*Typical OpenGL/GLUT Main function */ 
int main(int argc, char **argv) { /* program arguments */

  /* initialize GLUT and OpenGL; Must be called first */
  glutInit( &argc, argv ) ;
  
  /* our own initializations; we'll define these setup procedures */
  glut_setup() ;  
  gl_setup() ;
  my_setup();

  /* turn control over to GLUT */
  glutMainLoop() ;

  return(0) ; /* make the compiler happy */
}



/* Typical function to set up GLUT*/
void glut_setup(void) {

  /* specify display mode -- here we ask for double buffering and RGB coloring */
  glutInitDisplayMode (GLUT_DOUBLE |GLUT_RGB);

  /* make a window of size window_w by window_h; title of "GLUT Basic Interaction" placed at top left corner */
  glutInitWindowSize(window_w, window_h);
  glutInitWindowPosition(0,0);
  glutCreateWindow("GLUT Basic Interaction");

  /*initialize typical callback functions */
  glutDisplayFunc( my_display );
  glutReshapeFunc( my_reshape ); 
  glutIdleFunc( my_idle );
  glutMouseFunc( my_mouse );	
  glutKeyboardFunc( my_keyboard );	
  glutTimerFunc( 20, my_TimeOut, 0);/* schedule a my_TimeOut call with the ID 0 in 20 milliseconds from now */

  return ;
}

/* Typical function to set up OpenGL */
/* For now, ignore the contents of function */
void gl_setup(void) {

  /* specifies a background color: black in this case */
  glClearColor(0,0,0,0) ;

  /* setup for simple 2d projection */
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity();
  /* map unit square to viewport window */
  gluOrtho2D(0.0, 1.0, 0.0, 1.0); 

  return ;
}


void my_idle(void) {
  return ;
}


void my_setup(void) {
  return;
}




/* drawing function */
/* draws the circles and rectangles at their locations */
void my_display(void) {
  /* clear the buffer */
  glClear(GL_COLOR_BUFFER_BIT) ;
  
  for(i = 0; i < num_shapes; i++)
  {
	  struct shape curr_shape = shapes[i];
	  if(curr_shape.type == 0)
	  {
		  glColor3f(0,0,1) ; /* (Red, Green, Blue); so here we ask for Blue */
		  /* draw small rectangle; specify two opposing corners as arguments*/
		  /* note how the rectangle coordinates we pass as arguments are mapped to the window coordinates */
		  /* basically, we need to map x values between (0,window_w) to the (0,1) interval etc. */
		  //Keeping the original for reference:
		  //my_x and my_y are no longer defined because there are not used anywhere else
		  //glRectf( my_x/window_w, my_y/window_h, my_x/window_w + my_rect_w, my_y/window_h + my_rect_h);

		  glRectf(curr_shape.x/window_w, curr_shape.y/window_h, curr_shape.x/window_w + .1, curr_shape.y/window_h + .1); 
	  }
	  else
	  {
		  /* similar to drawing a rectangle above, but a circle, estimated with triangles*/
		  glColor3f(1,0,0);
		  glBegin(GL_TRIANGLE_FAN);
		  for(j = 0; j < num_vertices; j++)
		  {
			  glVertex2f(curr_shape.circle_vertices[j][0], curr_shape.circle_vertices[j][1]);
		  }
		  glEnd();
	  }

  }

  



  /* buffer is ready; show me the money! */

  glutSwapBuffers();
	
  return ;
}





/* called if resize event (i.e., when we create the window and later whenever we resize the window) */ 
void my_reshape(int w, int h) {
  /* define the viewport to be the entire window */
  glViewport (0, 0, w, h); 
  window_w = w;
  window_h = h;
  return;
}





/* called if mouse event (i.e., mouse click) */
void my_mouse(int b, int s, int x, int y) {
  switch (b) {            /* b indicates the button */
  case GLUT_LEFT_BUTTON:
    if (s == GLUT_DOWN)  {    /* button pressed */
      printf("Left button pressed at %d %d\n", x, y);
	  //Filling the struct for the new shape
	  if(num_shapes < 25)
	  {
		  num_shapes++;
	  }
	  shapes[shapes_head].type = next_shape;
	  //next_shape = (next_shape + 1) % 2; //cycles it between 0 and 1
	  if(next_shape == 0)
		  next_shape = 1;
	  else
		  next_shape = 0;
	  shapes[shapes_head].color[2] = 1; //default to blue
	  if(shapes[shapes_head].type == 1)
		  shapes[shapes_head].color[0] = 1; //if its a circle, switch to red
	  shapes[shapes_head].radius = window_w / 13; //its a good proportion
	  shapes[shapes_head].x = x; //set the position of the shape
	  shapes[shapes_head].y = window_h - y;
	  //sets up the vertices of the circle, if it is time for a circle rather than a rectangle
	  if(shapes[shapes_head].type == 1)
	  {
		  for(i = 0; i < num_vertices; i++)
		  {
			  shapes[shapes_head].circle_vertices[i][0] = (shapes[shapes_head].x + cos((float)(i)/num_vertices * 2 * M_PI) * shapes[shapes_head].radius) / window_w;
			  //printf("%f  ", circle_vertices[i][0]);
			  shapes[shapes_head].circle_vertices[i][1] = (shapes[shapes_head].y + sin((float)(i)/num_vertices * 2 * M_PI) * shapes[shapes_head].radius) / window_h;
			  //printf("%f\n", circle_vertices[i][1]);
		  }
	  }
	  //advance the head of the circular array
	  shapes_head++;
	  if(shapes_head > 24)
	  {
		  shapes_head = 0;
	  }

      my_display();

    }
    else if (s == GLUT_UP) { /* button released */
      printf("Left button released\n");
    }
    break;
  case GLUT_RIGHT_BUTTON:
    if (s == GLUT_DOWN)  {    /* button pressed */
      printf("Right button pressed\n");
    }
    else if (s == GLUT_UP) { /* button released */
      printf("Right button released\n");
    }
    break;
  case GLUT_MIDDLE_BUTTON:
    if (s == GLUT_DOWN)  {    /* button pressed */
      printf("Middle button pressed at %d %d\n", x, y);
    }
    else if (s == GLUT_UP) { /* button released */
      printf("Middle button released\n");
    }
    break;

  }
}


/* called if keyboard event (keyboard key hit) */
/* TODO for EC: do something fun in here */
void my_keyboard(unsigned char c, int x, int y) {
  switch (c) {  /* c is the key that is being hit */
	//WASD can be used to exert a force on the most recent shape
  case 'w': {
    //printf("w key is hit\n");
  }
   case 'W': {
    //printf("W key is hit\n");
	//Without a break above, both w and W will execute this.
	//Move up
	   shapes[shapes_head - 1].force_y += .2;
  }
    break;
  case 'a': {
  }
  case 'A': {
	  //Move left
	   shapes[shapes_head - 1].force_x -= .2;
  }
    break;
  case 's': {
  }
  case 'S': {
	  //Move down
	   shapes[shapes_head - 1].force_y -= .2;
  }
    break;
  case 'd': {
  }
  case 'D': {
	  //Move right
	   shapes[shapes_head - 1].force_x += .2;
  }
    break;
  }


}


/* called if timer event */
/* TODO for EC: do something fun in here */
void my_TimeOut(int id) { 
  /* id is the id of the timer; you can schedule as many timer events as you want in the callback setup section of glut_setup */

  /*...you could use this timer event to advance the state of animation incrementally */
  /* if animating you shd replace the 20 secs with something smaller, like 20 msec */
  /* but anyway, right now it only displays a benign message */
  //printf("Tick! It's been 20 seconds: benign timer at your service!\n");

	/*As time goes on, things fall. */
	for(i = 0; i <= num_shapes; i++)
	{
		if(shapes[i].type == 0)
		{
			if(shapes[i].y > 0)
			{
				shapes[i].y--;
				shapes[i].x += shapes[i].force_x;
				shapes[i].y += shapes[i].force_y;
			}
			
		}
		else
		{
			if(shapes[i].circle_vertices[num_vertices * 3 / 4][1] > 1.0 / window_h)
			{
				for(j = 0; j < num_vertices; j++)
				{
					shapes[i].circle_vertices[j][1] -= 1.0 / window_h;
					shapes[i].circle_vertices[j][0] += shapes[i].force_x / window_w;
					shapes[i].circle_vertices[j][1] += shapes[i].force_y / window_h;
				}
			}
		}
	}
	
 
  glutTimerFunc(20, my_TimeOut, 0);/* schedule next timer event, 20 msecs from now */
  my_display();
}
