#include "systemclass.h"

SystemClass::SystemClass()
{
	m_Input = 0;
	m_Graphics = 0;
}

SystemClass::SystemClass(const SystemClass& other)
{

}

SystemClass::~SystemClass()
{

}

bool SystemClass::Initialize()
{
	//스크린 크기 설정
	int screenWidth, screenHeight;
	//실행결과 변수
	bool result;

	//함수에 변수 전달 받기 전에 값을 0으로 초기화
	screenWidth = 0;
	screenHeight = 0;

	//windows api 실행(int&,int&)형
	InitializeWindows(screenWidth, screenHeight);

	//키 입력받을 입력 오브젝트 생성
	m_Input = new InputClass;
}