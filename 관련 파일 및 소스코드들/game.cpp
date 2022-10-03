
#include "Unifiedheader.h"
#include "time.h"
#include<glm/gtx/vector_angle.hpp>
#include <math.h> 

//���� assimp ���Ϸ�!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1�̤̤̤̤̤̤̤̤̤̤̤̤̤̤̤̤̤̤̤̤̤̤̤̤̤�

float obj_rx, obj_rz;
float obj_x, obj_y, obj_z;
float obj_ry = 90.0;
 
float g_CameraLength = 25.f;
glm::vec3 g_posCameraEye; // ī�޶� ��ġ

vector<float> m_lastpoints;
const int heightacumamount = 10; // �׳��ϸ� ���� �䵿�� ġ�µ� �������� �ε��Ҷ� �������� Ȯ��ġ �ʾƼ�. �׳� �ε巴�� ��������. ���ڸ� ���̸� ������ ���̸� ����������!! 


vector<float> m_lastdecline;
const int declinetacumamount = 20; //  �� ���� ���⸦ �ε巴��.

#define PI 3.141592653589
 
//////////////////////////////////////////////////////////////////////
// �ڵ��� �� ����



//GLMmodel* g_pModelInfo = NULL;
glm::vec3 g_posMainCar(211, 0, 434 - 30);         //�ڵ��� ��ġ

glm::vec3 g_vecDirCar(1.0, .0, 1.0);   //�ڵ��� ���⺤��
float g_velocityCar = 0.01f;                       //�ڵ��� �ӷ� ��Į�� 
float g_velocityInterval = 0.05f;                       //�ڵ��� ���ӷ�
float g_registCar = 0.98;					// �ڵ��� ���� ����
float g_maxSpeed = 12.0;				// �ڵ��� �ִ� �ӵ�

BOOL g_cockpitMode = false; // ������ ���

BOOL g_goalMode = false; // ����� ��� �÷���

// ������Ʈ (��水ü)
struct sMapObjectInfo
{
	int type; // ��ü�鿡 ���� idx 
	glm::vec3 pos; // ��ġ



	sMapObjectInfo()
	{
		type = -1; 
		pos.x = 0;
		pos.y = 0;
		pos.z = 0; 
		//modelData = NULL;	//�ʿ��� �ϴ� �ּ�
	}
};  

//////////////////////////////////////////////////////////////////////


const int		 g_nObjectMaxCount = 50; // ���� ��ü 100���� 
std::vector<sMapObjectInfo> g_vecMapObjectInfo; // ��水ü ���� 

//////////////////////////////////////////////////////////////////////


void car_front_vector_set() 
{               //�ڵ����� �պκ� ���⺤��. (�ڷΰ��� �� ���� ���⺤�� �ȹٲ�ٴ� ��.!!)
	g_vecDirCar.x = 0;
	g_vecDirCar.y = 0;
	g_vecDirCar.z = 0;

    g_vecDirCar.x = sin(obj_ry * PI / 180);
    g_vecDirCar.z = -cos(obj_ry * PI  / 180);
    g_vecDirCar = glm::normalize(g_vecDirCar);
}


//�ڵ��� ����ٴϴ� ī�޶� ���� 
void SettingCameraInfo() 
{
	car_front_vector_set();
	glm::vec3 vecDir = g_vecDirCar;
	glm::normalize(vecDir);

	glm::vec3 vecUp(0, 1, 0);

	glm::vec3 vecRight;
	vecRight = glm::cross(vecDir, vecUp);

	glm::normalize(vecRight);

	float fDirDistance = g_CameraLength * 6;
	//float fUpDistance = g_CameraLength * 0.4 * 5;		// �׽�Ʈ�� 

	float fUpDistance = g_CameraLength  * 3;

	if (g_cockpitMode)
	{
		fDirDistance = g_CameraLength;
		fUpDistance = g_CameraLength * 0.25;
	}  
	
	g_posCameraEye = g_posMainCar - (g_vecDirCar * fDirDistance) + (vecUp * fUpDistance); 
	//printf("g_posMainCar : %.3f %.3f %.3f\n", g_posMainCar.x, g_posMainCar.z, g_posMainCar.y);			//���� ������ ����ڵ� �̤�
	//if (g_gameStatus == E_PLAY)
	//{
	//	g_timeEnd = clock();
	//	g_dCurrentTime = (double)(g_timeEnd - g_timeStart);
	//	//printf("g_dCurrentTime : %.1f\n", g_dCurrentTime);
	//} 

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glm::vec3 camerapos(g_posCameraEye.x, g_posCameraEye.y, g_posCameraEye.z);
	glm::vec3 carpos(g_posMainCar.x, g_posMainCar.y, g_posMainCar.z);
	vec3 camtocar = camerapos - carpos;
	camtocar = glm::normalize( camtocar );

	vec3 side= cross( vec3( 0,1,0),  camtocar);
	vec3 trueup = cross(  camtocar, side);

	// up�� ����Ȯ�� �� �� ��.  �׳� �����ϰ� �ش� ������ . 
	gluLookAt(camerapos.x-10, camerapos.y +25, camerapos.z,
				carpos.x, carpos.y+5, carpos.z, trueup.x, trueup.y,trueup.z	); 
/*
    gluLookAt(g_posCameraEye.x, g_posCameraEye.y, g_posCameraEye.z,
				g_posMainCar.x, g_posMainCar.y, g_posMainCar.z, 0, 1, 0);*/ 
	/* gluLookAt(g_posCameraEye.x, g_posCameraEye.y , g_posCameraEye.z,
				g_posMainCar.x, g_posMainCar.y, g_posMainCar.z, 0, 1, 0);*/
} 

void KeyEventCarMove(void)
{
/*
	if (E_PLAY != g_gameStatus)
		return;
*/
	if (GetAsyncKeyState(VK_UP)|| GetAsyncKeyState('w') || GetAsyncKeyState('W') )
		g_velocityCar += g_velocityInterval;

	if (GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState('s') || GetAsyncKeyState('S') )
	{
		if (g_velocityCar > 0)
		{
			g_velocityCar -= g_velocityInterval;

			if (g_velocityCar < 0)
				g_velocityCar = 0;
		}
	}

	if (g_velocityCar > g_maxSpeed)
		g_velocityCar = g_maxSpeed;
	 

	if (GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState('a') || GetAsyncKeyState('A')  )
	{
		obj_ry -= 1.3;                  //ȸ����                     
		if (obj_ry > 360 || obj_ry < -360) 
			obj_ry = 0.0;
	}

	if (GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState('d') || GetAsyncKeyState('D')  )
	{
		obj_ry += 1.3;
		if (obj_ry > 360 || obj_ry < -360) 
			obj_ry = 0.0;
	}

}

float ProcessCarRolling()
{
	vec3 normalvelocity;
	
	float distancetofront = 4.0;
	normalvelocity.x  = g_vecDirCar.x  *distancetofront;
	normalvelocity.y  = g_vecDirCar.y  *distancetofront;
	normalvelocity.z  = g_vecDirCar.z  *distancetofront;

	vec3 frontpos = g_posMainCar + normalvelocity; 

	float heightofcar = TerrainHeightbyRealPosition( g_posMainCar.x, g_posMainCar.z);
	float heightoffront = TerrainHeightbyRealPosition( frontpos.x, frontpos.z);



// 2�����̴ϱ� 2�������� ���ؾߵȴ�.!! ���� 

	float slope = (heightoffront - heightofcar)/ distancetofront; 
	 
	float angleamount = atan (slope) * 180 / PI; 
	//angleamount = degrees(angleamount) * 0.7;

	return angleamount;

}



// 
float SmoothCarheight( float newheight )
{
	
	if( m_lastpoints.size() < heightacumamount )
	{
		m_lastpoints.push_back(newheight );
	}
	else
	{
		m_lastpoints.erase(m_lastpoints.begin() );
		m_lastpoints.push_back(newheight );
	}
	float accum = 0;

	for( int i = 0; i < m_lastpoints.size() ;i ++ )
	{
		accum += m_lastpoints[i];
	}
	accum /= (float)m_lastpoints.size();
	return accum;
}



float SmoothCarRolling( float newRollingangle )
{
	
	if( m_lastdecline.size() < declinetacumamount )
	{
		m_lastdecline.push_back(newRollingangle );
	}
	else
	{
		m_lastdecline.erase(m_lastdecline.begin() );
		m_lastdecline.push_back(newRollingangle );
	}
	float accum = 0;

	for( int i = 0; i < m_lastdecline.size() ;i ++ )
	{
		accum += m_lastdecline[i];
	}
	accum /= (float)m_lastdecline.size();
	return accum;
}

void RenderMainCar()
{
	KeyEventCarMove();

//	SettingCameraInfo();

    car_front_vector_set();             //�ڵ��� �պκ� ���⺤�� ���� 
   
	//����Ű�� �� ���� ������ ���ߴ� ��. 
	g_velocityCar = g_velocityCar * g_registCar;              

	//�ڵ��� ��ġ = ���� ��ġ + �ڵ��� �չ��⺤�� * ��Į��(�ε� �յ� �̵��� g_velocityCar�� ��������. g_velocityCar�� ���� �� �ڷΰ��°�.! �� �ڷ� ������ �����ߵ�  )  
	g_posMainCar += g_vecDirCar * g_velocityCar;            

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// �浹������ ���
	// ���⼭ map block ������ ���ؼ� block 0 �κκ����� �����ϸ� ��ġ ���� X �ӵ��� ����ӵ��� 30% �� ���ӵ��� �־ �ڷ� ƨ�ܳ�����.
	
	glMatrixMode(GL_MODELVIEW);            
    glPushMatrix();
	// �ڵ����� ���������ϴ� �Լ� ���� 
	float carheight = TerrainHeightbyRealPosition(g_posMainCar.x, g_posMainCar.z ); 
	carheight = SmoothCarheight(carheight);
    glTranslatef(g_posMainCar.x, g_posMainCar.y + 5 +carheight, g_posMainCar.z);
   glRotatef(-obj_ry, 0, 1, 0);    //�� ���� ������  
	
		glPushMatrix();  
			float declineamount = SmoothCarRolling(ProcessCarRolling() );
			glRotatef(270 + declineamount , 1, 0, 0);
			g_MainCar.Draw(1.5); // �̼��ڸ� �ٲٸ� �ڵ��� ũ�Ⱑ �޶���. !!assimp Ȱ�� �ϴ� �ſ� ��������!!!! 

		glPopMatrix();

	glPopMatrix();
}

//��� ��ü�� �������� ��ġ�ϰ� �ϴ� �Լ� . 
void SetUpTreesPosition()
{
	
	// �����Լ�
	srand((unsigned)time(NULL));

	for (int n = 0; n < g_nObjectMaxCount; n++)
	{

		float newX = rand() % (int) g_TerrainWidthX - g_TerrainWidthX/2;
		float newZ = rand()% (int) g_TerrainLenghthZ  - g_TerrainLenghthZ/2;
		float newY = TerrainHeightbyRealPosition( newX, newZ ) +2;

		sMapObjectInfo item; 
		item.type = rand() % numTree; //���� �޽��� ����  
		item.pos.x = float(newX);
		item.pos.y = newY;
		item.pos.z = float(newZ) ; 

		g_vecMapObjectInfo.push_back(item);
	}
}
//��水ü�� �׸��� �Լ�!
void RenderTrees()
{ 
	int nSize = int(g_vecMapObjectInfo.size()); 
	for (int n = 0; n < nSize; n++)
	{
		glPushMatrix();
		vec3 pos = g_vecMapObjectInfo[n].pos;
		glTranslatef( pos.x, pos.y, pos.z);
		if (g_vecMapObjectInfo[n].type == 0 || g_vecMapObjectInfo[n].type == 1) {
			g_Trees[g_vecMapObjectInfo[n].type].Draw(3);
		}
		else {
		g_Trees[g_vecMapObjectInfo[n].type].Draw(4);			
		}

	//	g_Trees[2].Draw(7);
		glPopMatrix();
	}

	
}

