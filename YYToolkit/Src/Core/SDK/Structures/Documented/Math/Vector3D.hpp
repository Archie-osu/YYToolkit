#pragma once
struct Vector3D
{
private:
	float m_X;
	float m_Y;
	float m_Z;
public:
	// Constructors
	Vector3D() : m_X(0), m_Y(0), m_Z(0) {}
	Vector3D(float X, float Y, float Z) : m_X(X), m_Y(Y), m_Z(Z) {}
	Vector3D(float X, float Y) : m_X(X), m_Y(Y), m_Z(0) {}
	Vector3D(const Vector3D& Other) : m_X(Other.m_X), m_Y(Other.m_Y), m_Z(Other.m_Z) {}
	 
	// Accessors
	float operator[](size_t Index) const;
	float& operator[](size_t Index);
	bool Valid() const;

	// Equalizers (idk what these are called)
	bool operator==(const Vector3D& Other) const;
	bool operator!=(const Vector3D& Other) const;

	// Arithmetic
	Vector3D& operator+=(const Vector3D& Other);
	Vector3D& operator+=(const float& Other);
	Vector3D& operator-=(const Vector3D& Other);
	Vector3D& operator-=(const float& Other);
	Vector3D& operator*=(const Vector3D& Other);
	Vector3D& operator*=(const float& Other);
	Vector3D& operator/=(const Vector3D& Other);
	Vector3D& operator/=(const float& Other);

	Vector3D operator+(const Vector3D& Other) const;
	Vector3D operator-(const Vector3D& Other) const;
	Vector3D operator*(const Vector3D& Other) const;
	Vector3D operator/(const Vector3D& Other) const;

	float Length() const;
	float DistTo(const Vector3D& Other) const;

	void Normalize();
};