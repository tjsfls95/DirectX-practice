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

	//위쪽을 가리키는 벡터 설정
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	//XMVECTOR 구조체에 로드
	upVector = XMLoadFloat3(&up);

	//3D월드에서 카메라의 위치를 설정합니다
	position = XMFLOAT3(m_positionX, m_positionY, m_positionZ);

	//XMVECTOR 구조체에 로드
	positionVector = XMLoadFloat3(&position);

	//카메라 방향 기본값을 설정
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	lookAtVector = XMLoadFloat3(&lookAt);

	//회전값을 라디언값으로 설정
	pitch = m_rotationX * XMConvertToRadians(1.0);
	yaw = m_rotationY * XMConvertToRadians(1.0);
	roll = m_rotationZ * XMConvertToRadians(1.0);

	//yaw,pitch,roll을 통해 회전행렬 생성
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	//lookAt 및 up 벡터를 회전행렬로 변형하여 뷰가 원점에서 올바르게 회복하도록 한다
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	//회전 된 카메라 위치를 뷰어 위치로 변환
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	//마지막으로 세 개의 업데이트 된 벡터에서 뷰 행렬을 만듭니다
	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector);
}


void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
}

