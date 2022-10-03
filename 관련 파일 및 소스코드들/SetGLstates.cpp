#include "Unifiedheader.h"





void Set_Light_STATE()
{ 
	//glClearColor (0.0, 0.0, 0.0, 0.0);
	glShadeModel (GL_SMOOTH); 
	float ambient =.2;
	float diffuse = .9;
	float specular = 0.5;
	float diffuselight[] ={diffuse,diffuse,diffuse,1};
	float ambientlight[] ={ambient,ambient,ambient,1};
	float specularlight[] ={ specular,specular,specular,1}; 

    ////±¤¿ø 
   glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0); 
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuselight);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientlight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularlight);
	 
//	GLfloat lightpos[] = { g_LightX, g_LightY, g_LightZ, 1 }; 
	GLfloat lightpos[] = {  100 , 100, 100, 1 };

	glLightfv(GL_LIGHT0, GL_POSITION, lightpos); 
	

	 
}

void Set_TerrainMaterial()
{
	float test = 0.99;
	float white[] ={test,test,test,1};
	float gray = 0.2;
	float grays[] ={ gray, gray,gray,1};
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, white);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, white);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white); 
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 50.0); 

}