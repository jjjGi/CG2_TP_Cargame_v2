

#include"Unifiedheader.h"
#include "terrain.h"

int g_TerrainObjectGLCALLID = -1;

DrawAIObject g_MainCar;
DrawAIObject g_Trees[5];  


bool LoadCarandTree()
{
	//주석은 테스트용 코드
//	g_MainCar.LoadAIScene("policeman.obj"); 
//	g_MainCar.LoadAIScene("jeep1.3ds");
//	g_MainCar.LoadAIScene("cube.obj");
	//g_MainCar.LoadAIScene("tree03.obj");
//	g_MainCar.LoadAIScene("Windmill.obj"); 
	g_Trees[0].LoadAIScene("Barn.obj");
	g_Trees[1].LoadAIScene("ChickenCoop.obj"); 
	g_Trees[2].LoadAIScene("tree03.obj"); 
	g_Trees[3].LoadAIScene("tree04.obj"); 
	g_Trees[4].LoadAIScene("tree05.obj");
	g_MainCar.LoadAIScene("jeep1.3ds"); 

	return true;

}

//깃허브 참고 내용 
int InitAll()
{							
	/* Before calling ilInit() version should be checked. */
	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION)
	{
		ILint test = ilGetInteger(IL_VERSION_NUM);
		/// wrong DevIL version ///
		//std::string err_msg = "Wrong DevIL version. Old devil.dll in system32/SysWow64?";
		//char* cErr_msg = (char *) err_msg.c_str();
		//abortGLInit(cErr_msg);
		return -1;
	}

	ilInit(); /* Initialization of DevIL */


	////////////////////////////////////////////////
	// 지형관련시작.~~~!!

	 g_terrainDiffuseMapID = DLGLtextureLoad( "data\\roadmap.bmp");
 	if (terrainLoadFromImage("data\\roadmapheight4.tga",1) != TERRAIN_OK) 
 		return(-1);
	 
	g_TerrainObjectGLCALLID = terrainCreateDL(); 

	float testheight = terrainGetHeight( 0,0);

	float test_result;

	for( float y = 0; y <= 1.0; y+= 0.1)
	for( float x = 0; x <= 1.0 ; x+= 0.1)
	{
		test_result = TerrainHeightbyRealPosition( x * g_TerrainWidthX - g_TerrainWidthX /2.0 , 
			y * g_TerrainLenghthZ - g_TerrainLenghthZ / 2.0 );
	} 
	//////////////////////////////////////////////////////////////////
	// 자동차, 트리등로딩
	LoadCarandTree();
	//나무 위치 세팅 
	SetUpTreesPosition();
}

void DestroyAll()
{
	//main으로 ㄱ 
}



