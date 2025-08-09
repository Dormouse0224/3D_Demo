#pragma once



// Vertex
// 물체의 형태를 이루는 가장 기본 단위
struct Vtx
{
	Vec3	vPos;		// 정점의 위치
	Vec4	vColor;		// 정점의 색상	
	Vec2	vUV;		// 정점에 입혀질 텍스쳐의 위치(좌표)

	// 방향벡터
	Vec3	vTangent;	// 접선벡터
	Vec3	vNormal;	// 법선벡터
	Vec3	vBinormal;	// 종법선벡터

    // Skinning 용 데이터
    Vec4	vWeights;  // Bone 가중치
    Vec4	vIndices;  // Bone 인덱스
};

// DebugShapeInfo
struct tDebugShapeInfo
{
	DEBUG_SHAPE Shape;
	Vec3	    WorldPos;
	Vec3		WorldScale;
	Vec4		WorldRotation;
	Matrix		MatWorld;
	Vec4		Color;
	float		CurTime;
	float		Duration;
	bool		DepthTest;
};


// =============
// Particle 관련
// =============
struct tParticle
{
	Vec4		LocalPos;		// 소유 오브젝트로 부터의 상대 좌표 (World 공간계인 경우 셰이더에서 ObjectPos 를 더함)
	Vec4		WorldScale;		// 파티클 월드 크기
	Vec4		Color;			// 파티클 색상

	Vec4		Force;			// 파티클에 주어지고 있는 힘의 총합
	Vec4		Velocity;		// 파티클 속도
	float		Mass;			// 파티클 질량

	float		Age;			// 파티클 나이, Age 가 Life 에 도달하면 수명이 다한 것
	float		Life;			// 파티클 최대 수명
	float		NormalizedAge;  // 전체 수명 대비, 현재 Age 비율. 자신의 Age 를 Life 대비 정규화 한 값

	int			Active;			// 파티클 활성화 여부
	int			PrevActive;		// 이전 프레임에서 파티클 활성화 여부

	UINT		EntityID;

	int			padding;
};

struct tSpawnCount
{
	int		SpawnCount;
	int		padding[3];
};


// Particle Module
struct tParticleModule
{
	// Spawn Modlue
	float	SpawnRate;			// 초당 파티클 생성량
	Vec4	StartColor;			// 초기 파티클 색상
	Vec4	EndColor;			// 파티클 최종 색상
	Vec3	MinScale;			// 생성 시 최소 크기
	Vec3	MaxScale;			// 생성 시 최대 크기
	float	MinSpeed;			// 생성 시 최소 속력
	float	MaxSpeed;			// 생성 시 최대 속력
	float	MinLife;			// 생성 시 최소 주기
	float	MaxLife;			// 생성 시 최대 주기
	int		SpawnShape;			// 0 : Box, 1 : Sphere
	Vec3	SpawnShapeScale;
	Vec3	SpawnDir;
	float	SpawnDirRandomize;	// 0 ~ 1, 방향 랜덤화 비율
	int		SpaceType;			// 0 : Local, 1 : World
	int		VelocityAllignment;

	// 추가 데이터
	Vec3	ObjectWorldPos;

	//int		padding;
};

struct tLightModule
{
	LIGHT_TYPE	Type;		// 광원 종류
	Vec3		Color;		// 광원 색상
	Vec3		WorldPos;	// 광원 위치	
	float		Radius;		// 광원 범위, 반경
	Vec3		Dir;		// 광원의 방향
	float		Angle;		// 광원 각도(스포트 라이트)
};

struct tRay
{
    Vec3	vStart;
    Vec3	vDir;
};

// ============
// Animation 3D
// ============
struct tFrameTrans
{
    Vec4	vTranslate;
    Vec4	vScale;
    Vec4	qRot;
};

struct tMTKeyFrame
{
    double	dTime;
    int		iFrame;
    Vec3	vTranslate;
    Vec3	vScale;
    Vec4	qRot;
};

struct tMTBone
{
    wstring				strBoneName;
    int					iDepth;
    int					iIdx;
    int					iParentIndx;
    vector<int>         vecChildIdx;
    Matrix				matOffset;	// Inverse 행렬 (바인딩 시점 본 위치 -> 메시의 로컬 원점)
    Matrix				matBone;
    vector<tMTKeyFrame>	vecKeyFrame;
};

struct tMTAnimClip
{
    wstring			strAnimName;
    int				iStartFrame;
    int				iEndFrame;
    int				iFrameLength;

    double			dStartTime;
    double			dEndTime;
    double			dTimeLength;
    float			fUpdateTime; // 이거 안씀

    FbxTime::EMode	eMode;
};

// ===================
// 상수버퍼 연동 구조체
// ===================

struct tTransform
{
	Matrix	matWorld;
	Matrix	matView;
	Matrix	matProj;

    Matrix	matInvWorld;
    Matrix	matInvView;
    Matrix	matInvProj;

	Matrix	matWV;
	Matrix	matVP;
	Matrix	matWVP;
};
extern tTransform g_Trans;

// Material 계수
struct tMtrlData
{
    Vec4 vDiff;
    Vec4 vSpec;
    Vec4 vAmb;
    Vec4 vEmv;
};

// 재질을 통해서 전달되는 상수
struct tMtrlConst
{
    tMtrlData	mtrl;

	int		iArr[6];
	float	fArr[6];
	Vec2	v2Arr[6];
	Vec4	v4Arr[6];
	Matrix	matArr[6];

	//텍스쳐 바인딩 정보
	int		bTex[TEX_END];

    // 3D Animation 정보
    int		arrAnimData[2];

    int     padding[2];
};

// 광원에서 추가로 사용하는 VP 행렬
struct tShadowMat
{
    Matrix	matVP_0;
    Matrix	matVP_1;
    Matrix	matVP_2;
    Matrix	matVP_3;
    Matrix	matVP_4;
    Matrix	matVP_5;
};

// Global Data
struct tGlobal
{
	Vec2	g_Resolution;
	float   g_DT;
	float   g_EngineDT;
	float	g_Time;
	float	g_EngineTime;
	int     g_Light2DCount;
	int     g_Light3DCount;
};
extern tGlobal g_global;