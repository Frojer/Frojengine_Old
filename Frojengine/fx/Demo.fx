//////////////////////////////////////////////////////
//
//  Demo.fx : 셰이더 연습 코드
//
//////////////////////////////////////////////////////

// 상수 버퍼 Constant Buffer Variables
cbuffer ConstBuffer //: register(b0) //상수버퍼 0번에 등록 (최대 14개. 0~13)
{
	matrix mTM;     //월드 행렬. 
	matrix mView;   //뷰 변환 행렬. 
	matrix mProj;   //투영 변환 행렬. 
}


//VS 출력 구조체.
struct VSOutput
{
	float4 pos : SV_POSITION;		// [출력] (변환된) 정점 좌표 
	float4 col : COLOR0;			// [출력] (계산된) 정점색 : "Diffuse"
};

////////////////////////////////////////////////////////////////////////////// 
//
//  Vertex Shader Main : 정점 셰이더 메인 함수.
//
//  바른 렌더링 결과를 위해서는 아래의 조건이 동일 또는 호환되어야 합니다.
//  1.정점 버퍼의 데이터.  Vertex Buffer Data
//  2.정점 입력 구조 "Vertex format" (Input Layout)
//  3.셰이더 함수의 입력구조.  Vertex Shader (Input Layout)
//  4.각종 변환 처리 Vertex Transform
//
//  [주의] 각 셰이더의 진입점 (메인)함수 (VS_Main, PS_Main..) 는 시멘틱(Semantics) 지정이 필요합니다
//  시멘틱은 셰이더에 공급/출력될 데이터와  GPU 내 연산 메모리(레지스터 Register) 와의 '연결성 Binding' 을 보다 명확하게 하기 위함입니다.
//  셰이더 어셈블러 Assember 를 사용하던 시대에는 불필요한 기능입니다만, DX10 부터는 셰이더 어셈플러가 더이상 지원되지 않습니다. 
//
//  엔진(어플리케이션)에서  Draw 가 호출되면 전체 렌더링 파이프라인이 동작하며,
//  정점버퍼로부터 각 정점의 정보가 1세트씩 정점 셰이더로 공급됩니다.
//  정점셰이더는 정점별(Per-Vertex) 연산을 수행함을 기억합시다.
//
////////////////////////////////////////////////////////////////////////////// 

VSOutput VS_Main(
	float4 pos : POSITION,		// [입력] 정점좌표. Vertex Position (Model Space, 3D)
	float3 nor : NORMAL,		// [입력] 노멀값
	float3 tex : TEXCOORD0		// [입력] 텍스쳐 좌표
	//float4 col : COLOR0			// [입력] 정점색 Vertex Color	: "Diffuse"	
)
{
	// 정점 셰이더에서 수행할 대표적인 작업 리스트.
	/*
	// [1] 정점변환 (Vertex Transform)		(현재)
	// [2] 조명 (Per-Vertex Lighting)
	// [3] 안개 (Vertex Fog)
	// [4] 스키닝 (Skinning), 정점혼합 (Vertex Blending)
	// [5] 텍스처 애니메이션 (Texture Animation)
	// [6] 그림자 (Shadow)
	// [7] 특수효과... 등등..
	*/

	//* 아래의 테스트를 수행하기 전에  
	//* VS 에 상수 버퍼가 설정되어 있어야 합니다.    
	pos.w = 1;

	//월드 변환.(World Transform) 
	pos = mul(pos, mTM);        //pos = pos * mTM

								//시야-뷰 변환 (View Transform)
	pos = mul(pos, mView);

	//원근 투영 변환 (Projection Transform)
	pos = mul(pos, mProj);


	//정보 출력.
	VSOutput o = (VSOutput)0;
	o.pos = pos;
	o.col = float4(1.0f, 1.0f, 1.0f, 1.0f);

	// 데이터 출력 : 출력된 결과는 다음 '스테이지' 로 전달 됩니다
	// 정점 셰이더는 '위치'를 반드시 출력해야 합니다.
	return o;
}



////////////////////////////////////////////////////////////////////////////// 
//
//  Pixel Shader Main : 픽셀 셰이더 메인 함수.
//
////////////////////////////////////////////////////////////////////////////// 
float4 PS_Main(
	float4 pos : SV_POSITION,   // [입력] (보간된) 픽셀별 좌표. (Screen, 2D)
	float4 col : COLOR0         // [입력] (보간된) 픽셀별 색상. (Vertex Color : "Diffuse")
) : SV_TARGET					// [출력] 색상.(필수), "렌더타겟" 으로 출력합니다.
{
	// 픽셀 셰이더에서 수행할 대표 작업 리스트.
	/*
	// [1] 색상 혼합 (Color Blending)
	// [2] 텍스처 혼합 (Texture Blending)
	// [3] 조명 (Per-Pixel Lighting)
	// [4] 안개 (Pixel Fog)
	// [5] 그림자 (Shadow)
	// [6] 특수효과... 등등..
	*/

	// 지역 변수. 색상값은 4성분 벡터로 표현 합니다. { R, G, B, A } 순.
	// 아래와 같은 다양한 방식으로 초기화가 가능합니다.
	//float4 color = 1;                       // color = { 1, 1, 1, 1 } 와 동일의미. "100% 흰색"
	//float4 color = { 1, 0, 0, 1 };        // color = { 1, 0, 0, 1 }  "100% 빨강"
	//float4 color = float4(1, 0, 0, 1);    // color = { 1, 0, 0, 1 }  "100% 빨강", 생성자 타입 초기화.
	//float4 color = { 0.5f, 0, 0, 1 };     // color = { 0.5, 0, 0, 1 }  "50% 빨강"     

	/*//변수 성분별 접근법
	// 마지막 성분 Alpha 채널은 '불투명Opacity' 로 사용되며 기본값은 1 (100%) 입니다.
	color.r = 1;
	color.g = 0;
	color.b = 0;
	color.a = 1;
	*/

	// [테스트1] 외부-렌더링 파이프라인에서 공급된 색상을 (그대로) 출력
	//color = col;            

	// [테스트2] 셰이더 내부에서 계산후 출력하기
	//color = float4(1, 1, 0, 1);       //'100%' 노랑.. 

	// [테스트3] 색상 혼합 테스트
	//color = col + float4(1, 0, 0, 1);     // Red  강조.
	//color = col - float4(1, 0, 0, 1);     // Red  빼기
	//color = col * float4(1, 0, 0, 1);     // Red  만 남기기.
	//color = col * 0.5f;                   // 모든 채널 50% 남기기.
	//color = col + 0.5f;                   // 모든 채널 50% 증가.
	//color = col + float4(1, 1, 1, 1);     // 모든 채널 100% 증가. (결과는..??)
	//color = col + 1;                      // 모든 채널 100% 증가. 상동표현.
	//color = 1 - col;                      // 색상 반전.


	// [계산 결과] 색상 출력. 
	return col;
}