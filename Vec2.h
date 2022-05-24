#pragma once
#include "JPorta.h"

#include <iostream>
#include <cmath>

#include "memtrace.h"

#define M_PI 3.14159265358979323846

struct Vec2 {
private:
	double _x;
	double _y;
public:

	Vec2(const double _x = 0, const double _y = 0) : _x(_x), _y(_y) {}

	double& x() { return _x; }
	double& y() { return _y; }

	long double GetAngle() const {
		return (_y > 0) ? acos(_x / GetLength()) : M_PI * 2 - acos(_x / GetLength());
	}
	void SetAngle(long double angle) {
		_x = cos(angle) * GetLength();
		_y = sin(angle) * GetLength();
	}
	Vec2 Rotate(long double angle) const {
		double newX = _x * cos(angle) - _y * sin(angle);
		double newY = _x * sin(angle) + _y * cos(angle);
		return Vec2(newX, newY);
	}
	static long double Angle(const Vec2& a, const Vec2& b) {
		return atan2(a.Det(b), a.Dot(b));
	}

	long double GetLength() const {
		return sqrt(_x * _x + _y * _y);
	}
	void SetLength(long double length) {
		_x = cos(GetAngle()) * length;
		_y = sin(GetAngle()) * length;
	}

	long double Dot(const Vec2& vec) const {
		return _x * vec._x + _y * vec._y;
	}
	static long double Dot(const Vec2 a, const Vec2 b) {
		return a._x * b._x + a._y * b._y;
	}

	long double Det(const Vec2& vec) const {
		return _x * vec._y - _y * vec._x;
	}
	static long double Det(const Vec2 a, const Vec2 b) {
		return a._x * b._x - a._y * b._y;
	}

	void Print(std::ostream& os = std::cout) const {
		os << "Vec(" << _x << _y << ")";
	}

	Vec2& operator=(const Vec2& vec) {
		_x = vec._x;
		_y = vec._y;
		return *this;
	}

	Vec2 operator+(const Vec2& vec) const {
		return Vec2(_x + vec._x, _y + vec._y);
	}
	void operator+=(const Vec2& vec) {
		_x += vec._x;
		_y += vec._y;
	}

	Vec2 operator-(const Vec2& vec) const {
		return Vec2(_x - vec._x, _y - vec._y);
	}
	void operator-=(const Vec2& vec) {
		_x -= vec._x;
		_y -= vec._y;
	}

	bool operator>(const Vec2& vec) const {
		return GetLength() > vec.GetLength();
	}
	bool operator>=(const Vec2& vec) const {
		return GetLength() >= vec.GetLength();
	}

	bool operator<(const Vec2& vec) const {
		return GetLength() < vec.GetLength();
	}
	bool operator<=(const Vec2& vec) const {
		return GetLength() <= vec.GetLength();
	}

	Vec2 operator*(const Vec2& vec) const {
		return Vec2(_x * vec._x, _y * vec._y);
	}
	void operator*=(const Vec2& vec) {
		_x *= vec._x;
		_y *= vec._y;
	}

	Vec2 operator*(const long double d) const {
		return Vec2(_x * d, _y * d);
	}
	void operator*=(const long double d) {
		_x *= d;
		_y *= d;
	}
};
