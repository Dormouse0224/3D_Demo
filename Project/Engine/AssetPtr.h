#pragma once

template <typename T>
class AssetPtr
{
private:
	T* m_Asset;

public:
	T* Get() { return m_Asset; }
	T** GetAdressOf() { return	&m_Asset; }

	bool operator == (T* _Asset)
	{
		if (m_Asset == _Asset)
			return true;
		return false;
	}

	bool operator == (const  AssetPtr<T>& _Other)
	{
		if (m_Asset == _Other.m_Asset)
			return true;
		return false;
	}

	bool operator != (T* _Asset)
	{
		if (m_Asset == _Asset)
			return false;
		return true;
	}

	bool operator != (const  AssetPtr<T>& _Other)
	{
		if (m_Asset == _Other.m_Asset)
			return false;
		return true;
	}






	AssetPtr& operator = (T* _Asset)
	{
		if (nullptr != m_Asset)
			m_Asset->Release();

		m_Asset = _Asset;

		if (nullptr != m_Asset)
			m_Asset->AddRef();

		return *this;
	}

	AssetPtr& operator = (const AssetPtr<T>& _Other)
	{
		if (nullptr != m_Asset)
			m_Asset->Release();

		m_Asset = _Other.m_Asset;

		if (nullptr != m_Asset)
			m_Asset->AddRef();

		return *this;
	}

	T* operator ->()
	{
		return m_Asset;
	}


public:
	AssetPtr()
		: m_Asset(nullptr)
	{}

	AssetPtr(T* _Asset)
		: m_Asset(_Asset)
	{
		if (nullptr != m_Asset)
			m_Asset->AddRef();
	}

	AssetPtr(const AssetPtr<T>& _Other)
		: m_Asset(_Other.m_Asset)
	{
		if (nullptr != m_Asset)
			m_Asset->AddRef();
	}

	~AssetPtr()
	{
		if (nullptr != m_Asset)
			m_Asset->Release();
	}
};


template<typename T>
bool operator == (void* _Left, AssetPtr<T> _Right)
{
	if (_Left == _Right.Get())
		return true;

	return false;
}

template<typename T>
bool operator != (void* _Left, AssetPtr<T> _Right)
{
	if (_Left == _Right.Get())
		return false;

	return true;
}