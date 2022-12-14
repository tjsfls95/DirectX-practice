//전역 상수버퍼
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

//타입 정의
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

//버텍스 쉐이더
PixelInputType ColorVertexShader(VertexInputType input)
{
	PixelInputType output;

	//적절한 행렬 계산을 위해 위치벡터를 4단위로 변경
	input.position.w = 1.0f;

	//world 좌표,뷰 및 투영 행렬(projection)에 대한 vertex의 위치 계산
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	//픽셀 쉐이더에 사용하는 입력 색상 저장
	output.color = input.color;

	return output;
}

/*float4 main(float4 pos : POSITION) : SV_POSITION
{
	return pos;
}*/