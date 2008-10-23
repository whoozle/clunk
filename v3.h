#ifndef CLUNK_V3_H__
#define CLUNK_V3_H__

/* libclunk - realtime 2d/3d sound render library
 * Copyright (C) 2007-2008 Netive Media Group
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/

#include <math.h>
namespace clunk {
/*! 
	\brief 3d vector 
	\tparam T type of the axis. usually int or float. 
*/
template <typename T> class v3  {
public:
	///x component
	T x;
	///y component
	T y;
	///z component
	T z;
	///default ctor: initializes all components with zeroes.
	inline v3<T>() : x(0), y(0), z(0) {}
	///initializes all components with given values
	inline v3<T>(const T x, const T y, const T z) : x(x), y(y), z(z) {} 
	
	///nullify vector
	inline void clear() { x = y = z = 0; }
	///returns true if x == y == z == 0 ? 
	inline const bool is0() const {
		return x== 0 && y == 0 && z == 0;
	}

	/*! 
		\brief normalizes vector. 
		\return old length of this vector
	*/ 
	inline const T normalize() {
		const T len = length();
		if (len == (T)0 || len ==(T)1) 
			return len;
		
		x /= len;
		y /= len;
		z /= len;
		return len;
	}

	/*! 
		\brief normalizes vector with given length
		\param[in] nlen length
		\return old length of this vector
	*/ 

	inline const T normalize(const T nlen) {
		const T len = length();
		if (len == (T)0 || len == nlen) 
			return len;
		
		x *= nlen / len;
		y *= nlen / len;
		z *= nlen / len;
		return len;
	}
	
	inline const T dot_product(const v3<T> &v) const {
		return x * v.x + y * v.y + z * v.z;
	}
	
	///returns length of this vector
	inline const T length() const {
		const T ql = x * x + y * y + z * z;
		if (ql == (T)0 || ql == (T)1) 
			return ql;
		
		return (T)sqrt(ql);
	}
	///returns square of length. To avoid sqrt if needed.
	inline const T quick_length() const {
		return (T)(x * x + y * y + z * z);
	}

	///converts to vector of another type
	template <typename T2> 
		inline v3<T2> convert() const { return v3<T2>((T2)x, (T2)y, (T2)z); }

	///returns distance between two points	
	inline const T distance(const v3<T>& other) const {
		v3<T>d(*this);
		d-= other;
		return d.length();
	}
	
	///return square distance between two points
	inline const T quick_distance(const v3<T>& other) const {
		const T dx = x - other.x;
		const T dy = y - other.y;
		const T dz = z - other.z;
		return (dx * dx + dy * dy + dz * dz);
	}

	///allows v3 be placed in sorted STL container such std::map
	inline const bool operator<(const v3<T> &other) const {
		if (x != other.x) {
			return x < other.x;
		}
		if (y != other.y) {
			return y < other.y;
		} 
		return z < other.z;
	}

	///negate all components
	inline const v3<T> operator-() const {
		return v3<T>(-x, -y, -z);
	}
	///test equality 
	inline const bool operator==(const v3<T> &other) const {
		return x == other.x && y == other.y && z == other.z;
	}

	///test inequality 
	inline const bool operator!=(const v3<T> &other) const {
		return x != other.x || y != other.y || z != other.z;
	}
	
	///adds another vector
	inline const v3<T>& operator+=(const v3<T>& other) {
		x += other.x; y += other.y; z += other.z;
		return *this;
	}

	///substracts another vector
	inline const v3<T>& operator-=(const v3<T>& other) {
		x -= other.x; y -= other.y; z -= other.z;
		return *this;
	}

	///multiplies another vector
	inline const v3<T>& operator*=(const v3<T>& other) {
		x *= other.x; y *= other.y; z *= other.z;
		return *this;
	}

	///divide with another vector
	inline const v3<T>& operator/=(const v3<T>& other) {
		x /= other.x; y /= other.y; z /= other.z;
		return *this;
	}
	///multiplication
	inline const v3<T> operator*(const v3<T>& other) const {
		return v3<T>(x * other.x, y * other.y, z * other.z);
	}
	///summing
	inline const v3<T> operator+(const v3<T>& other) const {
		return v3<T>(x + other.x, y + other.y, z + other.z);
	}
	///substraction
	inline const v3<T> operator-(const v3<T>& other) const {
		return v3<T>(x - other.x, y - other.y, z - other.z);
	}
	///division
	inline const v3<T> operator/(const v3<T>& other) const {
		return v3<T>(x / other.x, y / other.y, z / other.z);
	}
	///multiplies all components with constant
	inline const v3<T> operator*(const T& other) const {
		return v3<T>(x * other, y * other, z * other);
	}
	///sums all components with constant
	inline const v3<T> operator+(const T& other) const {
		return v3<T>(x + other, y + other, z + other);
	}
	///substracts all components with constant
	inline const v3<T> operator-(const T& other) const {
		return v3<T>(x - other, y - other, z - other);
	}
	///divides all components by constant
	inline const v3<T> operator/(const T& other) const {
		return v3<T>(x / other, y / other, z / other);
	}
	///divides this vector by constant
	inline const v3<T>& operator/=(const T& other) {
		x /= other;
		y /= other;
		z /= other;
		return *this;
	}

	///multiplies this vector with constant
	inline const v3<T>& operator*=(const T& other) {
		x *= other;
		y *= other;
		z *= other;
		return *this;
	}

	///sums this vector with constant
	inline const v3<T>& operator+=(const T& other) {
		x += other;
		y += other;
		z += other;
		return *this;
	}

	///substracts this vector with constant
	inline const v3<T>& operator-=(const T& other) {
		x -= other;
		y -= other;
		z -= other;
		return *this;
	}

	///adds constant to the vector
	inline const v3<T> operator+(const T a)  {
		return v3<T>(x + a, y + a, z + a);
	}

	///subs constant from the vector
	inline const v3<T> operator-(const T a)  {
		return v3<T>(x - a, y - a, z - a);
	}

	///muls constant to the vector
	inline const v3<T> operator*(const T a)  {
		return v3<T>(x * a, y * a, z * a);
	}

	///divs constant to the vector
	inline const v3<T> operator/(const T a)  {
		return v3<T>(x / a, y  / a, z / a);
	}
};
///external operator+
template <typename T>
	inline const v3<T> operator+(const T a, const v3<T> &v)  {
		return v3<T>(v.x + a, v.y + a, v.z + a);
	}

///external operator-
template <typename T>
	inline const v3<T> operator-(const T a, const v3<T> &v)  {
		return v3<T>(a - v.x, a - v.y, a - v.z);
	}

///external operator*
template <typename T>
	inline const v3<T> operator*(const T a, const v3<T> &v)  {
		return v3<T>(v.x * a, v.y * a, v.z * a);
	}

///external operator/
template <typename T>
	inline const v3<T> operator/(const T a, const v3<T> &v)  {
		return v3<T>(a / v.x, a / v.y, a / v.z);
	}

	
} //namespace clunk

#endif

