//���� �������
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

//Ÿ�� ����
struct VertexInputType
{
	float4 position : POSITION;
	float4 color : COLOR;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

//���ؽ� ���̴�
PixelInputType ColorVertexShader(VertexInputType input)
{
	PixelInputType output;

	//������ ��� ����� ���� ��ġ���͸� 4������ ����
	input.position.w = 1.0f;

	//world ��ǥ,�� �� ���� ���(projection)�� ���� vertex�� ��ġ ���
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	//�ȼ� ���̴��� ����ϴ� �Է� ���� ����
	output.color = input.color;

	return output;
}

/*float4 main(float4 pos : POSITION) : SV_POSITION
{
	return pos;
}*/