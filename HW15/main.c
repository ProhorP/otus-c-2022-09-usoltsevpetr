#include <stdio.h>
#include <GL/glut.h>
#include <GL/gl.h>

int WindW, WindH;
int i;
int alpha;

void
Reshape (int width, int height)	// Reshape function
{
  glViewport (0, 0, width, height);
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  glOrtho (-1, 1, -1, 1, 0, 10);
  glMatrixMode (GL_MODELVIEW);

  WindW = width;
  WindH = height;
}

void
Draw (void)			// Window redraw function
{

  glClear (GL_COLOR_BUFFER_BIT);

  glLineWidth (3);
  glColor3f (0.0f, 0.6f, 0.9f);

  glPushMatrix ();
  glRotatef (alpha, 1.0f, 0.0f, 0.0f);
  glRotatef (alpha, 0.0f, 1.0f, 0.0f);
  glRotatef (alpha, 0.0f, 0.0f, 1.0f);

  alpha += 2;
  if (alpha > 359)
    alpha = 0;
  glBegin (GL_QUADS);

  // FRONT
  glColor3f (1.0f, 0.0f, 0.0f);
  glVertex3f (-0.5f, -0.5f, 0.5f);
  glVertex3f (0.5f, -0.5f, 0.5f);
  glVertex3f (0.5f, 0.5f, 0.5f);
  glVertex3f (-0.5f, 0.5f, 0.5f);

  // BACK
  glColor3f (0.0f, 1.0f, 0.0f);
  glVertex3f (-0.5f, -0.5f, -0.5f);
  glVertex3f (-0.5f, 0.5f, -0.5f);
  glVertex3f (0.5f, 0.5f, -0.5f);
  glVertex3f (0.5f, -0.5f, -0.5f);

  // LEFT
  glColor3f (0.0f, 0.0f, 1.0f);
  glVertex3f (-0.5f, -0.5f, 0.5f);
  glVertex3f (-0.5f, 0.5f, 0.5f);
  glVertex3f (-0.5f, 0.5f, -0.5f);
  glVertex3f (-0.5f, -0.5f, -0.5f);

  // RIGHT
  glColor3f (1.0f, 1.0f, 0.0f);
  glVertex3f (0.5f, -0.5f, -0.5f);
  glVertex3f (0.5f, 0.5f, -0.5f);
  glVertex3f (0.5f, 0.5f, 0.5f);
  glVertex3f (0.5f, -0.5f, 0.5f);

  // TOP
  glColor3f (0.0f, 1.0f, 1.0f);
  glVertex3f (-0.5f, 0.5f, 0.5f);
  glVertex3f (0.5f, 0.5f, 0.5f);
  glVertex3f (0.5f, 0.5f, -0.5f);
  glVertex3f (-0.5f, 0.5f, -0.5f);

  // BOTTOM
  glColor3f (1.0f, 0.0f, 1.0f);
  glVertex3f (-0.5f, -0.5f, 0.5f);
  glVertex3f (-0.5f, -0.5f, -0.5f);
  glVertex3f (0.5f, -0.5f, -0.5f);
  glVertex3f (0.5f, -0.5f, 0.5f);

  glEnd ();
  glPopMatrix ();

  glFlush ();
  glutSwapBuffers ();

}

void
Visibility (int state)		// Visibility function
{
  if (state == GLUT_NOT_VISIBLE)
    printf ("Window not visible!\n");
  if (state == GLUT_VISIBLE)
    printf ("Window visible!\n");
}

void
timf ()				// Timer function
{
  glutPostRedisplay ();		// Redraw windows
  glutTimerFunc (40, timf, 0);	// Setup next timer
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

void
keyboard (unsigned char key, int x, int y)
{
  switch (key)
    {
    case 27:
      exit (1);
      break;
    }

  glutPostRedisplay ();
}

#pragma GCC diagnostic pop

int
main (int argc, char *argv[])
{
  WindW = 400;
  WindH = 300;
  alpha = 4;

  glutInit (&argc, argv);
  glutInitWindowSize (WindW, WindH);
  glutInitDisplayMode (GLUT_RGB | GLUT_DOUBLE);
  (void) glutCreateWindow ("OTUS OpenGL");

  glutReshapeFunc (Reshape);	// Set up reshape function
  glutDisplayFunc (Draw);	// Set up redisplay function
  glutKeyboardFunc (keyboard);
  glutTimerFunc (40, timf, 0);	// Set up timer for 40ms, about 25 fps
  glutVisibilityFunc (Visibility);	// Set up visibility funtion
  glClearColor (1, 1, 1, 0);

  glutMainLoop ();

  return 0;
}
