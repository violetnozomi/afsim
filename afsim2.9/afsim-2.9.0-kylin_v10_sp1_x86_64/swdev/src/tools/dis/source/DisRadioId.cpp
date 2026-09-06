// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "DisRadioId.hpp"

#include <iostream>

#include "Dis.hpp"
#include "DisEntityId.hpp"
#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"
#include "UtLog.hpp"

#ifdef _WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x400
#endif
#include <winsock.h>

#include <windows.h>
#else // Linux
#include <arpa/inet.h>
#endif

DisUint16 DisRadioId::mNextRadio = 0;

DisRadioId::DisRadioId(DisUint16 aSite, DisUint16 aApplication, DisUint16 aEntity, DisUint16 aRadio)
   : mSite(aSite)
   , mApplication(aApplication)
   , mEntity(aEntity)
   , mRadio(aRadio)
{
}

DisRadioId::DisRadioId(const DisEntityId& aId, DisUint16 aRadio)
   : mSite(aId.GetSite())
   , mApplication(aId.GetApplication())
   , mEntity(aId.GetEntity())
   , mRadio(aRadio)
{
}

// Set the next entity number to be assigned by 'AssignNewRadio()'.
// static
void DisRadioId::SetNextRadio(DisUint16 aNextRadio)
{
   mNextRadio = aNextRadio;
}

// Assigns the next sequential entity number.  Site and Application
// are not modified.
void DisRadioId::AssignNewRadio()
{
   while ((mNextRadio == NO_ENTITY) || (mNextRadio == ALL_ENTITIES) || (mNextRadio == RQST_ASSIGN_ID))
   {
      ++mNextRadio;
   }
   mRadio = mNextRadio;
   ++mNextRadio;
}

void DisRadioId::Get(GenI& aGenI)
{
   aGenI >> mSite;
   aGenI >> mApplication;
   aGenI >> mEntity;
   aGenI >> mRadio;
}

void DisRadioId::Put(GenO& aGenO) const
{
   aGenO << mSite;
   aGenO << mApplication;
   aGenO << mEntity;
   aGenO << mRadio;
}

std::ostream& DisRadioId::Pack(std::ostream& aOs, bool aToNet) const
{
   if (aOs.good())
   {
      DisUint16 tmp[4];
      tmp[0] = aToNet ? htons(mSite) : mSite;
      tmp[1] = aToNet ? htons(mApplication) : mApplication;
      tmp[2] = aToNet ? htons(mEntity) : mEntity;
      tmp[3] = aToNet ? htons(mRadio) : mRadio;
      aOs.write(reinterpret_cast<const char*>(tmp), 4 * sizeof(DisUint16));
      if (aOs.fail() && Dis::ShowError(Dis::cIO_ERRORS))
      {
         ut::log::error() << "DisRadioId::Pack stream failed.";
      }
   }
   else if (Dis::ShowError(Dis::cIO_ERRORS))
   {
      ut::log::error() << "DisRadioId::Pack was sent a stream that was not good.";
   }
   return aOs;
}

std::istream& DisRadioId::UnPack(std::istream& aIs, bool aToHost)
{
   if (aIs.good())
   {
      DisUint16 tmp[4];
      aIs.read(reinterpret_cast<char*>(tmp), 4 * sizeof(DisUint16));
      mSite        = aToHost ? ntohs(tmp[0]) : tmp[0];
      mApplication = aToHost ? ntohs(tmp[1]) : tmp[1];
      mEntity      = aToHost ? ntohs(tmp[2]) : tmp[2];
      mRadio       = aToHost ? ntohs(tmp[3]) : tmp[3];
      if (aIs.fail() && Dis::ShowError(Dis::cIO_ERRORS))
      {
         ut::log::error() << "DisRadioId::UnPack stream failed.";
      }
   }
   else if (Dis::ShowError(Dis::cIO_ERRORS))
   {
      ut::log::error() << "DisRadioId::UnPack was sent a stream that was not good.";
   }
   return aIs;
}

inline bool DisRadioId::operator==(const DisEntityId& aRhs) const
{
   return ((mEntity == aRhs.GetEntity()) && (mApplication == aRhs.GetApplication()) && (mSite == aRhs.GetSite()));
}

inline bool DisRadioId::operator!=(const DisEntityId& aRhs) const
{
   return ((mEntity != aRhs.GetEntity()) || (mApplication != aRhs.GetApplication()) || (mSite != aRhs.GetSite()));
}

bool DisRadioId::operator<(const DisRadioId& aRhs) const
{
   bool isLess = true;

   if (mSite > aRhs.mSite)
   {
      isLess = false;
   }
   else if (mSite == aRhs.mSite)
   {
      if (mApplication > aRhs.mApplication)
      {
         isLess = false;
      }
      else if (mApplication == aRhs.mApplication)
      {
         if (mEntity > aRhs.mEntity)
         {
            isLess = false;
         }
         else if (mEntity == aRhs.mEntity)
         {
            if (mRadio >= aRhs.mRadio)
            {
               isLess = false;
            }
         }
      }
   }
   return isLess;
}

bool DisRadioId::IsValid() const
{
   return true;
}

void DisRadioId::Stream(std::ostream& aStream) const
{
   aStream << ToString();
}

std::string DisRadioId::ToString() const
{
   return std::to_string(mSite) + ':' + std::to_string(mApplication) + ':' + std::to_string(mEntity) + ':' +
          std::to_string(mRadio);
}
