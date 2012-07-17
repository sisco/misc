/*******************************************************
FILE: stitcher.c
AUTHOR: Brian Sisco (skeleton by gem, loosely based on random hwa skel)
DATE: 10/4/11
DOES: homework 2, stitcher, draws 3d shapes and their vertex normals
PLATFORM: tested on vs, should work elsewhere
********************************************************/

#include "stitcher.h"
#include <math.h>
#include <stdio.h>
#define _USE_MATH_DEFINES //this defines M_PI among other things

int i;
int j;
double theta;
double phi;
double t;

int theta_x;
int theta_y;
int crt_render_mode;
int crt_shape, crt_rs, crt_vs;
int crt_norms = 0;
int tor_rs = 20;
int tor_vs = 20;
double cyl_height=1;
double cyl_ray=.5;
double sph_ray=1;
double con_height=1;
double con_ray=.5;
double tor_ray1=1.2;
double tor_ray2=.2;

//for finding normals
GLfloat p0[4];
GLfloat p1[4];
GLfloat p2[4];
GLfloat v1[4];
GLfloat v2[4];
GLfloat n[4];
GLfloat mag; //for magnitude, used in normalizing
GLfloat norm_length = .2;

 
/*******************************************************
FUNCTION: main
ARGS: argc, argv
RETURN: 0
DOES: main function (duh!); starts GL, GLU, GLUT, then loops 
********************************************************/
int main(int argc, char **argv) {

  /* General initialization for GLUT and OpenGL
     Must be called first */
  glutInit( &argc, argv ) ;
  
  /* we define these setup procedures */
  glut_setup() ;  
  gl_setup() ;
  my_setup();

  /* go into the main event loop */
  glutMainLoop() ;

  return(0) ;
}


/*******************************************************
FUNCTION: glut_setup
ARGS: none
RETURN: none
DOES: sets up GLUT; done here because we like our 'main's tidy
********************************************************/
/* This function sets up the windowing related glut calls */
void glut_setup(void) {

  /* specify display mode -- here we ask for a double buffer and RGB coloring */
  /* NEW: tell display we care about depth now */
  glutInitDisplayMode (GLUT_DOUBLE |GLUT_RGB |GLUT_DEPTH);

  /* make a 400x400 window with the title of "Stitcher" placed at the top left corner */
  glutInitWindowSize(400,400);
  glutInitWindowPosition(0,0);
  glutCreateWindow("Stitcher");

  /*initialize callback functions */
  glutDisplayFunc( my_display );
  glutReshapeFunc( my_reshape ); 
  glutMouseFunc( my_mouse);
  glutKeyboardFunc(my_keyboard);

  /*just for fun, uncomment the line below and observe the effect :-)
    Do you understand now why you shd use timer events, 
    and stay away from the idle event?  */
  //  glutIdleFunc( my_idle );
	
  glutTimerFunc( 20000, my_TimeOut, 0);/* schedule a my_TimeOut call with the ID 0 in 20 seconds from now */
  return ;
}


/*******************************************************
FUNCTION: gl_setup
ARGS: none
RETURN: none
DOES: sets up OpenGL (name starts with gl but not followed by capital letter, so it's ours)
********************************************************/
void gl_setup(void) {

  /* specifies a background color: black in this case */
  glClearColor(0,0,0,0) ;

  /* NEW: now we have to enable depth handling (z-buffer) */
  glEnable(GL_DEPTH_TEST);

  /* NEW: setup for 3d projection */
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity();
  // perspective view
  gluPerspective( 20.0, 1.0, 1.0, 100.0);
  return;
}

/*******************************************************
FUNCTION: my_setup
ARGS: 
RETURN:
DOES: pre-computes stuff and presets some values
********************************************************/
/*TODO EC: add make_torus etc.   */
void my_setup(void) {
  theta_x = 0;
  theta_y = 0;
  crt_render_mode = GL_LINE_LOOP;
  crt_shape = HOUSE;
  crt_rs = 10; //was 20
  crt_vs = 10;
  make_cube_smart(1);
  make_cylinder(cyl_height,cyl_ray,crt_rs,crt_vs);
  make_sphere(sph_ray,crt_rs,crt_vs);
  make_cone(con_height, con_ray, crt_rs, crt_vs);
  make_torus(tor_ray1, tor_ray2, tor_rs, tor_vs);
  return;
}

/*******************************************************
FUNCTION: my_reshape
ARGS: new window width and height 
RETURN:
DOES: remaps viewport to the Window Manager's window
********************************************************/
void my_reshape(int w, int h) {

  /* define viewport -- x, y, (origin is at lower left corner) width, height */
  glViewport (0, 0, min(w,h), min(w,h));
  return;
}


/*******************************************************
FUNCTION: my_keyboard
ARGS: key id, x, y
RETURN:
DOES: handles keyboard events
********************************************************/
/*TODO: expand to add asgn 2 keyboard events */
void my_keyboard( unsigned char key, int x, int y ) {
  
  switch( key ) {
  case 'y':
  case 'Y': {
    theta_y = (theta_y+2) %360;
    glutPostRedisplay(); 
  }; break;
  case 'x':
  case 'X': {
    theta_x = (theta_x+2) %360;
    glutPostRedisplay(); 
  }; break;
  case 'B':
  case 'b': {
    crt_shape = CUBE;
    glutPostRedisplay();
  }; break;
  case 'H':
  case 'h': {
    crt_shape = HOUSE;
    glutPostRedisplay();
  }; break;
  case 'C':
  case 'c': {
    crt_shape = CYLINDER;
    glutPostRedisplay();
  }; break;
  case 'S':
  case 's': {
    crt_shape = SPHERE;
    glutPostRedisplay();
  }; break;
  case 'N':
  case 'n': {
    crt_shape = CONE;
    glutPostRedisplay();
  }; break;
  case 'M':
  case 'm': {
    crt_shape = TORUS;
    glutPostRedisplay();
  }; break;
  case '+': {
    crt_vs++;
	remake_object(crt_shape);
    glutPostRedisplay();
  }; break;
  case '-': {
    crt_vs--;
	remake_object(crt_shape);
    glutPostRedisplay();
  }; break;
  case '<': {
    crt_rs++;
	remake_object(crt_shape);
    glutPostRedisplay();
  }; break;
  case '>': {
    crt_rs--;
	remake_object(crt_shape);
    glutPostRedisplay();
  }; break;
  case 'V':
  case 'v': {
    crt_norms = (crt_norms + 1) % 2;
	//printf("%d", crt_norms);
    glutPostRedisplay();
  }; break;
 case 'q': 
  case 'Q':
    exit(0) ;
  default: break;
  }
  
  return ;
}


/*******************************************************
FUNCTION: my_mouse
ARGS: button, state, x, y
RETURN:
DOES: handles mouse events
********************************************************/
void my_mouse(int button, int state, int mousex, int mousey) {

  switch( button ) {
	
  case GLUT_LEFT_BUTTON:
    if( state == GLUT_DOWN ) {
      crt_render_mode = GL_LINE_LOOP;
      /* if you're not sure what glutPostRedisplay does for us,
	 try commenting out the line below; observe the effect.*/
      glutPostRedisplay();
    }
    if( state == GLUT_UP ) {
    }
    break ;

  case GLUT_RIGHT_BUTTON:
    if ( state == GLUT_DOWN ) {
      crt_render_mode = GL_POLYGON;
      glutPostRedisplay();
    }
    if( state == GLUT_UP ) {
    }
    break ;
  }
  
  return ;
}



/***********************************
  FUNCTION: draw_triangle 
  ARGS: - a vertex array
        - 3 indices into the vertex array defining a triangular face
        - an index into the color array.
  RETURN: none
  DOES:  helper drawing function; draws one triangle. 
   For the normal to work out, follow left-hand-rule (i.e., ccw)
*************************************/
void draw_triangle(GLfloat vertices[][4], int iv1, int iv2, int iv3, int ic) {
  glBegin(crt_render_mode); 
  {
    glColor3fv(colors[ic]);
    /*note the explicit use of homogeneous coords below: glVertex4f*/
    glVertex4fv(vertices[iv1]);
    glVertex4fv(vertices[iv2]);
    glVertex4fv(vertices[iv3]);
  }
  glEnd();
}

/***********************************
  FUNCTION: draw_quad 
  ARGS: - a vertex array
        - 4 indices into the vertex array defining a quad face
        - an index into the color array.
  RETURN: none
  DOES:  helper drawing function; draws one quad. 
   For the normal to work out, follow left-hand-rule (i.e., counter clock
   wise) 
*************************************/
void draw_quad(GLfloat vertices[][4], int iv1, int iv2, int iv3, int iv4, int ic) {
  glBegin(crt_render_mode); 
  {
    glColor3fv(colors[ic]);
    /*note the explicit use of homogeneous coords below: glVertex4f*/
    glVertex4fv(vertices[iv1]);
    glVertex4fv(vertices[iv2]);
    glVertex4fv(vertices[iv3]);
    glVertex4fv(vertices[iv4]);
  }
  glEnd();
}

/***********************************
  FUNCTION: draw_param_quad 
  ARGS: - a 50 by 50 by 4 vertex array
        - 2 indices into the vertex array defining the top left of a quad face
        - an index into the color array.
  RETURN: none
  DOES:  helper drawing function; draws one quad. 
*************************************/
/*TODO: redefine and implement as needed, according to your data structures */
void draw_param_quad(GLfloat vertices[][50][4], int line, int col, int ic) {

}


/***********************************
  FUNCTION: draw_house 
  ARGS: none
  RETURN: none
  DOES: helper build function; 
        defines a hollow cube with a bottomless pyramid as rooftop
************************************/
void draw_house()
{
	//these four are the pyramid
  draw_triangle(vertices_house,0,1,2,RED);
  draw_triangle(vertices_house,0,2,3,GREEN);
  draw_triangle(vertices_house,0,3,4,WHITE);
  draw_triangle(vertices_house,0,4,1,GREY);


  draw_quad(vertices_house,2,1,5,6, BLUE);
  draw_triangle(vertices_house,2,6,3, CYAN);
  draw_triangle(vertices_house,3,6,7, CYAN);
  draw_triangle(vertices_house,3,7,8, YELLOW);
  draw_triangle(vertices_house,8,3,4, YELLOW);
  draw_triangle(vertices_house,4,8,1, MAGENTA);
  draw_triangle(vertices_house,1,8,5, MAGENTA);

  
  //let's give it a top and bottom
	draw_triangle(vertices_house,1,2,3,BLUE);
	draw_triangle(vertices_house,3,4,1,BLUE);

	draw_triangle(vertices_house,5,6,7,WHITE);
	draw_triangle(vertices_house,7,8,5,WHITE);
}

/***********************************
  FUNCTION: draw_cube_brute 
  ARGS: none
  RETURN: none
  DOES: helper build function; 
        defines a hollow cube without a bottom or a top
************************************/
/*TODO: add bottom and top face triangles*/
void draw_cube_brute()
{
  draw_triangle(vertices_cube_brute, 4,5,1,BLUE);
  draw_triangle(vertices_cube_brute, 0,4,1,BLUE);
  draw_triangle(vertices_cube_brute, 5,6,2,CYAN);
  draw_triangle(vertices_cube_brute, 1,5,2,CYAN);
  draw_triangle(vertices_cube_brute, 3,2,6,YELLOW);
  draw_triangle(vertices_cube_brute, 7,3,6,YELLOW);
  draw_triangle(vertices_cube_brute, 0,3,7,MAGENTA);
  draw_triangle(vertices_cube_brute, 4,0,7,MAGENTA);

}



/***********************************
  FUNCTION: make_cube_smart 
  ARGS: size (the edge length)
  RETURN: none
  DOES: helper build function; 
        defines a hollow cube centered at the origin, with edge length 'size'
        the cube has no bottom nor a top
************************************/
/*TODO: compute cube vertices*/
void make_cube_smart(double size)
{
	//This is what I would want to do:
	/*
	vertices_cube_smart[][4] = { 
		{-1 * size,1 * size,1 * size,1}, {1 * size,1 * size,1 * size,1}, 
		{1 * size,1 * size,-1 * size,1}, {-1 * size,1 * size,-1 * size,1}, 
		{-1 * size,-1 * size,1 * size,1}, {1 * size,-1 * size,1 * size,1},
		{1 * size,-1 * size,-1 * size,1}, {-1 * size,-1 * size,-1 * size,1}
		};

	//Or maybe this:
	vertices_cube_smart[0][] = {-1 * size,1 * size,1 * size,1};
	vertices_cube_smart[1][] = (1 * size,1 * size,1 * size,1);
	vertices_cube_smart[2][] = (1 * size,1 * size,-1 * size,1);
	vertices_cube_smart[3][] = (-1 * size,1 * size,-1 * size,1);
	vertices_cube_smart[4][] = (-1 * size,-1 * size,1 * size,1);
	vertices_cube_smart[5][] = (1 * size,-1 * size,1 * size,1);
	vertices_cube_smart[6][] = (1 * size,-1 * size,-1 * size,1);
	vertices_cube_smart[7][] = (-1 * size,-1 * size,-1 * size,1);
	*/

	//But instead, this will have to do.
	//The 2nd and 4th coordinates are pretty similar, so a loop works well.
	for(i = 0; i < 4; i++)
	{
		vertices_cube_smart[i][1] = size;
		vertices_cube_smart[i][3] = 1;
	}
	for(i = 4; i < 8; i++)
	{
		vertices_cube_smart[i][1] = -size;
		vertices_cube_smart[i][3] = 1;
	}
	vertices_cube_smart[0][0] = vertices_cube_smart[3][0] = vertices_cube_smart[4][0] = vertices_cube_smart[7][0] = -size;
	vertices_cube_smart[1][0] = vertices_cube_smart[2][0] = vertices_cube_smart[5][0] = vertices_cube_smart[6][0] = size;
	vertices_cube_smart[2][2] = vertices_cube_smart[3][2] = vertices_cube_smart[6][2] = vertices_cube_smart[7][2] = -size;
	vertices_cube_smart[0][2] = vertices_cube_smart[1][2] = vertices_cube_smart[4][2] = vertices_cube_smart[5][2] = size;
}


/***********************************
  FUNCTION: draw_cube_smart 
  ARGS: none
  RETURN: none
  DOES: helper drawing function; constructs cube faces given cube vertices
************************************/
/*TODO: use precomputed vertices,
        stitch vertices together to make faces
        don't bother about color*/
void draw_cube_smart()
{
	draw_triangle(vertices_cube_smart, 4,5,1,BLUE);
	draw_triangle(vertices_cube_smart, 0,4,1,BLUE);
	draw_triangle(vertices_cube_smart, 5,6,2,CYAN);
	draw_triangle(vertices_cube_smart, 1,5,2,CYAN);
	draw_triangle(vertices_cube_smart, 3,2,6,YELLOW);
	draw_triangle(vertices_cube_smart, 7,3,6,YELLOW);
	draw_triangle(vertices_cube_smart, 0,3,7,MAGENTA);
	draw_triangle(vertices_cube_smart, 4,0,7,MAGENTA);

	//let's give it a top and bottom
	draw_triangle(vertices_cube_smart,0,1,2,RED);
	draw_triangle(vertices_cube_smart,2,3,0,RED);

	draw_triangle(vertices_cube_smart,4,5,6,WHITE);
	draw_triangle(vertices_cube_smart,6,7,4,WHITE);

	if(crt_norms == 1)
	{
		//if showing normals is toggled on, draw the normals
		glColor3f(1,0,0);
		for(i = 0; i < 8; i++)
		{
			glBegin(GL_LINES);
			glVertex4fv(vertices_cube_smart[i]);
			//Calculate V' = V + .2 * N
			glVertex4f(vertices_cube_smart[i][0] + norm_length * vertices_cube_smart[i][0],
						vertices_cube_smart[i][1] + norm_length * vertices_cube_smart[i][1],
						vertices_cube_smart[i][2] + norm_length * vertices_cube_smart[i][2],
						1);
			glEnd();
		}
	}
}




/***********************************
  FUNCTION: make_cylinder 
  ARGS: height (along the Y axis), ray, and tesselation parameter (how many side faces) 
  RETURN: none
  DOES: helper build function; 
        defines a cylinder centered at the origin, 
        of height 'height' along the Y axis,
        and ray 'ray' in the XOZ plane
************************************/
/*TODO: compute cylinder vertices*/
void make_cylinder(double height, double ray, int rs, int vs)
{
  /*hint: top-face vertices lie on a circle (same for bottom); 
    if need be, generate top-face vertices by cutting the circle in pie slices*/
	for(i = 0; i <= vs; i++)
	{
		for(j = 0; j <= rs; j++)
		{
			t = 1.0 / vs * i;
			theta = 2 * M_PI / rs * j;
			cylinder.vertices[i][j][0] = ray * cos(theta);
			cylinder.vertices[i][j][1] = height * t - .5;
			cylinder.vertices[i][j][2] = ray * -sin(theta);
			cylinder.vertices[i][j][3] = 1;
		}
	}
	find_normals(&cylinder, rs, vs);
}


/***********************************
  FUNCTION: make_cone
  ARGS: height (along the Y axis), ray, and tesselation parameter (how many side faces) 
  RETURN: none
  DOES: helper build function; 
        defines a cone centered at the origin, 
        of height 'height' along the Y axis,
        and ray 'ray' in the XOZ plane
************************************/
/*TODO: compute cylinder vertices*/
void make_cone(double height, double ray, int rs, int vs)
{
  /*hint: its a cylinder that scales down as it translates up*/
	for(i = 0; i <= vs; i++)
	{
		for(j = 0; j <= rs; j++)
		{
			t = 1.0 / vs * i;
			theta = 2 * M_PI / rs * j;
			cone.vertices[i][j][0] = ray * cos(theta) * (1 - t);
			cone.vertices[i][j][1] = height * t - .5;
			cone.vertices[i][j][2] = ray * -sin(theta) * (1 - t);
			cone.vertices[i][j][3] = 1;
		}
	}
	find_normals(&cone, rs, vs);
}


/***********************************
  FUNCTION: make_sphere 
  ARGS: ray, and tessellation parameters (how many meridians, respectively parallels) 
  RETURN: none
  DOES: helper build function; 
        defines a sphere centered at the origin and of ray 'ray'
************************************/
/*TODO: compute sphere vertices, 
        don't bother about color*/
void make_sphere(double ray, int rs, int vs)
{
  /*hint: work out the math first*/
	for(i = 0; i <= vs; i++)
	{
		for(j = 0; j <= rs; j++)
		{
			theta = M_PI / vs * i - M_PI / 2;
			phi = 2 * M_PI / rs * j;
			//below I used [j][i] as in the part 1 solutions, lets see how that goes
			sphere.vertices[j][i][0] = ray * cos(theta) * cos(phi);
			sphere.vertices[j][i][1] = ray * sin(theta);
			sphere.vertices[j][i][2] = ray * cos(theta) * sin(phi);
			sphere.vertices[j][i][3] = 1;
		}
	}
	find_normals(&sphere, rs, vs);
}

/***********************************
  FUNCTION: make_torus 
  ARGS: rays, and tessellation parameters (how many meridians, respectively parallels) 
  RETURN: none
  DOES: helper build function; 
        defines a torus centered at the origin with the given rays specifying its size
************************************/
void make_torus(double ray1, double ray2, int rs, int vs)
{
	for(i = 0; i <= vs; i++)
	{
		for(j = 0; j <= rs; j++)
		{
			theta = 2 * M_PI / vs * i;
			phi = 2 * M_PI / rs * j;
			torus.vertices[i][j][0] = (ray2 * cos(theta) + ray1) * cos(phi);
			torus.vertices[i][j][1] = ray2 * sin(theta);
			torus.vertices[i][j][2] = (ray2 * cos(theta) + ray1) * -sin(phi);
			torus.vertices[i][j][3] = 1;
		}
	}
	find_normals(&torus, rs, vs);
}

/***********************************
  FUNCTION: draw_shape3D() 
  ARGS: shape3D
  RETURN: none
  DOES: draws an object represented by a shape3D struct
************************************/
void draw_shape3D(struct shape3D shape, int rs, int vs)
{
	
	for(i = 0; i <= rs; i++)
	{
		for(j = 0; j <= vs; j++)
		{
			glColor3f(0,0,1);
			glBegin(crt_render_mode);
			glVertex4fv(shape.vertices[i][j]);
			glVertex4fv(shape.vertices[i+1][j]);
			glVertex4fv(shape.vertices[i+1][j+1]);
			glVertex4fv(shape.vertices[i][j+1]);
			glEnd();
			if(crt_norms == 1)
			{
				//if showing normals is toggled on, draw the normal
				glColor3f(1,0,0);
				glBegin(GL_LINES);
				glVertex4fv(shape.vertices[i][j]);
				//Calculate V' = V + .2 * N
				glVertex4f(shape.vertices[i][j][0] + norm_length * shape.normals[i][j][0],
							shape.vertices[i][j][1] + norm_length * shape.normals[i][j][1],
							shape.vertices[i][j][2] + norm_length * shape.normals[i][j][2],
							1);
				//printf("%f, %f, %f, %f...", shape.normals[i][j][0], shape.normals[i][j][1], shape.normals[i][j][2], shape.normals[i][j][3]);
				glEnd();
			}
		}
	}
	
}


/***********************************
  FUNCTION: draw_cylinder() 
  ARGS: none
  RETURN: none
  DOES: draws a cylinder from triangles
************************************/
/*TODO: stitch cylinder vertices together to make faces
don't call gl directly, use my_draw_triangle and my_draw_quad instead*/
void draw_cylinder(int rs, int vs)
{
	//this can be done generically
	draw_shape3D(cylinder, rs, vs);
}


/***********************************
  FUNCTION: draw_cone() 
  ARGS: none
  RETURN: none
  DOES: draws a cone from quads
************************************/
/*TODO: stitch cone vertices together to make faces
don't call gl directly, use my_draw_triangle and my_draw_quad instead*/
void draw_cone(int rs, int vs)
{
	//this can be handled generically
	draw_shape3D(cone, rs, vs);
}

/***********************************
  FUNCTION: draw_torus() 
  ARGS: none
  RETURN: none
  DOES: draws a torus from quads
************************************/
/*TODO: stitch cone vertices together to make faces
don't call gl directly, use my_draw_triangle and my_draw_quad instead*/
void draw_torus(int rs, int vs)
{
	//this can be handled generically
	draw_shape3D(torus, rs, vs);
}

/***********************************
  FUNCTION: draw_sphere() 
  ARGS: none
  RETURN: none
  DOES: draws a sphere from quads
************************************/
/*TODO: stitch sphere vertices together to make faces
don't call gl directly, use make_triangle and make_quad instead*/
void draw_sphere(int rs, int vs)
{
	/*
	glBegin(crt_render_mode);
	for(i = 0; i <= rs; i++)
	{
		for(j = 0; j <= vs; j++)
		{
			//below I used [j][i] as in the part 1 solutions, lets see how that goes
			glVertex4fv(sphere.vertices[i][j]);
			glVertex4fv(sphere.vertices[i+1][j]);
			glVertex4fv(sphere.vertices[i+1][j+1]);
			glVertex4fv(sphere.vertices[i][j+1]);
		}
	}
	glEnd();
	*/
	draw_shape3D(sphere, rs, vs);
}


/***********************************
  FUNCTION: draw_object 
  ARGS: shape to create (HOUSE, CUBE, CYLINDER, etc)
  RETURN: none
  DOES: draws an object from triangles
************************************/
/*TODO: expand according to assignment 2 specs*/
void draw_object(int shape) {

  switch(shape){
  case HOUSE: draw_house(); break;
  case CUBE: draw_cube_smart(); break; /*TODO: replace with draw_cube_smart*/
  case CYLINDER: draw_cylinder(crt_rs, crt_vs);  break;
  case SPHERE: draw_sphere(crt_rs, crt_vs);  break;
  case CONE: draw_cone(crt_rs, crt_vs);  break;
  case TORUS: draw_torus(tor_rs, tor_vs); ; break;
  case MESH: /*TODO EC: call your function here*/ ; break;

  default: break;
  }

}

/***********************************
  FUNCTION: remake_object 
  ARGS: shape to remake (HOUSE, CUBE, CYLINDER, etc)
  RETURN: none
  DOES: recalculates vertices
************************************/
void remake_object(int shape) {

  switch(shape){
  case HOUSE: ; break;
  case CUBE: ; break;
  case CYLINDER: make_cylinder(cyl_height,cyl_ray,crt_rs,crt_vs);  break;
  case SPHERE: make_sphere(sph_ray,crt_rs,crt_vs);  break;
  case CONE: make_cone(con_height, con_ray, crt_rs, crt_vs);  break;
  case TORUS: make_torus(tor_ray1, tor_ray2, tor_rs, tor_vs); break;
  case MESH: /*TODO EC: call your function here*/ ; break;

  default: break;
  }

}

/***********************************
  FUNCTION: find_normals
  ARGS: shape3D to find normals for (HOUSE, CUBE, CYLINDER, etc)
  RETURN: none
  DOES: recalculates normals
************************************/
void find_normals(struct shape3D *shape, int rs, int vs) {
	for(i = 0; i < rs; i++)
	{
		for(j = 0; j < vs; j++)
		{
			//I apologize for the following being so long.
			p0[0] = shape->vertices[i][j][0];
			p0[1] = shape->vertices[i][j][1];
			p0[2] = shape->vertices[i][j][2];
			p0[3] = shape->vertices[i][j][3];

			p1[0] = shape->vertices[i][j+1][0];
			p1[1] = shape->vertices[i][j+1][1];
			p1[2] = shape->vertices[i][j+1][2];
			p1[3] = shape->vertices[i][j+1][3];

			p2[0] = shape->vertices[i+1][j][0];
			p2[1] = shape->vertices[i+1][j][1];
			p2[2] = shape->vertices[i+1][j][2];
			p2[3] = shape->vertices[i+1][j][3];

			v1[0] = p1[0] - p0[0];
			v1[1] = p1[1] - p0[1];
			v1[2] = p1[2] - p0[2];
			v1[3] = p1[3] - p0[3];

			v2[0] = p2[0] - p0[0];
			v2[1] = p2[1] - p0[1];
			v2[2] = p2[2] - p0[2];
			v2[3] = p2[3] - p0[3];

			
			//n = cross(v1, v2);
			n[0] = v1[1] * v2[2] - v1[2] * v2[1];
			n[1] = -1 * (v1[0] * v2[2] - v1[2] * v2[0]);
			n[2] = v1[0] * v2[1] - v1[1] * v2[0];
			n[3] = 0; //this is a vector, not a point

			shape->normals[i][j][0] += n[0];
			shape->normals[i][j][1] += n[1];
			shape->normals[i][j][2] += n[2];

			shape->normals[i][j+1][0] += n[0];
			shape->normals[i][j+1][1] += n[1];
			shape->normals[i][j+1][2] += n[2];

			shape->normals[i+1][j][0] += n[0];
			shape->normals[i+1][j][1] += n[1];
			shape->normals[i+1][j][2] += n[2];

			shape->normals[i+1][j+1][0] += n[0];
			shape->normals[i+1][j+1][1] += n[1];
			shape->normals[i+1][j+1][2] += n[2];
			
		}
	}
	for(i = 0; i < rs; i++)
	{
		for(j = 0; j < vs; j++)
		{
			mag = (shape->normals[i][j][0])*(shape->normals[i][j][0]) + (shape->normals[i][j][1])*(shape->normals[i][j][1]) + (shape->normals[i][j][2])*(shape->normals[i][j][2]);
			mag = sqrt(mag);
			
			shape->normals[i][j][0] /= mag;
			shape->normals[i][j][1] /= mag;
			shape->normals[i][j][2] /= mag;
		}
	}
}



/***********************************
  FUNCTION: my_display
  ARGS: none
  RETURN: none
  DOES: main drawing function
************************************/
void my_display(void) {
  /* clear the buffer */
  /* NEW: now we have to clear depth as well */
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT) ;

  glMatrixMode(GL_MODELVIEW) ;
  glLoadIdentity();
  gluLookAt(0.0, 5.0, 15.0,  // x,y,z coord of the camera 
	    0.0, 0.0, 0.0,  // x,y,z coord of the origin
	    0.0, 1.0, 0.0); // the direction of up (default is y-axis)

  glRotatef(theta_y,0,1,0);
  glRotatef(theta_x,1,0,0);
  draw_object(crt_shape);

  /* buffer is ready */
  glutSwapBuffers();
	
  return ;
}


/***********************************
  FUNCTION: my_idle
  ARGS: none
  RETURN: none
  DOES: handles the "idle" event
************************************/
void my_idle(void) {
  theta_y = (theta_y+2) %360;
  glutPostRedisplay();
  return ;
}


/***********************************
  FUNCTION: my_TimeOut
  ARGS: timer id
  RETURN: none
  DOES: handles "timer" events
************************************/
void my_TimeOut(int id) {
  /* right now, does nothing*/
  /* schedule next timer event, 20 secs from now */ 
  glutTimerFunc(20000, my_TimeOut, 0);

  return ;
}



