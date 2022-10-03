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
float terrainGetHeight(int x, int z); //�̰� ������ġ�� �ƴ϶� float ��ġ���迭���� �ε����� ã�°�.

 float TerrainHeightbyRealPosition( float inX , float inY ); // �������̸� ��ǥ��� ã��. inY�� ������ ��谪�̸� �����Ⱚ�� ����
