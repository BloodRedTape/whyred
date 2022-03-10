#pragma once

#include <core/allocators/allocator.hpp>
#include <core/list.hpp>

template <typename Type>
class ReusableObjectsPool{
private:
	List<Type *> m_TrackList;
	size_t m_Top = 0;
public:
	ReusableObjectsPool() = default;

	~ReusableObjectsPool()	{
		for(Type *obj: m_TrackList)
			delete obj;
	}

	Type* NewOrGet() {
		if(m_Top < m_TrackList.Size())
			return m_TrackList[m_Top++];

		Type *obj = new Type();
		m_TrackList.Add(obj);
		m_Top++;
		return obj;
	}

	size_t Size()const {
		return m_Top;
	}

	Type* operator[](size_t index) {
		return m_TrackList[index];
	}

	void Reset() {
		m_Top = 0;
	}
};