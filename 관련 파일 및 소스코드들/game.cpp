
#include "Unifiedheader.h"
#include "time.h"
#include<glm/gtx/vector_angle.hpp>
#include <math.h> 

//드디어 assimp 사용완료!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1ㅜㅜㅜㅜㅜㅜㅜㅜㅜㅜㅜㅜㅜㅜㅜㅜㅜㅜㅜㅜㅜㅜㅜㅜㅜㅜ

float obj_rx, obj_rz;
float obj_x, obj_y, obj_z;
float obj_ry = 90.0;
 
float g_CameraLength = 25.f;
glm::vec3 g_posCameraEye; // 카메라 위치

vector<float> m_lastpoints;
const int heightacumamount = 10; // 그냥하면 차가 요동을 치는데 지형높이 로드할때 버그인지 확실치 않아서. 그냥 부드럽게 보간해줌. 숫자를 늘이면 덜떨고 줄이면 역동적으로!! 


vector<float> m_lastdecline;
const int declinetacumamount = 20; //  상동 차의 기울기를 부드럽게.

#define PI 3.141592653589
 
//////////////////////////////////////////////////////////////////////
// 자동차 모델 정보



//GLMmodel* g_pModelInfo = NULL;
glm::vec3 g_posMainCar(211, 0, 434 - 30);         //자동차 위치

glm::vec3 g_vecDirCar(1.0, .0, 1.0);   //자동차 방향벡터
float g_velocityCar = 0.01f;                       //자동차 속력 스칼라 
float g_velocityInterval = 0.05f;                       //자동차 가속력
float g_registCar = 0.98;					// 자동차 감속 저항
float g_maxSpeed = 12.0;				// 자동차 최대 속도

BOOL g_cockpitMode = false; // 조종사 모드

BOOL g_goalMode = false; // 출발점 통과 플러그

// 오브젝트 (배경객체)
struct sMapObjectInfo
{
	int type; // 객체들에 대한 idx 
	glm::vec3 pos; // 위치



	sMapObjectInfo()
	{
		type = -1; 
		pos.x = 0;
		pos.y = 0;
		pos.z = 0; 
		//modelData = NULL;	//필요없어서 일단 주석
	}
};  

//////////////////////////////////////////////////////////////////////


const int		 g_nObjectMaxCount = 50; // 각각 객체 100개씩 
std::vector<sMapObjectInfo> g_vecMapObjectInfo; // 배경객체 벡터 

//////////////////////////////////////////////////////////////////////


void car_front_vector_set() 
{               //자동차의 앞부분 방향벡터. (뒤로가기 할 때는 방향벡터 안바뀐다는 뜻.!!)
	g_vecDirCar.x = 0;
	g_vecDirCar.y = 0;
	g_vecDirCar.z = 0;

    g_vecDirCar.x = sin(obj_ry * PI / 180);
    g_vecDirCar.z = -cos(obj_ry * PI  / 180);
    g_vecDirCar = glm::normalize(g_vecDirCar);
}


//자동차 따라다니는 카메라 세팅 
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
	//float fUpDistance = g_CameraLength * 0.4 * 5;		// 테스트용 

	float fUpDistance = g_CameraLength  * 3;

	if (g_cockpitMode)
	{
		fDirDistance = g_CameraLength;
		fUpDistance = g_CameraLength * 0.25;
	}  
	
	g_posCameraEye = g_posMainCar - (g_vecDirCar * fDirDistance) + (vecUp * fUpDistance); 
	//printf("g_posMainCar : %.3f %.3f %.3f\n", g_posMainCar.x, g_posMainCar.z, g_posMainCar.y);			//오류 수정용 출력코드 ㅜㅜ
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

	// up이 부정확할 수 도 ㅇ.  그냥 안전하게 해당 식으로 . 
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
		obj_ry -= 1.3;                  //회전각                     
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



// 2차원이니까 2차원으로 구해야된다.!! 주의 

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

    car_front_vector_set();             //자동차 앞부분 방향벡터 설정 
   
	//방향키에 손 떼면 서서히 멈추는 것. 
	g_velocityCar = g_velocityCar * g_registCar;              

	//자동차 위치 = 기존 위치 + 자동차 앞방향벡터 * 스칼라(인데 앞뒤 이동은 g_velocityCar로 움직여짐. g_velocityCar가 음수 면 뒤로가는것.! 즉 뒤로 못가게 구현했따  )  
	g_posMainCar += g_vecDirCar * g_velocityCar;            

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 충돌했을때 모션
	// 여기서 map block 정보와 비교해서 block 0 인부분으로 접근하면 위치 갱신 X 속도를 현재속도의 30% 로 역속도를 주어서 뒤로 튕겨나간다.
	
	glMatrixMode(GL_MODELVIEW);            
    glPushMatrix();
	// 자동차의 높이측정하는 함수 따로 
	float carheight = TerrainHeightbyRealPosition(g_posMainCar.x, g_posMainCar.z ); 
	carheight = SmoothCarheight(carheight);
    glTranslatef(g_posMainCar.x, g_posMainCar.y + 5 +carheight, g_posMainCar.z);
   glRotatef(-obj_ry, 0, 1, 0);    //차 방향 돌리기  
	
		glPushMatrix();  
			float declineamount = SmoothCarRolling(ProcessCarRolling() );
			glRotatef(270 + declineamount , 1, 0, 0);
			g_MainCar.Draw(1.5); // 이숫자를 바꾸면 자동차 크기가 달라짐. !!assimp 활용 하니 매우 쉬워졌음!!!! 

		glPopMatrix();

	glPopMatrix();
}

//배경 객체들 랜덤으로 위치하게 하는 함수 . 
void SetUpTreesPosition()
{
	
	// 랜덤함수
	srand((unsigned)time(NULL));

	for (int n = 0; n < g_nObjectMaxCount; n++)
	{

		float newX = rand() % (int) g_TerrainWidthX - g_TerrainWidthX/2;
		float newZ = rand()% (int) g_TerrainLenghthZ  - g_TerrainLenghthZ/2;
		float newY = TerrainHeightbyRealPosition( newX, newZ ) +2;

		sMapObjectInfo item; 
		item.type = rand() % numTree; //나무 메시의 갯수  
		item.pos.x = float(newX);
		item.pos.y = newY;
		item.pos.z = float(newZ) ; 

		g_vecMapObjectInfo.push_back(item);
	}
}
//배경객체들 그리는 함수!
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

