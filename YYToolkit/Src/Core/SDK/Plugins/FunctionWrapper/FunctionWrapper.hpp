#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif

#include "../../Enums/Enums.hpp"
#include "../../FwdDecls/FwdDecls.hpp"
#include <d3d9.h>
#include <d3d11.h>
#include <functional>

#pragma pack(push, 1)
template <typename>
class FunctionWrapper;

template <typename Ret, typename ...Args>
class FunctionWrapper<Ret(Args...)>
{
private:
	std::tuple<Args...> m_Arguments;
	std::function<Ret(Args...)> m_Function;
	bool m_CalledOriginal;
	Ret m_Result;
public:
	FunctionWrapper(std::function<Ret(Args...)> Function, Args... Arguments)
	{
		m_Function = Function;
		m_Arguments = std::make_tuple(Arguments...);
		m_CalledOriginal = false;
	}

	Ret Call()
	{
		m_CalledOriginal = true;
		m_Result = std::apply(m_Function, m_Arguments);
		return m_Result;
	}

	std::tuple<Args...>& Arguments()
	{
		return m_Arguments;
	}

	const std::tuple<Args...>& Arguments() const
	{
		return m_Arguments;
	}

	Ret Call(Args... Arguments)
	{
		m_CalledOriginal = true;
		m_Result = m_Function(Arguments...);
		return m_Result;
	}

	bool CalledOriginal() const
	{
		return m_CalledOriginal;
	}

	Ret Result() const
	{
		return m_Result;
	}

	void Override(const Ret& newValue)
	{
		m_CalledOriginal = true;
		m_Result = newValue;
	}
};

template <typename ...Args>
class FunctionWrapper<void(Args...)>
{
private:
	std::tuple<Args...> m_Arguments;
	std::function<void(Args...)> m_Function;
	bool m_CalledOriginal;
public:
	FunctionWrapper(std::function<void(Args...)> Function, Args... Arguments)
	{
		m_Function = Function;
		m_Arguments = std::make_tuple(Arguments...);
		m_CalledOriginal = false;
	}

	void Call()
	{
		m_CalledOriginal = true;
		std::apply(m_Function, m_Arguments);
	}

	std::tuple<Args...>& Arguments()
	{
		return m_Arguments;
	}

	const std::tuple<Args...>& Arguments() const
	{
		return m_Arguments;
	}

	void Call(Args... Arguments)
	{
		m_CalledOriginal = true;
		m_Function(Arguments...);
	}

	bool CalledOriginal() const
	{
		return m_CalledOriginal;
	}

	void Override()
	{
		m_CalledOriginal = true;
	}
};

// bool Function(CInstance* pSelf, CInstance* pOther, CCode* Code, RValue* Res, int Flags)
using FWExecuteIt = FunctionWrapper<bool(CInstance*, CInstance*, CCode*, RValue*, int)>;

// HRESULT __stdcall Function(LPDIRECT3DDEVICE9 _this)
using FWEndScene = FunctionWrapper<HRESULT(LPDIRECT3DDEVICE9)>;

// HRESULT __stdcall Function(IDXGISwapChain* _this, unsigned int Sync, unsigned int Flags)
using FWPresent = FunctionWrapper<HRESULT(IDXGISwapChain*, UINT, UINT)>;

// LRESULT __stdcall Function(HWND hwnd, unsigned int Msg, WPARAM w, LPARAM l)
using FWWindowProc = FunctionWrapper<LRESULT(HWND, UINT, WPARAM, LPARAM)>;

// HRESULT __stdcall Function(IDXGISwapChain* _this, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
using FWResizeBuffers = FunctionWrapper<HRESULT(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT)>;

// void Function(const char* pFormat, ...)
using FWYYError = FunctionWrapper<void(const char*)>;

// char* Function(CScript* pScript, int argc, char* pStackPointer, VMExec* pVM, YYObjectBase* pLocals, YYObjectBase* pArguments)
using FWDoCallScript = FunctionWrapper<char*(CScript*, int, char*, VMExec*, YYObjectBase*, YYObjectBase*)>;
#pragma pack(pop)