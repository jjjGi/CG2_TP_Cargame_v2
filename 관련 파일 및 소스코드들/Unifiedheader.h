

#pragma once 


#include <windows.h>
#include <stdio.h>


#include <fstream>

//to map image filenames to textureIds
#include <string.h>
#include <map>
#include <vector>
   

// assimp include files. These three are usually needed. 
#include <gl\GL.h>
#include <gl\GLU.h> 
#include "GL/glut.h"
#include <IL\il.h>

#pragma warning( disable : 4819 )

#define GLM_COMPILER 0
#include "glm/glm.hpp"

using namespace glm; 
using namespace std;


#include "assimp.h"
#include "aiScene.h"
#include "aiPostProcess.h"


static const std::string basepath = "data/"; //mesh, obj , texture 등은 data안에 다 들어있게 정리. 
class DrawAIObject;


#define aisgl_min(x,y) (x<y?x:y)
#define aisgl_max(x,y) (y>x?y:x) 

const int numTree = 5;

extern void Light_STATE();
extern int g_TerrainObjectGLCALLID;

extern GLuint g_terrainDiffuseMapID;

GLuint DLGLtextureLoad( char * path );
void DrawTestPlane(int gltextureID);

int InitAll();

extern float g_TerrainWidthX  , g_TerrainLenghthZ  , g_TerrainHeightMultiplier; // 땅의 x, z y(높이) 값 terrain에서 조절
void recursive_render (const struct aiScene *sc, const struct aiNode* nd,std::map<std::string, GLuint*> textureIdMap,  float scale = 1); 

int LoadGLTextures(const aiScene* scene,
	std::map<std::string, GLuint*> & textureIdMap,	// map image filenames to textureIds
	GLuint*		textureIds );
void get_bounding_box ( const aiScene * scene,  struct aiVector3D* min, struct aiVector3D* max);

float TerrainHeightbyRealPosition( float inX , float inY );


void Set_Light_STATE();
void Set_TerrainMaterial();

// 기존코드들 시작.
void RenderTrees();
void RenderMainCar();
void SettingCameraInfo() ; 
void SetUpTreesPosition();

// 기존 코드 종료.
class DrawAIObject
{
public:

	const aiScene * m_scene;
	GLuint m_scene_list;

	std::map<std::string, GLuint*> m_textureIdMap;	// map image filenames to textureIds
	GLuint*		m_textureIds;							// pointer to texture Array

	aiVector3D scene_min, scene_max, scene_center;

	DrawAIObject()
	{
		m_scene = 0;
		m_scene_list = 0;
		m_textureIds = 0;
	}
	~DrawAIObject()
	{
		delete m_textureIds;
	}

	void AutoScaleDraw( float scale = 1) // 이걸 수정해야한다.
	{
		//glPushMatrix();
		float tmp = scene_max.x-scene_min.x;
		tmp = aisgl_max(scene_max.y - scene_min.y,tmp);
		tmp = aisgl_max(scene_max.z - scene_min.z,tmp);
	//	tmp *= 30;
		tmp = 1.f / tmp ; 
		glScalef(tmp, tmp, tmp);  
		glTranslatef( -scene_center.x, -scene_center.y, -scene_center.z );

		if(m_scene_list == 0) {
			m_scene_list = glGenLists(1);
			glNewList(m_scene_list, GL_COMPILE);
		 	recursive_render(m_scene, m_scene->mRootNode,m_textureIdMap, scale);
			glEndList();
		} 
		
		glCallList(m_scene_list);
	//	glPopMatrix();				//푸쉬,팝 안해줘도 ㅇ -> 속도 느림의 원인이었다!! 주의 
	}
	void Draw( float scale = 1)
	{
		//if(m_scene_list == 0) {
		//	m_scene_list = glGenLists(1);
		//	glNewList(m_scene_list, GL_COMPILE);
		 	recursive_render(m_scene, m_scene->mRootNode,m_textureIdMap, scale);
		//	glEndList();
		//} 
		
		//glCallList(m_scene_list);
	}
	void LoadAIScene( char * filename )
	{
		string filenameandpath =basepath + filename; 
		m_scene = aiImportFile( filenameandpath.c_str(), aiProcessPreset_TargetRealtime_Quality);
		if(m_scene)
			LoadGLTextures(m_scene, m_textureIdMap, m_textureIds );
		else
		{
			assert( m_scene && "메시로드실패");
			return;
		}

		get_bounding_box(m_scene, &scene_min, &scene_max ); 
		scene_center.x = (scene_min.x + scene_max.x) / 2.0f;
		scene_center.y = (scene_min.y + scene_max.y) / 2.0f;
		scene_center.z = (scene_min.z + scene_max.z) / 2.0f;
	} 
}; 
extern DrawAIObject g_MainCar;
extern DrawAIObject g_Trees[numTree];  