#include "pch.h"

#include "CRenderMgr.h"
#include "CGameObject.h"
#include "CRenderComponent.h"
#include "CCamera.h"
#include "CEngine.h"

void DrawDebugRect(Vec4 _Color, Vec3 _WorldPos, Vec3 _WorldScale, Vec3 _WorldRotation
	, bool _DepthTest, float _Duration)
{
	tDebugShapeInfo info = {};
	info.Shape = DEBUG_SHAPE::RECT;
	info.Color = _Color;
	info.WorldPos = _WorldPos;
	info.WorldScale = _WorldScale;
	info.WorldRotation = _WorldRotation;
	info.MatWorld = XMMatrixIdentity();
	info.CurTime = 0.f;
	info.Duration = _Duration;
	info.DepthTest = _DepthTest;

	CRenderMgr::GetInst()->AddDebugShapeInfo(info);
}

void DrawDebugRect(Vec4 _Color, const Matrix& _matWorld, bool _DepthTest, float _Duration)
{
	tDebugShapeInfo info = {};
	info.Shape = DEBUG_SHAPE::RECT;
	info.Color = _Color;
	info.MatWorld = _matWorld;
	info.CurTime = 0.f;
	info.Duration = _Duration;
	info.DepthTest = _DepthTest;

	CRenderMgr::GetInst()->AddDebugShapeInfo(info);
}

void DrawDebugCircle(Vec4 _Color, Vec3 _WorldPos, float _Radius, Vec3 _WorldRotation
	, bool _DepthTest, float _Duration)
{
	tDebugShapeInfo info = {};
	info.Shape = DEBUG_SHAPE::CIRCLE;
	info.Color = _Color;
	info.WorldPos = _WorldPos;
	info.WorldScale = Vec3(_Radius * 2.f, _Radius * 2.f, 1.f);
	info.WorldRotation = _WorldRotation;
	info.CurTime = 0.f;
	info.Duration = _Duration;
	info.DepthTest = _DepthTest;

	CRenderMgr::GetInst()->AddDebugShapeInfo(info);
}

bool IsRenderable(CGameObject* obj)
{
	auto* render = obj->GetRenderComponent();
	if (render != nullptr
		&& render->GetMesh().Get() != nullptr
		&& render->GetMaterial().Get() != nullptr
		&& render->GetMaterial()->GetShader().Get() != nullptr)
		return true;
	else
		return false;
}

Vec2 GetScreenFromWorld(Vec3 _WorldPos, CCamera* _Cam)
{
	Vec4 wpos = Vec4(_WorldPos, 1);
	Matrix VM = _Cam->GetViewMat();
	Matrix PM = _Cam->GetProjMat();
	Vec4 clip = XMVector4Transform(wpos, VM * PM);
	Vec3 ndc = Vec3(clip.x / clip.w, clip.y / clip.w, clip.z / clip.w);
	Vec2 screen(CEngine::GetInst()->GetResolution().x * (ndc.x * 0.5f + 0.5f), CEngine::GetInst()->GetResolution().y * (-ndc.y * 0.5f + 0.5f));
	return screen;
}

void enumSizeCheck()
{
	assert(COMPONENT_TYPE_SAVE_MAP.size() == static_cast<int>(COMPONENT_TYPE_SAVE::COMPONENT_END) + 2);
}
