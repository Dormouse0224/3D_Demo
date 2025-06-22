#pragma once


#define SINGLE(type)	friend class Singleton<type>;\
						type& operator = (const type& _Other) = delete;\
						type();\
						type(const type& _Origin) = delete;\
						~type();

#define DEVICE	CDevice::GetInst()->GetDevice().Get()
#define CONTEXT CDevice::GetInst()->GetContext().Get()
#define MRT_COUNT 3

#define DT CTimeMgr::GetInst()->GetDeltaTime()
#define EngineDT CTimeMgr::GetInst()->GetEngineDeltaTime()
#define KEY_CHECK(key)		CKeyMgr::GetInst()->GetKeyState(key)
#define KEY_TAP(key)		(CKeyMgr::GetInst()->GetKeyState(key) == Key_state::TAP)
#define KEY_PRESSED(key)	(CKeyMgr::GetInst()->GetKeyState(key) == Key_state::PRESSED)
#define KEY_RELEASED(key)	(CKeyMgr::GetInst()->GetKeyState(key) == Key_state::RELEASE)
#define KEY_NONE(key)		(CKeyMgr::GetInst()->GetKeyState(key) == Key_state::NONE)

#define CURSOR_ON		while (ShowCursor(true) < 0);
#define CURSOR_OFF		while (ShowCursor(false) >= 0);

#define CLONE(Type) virtual Type* Clone() override { return new Type(*this); }
#define CLONE_DISABLE(Type) virtual Type* Clone() override {return nullptr;}\
							private: Type(const Type& _Other) = delete;

#define DEBUG_NAME(Obj, Name)	{\
								char buff[255] = {};\
								sprintf_s(buff, 255, "%s_%d", Name, this->GetID());\
								Obj->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(buff) - 1, buff);\
								}

typedef Vector2 Vec2;
typedef Vector3 Vec3;
typedef Vector4 Vec4;