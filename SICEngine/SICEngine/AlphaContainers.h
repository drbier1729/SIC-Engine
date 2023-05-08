/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: -
 * Additional Authors: -
 */
 

#pragma once

#include "SICTypes.h"
#include <cassert>
#include <memory_resource>

namespace sic {

	template<typename T>
	class ObjectPool
	{
	public:
		ObjectPool();
		ObjectPool(ObjectPool const&) = default;
		ObjectPool& operator=(ObjectPool const&) = default;
		ObjectPool(ObjectPool&&) = default;
		ObjectPool& operator=(ObjectPool&&) = default;
		~ObjectPool();

		void Init(u32 const numElements);
		T* Allocate();
		void FreeElement(void* pElement);

	private:
		void FreePool();
		u8* AddressFromIndex(u32 index) const;
		u32 IndexFromAddress(u8 const* pAddress) const;

	private:
		u32 m_element_size;
		u32 m_pool_size;
		u32 m_num_elements;
		u32 m_free_elements;
		u8* m_first_address;
		u8* m_next;
	};

	template<typename T>
	ObjectPool<T>::ObjectPool() :
		m_element_size{ sizeof(T) },
		m_pool_size{ 0 },
		m_num_elements{ 0 },
		m_free_elements{ 0 },
		m_first_address{ nullptr },
		m_next{ nullptr }
	{
		assert(sizeof(T) >= sizeof(u32) && "size of T must be greater than or equal to size of u32.");
	}

	template<typename T>
	ObjectPool<T>::~ObjectPool() {
		FreePool();
	}

	template<typename T>
	void ObjectPool<T>::FreePool() {
		delete[] m_first_address;
		m_first_address = nullptr;
	}

	// TODO: (aseem/dylan) make this work with our MemResources - currently uses 'new'
	template<typename T>
	void ObjectPool<T>::Init(u32 const numElements) {
		m_pool_size = numElements;
		m_free_elements = m_pool_size;
		m_first_address = new u8[m_pool_size * m_element_size];
		m_next = m_first_address;
	}

	template<typename T>
	T* ObjectPool<T>::Allocate() {
		if (m_num_elements < m_pool_size) {
			u8* pAddr = AddressFromIndex(m_num_elements);
			*pAddr = ++m_num_elements;
		}

		T* pResultAddr = nullptr;
		if (m_free_elements > 0) {
			pResultAddr = reinterpret_cast<T*>(m_next);
			--m_free_elements;

			if (m_free_elements > 0) {
				m_next = AddressFromIndex(*m_next);
			}
			else {
				m_next = nullptr;
			}
		}

		return pResultAddr;
	}

	// TODO: (aseem/dylan) solve "double-free" issue
	template<typename T>
	void ObjectPool<T>::FreeElement(void* pElement) {
		assert((pElement >= m_first_address && pElement <= AddressFromIndex(m_pool_size)) && "Address should be in range of pool.");

		if (m_next == nullptr) {
			*reinterpret_cast<u32*>(pElement) = m_pool_size;
		}
		else {
			*reinterpret_cast<u32*>(pElement) = IndexFromAddress(m_next);
		}
		m_next = reinterpret_cast<u8*>(pElement);
		++m_free_elements;
	}

	template<typename T>
	u8* ObjectPool<T>::AddressFromIndex(u32 index) const {
		return m_first_address + (index * m_element_size);
	}

	template<typename T>
	u32 ObjectPool<T>::IndexFromAddress(u8 const* pAddress) const {
		return (pAddress - m_first_address) / m_element_size;
	}

}

namespace sic {

	//template<class T, std::size_t Size>
	//class CircularArray
	//{
	//public:
	//	void push_back(T const&);
	//	void push_back(T &&);
	//	void pop_front(void);

	//private:
	//	std::array<T, Size> m_buffer;
	//	size_t size;
	//	T* begin;
	//	T* end;
	//};
}
