// CalculatorFactory.h : Declaration of the CCalculatorFactory

#pragma once
#include "resource.h"       // main symbols

#include "CalculatorsSvr_i.h"

using namespace ATL;


// CCalculatorFactory

class ATL_NO_VTABLE CCalculatorFactory :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCalculatorFactory, &CLSID_CalculatorFactory>,
	public ICalculatorFactory {
public:
	CCalculatorFactory() {
	}

	DECLARE_REGISTRY_RESOURCEID(106)


	BEGIN_COM_MAP(CCalculatorFactory)
		COM_INTERFACE_ENTRY(ICalculatorFactory)
	END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct() {
		return S_OK;
	}

	void FinalRelease() {
	}

public:
	STDMETHOD(CreateCalculator)(ICalculator** ppCalculator);
	STDMETHOD(CreateCalculator2)(ICalculator** ppCalculator);
};

OBJECT_ENTRY_AUTO(__uuidof(CalculatorFactory), CCalculatorFactory)
