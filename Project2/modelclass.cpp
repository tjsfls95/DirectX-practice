#include "modelclass.h"

ModelClass::ModelClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer;
}

ModelClass::ModelClass(const ModelClass& other)
{

}

ModelClass::~ModelClass()
{

}

bool ModelClass::Initialize(ID3D11Device* device)
{
	bool result;

	//삼각형의 기하적 정보를 가지고 있는 버텍스와 인덱스 버퍼를 초기화
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	return true;
}

void ModelClass::Shutdown()
{
	//버텍스와 인덱스 버퍼 해제
	ShutdownBuffers();
}

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	//그리기 위해 준비하는 그래픽 파이프라인에
	//버텍스와 인덱스 버퍼 삽입
	RenderBuffers(deviceContext);
}

int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	//버택스 배열에 버택스의 갯수 설정
	m_vertexCount = 3;

	//인덱스 배열의 인덱스의 갯수 설정
	m_indexCount;

	//버텍스 배열 생성
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	//인덱스 배열 생성
	indices = new unsigned long[m_indexCount];
	if (indices)
	{
		return false;
	}

	//데이터와 함께 버텍스 배열 불러오기
	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f); //왼쪽 아래
	vertices[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f); //중간 위
	vertices[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f); //오른쪽 아래
	vertices[2].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	//데이터와 함께 인덱스 배열 불러오기
	indices[0] = 0; //왼쪽 아래
	indices[1] = 1; //중간 위
	indices[2] = 2; //오른쪽 아래

	//정적 버텍스 버퍼의 구조체를 세팅한다
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//하위 리소스 구조체에 버텍스 데이터에 대한 포인터를 준다
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//Now create the vertex buffer
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//정적 인덱스 버퍼의 구조체는 설정한다
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	//하위 리소스 구조체에 인덱스 데이터에 대한 포인터를 준다
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	//인덱스 버퍼 생성
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//생성되고 값이 설정된 버텍스 버퍼와 인덱스 버퍼를 해제
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;
	

	return true;
}

void ModelClass::ShutdownBuffers()
{
	//인덱스 버퍼 해제
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	//버텍스 버퍼
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
}

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	//버텍스 버퍼의 단위와 오프셋 설정
	stride = sizeof(VertexType);
	offset = 0;

	//렌더링 할수 있게 입력 어셈블러에 있는 버텍스 버퍼를 활성화하도록 설정
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	//렌더링 할 수 있게 입력 어셈블러에 있는 인덱스 버퍼를 활성화한다
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//버텍스 버퍼로 그릴 기본형을 설정,여기서는 삼각형으로 설정
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


