#pragma once

template<typename T>
class Singleton
{
private:
	static T* g_this;

public:
	static T* GetInst()
	{
		if (!g_this)
		{
			g_this = new T;
		}
		return g_this;
	}

	static void Destroy()
	{
		delete g_this;
		g_this = nullptr;
	}

protected:
	Singleton()
	{
		typedef void(*FUNC)(void);
		atexit((FUNC)&Singleton::Destroy);
	}

	~Singleton()
	{

	}

	Singleton(const Singleton& _other) = delete;

	bool operator = (const Singleton& _other) = delete;
};

template<typename T>
T* Singleton<T>::g_this = nullptr;