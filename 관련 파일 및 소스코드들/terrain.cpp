#include <math.h>


#include <stdlib.h>
#include <stdio.h>
#include <gl\glut.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include "tga.h"
#include "terrain.h"



static int terrainGridWidth ,terrainGridLength; //터레인의 가로새로 크기. 이미지의 해상도로 지정. -> 작은거 쓰면작게나오도록. 
// 지형이 너무 삐죽하면, 포토샵에서 가우시안 블러를 줘서 부드럽게 해줘야됨 -> 지금은 괜찮은듯? 

static float *terrainHeights = NULL;
static float *terrainColors = NULL;
static float *terrainNormals = NULL;

double g_scaleX = 0.1, g_scaleZ = 0.1; //이미지크기의 역수. 사이트 튜토리얼에서는 여기서 이미지크기를 땅크기로 정했는데 그러면 너무 지형이 커져서 간단하게 scale조정. 
float g_TerrainWidthX = 1024, g_TerrainLenghthZ = 1024; //터레인의 가로 새로 x와 z축 크기
float g_TerrainHeightMultiplier = 100.0; // terrain의 최대 높이 최소는 0 .   

int g_terrainheightTexture = -1;


float *terrainCrossProduct(int x1,int z1,int x2,int z2,int x3,int z3) {

	float *auxNormal,v1[3],v2[3];
		
	v1[0] = x2-x1; 
	v1[1] = -terrainHeights[z1 * terrainGridWidth + x1] 
			+ terrainHeights[z2 * terrainGridWidth + x2];
	v1[2] = z2-z1; 


	v2[0] = x3-x1; 
	v2[1] = -terrainHeights[z1 * terrainGridWidth + x1] 
			+ terrainHeights[z3 * terrainGridWidth + x3];
	v2[2] = z3-z1; 

	auxNormal = (float *)malloc(sizeof(float)*3);

	auxNormal[2] = v1[0] * v2[1] - v1[1] * v2[0];
	auxNormal[0] = v1[1] * v2[2] - v1[2] * v2[1];
	auxNormal[1] = v1[2] * v2[0] - v1[0] * v2[2];

	return(auxNormal);
}

void terrainNormalize(float *v) {

	double d,x,y,z;

	x = v[0];
	y = v[1];
	z = v[2];
	
	d = sqrt((x*x) + (y*y) + (z*z));

	v[0] = x / d;
	v[1] = y / d;
	v[2] = z / d;
}

void terrainAddVector(float *a, float *b) {

	a[0] += b[0];
	a[1] += b[1];
	a[2] += b[2];
}

void terrainComputeNormals() {

	float *norm1,*norm2,*norm3,*norm4; 
	int i,j,k;
	
	if (terrainNormals == NULL)
		return;

	for(i = 0; i < terrainGridLength; i++)
		for(j = 0; j < terrainGridWidth; j++) {
			norm1 = NULL;
			norm2 = NULL;
			norm3 = NULL;
			norm4 = NULL;

			// normals for the four corners
			if (i == 0 && j == 0) {
				norm1 = terrainCrossProduct(0,0, 0,1, 1,0);	
				terrainNormalize(norm1);				
			}
			else if (j == terrainGridWidth-1 && i == terrainGridLength-1) {
				norm1 = terrainCrossProduct(i,j, j,i-1, j-1,i);	
				terrainNormalize(norm1);				
			}
			else if (j == 0 && i == terrainGridLength-1) {
				norm1 = terrainCrossProduct(i,j, j,i-1, j+1,i);	
				terrainNormalize(norm1);				
			}
			else if (j == terrainGridWidth-1 && i == 0) {
				norm1 = terrainCrossProduct(i,j, j,i+1, j-1,i);	
				terrainNormalize(norm1);				
			}

			// normals for the borders
			else if (i == 0) {
				norm1 = terrainCrossProduct(j,0, j-1,0, j,1);
				terrainNormalize(norm1);
				norm2 = terrainCrossProduct(j,0,j,1,j+1,0);
				terrainNormalize(norm2);
			}
			else if (j == 0) {
				norm1 = terrainCrossProduct(0,i, 1,i, 0,i-1);
				terrainNormalize(norm1);
				norm2 = terrainCrossProduct(0,i, 0,i+1, 1,i);
				terrainNormalize(norm2);
			}
			else if (i == terrainGridLength) {
				norm1 = terrainCrossProduct(j,i, j,i-1, j+1,i);
				terrainNormalize(norm1);
				norm2 = terrainCrossProduct(j,i, j+1,i, j,i-1);
				terrainNormalize(norm2);
			}
			else if (j == terrainGridWidth) {
				norm1 = terrainCrossProduct(j,i, j,i-1, j-1,i);
				terrainNormalize(norm1);
				norm2 = terrainCrossProduct(j,i, j-1,i, j,i+1);
				terrainNormalize(norm2);
			}

			// normals for the interior
			else {
				norm1 = terrainCrossProduct(j,i, j-1,i, j,i+1);
				terrainNormalize(norm1);
				norm2 = terrainCrossProduct(j,i, j,i+1, j+1,i);
				terrainNormalize(norm2);
				norm3 = terrainCrossProduct(j,i, j+1,i, j,i-1);
				terrainNormalize(norm3);
				norm4 = terrainCrossProduct(j,i, j,i-1, j-1,i);
				terrainNormalize(norm4);
			}
			if (norm2 != NULL) {
				terrainAddVector(norm1,norm2);
				free(norm2);
			}
			if (norm3 != NULL) {
				terrainAddVector(norm1,norm3);
				free(norm3);
			}
			if (norm4 != NULL) {
				terrainAddVector(norm1,norm4);
				free(norm4);
			}
			terrainNormalize(norm1);
			norm1[2] = - norm1[2];
			for (k = 0; k< 3; k++) 
				terrainNormals[3*(i*terrainGridWidth + j) + k] = norm1[k];
			free(norm1);
		}

}


int terrainLoadFromImage(char *filename, int normals) {

	tgaInfo *info;
	int mode;
	float pointHeight;

// if a terrain already exists, destroy it.
	if (terrainHeights != NULL)
		terrainDestroy();
		
// load the image, using the tgalib
	info = tgaLoad(filename);
// check to see if the image was properly loaded
// remember: only greyscale, RGB or RGBA noncompressed images
	if (info->status != TGA_OK)
		return(TERRAIN_ERROR_LOADING_IMAGE);

// if the image is RGB, convert it to greyscale
// mode will store the image's number of components
	mode = info->pixelDepth / 8;
	if (mode == 3) {
		tgaRGBtoGreyscale(info);
		mode = 1;
	}
	
// set the width and height of the terrain
	terrainGridWidth = info->width;
	terrainGridLength = info->height;

	//terrain 크기의 역수를 구해줌

	g_scaleX = 1.0 / terrainGridWidth;
	g_scaleZ = 1.0/ terrainGridLength;

// alocate memory for the terrain, and check for errors
	terrainHeights = (float *)malloc(terrainGridWidth * terrainGridLength * sizeof(float));
	if (terrainHeights == NULL)
		return(TERRAIN_ERROR_MEMORY_PROBLEM);

// allocate memory for the normals, and check for errors
	if (normals) {
		terrainNormals = (float *)malloc(terrainGridWidth * terrainGridLength * sizeof(float) * 3);
		if (terrainNormals == NULL)
			return(TERRAIN_ERROR_MEMORY_PROBLEM);
	}
	else
			terrainNormals = NULL;

// if mode = RGBA then allocate memory for colors, and check for errors
	if (mode == 4) {
		terrainColors = (float *)malloc(terrainGridWidth * terrainGridLength * sizeof(float)*3);
		if (terrainColors == NULL)
			return(TERRAIN_ERROR_MEMORY_PROBLEM);
	}
	else
		terrainColors = NULL;

// fill arrays
	for (int i = 0 ; i < terrainGridLength; i++)
		for (int j = 0;j < terrainGridWidth; j++) {
// compute the height as a value between 0.0 and 1.0
			pointHeight = info->imageData[mode*(i*terrainGridWidth + j)+(mode-1)] / 256.0;
			terrainHeights[i*terrainGridWidth + j] = pointHeight * g_TerrainHeightMultiplier;
// if mode = RGBA then fill the colors array as well
			if (mode==4) {
				terrainColors[3*(i*terrainGridWidth + j)]   = info->imageData[mode*(i*terrainGridWidth + j)] / 256.0;
				terrainColors[3*(i*terrainGridWidth + j)+1] = info->imageData[mode*(i*terrainGridWidth + j)+1]/256.0;
				terrainColors[3*(i*terrainGridWidth + j)+2] = info->imageData[mode*(i*terrainGridWidth + j)+2]/256.0;
			}
		}
// if we want normals then compute them		
	if (normals)
		terrainComputeNormals();
// free the image's memory 
	tgaDestroy(info);
	
	return(TERRAIN_OK); 
}

int terrainScale(float min,float max) {

	float amp,aux,min1,max1,height;
	int total,i;

	if (terrainHeights == NULL)
		return(TERRAIN_ERROR_NOT_INITIALISED);

	if (min > max) {
		aux = min;
		min = max;
		max = aux;
	}

	amp = max - min;
	total = terrainGridWidth * terrainGridLength;

	min1 = terrainHeights[0];
	max1 = terrainHeights[0];
	for(i=1;i < total ; i++) {
		if (terrainHeights[i] > max1)
			max1 = terrainHeights[i];
		if (terrainHeights[i] < min1)
			min1 = terrainHeights[i];
	}
	for(i=0;i < total; i++) {
		height = (terrainHeights[i] - min1) / (max1-min1);
		terrainHeights[i] = height * amp - min;
	}
printf("bla %f %f %f %f %f %f\n",min,max,min1,max1,amp,height);
	if (terrainNormals != NULL)
		terrainComputeNormals();
	return(TERRAIN_OK);
}
	
void normalize( float & from )
{
	from =	(from + .5);
}

void flip( float & from )
{ 
	from = 1.0 - from;
}

//// yoffset = 지형을 공중에 띄우거나 0보다 낮추려면 설정. 
//지형 높낮이= g_TerrainHeightMultiplier로 설정. 나중에 변수명 좀더 확실하게 바꾸자!    -> 변경완료 ! 


float LinearInterpolation( float, float , float );

float BiLinearInterpolation( float U_start, float U_end,float SE_middle, 
							float D_start, float D_end, float UD_middle );
 
int terrainCreateDL() {


	//temp test code 

	/*float result = LinearInterpolation( 1,4, 0.5 );

	 result = LinearInterpolation( 1,4, 0.2 );
	  result = LinearInterpolation( 1,4, 0.7 );

    result =  BiLinearInterpolation( 0,4 , 0.5, 0, 4, 0.5);

	result =  BiLinearInterpolation( 0,4 , 0.5, 0, -4, 0.25);

	result =  BiLinearInterpolation( 0,4 , 0.5, 0, -4, 0.75);

	result =  BiLinearInterpolation( 0,4 , 0.5, 0, -4, 0.9);

	result = LinearInterpolation( 2, -2, 0.9);*/


	GLuint terrainDL;
	float startW,startL;
	int i,j;

	startW = terrainGridWidth / 2.0 - terrainGridWidth;
	startL = - terrainGridLength / 2.0 + terrainGridLength;

	terrainDL = glGenLists(1);

	glNewList(terrainDL,GL_COMPILE);
	if (terrainNormals != NULL && terrainColors != NULL) {
		glColorMaterial(GL_FRONT, GL_DIFFUSE);
		glEnable(GL_COLOR_MATERIAL);
	}

	for (i = 0 ; i < terrainGridLength-1; i++) {
		glBegin(GL_TRIANGLE_STRIP);
		for (j = 0;j < terrainGridWidth; j++) {
		
		 
			float xpos  = (startW + j ) * g_scaleX * g_TerrainWidthX;
			float ypos0 =  ( startL - (i+1) ) * g_scaleZ * g_TerrainLenghthZ ;
			float ypos1 =  (startL - i ) * g_scaleZ  * g_TerrainLenghthZ ;

		
			float u,v; 

			if (terrainColors != NULL) 
				glColor3f(terrainColors[3*((i+1)*terrainGridWidth + j)],
						  terrainColors[3*((i+1)*terrainGridWidth + j)+1],
						  terrainColors[3*((i+1)*terrainGridWidth + j)+2]);
			if (terrainNormals != NULL)
				glNormal3f(terrainNormals[3*((i+1)*terrainGridWidth + j)],
						  terrainNormals[3*((i+1)*terrainGridWidth + j)+1],
						  terrainNormals[3*((i+1)*terrainGridWidth + j)+2]);
			glVertex3f(
				xpos,
				terrainHeights[(i+1)*terrainGridWidth + (j)],
				ypos0 );

			//문제가  U V가 있을때 하나의 블럭에서 U나 V가 0.2 0.3 다음 0.3 0.4  가 아니라. 0.3 0.2 다음 0.4. 0.3 이런식인거 같다.
			// 아마... strip 때문인 것 같다. 
			u = float(startW + j ) /float (terrainGridWidth );
			v = float (startL - (i+1)) / float( terrainGridLength);

			//	 v = float ((startL - i )) / float( terrainGridLength);
			normalize( u); 
			normalize( v); 
			//flip(u);
			flip(v); 
			glTexCoord2f( u, v);

			if (terrainColors != NULL) 
				glColor3f(terrainColors[3*(i*terrainGridWidth + j)],
						  terrainColors[3*(i*terrainGridWidth + j)+1],
						  terrainColors[3*(i*terrainGridWidth + j)+2]);
			if (terrainNormals != NULL)
				glNormal3f(terrainNormals[3*(i*terrainGridWidth + j)],
						  terrainNormals[3*(i*terrainGridWidth + j)+1],
						  terrainNormals[3*(i*terrainGridWidth + j)+2]);
			glVertex3f(
				xpos, 
				terrainHeights[(i)*terrainGridWidth + j] ,
				ypos1 );	

			u = float(startW + j ) /float (terrainGridWidth );
			 v = float ((startL - i )) / float( terrainGridLength);
			 
			  normalize( u); 
			  normalize( v); 
			 // flip(u);
			  flip(v); 

			glTexCoord2f( u, v);
		
			 				
		} 
		glEnd();
	}
	glEndList();
	return(terrainDL);
}


// google linear sampling in image. -> 튜토리얼 4단계 


float LinearInterpolation ( float start, float end ,float startTomiddle)
{
	float reverseAmt = 1.0 - startTomiddle;

	float result =  start * reverseAmt + end * startTomiddle;

	return result;
}

float BiLinearInterpolation( float U_start, float U_end,float SE_middle, 
							float D_start, float D_end, float UD_middle )
{

	float U = LinearInterpolation(U_start, U_end, SE_middle);
	float D = LinearInterpolation( D_start, D_end,SE_middle);

	float result = LinearInterpolation( U,D, UD_middle);

	return result;
}

void changeRealPositionToHeightDataIndex( float inX, float inY, float & outIDXX, float & outIDXY)
{ 
	inX = inX / g_TerrainWidthX; // -0.5 ~ 0.5 으로 노말라이즈
	inY = inY / g_TerrainLenghthZ;
	inX = inX * terrainGridWidth; 
	inY = inY * terrainGridLength; 

	outIDXX = inX;
	outIDXY = inY; 
}

float TerrainHeightbyRealPosition( float inX , float inY )
{
	// 원래는 인덱스가 실제위치인데 이걸 나는 전체를 0 -1 로만든후에 내가 가변 위치형태로 바꿨다.
	// 		float xpos  = (startW + j ) * g_scaleX * g_TerrainWidthX; 
	// 땅의 - 시작값 + 반복자 를 나누기 이미지의 크기 (-0.5 ~0.5) * 실제땅의 크기 

	// 따라서 중간변환을 해줘야하는데. 
	// 높이값은 실제 위치이다.
	// 배열값은 이미지 크기이다. 
	// 즉, 높이값을 배열의 인덱스로 변환해야한다. -> 주의!!  


	float fx;
	float fy;

	changeRealPositionToHeightDataIndex( inX, inY, fx, fy);

	float reminderX = fx - int(fx);
	float reminderY = fy - int(fy);
	 
	int lowX, highX;
	int lowY, highY;

	if( reminderX == 0 )
	{
		lowX= highX = int( fx);
	}
	else
	{
		lowX=   int( fx);
		highX = lowX + 1;
	}

	if( reminderY == 0 )
	{
		lowY= highY = int( fy);
	}
	else
	{
		lowY=   int( fy);
		highY = lowY + 1;
	}

	float LX_LY = terrainGetHeight(lowX, lowY );
	float LX_HY = terrainGetHeight( lowX, highY );
	float HX_LY = terrainGetHeight( highX, lowY );
	float HX_HY = terrainGetHeight( highX, highY );

	float result = BiLinearInterpolation( LX_HY, HX_HY,reminderX,  LX_LY, HX_LY, reminderY);

	return result ;
}
float terrainGetHeight(int x, int z) {  
	
	int xt,zt;

	if (terrainHeights == NULL) 
			return(0.0); 
 
	xt = x + terrainGridWidth /2;
	zt = terrainGridWidth - (z + terrainGridLength /2); 

	if ((xt > terrainGridWidth) || (zt > terrainGridLength) || (xt < 0) || (zt < 0))
		return(0.0);

	return(terrainHeights[zt * terrainGridWidth + xt]);
}


void terrainDestroy() {

	if (terrainHeights != NULL)
		free(terrainHeights);

	if (terrainColors != NULL)
		free(terrainColors);

	if (terrainNormals != NULL)
		free(terrainNormals);
}