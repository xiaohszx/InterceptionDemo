// CalculatorFactory.cpp : Implementation of CCalculatorFactory

#include "stdafx.h"
#include "CalculatorFactory.h"
#include "Calculator.h"

class CGenericInterceptorSink : public CComObjectRoot, public ICallFrameEvents {
public:
	BEGIN_COM_MAP(CGenericInterceptorSink)
		COM_INTERFACE_ENTRY(ICallFrameEvents)
	END_COM_MAP()

	STDMETHOD(OnCall)(ICallFrame* pFrame) {
		PWSTR interfaceName, methodName;
		pFrame->GetNames(&interfaceName, &methodName);
		printf("%ws::%ws\n", interfaceName, methodName);
		::CoTaskMemFree(interfaceName);
		::CoTaskMemFree(methodName);

		// invoke the method on the real object

		pFrame->Invoke(m_spObject);

		return pFrame->GetReturnValue();
	}

	void SetObject(IUnknown* pObject) {
		m_spObject = pObject;
	}

	CComPtr<IUnknown> m_spObject;
};

struct __declspec(uuid("{CAEEA7BE-BB01-4B01-AEF4-DD3EDD914930}")) IDummyInterceptor : IUnknown {};

class CCalculatorInterceptor : public CComObjectRoot, public IDummyInterceptor {
public:
	BEGIN_COM_MAP(CCalculatorInterceptor)
		COM_INTERFACE_ENTRY(IDummyInterceptor)
		COM_INTERFACE_ENTRY_AGGREGATE(__uuidof(ICalculator), m_spCalcInterceptor.p)
		COM_INTERFACE_ENTRY_AGGREGATE(__uuidof(ICalculator2), m_spCalc2Interceptor.p)
	END_COM_MAP()

	DECLARE_GET_CONTROLLING_UNKNOWN()
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	void SetInterceptors(IUnknown* i1, IUnknown* i2) {
		m_spCalcInterceptor = i1;
		m_spCalc2Interceptor = i2;
	}

	CComPtr<IUnknown> m_spCalcInterceptor, m_spCalc2Interceptor;
};

// CCalculatorFactory

STDMETHODIMP CCalculatorFactory::CreateCalculator(ICalculator** ppCalculator) {
	CComPtr<ICallInterceptor> spInterceptor;
	auto hr = ::CoGetInterceptor(__uuidof(ICalculator), nullptr, __uuidof(ICallInterceptor), reinterpret_cast<void**>(&spInterceptor));
	if (FAILED(hr))
		return hr;

	CComObject<CCalculator>* pCalculator;
	hr = pCalculator->CreateInstance(&pCalculator);
	if (FAILED(hr))
		return hr;

	//CComPtr<ITypeLib> spTypeLibrary;
	//::LoadTypeLib(L"mytlb.tlb", &spTypeLibrary);
	//CComPtr<ITypeInfo>spTypeInfo;
	//spTypeLibrary->GetTypeInfoOfGuid(__uuidof(ICalculator), &spTypeInfo);
	//::CoGetInterceptorFromTypeInfo(__uuidof(ICalculator), nullptr, spTypeInfo, __uuidof(ICallInterceptor), 
	//	reinterpret_cast<void**>(&spInterceptor));

	CComObject<CGenericInterceptorSink>* pGenericInterceptor;
	pGenericInterceptor->CreateInstance(&pGenericInterceptor);
	pGenericInterceptor->SetObject(pCalculator->GetUnknown());

	spInterceptor->RegisterSink(pGenericInterceptor);

	return spInterceptor->QueryInterface(ppCalculator);
}

STDMETHODIMP CCalculatorFactory::CreateCalculator2(ICalculator** ppCalculator) {
	// create the manager

	CComObject<CCalculatorInterceptor>* pIntManager;
	auto hr = pIntManager->CreateInstance(&pIntManager);
	if (FAILED(hr))
		return hr;

	// create the real object

	CComObject<CCalculator>* pCalculator;
	hr = pCalculator->CreateInstance(&pCalculator);
	if (FAILED(hr))
		return hr;

	// create the interceptors

	CComPtr<IUnknown> spUnkInterceptor1;
	hr = ::CoGetInterceptor(__uuidof(ICalculator), pIntManager->GetControllingUnknown(), __uuidof(IUnknown), reinterpret_cast<void**>(&spUnkInterceptor1));
	if (FAILED(hr))
		return hr;

	CComPtr<IUnknown> spUnkInterceptor2;
	hr = ::CoGetInterceptor(__uuidof(ICalculator2), pIntManager->GetControllingUnknown(), __uuidof(IUnknown), reinterpret_cast<void**>(&spUnkInterceptor2));
	if (FAILED(hr))
		return hr;

	pIntManager->SetInterceptors(spUnkInterceptor1, spUnkInterceptor2);

	CComObject<CGenericInterceptorSink>* pGenericInterceptor;
	pGenericInterceptor->CreateInstance(&pGenericInterceptor);
	pGenericInterceptor->SetObject(pCalculator->GetUnknown());

	CComQIPtr<ICallInterceptor> spInterceptor1(spUnkInterceptor1);
	spInterceptor1->RegisterSink(pGenericInterceptor);

	CComQIPtr<ICallInterceptor> spInterceptor2(spUnkInterceptor2);
	spInterceptor2->RegisterSink(pGenericInterceptor);

	return pIntManager->QueryInterface(ppCalculator);
}
