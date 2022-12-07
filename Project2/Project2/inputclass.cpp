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

void InputClass::KeyDown(unsigned int input)
{
	//Ű�� ������ ���¶�� Ű ���� �迭�� ����
	m_keys[input] = true;
	return;
}

void InputClass::KeyUp(unsigned int input)
{
	//Ű�� ���� ���¶�� Ű ���� �迭 �ʱ�ȭ
	m_keys[input] = false;
	return;
}

bool InputClass::IsKeyDown(unsigned int key)
{
	//Ű�� ���������� �ƴ��� ���¸� ��ȯ
	return m_keys[key];
}