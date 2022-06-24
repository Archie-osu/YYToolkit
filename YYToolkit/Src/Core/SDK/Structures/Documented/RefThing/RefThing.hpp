#pragma once
#include <cstring>
struct RefString
{
	char* m_Thing;
	int m_refCount;
	int m_Size;

	RefString(const char* _Thing, size_t _Size, bool _NoAutoFree);

	~RefString();

	void Inc();

	void Dec();

	const char* Get() const;

	int Size() const;

	static RefString* Alloc(const char* _Thing, const size_t& _Size);
	static RefString* Alloc(const char* _Thing, const size_t& _Size, bool _NoAutoFree);

	static RefString* Assign(RefString* _Other);

	static RefString* Remove(RefString* _Other);

	static RefString* Destroy(RefString* _Other);
};