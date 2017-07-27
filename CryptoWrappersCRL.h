/*
Module : CryptoWrappersCRL.h
Purpose: Defines the interface for a C++ wrapper class for a CryptoAPI PCCRL_CONTEXT
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

#ifndef __CRYPTOWRAPPERSCRL_H__
#define __CRYPTOWRAPPERSCRL_H__

#pragma comment(lib, "Crypt32.lib")  //Automatically link in the Crypto dll


////////////////////////// Includes ///////////////////////////////////////////

#ifndef __WINCRYPT_H__
#pragma message("To avoid this message, please put wincrypt.h in your pre compiled header (normally stdafx.h)")
#include <wincrypt.h>
#endif //#ifndef __WINCRYPT_H__


////////////////////////// Classes ////////////////////////////////////////////

namespace CryptoWrappers
{


//Encapsulates a CryptoAPI PCCRL_CONTEXT
class CCRL
{
public:
//Constructors / Destructors
  CCRL() : m_pCRL(nullptr)
  {
  }
  
  CCRL(_In_ CCRL&& crl) : m_pCRL(nullptr)
  {
    Attach(crl.Detach());
  }

  CCRL(_In_ const CCRL& crl) : m_pCRL(nullptr)
  {
    if (!crl.Duplicate(*this))
      m_pCRL = nullptr;
  }
  
  explicit CCRL(_In_opt_ PCCRL_CONTEXT pCRL) : m_pCRL(pCRL)
  {
  }

  ~CCRL()
  {
    if (m_pCRL != nullptr)
      Free();
  }
  
//Methods
  CCRL& operator=(_In_ const CCRL& crl)
  {
    if (this != &crl)
    {
      if (m_pCRL != nullptr)
        Free();
      if (!crl.Duplicate(*this))
        m_pCRL = nullptr;
    }
    
    return *this;
  }

  CCRL& operator=(_In_ CCRL&& crl)
  {
    if (m_pCRL != nullptr)
      Free();
    Attach(crl.Detach());
    
    return *this;
  }

  PCCRL_CONTEXT Handle() const
  {
    return m_pCRL;
  }

  void Attach(_In_opt_ PCCRL_CONTEXT pCRL)
  {
    //Validate our parameters
    ATLASSERT(m_pCRL == nullptr);
    
    m_pCRL = pCRL;
  }

  PCCRL_CONTEXT Detach()
  {
    PCCRL_CONTEXT pCRL = m_pCRL;
    m_pCRL = nullptr;
    return pCRL;
  }

  _Success_(return != FALSE)
  BOOL Create(_In_ DWORD dwCertEncodingType, _In_reads_bytes_(cbCrlEncoded) const BYTE* pbCrlEncoded, _In_ DWORD cbCrlEncoded)
  {
    //Validate our parameters
    ATLASSERT(m_pCRL == nullptr);
    
    m_pCRL = CertCreateCRLContext(dwCertEncodingType, pbCrlEncoded, cbCrlEncoded);
    return (m_pCRL != nullptr);
  }

  _Success_(return != FALSE)
  BOOL Free()
  {
    //Validate our parameters
    ATLASSERT(m_pCRL != nullptr);

    BOOL bResult = CertFreeCRLContext(m_pCRL);
    m_pCRL = nullptr;
    return bResult;
  }

  _Success_(return != FALSE)
  BOOL Duplicate(_Inout_ CCRL& crl) const
  {
    //Validate our parameters
    ATLASSERT(m_pCRL != nullptr);
    ATLASSERT(crl.m_pCRL == nullptr);

    crl.m_pCRL = CertDuplicateCRLContext(m_pCRL);
    return (crl.m_pCRL != nullptr);
  }

  _Success_(return != FALSE)
  BOOL DeleteFromStore()
  {
    //Validate our parameters
    ATLASSERT(m_pCRL != nullptr);
    
    BOOL bResult = CertDeleteCRLFromStore(m_pCRL);
    m_pCRL = nullptr; //Required because CertDeleteCRLFromStore frees the pPrevCertContext parameter
    return bResult;
  }

  _Success_(return != FALSE)
  BOOL SerializeStoreElement(_Out_writes_bytes_to_opt_(*pcbElement, *pcbElement) BYTE* pbElement,_Inout_ DWORD* pcbElement)
  {
    //Validate our parameters
    ATLASSERT(m_pCRL != nullptr);

    return CertSerializeCRLStoreElement(m_pCRL, 0, pbElement, pcbElement);
  }

  _Success_(return != FALSE)
  BOOL DlgView(_In_opt_ HWND hwnd, _In_opt_ LPCWSTR pwszTitle)
  {
    //Validate our parameters
    ATLASSERT(m_pCRL != nullptr);

  #pragma warning(suppress: 6387) //There are SAL annotation errors on the sixth parameter to CryptUIDlgViewContext even in the latest Windows 10 SDK
    return CryptUIDlgViewContext(CERT_STORE_CRL_CONTEXT, m_pCRL, hwnd, pwszTitle, 0, nullptr);
  }

  _Success_(return != FALSE)
  BOOL FindCertificate(_In_ CCertificate& certificate, _Out_ PCRL_ENTRY* ppCrlEntry)
  {
    //Validate our parameters
    ATLASSERT(m_pCRL != nullptr);

    return CertFindCertificateInCRL(certificate.Handle(), m_pCRL, 0, nullptr, ppCrlEntry);
  }

  DWORD EnumProperties(_In_ DWORD dwPropId)
  {
    //Validate our parameters
    ATLASSERT(m_pCRL != nullptr);

    return CertEnumCRLContextProperties(m_pCRL, dwPropId);
  }

  _Success_(return != FALSE)
  BOOL GetProperty(_In_ DWORD dwPropId, _Out_writes_bytes_to_opt_(*pcbData, *pcbData) void* pvData, _Inout_ DWORD* pcbData)
  {
    //Validate our parameters
    ATLASSERT(m_pCRL != nullptr);

    return CertGetCRLContextProperty(m_pCRL, dwPropId, pvData, pcbData);
  }

  _Success_(return != FALSE)
  BOOL SetProperty(_In_ DWORD dwPropId, _In_ DWORD dwFlags, _In_opt_ const void* pvData)
  {
    //Validate our parameters
    ATLASSERT(m_pCRL != nullptr);

    return CertSetCRLContextProperty(m_pCRL, dwPropId, dwFlags, pvData);
  }

//Member variables
  PCCRL_CONTEXT m_pCRL;

  friend class CStore;
};


}; //namespace CryptoWrappers

#endif //#ifndef __CRYPTOWRAPPERSCRL_H__
