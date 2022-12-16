#include "cameraclass.h"

CameraClass::CameraClass()
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;
}

CameraClass::CameraClass(const CameraClass& other)
{

}

CameraClass::~CameraClass()
{

}

void CameraClass::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
}

void CameraClass::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
}

XMFLOAT3 CameraClass::GetPosition()
{
	return XMFLOAT3(m_positionX, m_positionY, m_positionZ);
}

XMFLOAT3 CameraClass::GetRotation()
{
	return XMFLOAT3(m_rotationX, m_rotationY, m_rotationZ);
}

void CameraClass::Render()
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	//������ ����Ű�� ���� ����
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	//XMVECTOR ����ü�� �ε�
	upVector = XMLoadFloat3(&up);

	//3D���忡�� ī�޶��� ��ġ�� �����մϴ�
	position = XMFLOAT3(m_positionX, m_positionY, m_positionZ);

	//XMVECTOR ����ü�� �ε�
	positionVector = XMLoadFloat3(&position);

	//ī�޶� ���� �⺻���� ����
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	lookAtVector = XMLoadFloat3(&lookAt);

	//ȸ������ �������� ����
	pitch = m_rotationX * XMConvertToRadians(1.0);
	yaw = m_rotationY * XMConvertToRadians(1.0);
	roll = m_rotationZ * XMConvertToRadians(1.0);

	//yaw,pitch,roll�� ���� ȸ����� ����
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	//lookAt �� up ���͸� ȸ����ķ� �����Ͽ� �䰡 �������� �ùٸ��� ȸ���ϵ��� �Ѵ�
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	//ȸ�� �� ī�޶� ��ġ�� ��� ��ġ�� ��ȯ
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	//���������� �� ���� ������Ʈ �� ���Ϳ��� �� ����� ����ϴ�
	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector);
}


void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
}

