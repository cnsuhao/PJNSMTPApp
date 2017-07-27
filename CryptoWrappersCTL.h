/*
Module : CryptoWrappersCTL.h
Purpose: Defines the interface for a C++ wrapper class for a CryptoAPI PCCTL_CONTEXT
History: PJN / 01-08-2014 1. Initial public release
         PJN / 12-05-2015 1. Updated copyright details.
                          2. Updated code to be /analyze clean on VC 2015

Copyright (c) 2014 - 2017 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


////////////////////////// Macros / Defines ///////////////////////////////////

#pragma once

#ifndef __CRYPTOWRAPPERSCTL_H__
#define __CRYPTOWRAPPERSCTL_H__

#pragma comment(lib, "Crypt32.lib")  //Automatically link in the Crypto dll


////////////////////////// Includes ///////////////////////////////////////////

#ifndef __WINCRYPT_H__
#pragma message("To avoid this message, please put wincrypt.h in your pre compiled header (normally stdafx.h)")
#include <wincrypt.h>
#endif //#ifndef __WINCRYPT_H__


////////////////////////// Classes ////////////////////////////////////////////

namespace CryptoWrappers
{


//Encapsulates a CryptoAPI PCCTL_CONTEXT
class CCTL
{
public:
//Constructors / Destructors
  CCTL() : m_pCTL(nullptr)
  {
  }
  
  CCTL(_In_ CCTL&& ctl) : m_pCTL(nullptr)
  {
    Attach(ctl.Detach());
  }

  CCTL(_In_ const CCTL& ctl) : m_pCTL(nullptr)
  {
    if (!ctl.Duplicate(*this))
      m_pCTL = nullptr;
  }
  
  explicit CCTL(_In_opt_ PCCTL_CONTEXT pCTL) : m_pCTL(pCTL)
  {
  }

  ~CCTL()
  {
    if (m_pCTL != nullptr)
      Free();
  }
  
//Methods
  CCTL& operator=(_In_ const CCTL& ctl)
  {
    if (this != &ctl)
    {
      if (m_pCTL != nullptr)
        Free();
      if (!ctl.Duplicate(*this))
        m_pCTL = nullptr;
    }
    
    return *this;
  }

  CCTL& operator=(_In_ CCTL&& ctl)
  {
    if (m_pCTL != nullptr)
      Free();
    Attach(ctl.Detach());
    
    return *this;
  }

  PCCTL_CONTEXT Handle() const
  {
    return m_pCTL;
  }

  void Attach(_In_opt_ PCCTL_CONTEXT pCTL)
  {
    //Validate our parameters
    ATLASSERT(m_pCTL == nullptr);
    
    m_pCTL = pCTL;
  }

  PCCTL_CONTEXT Detach()
  {
    PCCTL_CONTEXT pCTL = m_pCTL;
    m_pCTL = nullptr;
    return pCTL;
  }

  _Success_(return != FALSE)
  BOOL Create(_In_ DWORD dwMsgAndCertEncodingType, _In_reads_bytes_(cbCtlEncoded) const BYTE* pbCtlEncoded, _In_ DWORD cbCtlEncoded)
  {
    //Validate our parameters
    ATLASSERT(m_pCTL == nullptr);
    
    m_pCTL = CertCreateCTLContext(dwMsgAndCertEncodingType, pbCtlEncoded, cbCtlEncoded);
    return (m_pCTL != nullptr);
  }

  _Success_(return != FALSE)
  BOOL Free()
  {
    //Validate our parameters
    ATLASSERT(m_pCTL != nullptr);

    BOOL bResult = CertFreeCTLContext(m_pCTL);
    m_pCTL = nullptr;
    return bResult;
  }

  _Success_(return != FALSE)
  BOOL Duplicate(_Inout_ CCTL& ctl) const
  {
    //Validate our parameters
    ATLASSERT(m_pCTL != nullptr);
    ATLASSERT(ctl.m_pCTL == nullptr);

    ctl.m_pCTL = CertDuplicateCTLContext(m_pCTL);
    return (ctl.m_pCTL != nullptr);
  }

  _Success_(return != FALSE)
  BOOL DeleteFromStore()
  {
    //Validate our parameters
    ATLASSERT(m_pCTL != nullptr);
    
    BOOL bResult = CertDeleteCTLFromStore(m_pCTL);
    m_pCTL = nullptr; //Required because CertDeleteCTLFromStore frees the pCtlContext parameter
    return bResult;
  }

  _Success_(return != FALSE)
  BOOL SerializeStoreElement(_Out_writes_bytes_to_opt_(*pcbElement, *pcbElement) BYTE* pbElement, _Inout_ DWORD* pcbElement)
  {
    //Validate our parameters
    ATLASSERT(m_pCTL != nullptr);

    return CertSerializeCTLStoreElement(m_pCTL, 0, pbElement, pcbElement);
  }

  _Success_(return != FALSE)
  BOOL DlgView(_In_opt_ HWND hwnd, _In_opt_ LPCWSTR pwszTitle)
  {
    //Validate our parameters
    ATLASSERT(m_pCTL != nullptr);

  #pragma warning(suppress: 6387) //There are SAL annotation errors on the sixth parameter to CryptUIDlgViewContext even in the latest Windows 10 SDK
    return CryptUIDlgViewContext(CERT_STORE_CTL_CONTEXT, m_pCTL, hwnd, pwszTitle, 0, nullptr);
  }

  DWORD EnumProperties(_In_ DWORD dwPropId)
  {
    //Validate our parameters
    ATLASSERT(m_pCTL != nullptr);

    return CertEnumCTLContextProperties(m_pCTL, dwPropId);
  }

  _Success_(return != FALSE)
  BOOL GetProperty(_In_ DWORD dwPropId, _Out_writes_bytes_to_opt_(*pcbData, *pcbData) void* pvData, _Inout_ DWORD* pcbData)
  {
    //Validate our parameters
    ATLASSERT(m_pCTL != nullptr);

    return CertGetCTLContextProperty(m_pCTL, dwPropId, pvData, pcbData);
  }

  _Success_(return != FALSE)
  BOOL SetProperty(_In_ DWORD dwPropId, _In_ DWORD dwFlags, _In_opt_ const void* pvData)
  {
    //Validate our parameters
    ATLASSERT(m_pCTL != nullptr);

    return CertSetCTLContextProperty(m_pCTL, dwPropId, dwFlags, pvData);
  }

  _Success_(return != FALSE)
  BOOL EnumSubjectInSorted(_Inout_ void** ppvNextSubject, _Out_ PCRYPT_DER_BLOB pSubjectIdentifier, _Out_ PCRYPT_DER_BLOB pEncodedAttributes)
  {
    //Validate our parameters
    ATLASSERT(m_pCTL != nullptr);

    return CertEnumSubjectInSortedCTL(m_pCTL, ppvNextSubject, pSubjectIdentifier, pEncodedAttributes);
  }

  PCTL_ENTRY FindSubject(_In_ DWORD dwEncodingType, _In_ DWORD dwSubjectType, _In_ void* pvSubject)
  {
    //Validate our parameters
    ATLASSERT(m_pCTL != nullptr);

    return CertFindSubjectInCTL(dwEncodingType, dwSubjectType, pvSubject, m_pCTL, 0);
  }

  BOOL FindSubjectInSorted(_In_ PCRYPT_DATA_BLOB pSubjectIdentifier, _Out_opt_ PCRYPT_DER_BLOB pEncodedAttributes)
  {
    //Validate our parameters
    ATLASSERT(m_pCTL != nullptr);

    return CertFindSubjectInSortedCTL(pSubjectIdentifier, m_pCTL, 0, nullptr, pEncodedAttributes);
  }

//Member variables
  PCCTL_CONTEXT m_pCTL;

  friend class CStore;
};


}; //namespace CryptoWrappers

#endif //#ifndef __CRYPTOWRAPPERSCTL_H__
