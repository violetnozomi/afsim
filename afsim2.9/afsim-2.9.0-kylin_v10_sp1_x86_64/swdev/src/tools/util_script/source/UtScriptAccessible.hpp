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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef UTSCRIPTACCESSIBLE_HPP
#define UTSCRIPTACCESSIBLE_HPP

#include "util_script_export.h"

class UtScriptContext;

//! Classes which may be exposed to script should implement this interface.
//! If this class is not implemented, up-casting (e.g. WsfAssociationMessage msg = (WsfAssociationMessage)MESSAGE; )
//! cannot be done safely and is disabled.
class UTIL_SCRIPT_EXPORT UtScriptAccessible
{
public:
   virtual ~UtScriptAccessible() {}
   virtual const char* GetScriptClassName() const = 0;
   //! Override this if this object owns a script context
   virtual UtScriptContext* GetScriptAccessibleContext() const { return nullptr; }
};

template<class T>
constexpr const char* UtScriptClassName()
{
   return nullptr;
}

//! allows overriding the script name of a c++ type for use
//! when the c++ name and the script name don't match up.
//! also ties UtScriptClassName<CppClass> and CppClass::GetScriptClassName() to ensure they don't deviate.
#define UT_MAP_CLASS_TO_SCRIPT_NAME(CppClass, QuotedScriptName) \
   template<>                                                   \
   constexpr const char* UtScriptClassName<CppClass>()          \
   {                                                            \
      return QuotedScriptName;                                  \
   }                                                            \
   inline const char* CppClass::GetScriptClassName() const { return QuotedScriptName; }

#endif
