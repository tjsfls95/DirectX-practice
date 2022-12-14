#include "graphicsclass.h"

GraphicsClass::GraphicsClass()
{
	m_Direct3D = 0;
}

GraphicsClass::GraphicsClass(const GraphicsClass& other)
{

}

GraphicsClass::~GraphicsClass()
{

}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	//Direct3D ������Ʈ ����
	//m_Direct3D = new D3DClass;
	m_Direct3D = (D3DClass*)_aligned_malloc(sizeof(D3DClass), 16);
	if (!m_Direct3D)
	{
		return false;
	}

	//Direct3D ������Ʈ �ʱ�ȭ
	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	return true;
}

void GraphicsClass::Shutdown()
{
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		_aligned_free(m_Direct3D);
		m_Direct3D = 0;
	}
}

bool GraphicsClass::Frame()
{
	bool result;

	//�׷��� �� ������
	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool GraphicsClass::Render()
{
	//���� �����ϸ� ���۸� ����
	m_Direct3D->BeginScene(0.5f, 0.5f, 0.5f, 1.0f);

	//ȭ�鿡 �������� ���� ���
	m_Direct3D->EndScene();

	return true;
}