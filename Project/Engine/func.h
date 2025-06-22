#pragma once

class CGameObject;

void DrawDebugRect(Vec4 _Color, Vec3 _WorldPos, Vec3 _WorldScale, Vec3 _WorldRotation, bool _DepthTest = true, float _Duration = 0.f);
void DrawDebugRect(Vec4 _Color, const Matrix& _matWorld, bool _DepthTest = true, float _Duration = 0.f);


void DrawDebugCircle(Vec4 _Color, Vec3 _WorldPos, float _Radius, Vec3 _WorldRotation, bool _DepthTest = true, float _Duration = 0.f);

bool IsRenderable(CGameObject* obj);

inline std::string to_str(const std::wstring& _wstr) { return std::string(_wstr.begin(), _wstr.end()); }
inline std::wstring to_wstr(const std::string& _str) { return std::wstring(_str.begin(), _str.end()); }

Vec2 GetScreenFromWorld(Vec3 _WorldPos, class CCamera* _Cam);

template<typename T>
void Delete_Vec(std::vector<T*>& _vec)
{
	for (size_t i = 0; i < _vec.size(); ++i)
	{
		if (nullptr != _vec[i])
			delete _vec[i];
	}

	_vec.clear();
}

void enumSizeCheck();