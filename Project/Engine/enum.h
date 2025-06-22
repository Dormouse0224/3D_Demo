#pragma once

enum class CB_TYPE
{
	TRANSFORM,	// b0
	MATERIAL,	// b1
	GLOBAL,		// b2

	END,
};


// Rasterizer State
enum class RS_TYPE
{
	CULL_BACK,	// cull_ccw, 뒷면(반시계 순서) 컬링
	CULL_FRONT, // cull_cw
	CULL_NONE,  // 컬링하지 않음
	WIRE_FRAME, // 컬링하지 않음, 

	END,
};

// DepthStencilState
enum class DS_TYPE
{
	LESS,				// 깊이판정 : 더 작아야 통과, 깊이 기록 O
	LESS_EQUAL,			// 깊이판정 : 더 작아야 같으면 통과, 깊이 기록 O

	GREATER,			// 깊이판정 : 더 멀어야 통과, 깊이 기록 0

	NO_WRITE,			// 깊이판정 : 더 작아야 통과(LESS), 깊이 기록 X
	NO_TEST_NO_WIRITE,	// 깊이판정 항상 성공(검사X), 깊이 기록 X

	END,
};

// BlendState
enum class BS_TYPE
{
	DEFAULT,	// Src : 1, Dst : 0
	ALPHABLEND, // Src : A, Dst : (1 - A)
	ALPHABLEND_COVERAGE,
	ONE_ONE,

	END,
};

// wstr 구조체 업데이트 할 것
enum class COMPONENT_TYPE
{
	TRANSFORM,
	CAMERA,
	PHYSXACTOR,
	FSM,

	// Rendering Component
	MESHRENDER,			// 가장 기본적인 RenderComponent
	SPRITERENDER,		// Sprite 하나 렌더링하는 컴포넌트
	FLIPBOOKRENDER,		// Flipbook 재생 컴포넌트
	TILERENDER,			// 타일맵 렌더링
	PARTICLERENDER,		// 입자 렌더링
	SKYBOX,				// 3D 배경
	DECAL,				// 문신
	LANDSCAPE,			// 3D 지형
	UICOM,

	// Additional
	LIGHT,

	COMPONENT_END,

	SCRIPT,				// 대본, 역할, 컨텐츠
};
extern std::wstring COMPONENT_TYPE_WSTR[];

enum class COMPONENT_TYPE_SAVE
{
	TRANSFORM,
	CAMERA,
	PHYSXACTOR,
	FSM,

	// Rendering Component
	MESHRENDER,			// 가장 기본적인 RenderComponent
	SPRITERENDER,		// Sprite 하나 렌더링하는 컴포넌트
	FLIPBOOKRENDER,		// Flipbook 재생 컴포넌트
	TILERENDER,			// 타일맵 렌더링
	PARTICLERENDER,		// 입자 렌더링
	SKYBOX,				// 3D 배경
	DECAL,				// 문신
	LANDSCAPE,			// 3D 지형
	UICOM,

	// Additional
	LIGHT,

	COMPONENT_END,

	SCRIPT,				// 대본, 역할, 컨텐츠
};
extern std::unordered_map<std::wstring, COMPONENT_TYPE_SAVE> COMPONENT_TYPE_SAVE_MAP;

enum class ASSET_TYPE
{
	MESH,
	MESHDATA,
	TEXTURE2D,
	MATERIAL,
	PREFAB,
	SOUND,
	GRAPHIC_SHADER,	// 렌더링
	COMPUTE_SHADER, // 계산, GP(General Purpose)
	SPRITE,
	FLIPBOOK,
	FSM_STATE,
	FONT,

	ASSET_END,
};
extern std::wstring ASSET_TYPE_WSTR[];


// 투영(Projection) 방식
enum PROJ_TYPE
{
	ORTHOGRAPHIC,	// 직교투영
	PERSPECTIVE,	// 원근투영
};


// 방향
enum class DIR
{
	RIGHT,
	UP,
	FRONT,
	END,
};

enum class SHADER_DOMAIN
{
	DOMAIN_QPAQUE,		// 불투명
	DOMAIN_MASKED,		// 불투명 + 완전 투명
	DOMAIN_TRANSPARENT, // 반투명 + 투명
	DOMAIN_POSTPROCESS,	// 후처리
	DOMAIN_UI,			// UI
	DOMAIN_DEBUG,		// 디버그 렌더링
	DOMAIN_SYSTEM,		// 카메라 분류에 사용되지 않음
};

enum CONST_PARAM
{
	INT_0,		INT_1,		INT_2,		INT_3,
	FLOAT_0,	FLOAT_1,	FLOAT_2,	FLOAT_3,
	VEC2_0,		VEC2_1,		VEC2_2,		VEC2_3,
	VEC4_0,		VEC4_1,		VEC4_2,		VEC4_3,
	MAT_0,		MAT_1,		MAT_2,		MAT_3,
};

enum TEX_PARAM
{
	TEX_0,
	TEX_1,
	TEX_2,
	TEX_3,
	TEX_4,
	TEX_5,

	TEX_END,
};


enum class DEBUG_SHAPE
{
	// 2D
	RECT,
	CIRCLE,
	LINE,

	// 3D
	CUBE,
	SPHERE,
};

enum COLLISION_LAYER
{
	ePLAYER				= (1<<0),
	eLANDSCAPE			= (1<<1),
	eMONSTER			= (1<<2),
	ePARTICLE			= (1<<3),

	END = 4
};
extern std::wstring COLLISION_LAYER_WSTR[COLLISION_LAYER::END];

// wstr 구조체 업데이트 할 것
enum class LAYER
{
	Default,
	Background,
	Tile,
	Player,
	PlayerProjectile,
	Enemy,
	EnemyProjectile,

	UI,

	END
};
extern std::wstring LAYER_WSTR[static_cast<int>(LAYER::END)];

enum class LIGHT_TYPE
{
	DIRECTIONAL,	// 방향성 광원, 전역 (태양, 달)
	POINT,			// 점광원, 전구, 전등, 횃불
	SPOT,			// 스포트라이트, 손전등

	END
};
extern std::wstring LIGHT_TYPE_WSTR[static_cast<int>(LIGHT_TYPE::END)];

enum MRT_TYPE
{
	DEFERRED,
	MERGE,

	MRT_END
};