//////////////////////////////////////////////////////
//
//  Demo.fx : ���̴� ���� �ڵ�
//
//////////////////////////////////////////////////////

// ��� ���� Constant Buffer Variables
cbuffer ConstBuffer //: register(b0) // ������� 0���� ��� (�ִ� 14��. 0~13)
{
	matrix mTM;     // ���� ���. 
	matrix mView;   // �� ��ȯ ���. 
	matrix mProj;   // ���� ��ȯ ���. 
}


//VS ��� ����ü.
struct VSOutput
{
	float4 pos : SV_POSITION;		// [���] (��ȯ��) ���� ��ǥ 
	float4 col : COLOR0;			// [���] (����) ������ : "Diffuse"
};

VSOutput VS_Main(
	float4 pos : POSITION,		// [�Է�] ������ǥ. Vertex Position (Model Space, 3D)
	float4 col : COLOR0			// [�Է�] ������ Vertex Color	: "Diffuse"	
)
{
	pos.w = 1;

	// ���� ��ȯ.(World Transform) 
	pos = mul(pos, mTM);        //pos = pos * mTM

								//�þ�-�� ��ȯ (View Transform)
	pos = mul(pos, mView);

	// ���� ���� ��ȯ (Projection Transform)
	pos = mul(pos, mProj);


	// ���� ���.
	VSOutput o = (VSOutput)0;
	o.pos = pos;
	o.col = col;

	// ������ ��� : ��µ� ����� ���� '��������' �� ���� �˴ϴ�
	// ���� ���̴��� '��ġ'�� �ݵ�� ����ؾ� �մϴ�.
	return o;
}

float4 PS_Main(
	float4 pos : SV_POSITION,   // [�Է�] (������) �ȼ��� ��ǥ. (Screen, 2D)
	float4 col : COLOR0         // [�Է�] (������) �ȼ��� ����. (Vertex Color : "Diffuse")
) : SV_TARGET					// [���] ����.(�ʼ�), "����Ÿ��" ���� ����մϴ�.
{
	return col;
}