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
	//��ũ�� ũ�� ����
	int screenWidth, screenHeight;
	//������ ����
	bool result;

	//�Լ��� ���� ���� �ޱ� ���� ���� 0���� �ʱ�ȭ
	screenWidth = 0;
	screenHeight = 0;

	//windows api ����(int&,int&)��
	InitializeWindows(screenWidth, screenHeight);

	//Ű �Է¹��� �Է� ������Ʈ ����
	m_Input = new InputClass;
}