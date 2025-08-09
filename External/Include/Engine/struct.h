#pragma once



// Vertex
// ��ü�� ���¸� �̷�� ���� �⺻ ����
struct Vtx
{
	Vec3	vPos;		// ������ ��ġ
	Vec4	vColor;		// ������ ����	
	Vec2	vUV;		// ������ ������ �ؽ����� ��ġ(��ǥ)

	// ���⺤��
	Vec3	vTangent;	// ��������
	Vec3	vNormal;	// ��������
	Vec3	vBinormal;	// ����������

    // Skinning �� ������
    Vec4	vWeights;  // Bone ����ġ
    Vec4	vIndices;  // Bone �ε���
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
// Particle ����
// =============
struct tParticle
{
	Vec4		LocalPos;		// ���� ������Ʈ�� ������ ��� ��ǥ (World �������� ��� ���̴����� ObjectPos �� ����)
	Vec4		WorldScale;		// ��ƼŬ ���� ũ��
	Vec4		Color;			// ��ƼŬ ����

	Vec4		Force;			// ��ƼŬ�� �־����� �ִ� ���� ����
	Vec4		Velocity;		// ��ƼŬ �ӵ�
	float		Mass;			// ��ƼŬ ����

	float		Age;			// ��ƼŬ ����, Age �� Life �� �����ϸ� ������ ���� ��
	float		Life;			// ��ƼŬ �ִ� ����
	float		NormalizedAge;  // ��ü ���� ���, ���� Age ����. �ڽ��� Age �� Life ��� ����ȭ �� ��

	int			Active;			// ��ƼŬ Ȱ��ȭ ����
	int			PrevActive;		// ���� �����ӿ��� ��ƼŬ Ȱ��ȭ ����

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
	float	SpawnRate;			// �ʴ� ��ƼŬ ������
	Vec4	StartColor;			// �ʱ� ��ƼŬ ����
	Vec4	EndColor;			// ��ƼŬ ���� ����
	Vec3	MinScale;			// ���� �� �ּ� ũ��
	Vec3	MaxScale;			// ���� �� �ִ� ũ��
	float	MinSpeed;			// ���� �� �ּ� �ӷ�
	float	MaxSpeed;			// ���� �� �ִ� �ӷ�
	float	MinLife;			// ���� �� �ּ� �ֱ�
	float	MaxLife;			// ���� �� �ִ� �ֱ�
	int		SpawnShape;			// 0 : Box, 1 : Sphere
	Vec3	SpawnShapeScale;
	Vec3	SpawnDir;
	float	SpawnDirRandomize;	// 0 ~ 1, ���� ����ȭ ����
	int		SpaceType;			// 0 : Local, 1 : World
	int		VelocityAllignment;

	// �߰� ������
	Vec3	ObjectWorldPos;

	//int		padding;
};

struct tLightModule
{
	LIGHT_TYPE	Type;		// ���� ����
	Vec3		Color;		// ���� ����
	Vec3		WorldPos;	// ���� ��ġ	
	float		Radius;		// ���� ����, �ݰ�
	Vec3		Dir;		// ������ ����
	float		Angle;		// ���� ����(����Ʈ ����Ʈ)
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
    Matrix				matOffset;	// Inverse ��� (���ε� ���� �� ��ġ -> �޽��� ���� ����)
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
    float			fUpdateTime; // �̰� �Ⱦ�

    FbxTime::EMode	eMode;
};

// ===================
// ������� ���� ����ü
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

// Material ���
struct tMtrlData
{
    Vec4 vDiff;
    Vec4 vSpec;
    Vec4 vAmb;
    Vec4 vEmv;
};

// ������ ���ؼ� ���޵Ǵ� ���
struct tMtrlConst
{
    tMtrlData	mtrl;

	int		iArr[6];
	float	fArr[6];
	Vec2	v2Arr[6];
	Vec4	v4Arr[6];
	Matrix	matArr[6];

	//�ؽ��� ���ε� ����
	int		bTex[TEX_END];

    // 3D Animation ����
    int		arrAnimData[2];

    int     padding[2];
};

// �������� �߰��� ����ϴ� VP ���
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