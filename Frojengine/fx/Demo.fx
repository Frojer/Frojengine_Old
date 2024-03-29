//
//
// Demo.fx : 기본 셰이더 소스.
//
// 2003.11.4. Kihong Kim / mad_dog@hanmail.net
// 2010.12.01. Update. (DX11.Jun.2010)
// 2016.12.27. Update. (DX11. Windows SDK 8.1)
//


// 상수 버퍼
cbuffer cbDEFAULT : register(b0)
{
	matrix mTM;     //월드 행렬. 
	matrix mView;   //뷰 변환 행렬. 
	matrix mWV;     //월드-뷰 변환 행렬. 
	matrix mProj;   //투영 변환 행렬. 
};


//조명 정보용 상수버퍼★
cbuffer cbLIGHT : register(b1)
{
	float3 LitDirection; //빛의 방향.
	float4 LitDiffuse;   //주 광량 : 확산광 Diffuse Light.
	float4 LitAmbient;   //보조 광량 : 주변광 Ambient Light.
	float  LitRange;     //빛 도달 거리.
	bool   LitOn;        //조명 적용여부.
};

//재질 정보용 상수버퍼★
cbuffer cbMATERIAL : register(b2)
{
	float4 MtrlDiffuse;  //주 광량(확산광) 의 반사율(%) 
	float4 MtrlAmbient;  //보조 광량(주변광) 의 반사율(%) 
};




//VS 출력 구조체.
struct VSOutput
{
	float4 pos : SV_POSITION;
	float4 col : COLOR0;
};


// 조명 처리 함수.★
float4 Light(float4 nrm);




////////////////////////////////////////////////////////////////////////////// 
//
// Vertex Shader Main : 정점 셰이더 메인 함수.
//
////////////////////////////////////////////////////////////////////////////// 

VSOutput VS_Main(
	float4 pos : POSITION,    //[입력] 정점좌표. Vertex Position (Model Space, 3D)
	float4 nrm : NORMAL,       //[입력] 노멀 normal ★
	float3 tex : TEXCOORD
)
{
	//* 아래의 테스트를 수행하기 전에  
	//* VS 에 상수 버퍼가 설정되어 있어야 합니다.    
	pos.w = 1;

	//월드 변환.(World Transform) 
	pos = mul(pos, mTM);        //pos = pos * mTM

								//시야-뷰 변환 (View Transform)
	pos = mul(pos, mView);

	//원근 투영 변환 (Projection Transform)
	pos = mul(pos, mProj);

	//조명 계산.(Lighting)★
	float4 diff = 1;
	if (LitOn) diff = Light(nrm);


	//정보 출력.
	VSOutput o = (VSOutput)0;
	o.pos = pos;
	o.col = diff;

	return o;
}




////////////////////////////////////////////////////////////////////////////// 
//
// 조명 계산 : 램버트 라이팅 모델 적용. Lambert Lighting Model
//          : 뷰 공간 View Space 기준 처리.
float4 Light(float4 nrm)
{
	float4 N = nrm;    N.w = 0;
	float4 L = float4(LitDirection, 0);

	//뷰공간으로 정보를 변환.
	N = mul(N, mWV);
	L = mul(L, mView);

	//조명 계산 
	float4 diff = max(dot(N, L), 0) * LitDiffuse * MtrlDiffuse;
	float4 amb = LitAmbient * MtrlAmbient;

	return diff + amb;
}




////////////////////////////////////////////////////////////////////////////// 
//
// Pixel Shader Main : 픽셀 셰이더 메인 함수.
//
////////////////////////////////////////////////////////////////////////////// 

float4 PS_Main(
	float4 pos : SV_POSITION,   //[입력] (보간된) 픽셀별 좌표. (Screen, 2D)
	float4 col : COLOR0         //[입력] (보간된) 픽셀별 색상. (Pixel Color : "Diffuse")
) : SV_TARGET               //[출력] 색상.(필수), "렌더타겟" 으로 출력합니다.
{
	//특별한 변환 없이 입력된 정보를 그대로 출력합니다.
	//...

	return col;
}



/**************** end of file "Demo.fx" ***********************/
