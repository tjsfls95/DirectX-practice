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

	//������ ���� ��� Ű�� �ʱ�ȭ
	for (i = 0; i < 256; i++)
	{
		m_keys[i] = false;
	}

	return;
}