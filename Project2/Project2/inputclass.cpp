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