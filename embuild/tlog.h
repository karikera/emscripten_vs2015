#pragma once

class TLog;

class TLogGroup
{
public:
	TLogGroup(Map<TextW, Empty> * group) noexcept;
	void put(TextW value) noexcept;
	void putPath(TextW value) noexcept;
	void putPath(RefArray<ATextW> inputs) noexcept;

private:
	Map<TextW, Empty> * m_group;
};

class TLogInputs
{
	friend TLog;
public:
	void put(TextW value) noexcept;

private:
	ATextW m_inputs;
};

class TLog
{
public:
	TLog() noexcept;
	void load(pcwstr path) noexcept;
	void save() noexcept;
	TLogGroup reset(TextW input) noexcept;
	TLogGroup reset(RefArray<ATextW> inputs) noexcept;
	TLogGroup reset(const TLogInputs& inputs) noexcept;
	void put(TextW input, TextW value) noexcept;

private:
	ATextW m_filepath;
	Map<TextW, Map<TextW, Empty>> m_map;
};
