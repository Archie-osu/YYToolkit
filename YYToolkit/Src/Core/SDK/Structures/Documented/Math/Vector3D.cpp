#include "Vector3D.hpp"
#include <cmath>

float Vector3D::operator[](size_t Index) const
{
	if (Index > 3)
		throw;

	return reinterpret_cast<const float*>(this)[Index];
}

float& Vector3D::operator[](size_t Index)
{
	if (Index > 3)
		throw;

	return reinterpret_cast<float*>(this)[Index];
}

bool Vector3D::Valid() const
{
	return std::isfinite(m_X) && std::isfinite(m_Y) && std::isfinite(m_Z);
}

bool Vector3D::operator==(const Vector3D& Other) const
{
	return (this->m_X == Other.m_X) && (this->m_Y == Other.m_Y) && (this->m_Z == Other.m_Z);
}

bool Vector3D::operator!=(const Vector3D& Other) const
{
	return (this->m_X != Other.m_X) || (this->m_Y != Other.m_Y) || (this->m_Z != Other.m_Z);
}

Vector3D& Vector3D::operator+=(const Vector3D& Other)
{
	this->m_X += Other.m_X;
	this->m_Y += Other.m_Y;
	this->m_Z += Other.m_Z;

	return *this;
}

Vector3D& Vector3D::operator+=(const float& Other)
{
	this->m_X += Other;
	this->m_Y += Other;
	this->m_Z += Other;

	return *this;
}

Vector3D& Vector3D::operator-=(const Vector3D& Other)
{
	this->m_X -= Other.m_X;
	this->m_Y -= Other.m_Y;
	this->m_Z -= Other.m_Z;

	return *this;
}

Vector3D& Vector3D::operator-=(const float& Other)
{
	this->m_X -= Other;
	this->m_Y -= Other;
	this->m_Z -= Other;

	return *this;
}

Vector3D& Vector3D::operator*=(const Vector3D& Other)
{
	this->m_X *= Other.m_X;
	this->m_Y *= Other.m_Y;
	this->m_Z *= Other.m_Z;

	return *this;
}

Vector3D& Vector3D::operator*=(const float& Other)
{
	this->m_X *= Other;
	this->m_Y *= Other;
	this->m_Z *= Other;

	return *this;
}

Vector3D& Vector3D::operator/=(const Vector3D& Other)
{
	this->m_X /= Other.m_X;
	this->m_Y /= Other.m_Y;
	this->m_Z /= Other.m_Z;

	return *this;
}

Vector3D& Vector3D::operator/=(const float& Other)
{
	this->m_X /= Other;
	this->m_Y /= Other;
	this->m_Z /= Other;

	return *this;
}

Vector3D Vector3D::operator+(const Vector3D& Other) const
{
	return Vector3D(this->m_X + Other.m_X, this->m_Y + Other.m_Y, this->m_Z + Other.m_Z);
}

Vector3D Vector3D::operator-(const Vector3D& Other) const
{
	return Vector3D(this->m_X - Other.m_X, this->m_Y - Other.m_Y, this->m_Z - Other.m_Z);
}

Vector3D Vector3D::operator*(const Vector3D& Other) const
{
	return Vector3D(this->m_X * Other.m_X, this->m_Y * Other.m_Y, this->m_Z * Other.m_Z);
}

Vector3D Vector3D::operator/(const Vector3D& Other) const
{
	return Vector3D(this->m_X / Other.m_X, this->m_Y / Other.m_Y, this->m_Z / Other.m_Z);
}

float Vector3D::Length() const
{
	return sqrtf((m_X * m_X) + (m_Y * m_Y) + (m_Z * m_Z));
}

float Vector3D::DistTo(const Vector3D& Other) const
{
	return (*this - Other).Length();
}

void Vector3D::Normalize()
{
	*this /= this->Length();
}