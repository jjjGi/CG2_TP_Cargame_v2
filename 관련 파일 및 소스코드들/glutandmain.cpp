//assimp 공식 예제 활용. 

// ----------------------------------------------------------------------------
// Simple sample to prove that Assimp is easy to use with OpenGL.
// It takes a file name as command line parameter, loads it using standard
// settings and displays it.
//
// If you intend to _use_ this code sample in your app, do yourself a favour 
// and replace immediate mode calls with VBOs ...
//
// The vc8 solution links against assimp-release-dll_win32 - be sure to
// have this configuration built.
// ----------------------------------------------------------------------------
//
//#include <windows.h>
//#include <stdio.h>
//   
//
//// assimp include files. These three are usually needed. 
//#include <gl\GL.h>
//#include <gl\GLU.h> 
//#include "GL/glut.h"
//#include <IL\il.h>

#include "Unifiedheader.h" 
 


// current rotation angle
static float angle = 0.f; //디버그용으로 남겨둠 

// ----------------------------------------------------------------------------
void do_motion (void)
{
	static GLint prev_time = 0;

	int time = glutGet(GLUT_ELAPSED_TIME);
	angle += (time-prev_time)*0.01;
	prev_time = time;

	glutPostRedisplay ();
}

// ----------------------------------------------------------------------------

void reshape(int width, int height)
{
	const double aspectRatio = (float) width / height, fieldOfView = 45.0;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fieldOfView, aspectRatio,
		1.0, 1000.0);  /* Znear and Zfar */
	glViewport(0, 0, width, height);
}

void display(void)
{
	float tmp;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity(); 
	 gluPerspective(45.0f, 640.0/480.0, 0.1f, 10000.0); 

	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity(); 

	SettingCameraInfo();
	// 밑에껀 디버그용으로 
//gluLookAt( 0.f,1000.f,1000.f,		0.f,0.f,0.f,		0.f,1.f,0.f);
  

	Set_TerrainMaterial();
	glBindTexture(GL_TEXTURE_2D , g_terrainDiffuseMapID );
 
//	DrawTestPlane(g_terrainDiffuseMapID );  X Y Z 평면을 그리는 디버그용이다. 

	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	glCallList( g_TerrainObjectGLCALLID ); //지형을 그리는 함수 -> triangle strip 으로 그리는 것! 
//	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

	RenderMainCar();  // 자동차 그리기.
	RenderTrees();   //나무들 그리기.

	glutSwapBuffers();
}
 
// ----------------------------------------------------------------------------
int main(int argc, char **argv)
{ 
	glutInitWindowSize(600*2,400*2);
	glutInitWindowPosition(100,100);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInit(&argc, argv);

	glutCreateWindow("버전2!!");
	glutDisplayFunc(display);
	glutIdleFunc( display);
	glutReshapeFunc(reshape); 
	 
	InitAll();   
	float clearcolor = 0.9; 

	glClearColor(clearcolor,clearcolor,clearcolor,1.f);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);    // Uses default lighting parameters

	glEnable(GL_DEPTH_TEST);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glEnable(GL_NORMALIZE);

	Set_Light_STATE();

	// XXX docs say all polygons are emitted CCW, but tests show that some aren't.
	if(getenv("MODEL_IS_BROKEN"))  
		glFrontFace(GL_CW);

	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	glEnable(GL_TEXTURE_2D );

	glutGet(GLUT_ELAPSED_TIME);
	glutMainLoop();
	 
	return 0;
}
