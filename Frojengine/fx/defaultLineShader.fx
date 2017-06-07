//////////////////////////////////////////////////////
//
//  Demo.fx : 셰이더 연습 코드
//
//////////////////////////////////////////////////////

// 상수 버퍼 Constant Buffer Variables
cbuffer ConstBuffer //: register(b0) // 상수버퍼 0번에 등록 (최대 14개. 0~13)
{
	matrix mTM;     // 월드 행렬. 
	matrix mView;   // 뷰 변환 행렬. 
	matrix mProj;   // 투영 변환 행렬. 
}


//VS 출력 구조체.
struct VSOutput
{
	float4 pos : SV_POSITION;		// [출력] (변환된) 정점 좌표 
	float4 col : COLOR0;			// [출력] (계산된) 정점색 : "Diffuse"
};

VSOutput VS_Main(
	float4 pos : POSITION,		// [입력] 정점좌표. Vertex Position (Model Space, 3D)
	float4 col : COLOR0			// [입력] 정점색 Vertex Color	: "Diffuse"	
)
{
	pos.w = 1;

	// 월드 변환.(World Transform) 
	pos = mul(pos, mTM);        //pos = pos * mTM

								//시야-뷰 변환 (View Transform)
	pos = mul(pos, mView);

	// 원근 투영 변환 (Projection Transform)
	pos = mul(pos, mProj);


	// 정보 출력.
	VSOutput o = (VSOutput)0;
	o.pos = pos;
	o.col = col;

	// 데이터 출력 : 출력된 결과는 다음 '스테이지' 로 전달 됩니다
	// 정점 셰이더는 '위치'를 반드시 출력해야 합니다.
	return o;
}

float4 PS_Main(
	float4 pos : SV_POSITION,   // [입력] (보간된) 픽셀별 좌표. (Screen, 2D)
	float4 col : COLOR0         // [입력] (보간된) 픽셀별 색상. (Vertex Color : "Diffuse")
) : SV_TARGET					// [출력] 색상.(필수), "렌더타겟" 으로 출력합니다.
{
	return col;
}