#define TERRAIN_ERROR_LOADING_IMAGE		-4
#define TERRAIN_ERROR_MEMORY_PROBLEM	-3
#define	TERRAIN_ERROR_NOT_SAVED			-2
#define TERRAIN_ERROR_NOT_INITIALISED	-1
#define TERRAIN_OK						 0

int terrainLoadFromImage(char *filename, int normals);
//int terrainCreateDL(float xOffset, float yOffset, float zOffset);
int terrainCreateDL();
void terrainDestroy();
int terrainScale(float min,float max);
float terrainGetHeight(int x, int z); //이건 실제위치가 아니라 float 위치값배열에서 인덱스로 찾는것.

 float TerrainHeightbyRealPosition( float inX , float inY ); // 실제높이를 좌표계로 찾음. inY가 지형의 경계값이면 쓰레기값이 나옴
