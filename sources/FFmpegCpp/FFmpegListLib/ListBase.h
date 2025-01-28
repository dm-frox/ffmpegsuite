#pragma once

// ---------------------------------------------------------------------
// File: ListBase.h
// Classes: ListItemInfoPO, ListItemInfoBase (template), ListItemInfoBaseOpt (template), 
//     ListBase (template), ListBaseOpt (template), функция SetEmptyEx()
// Purpose: base classes for component list classes
// Module: FFmpegListLib - ffmpeg component lists
// Copyright (c) 2020 Dmitry Ponomarev
// Created by Dmitry Ponomarev 2023
// ---------------------------------------------------------------------

#include <vector>
#include <string>

#include "NonCopyable.h"


namespace Flist
{
	class ListItemInfoPO
	{
// data
	protected:
		mutable std::string  m_PrivOpts{};
		mutable std::string  m_PrivOptsEx{};
// ctor, dtor, copying
	protected:
		ListItemInfoPO() = default;
		~ListItemInfoPO() = default;

	}; // class ListItemInfoPO

	template<class BI>
	class ListItemInfoBase : private BI // BI - interface
	{
// types
	public:
		using BaseIface = BI;
// ctor, dtor, copying
	protected:
		ListItemInfoBase() = default;
		~ListItemInfoBase() = default;
// operations
	public:
		const BaseIface* GetIface() const { return static_cast<const BaseIface*>(this); }

	}; // template class ListItemInfoBase

	template<class BI>
	class ListItemInfoBaseOpt : public ListItemInfoPO, public ListItemInfoBase<BI>
	{
// ctor, dtor, copying
	protected:
		ListItemInfoBaseOpt() = default;
		~ListItemInfoBaseOpt() = default;

	}; // template class ListItemInfoBaseOpt

	template<typename T> // T - ListItemInfoBase, ListItemInfoBaseOpt
	class ListBase : Fcore::NonCopyable
	{
// data
	private:
		std::vector<T> m_List{};
// ctor, dtor, copying
	protected:
		ListBase() = default;
		~ListBase() = default;
// properties
	protected:
		std::vector<T>& List() { return m_List; }
		const std::vector<T>& List() const { return m_List; }
	public:
		int Count() const { return static_cast<int>(m_List.size()); }
		const typename T::BaseIface* operator[](int i) const { return m_List[i].GetIface(); }
// operations
	protected:
		bool IsEmpty() const { return m_List.empty(); }
		void Reserve(size_t capacity) { m_List.reserve(capacity); }

	}; // template class ListBase

	template<typename T>
	class ListBaseOpt : public ListBase<T>
	{
// data
	protected:
		mutable std::string m_CtxOptsEx{};
// ctor, dtor, copying
	protected:
		ListBaseOpt() = default;
		~ListBaseOpt() = default;

	}; // template class ListBaseOpt

	inline void SetEmptyEx(std::string& str) 
	{
		if (str.empty())
		{
			str = ' ';
		}
	}

} // namespace Flist
