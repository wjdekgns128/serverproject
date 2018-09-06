#pragma once
template <class T>

class SingeTon
{
protected:
	SingeTon() {}
	~SingeTon() {}
private:
	SingeTon(const SingeTon&);
	SingeTon& operator = (const SingeTon&);
public:
	static T& getInstance()
	{
		static T instance;
		return instance;
	}
};