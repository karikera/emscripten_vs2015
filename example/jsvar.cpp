#include "stdafx.h"
#include "jsvar.h"

void JsVar::init() noexcept
{
#ifdef __EMSCRIPTEN__
	EM_ASM(
		self.__em_object = [];
	self.__em_object_free = [];
	);
#endif
}
JsVar::JsVar() noexcept
{
#ifdef __EMSCRIPTEN__
	m_id = EM_ASM_INT_V(
		var emobj = self.__em_object;
	var freeobj = self.__em_object_free;
	if (freeobj.length != = 0) return freeobj.pop();
	var id = emobj.length;
	emobj.push(undefined);
	return id;
	);
#endif
}
JsVar::~JsVar() noexcept
{
#ifdef __EMSCRIPTEN__
	EM_ASM_(
		self.__em_object[$0] = undefined;
	self.__em_object_free.push($0);
	, m_id);
#endif
}
template <>
void JsVar::set<int>(const int &value) noexcept
{
#ifdef __EMSCRIPTEN__
	EM_ASM_(self.__em_object[$0] = $1, m_id, (int)value);
#else
#endif
}
template <>
void JsVar::set<double>(const double &value) noexcept
{
#ifdef __EMSCRIPTEN__
	EM_ASM_(self.__em_object[$0] = $1, m_id, (double)value);
#else
#endif
}
template <>
void JsVar::set<float>(const float &value) noexcept
{
	return set<double>(value);
}
template <>
void JsVar::set<Text>(const Text &value) noexcept
{
#ifdef __EMSCRIPTEN__
	EM_ASM_(self.__em_object[$0] = Module.Pointer_stringify($1, $2), m_id, value.data(), value.size());
#else
#endif
}
template <>
void JsVar::set<AText>(const AText &value) noexcept
{
	Text str = value;
	return set(str);
}
template <>
int JsVar::get<int>() noexcept
{
#ifdef __EMSCRIPTEN__
	return EM_ASM_INT(self.__em_object[$0] | 0, m_id);
#else
	return 0;
#endif
}
template <>
double JsVar::get<double>() noexcept
{
#ifdef __EMSCRIPTEN__
	return EM_ASM_DOUBLE(+self.__em_object[$0], m_id);
#else
	return 0;
#endif
}
template <>
float JsVar::get<float>() noexcept
{
	return (float)get<double>();
}
template <>
AText JsVar::get<AText>() noexcept
{
	AText text;
#ifdef __EMSCRIPTEN__
	int strlen = EM_ASM_INT(return Module.lengthBytesUTF8(self.__em_object[$0] + ""), m_id);
	text.resize(strlen, strlen + 1);
	EM_ASM_(
		var val = self.__em_object[$0] + "";
	Module.writeStringToMemory(val, $1, true);
	, m_id, text.data());
#else
#endif
	return text;
}
