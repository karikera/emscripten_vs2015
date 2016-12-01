#pragma once


class JsVar
{
public:
	static void init() noexcept;
	JsVar() noexcept;
	~JsVar() noexcept;
	template <typename T>
	void set(const T &value) noexcept
	{
		return set<int>((int)value);
	}
	template <typename T>
	T get() noexcept
	{
		return (T)get<int>();
	}

private:
#ifdef __EMSCRIPTEN__
	int m_id;
#else

#endif
};

template <>
void JsVar::set<int>(const int &value) noexcept;
template <>
void JsVar::set<double>(const double &value) noexcept;
template <>
void JsVar::set<float>(const float &value) noexcept;
template <>
void JsVar::set<Text>(const Text &value) noexcept;
template <>
void JsVar::set<AText>(const AText &value) noexcept;
template <>
int JsVar::get<int>() noexcept;
template <>
double JsVar::get<double>() noexcept;
template <>
float JsVar::get<float>() noexcept;
template <>
AText JsVar::get<AText>() noexcept;
