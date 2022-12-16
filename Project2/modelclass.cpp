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

	//�ﰢ���� ������ ������ ������ �ִ� ���ؽ��� �ε��� ���۸� �ʱ�ȭ
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	return true;
}

void ModelClass::Shutdown()
{
	//���ؽ��� �ε��� ���� ����
	ShutdownBuffers();
}

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	//�׸��� ���� �غ��ϴ� �׷��� ���������ο�
	//���ؽ��� �ε��� ���� ����
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

	//���ý� �迭�� ���ý��� ���� ����
	m_vertexCount = 3;

	//�ε��� �迭�� �ε����� ���� ����
	m_indexCount;

	//���ؽ� �迭 ����
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	//�ε��� �迭 ����
	indices = new unsigned long[m_indexCount];
	if (indices)
	{
		return false;
	}

	//�����Ϳ� �Բ� ���ؽ� �迭 �ҷ�����
	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f); //���� �Ʒ�
	vertices[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f); //�߰� ��
	vertices[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f); //������ �Ʒ�
	vertices[2].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	//�����Ϳ� �Բ� �ε��� �迭 �ҷ�����
	indices[0] = 0; //���� �Ʒ�
	indices[1] = 1; //�߰� ��
	indices[2] = 2; //������ �Ʒ�

	//���� ���ؽ� ������ ����ü�� �����Ѵ�
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//���� ���ҽ� ����ü�� ���ؽ� �����Ϳ� ���� �����͸� �ش�
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//Now create the vertex buffer
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//���� �ε��� ������ ����ü�� �����Ѵ�
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	//���� ���ҽ� ����ü�� �ε��� �����Ϳ� ���� �����͸� �ش�
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	//�ε��� ���� ����
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//�����ǰ� ���� ������ ���ؽ� ���ۿ� �ε��� ���۸� ����
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;
	

	return true;
}

void ModelClass::ShutdownBuffers()
{
	//�ε��� ���� ����
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	//���ؽ� ����
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

	//���ؽ� ������ ������ ������ ����
	stride = sizeof(VertexType);
	offset = 0;

	//������ �Ҽ� �ְ� �Է� ������� �ִ� ���ؽ� ���۸� Ȱ��ȭ�ϵ��� ����
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	//������ �� �� �ְ� �Է� ������� �ִ� �ε��� ���۸� Ȱ��ȭ�Ѵ�
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//���ؽ� ���۷� �׸� �⺻���� ����,���⼭�� �ﰢ������ ����
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


