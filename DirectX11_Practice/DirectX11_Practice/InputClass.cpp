#include "stdafx.h"
#include "InputClass.h"

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
	//키 배열을 초기화
	for (int i = 0; i < 256; i++)
	{
		m_keys[i] = false;
	}
}

void InputClass::KeyDown(unsigned int input)
{
	//키가 눌렸다면 해당 키값을 true로 저장
	m_keys[input] = true;
}

void InputClass::KeyUp(unsigned int input)
{
	//키가 해제되었다면 해당 키값을 false로 저장
	m_keys[input] = false;
}

bool InputClass::IsKeyDown(unsigned int key)
{
	//현재 키값이 눌러졌는지 아닌지 상태를 반환
	return m_keys[key];
}