//
//
// Demo.fx : �⺻ ���̴� �ҽ�.
//
// 2003.11.4. Kihong Kim / mad_dog@hanmail.net
// 2010.12.01. Update. (DX11.Jun.2010)
// 2016.12.27. Update. (DX11. Windows SDK 8.1)
//


// ��� ����
cbuffer cbDEFAULT : register(b0)
{
    matrix mTM;     //���� ���. 
    matrix mView;   //�� ��ȯ ���. 
    matrix mWV;     //����-�� ��ȯ ���. 
    matrix mProj;   //���� ��ȯ ���. 
};


//���� ������ ������ۡ�
cbuffer cbLIGHT : register(b1)
{
    float3 LitDirection; //���� ����.
    float4 LitDiffuse;   //�� ���� : Ȯ�걤 Diffuse Light.
    float4 LitAmbient;   //���� ���� : �ֺ��� Ambient Light.
    float  LitRange;     //�� ���� �Ÿ�.
    bool   LitOn;        //���� ���뿩��.
};

//���� ������ ������ۡ�
cbuffer cbMATERIAL : register(b2)
{
    float4 MtrlDiffuse;  //�� ����(Ȯ�걤) �� �ݻ���(%) 
    float4 MtrlAmbient;  //���� ����(�ֺ���) �� �ݻ���(%) 
};



 
//VS ��� ����ü.
struct VSOutput
{
    float4 pos : SV_POSITION;
    float4 col : COLOR0;
};


// ���� ó�� �Լ�.��
float4 Light(float4 nrm);




////////////////////////////////////////////////////////////////////////////// 
//
// Vertex Shader Main : ���� ���̴� ���� �Լ�.
//
////////////////////////////////////////////////////////////////////////////// 

VSOutput VS_Main(
				  float4 pos : POSITION,    //[�Է�] ������ǥ. Vertex Position (Model Space, 3D)
				  float4 nrm : NORMAL       //[�Է�] ��� normal ��
				)
{
	//* �Ʒ��� �׽�Ʈ�� �����ϱ� ����  
    //* VS �� ��� ���۰� �����Ǿ� �־�� �մϴ�.    
    pos.w = 1;

	//���� ��ȯ.(World Transform) 
    pos = mul(pos, mTM);        //pos = pos * mTM
	
    //�þ�-�� ��ȯ (View Transform)
    pos = mul(pos, mView);
 
    //���� ���� ��ȯ (Projection Transform)
    pos = mul(pos, mProj);

    //���� ���.(Lighting)��
    float4 diff = 1;    
    if(LitOn) diff = Light(nrm);


    //���� ���.
    VSOutput o = (VSOutput) 0;
    o.pos = pos; 
    o.col = diff;

    return o;
}




////////////////////////////////////////////////////////////////////////////// 
//
// ���� ��� : ����Ʈ ������ �� ����. Lambert Lighting Model
//          : �� ���� View Space ���� ó��.
float4 Light(float4 nrm)
{
    float4 N = nrm;    N.w = 0;
    float4 L = float4(LitDirection, 0);

    //��������� ������ ��ȯ.
    N = mul(N, mWV);
    L = mul(L, mView);

    //���� ��� 
    float4 diff = max(dot(N, L), 0) * LitDiffuse * MtrlDiffuse;
    float4 amb = LitAmbient * MtrlAmbient;
    
    return diff + amb;
}




////////////////////////////////////////////////////////////////////////////// 
//
// Pixel Shader Main : �ȼ� ���̴� ���� �Լ�.
//
////////////////////////////////////////////////////////////////////////////// 

float4 PS_Main(
				float4 pos : SV_POSITION,   //[�Է�] (������) �ȼ��� ��ǥ. (Screen, 2D)
				float4 col : COLOR0         //[�Է�] (������) �ȼ��� ����. (Pixel Color : "Diffuse")
				) : SV_TARGET               //[���] ����.(�ʼ�), "����Ÿ��" ���� ����մϴ�.
{
	//Ư���� ��ȯ ���� �Էµ� ������ �״�� ����մϴ�.
    //...
	 
    return col;
}



/**************** end of file "Demo.fx" ***********************/
