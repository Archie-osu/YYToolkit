#pragma once
#include "../../../Enums/Enums.hpp"
#include <string>

struct YYObjectBase;
struct RefString;

template <typename T>
struct CDynamicArrayRef;

// Base class with no overloading, just a pure RValue.
struct RValue
{
	union
	{
		double Real;
		int I32;
		long long I64;

		// Pointers
		union
		{
			YYObjectBase* Object;
			RefString* String;
			CDynamicArrayRef<RValue>* Array;
			void* Pointer;
		};
	};

	int Flags;
	int Kind;
};

// Struct with overloads, no direct access to variables.
struct YYRValue : protected RValue
{
	// Default constructor
	YYRValue() noexcept(true);

	// YYRValue V = 30.0;
	YYRValue(const double& Value) noexcept(true);

	// YYRValue V = 30.0f;
	YYRValue(const float& Value) noexcept(true);

	// YYRValue V = true;
	YYRValue(const bool& Value) noexcept(true);

	// YYRValue V = "Hello, World!";
	YYRValue(const char* Value) noexcept(true);

	// YYRValue V = std::string("Hello, std::string!");
	YYRValue(const std::string& Value) noexcept(true);

	// static_cast<double>(V);
	operator double() const noexcept(true);

	// static_cast<float>(V);
	operator float() const noexcept(true);

	// static_cast<bool>(V);
	operator bool() const noexcept(true);

	// static_cast<const char*>(V);
	operator const char* () const noexcept(true);

	// static_cast<std::string>(V);
	operator std::string() const noexcept(true);

	operator RefString*() const noexcept(true);

	// V += 30.0;
	YYRValue& operator +=(const double& Value);

	// V -= 30.0;
	YYRValue& operator -=(const double& Value);

	// V *= 30.0;
	YYRValue& operator *=(const double& Value);

	// V /= 30.0;
	YYRValue& operator /=(const double& Value);
};