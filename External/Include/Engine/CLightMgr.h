#pragma once

class CStructuredBuffer;
class CLight;

class CLightMgr
	: public Singleton<CLightMgr>
{
	SINGLE(CLightMgr);
private:
	vector<CLight*> m_vecLight;
	vector<tLightModule> m_vecLightModule;
	CStructuredBuffer* m_LightModuleBuffer;
	

public:
	void Tick();
	void RegisterLight(CLight* pLight);

	void Binding();
	void Render();
};

