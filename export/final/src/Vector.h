#pragma once
#include "JPorta.h"

#include <string.h>
#include <iostream>

#include "memtrace.h"

template <typename T>
class Vector {
	size_t _capacity;
	size_t _size;
	T* items;

	void resize(size_t newCapacity) {
		if (newCapacity < _size || newCapacity < 2) return;

		T* newItems = new T[newCapacity];
		for (size_t i = 0; i < _size; i++) newItems[i] = items[i];
		
		delete[] items;
		items = newItems;

		_capacity = newCapacity;
	}
public:
	Vector() : _capacity(2), _size(0), items(new T[2]) {}
	Vector(const Vector& vector) {
		_capacity = vector._capacity;
		_size = vector._size;
		items = new T[_capacity];
		for (size_t i = 0; i < capacity(); i++)
			items[i] = vector.items[i];
	}

	size_t size() const {
		return _size;
	}

	size_t capacity() const {
		return _capacity;
	}

	void push_back(T item) {
		if (_size + 1 > _capacity) resize(_capacity * 2);
		_size++;

		items[_size - 1] = item;
	}

	void pop_back() {
		splice(size() - 1, 1);
	}

	void splice(size_t start, size_t amount = 1) {
		if (start >= _size) throw std::out_of_range("Start index is out of range");

		size_t newSize = _size - std::min<size_t>(amount, (_size - start));
		T* newItems = new T[newSize];

		for (size_t i = 0; i < start; i++) newItems[i] = items[i];
		if (start + amount < _size)
			for (size_t i = 0; i < (_size - (start + amount)); i++) newItems[start + i] = items[start + amount + i];

		//if (capacity > 0) 
			delete[] items;
		items = newItems;
		_size = newSize;

		this->resize(ceil((this->size() / 2.0)) * 2);
	}

	T& at(size_t index) {
		if (index > _size - 1) throw std::out_of_range("Index is out of range");

		return items[index];
	}

	~Vector() {
		delete[] items;
	}
};
