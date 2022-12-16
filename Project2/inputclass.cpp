#include "inputclass.h"

InputClass::InputClass()
{

}

InputClass::InputClass(const InputClass& other)
{

}

InputClass::~InputClass()
{

}

void InputClass::Initialize()
{
	int i;

	//누르지 않은 모든 키들 초기화
	for (i = 0; i < 256; i++)
	{
		m_keys[i] = false;
	}

	return;
}

void InputClass::KeyDown(unsigned int input)
{
	//키가 눌러진 상태라면 키 상태 배열에 저장
	m_keys[input] = true;
}

void InputClass::KeyUp(unsigned int input)
{
	//키가 떼진 상태라면 키 상태 배열 초기화
	m_keys[input] = false;
}

bool InputClass::IsKeyDown(unsigned int key)
{
	//키가 눌러졌는지 아닌지 상태를 반환
	return m_keys[key];
}