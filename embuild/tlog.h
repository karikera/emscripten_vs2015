#pragma once

#include <KR3/data/set.h>

class TLog;

class TLogGroup
{
public:
	TLogGroup(Set<Text16> * group) noexcept;
	void put(Text16 value) noexcept;
	void putPath(Text16 value) noexcept;
	void putPath(View<AText16> inputs) noexcept;

private:
	Set<Text16> * m_group;
};

class TLogInputs
{
	friend TLog;
public:
	void put(Text16 value) noexcept;

private:
	AText16 m_inputs;
};

class TLog
{
public:
	TLog() noexcept;
	void load(pcstr16 path) noexcept;
	void save() noexcept;
	TLogGroup reset(Text16 input) noexcept;
	TLogGroup reset(View<AText16> inputs) noexcept;
	TLogGroup reset(const TLogInputs& inputs) noexcept;
	void put(Text16 input, Text16 value) noexcept;

private:
	AText16 m_filepath;
	Map<Text16, Set<Text16>> m_map;
};
