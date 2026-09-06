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

#ifndef UTSCRIPTPARSER_HPP
#define UTSCRIPTPARSER_HPP

#include "util_script_export.h"

#include <vector>

#include "UtScript.hpp"
#include "UtScriptContext.hpp"
#include "UtScriptLanguageParser.hpp"
#include "UtScriptLanguageScanner.hpp"
#include "UtScriptStream.hpp"
#include "UtScriptTypes.hpp"

//--------------------------------------------------------------
//!  UtScriptParser Given parses the input (UtScriptStream) to
//!  create a UtScript object which, subsequently can be
//!  executed by a UtScriptContext.
//--------------------------------------------------------------

class UTIL_SCRIPT_EXPORT UtScriptParser
{
public:
   class StreamLocationReporter
   {
   public:
      StreamLocationReporter(){};
      virtual void OffsetToLineColumn(int aOffset, int& aLine, int& aColumn) = 0;
      virtual ~StreamLocationReporter() {}
   };

   UtScriptParser(UtScriptEnvironment* aTypesPtr);
   virtual ~UtScriptParser();

   struct ImplicitArgument
   {
      UtScriptClass* mTypePtr;
      std::string    mName;
   };
   typedef std::vector<ImplicitArgument> ImplicitArgumentList;

   UtScript* Parse(UtScriptStream* aStreamPtr, UtScriptContext* aContextPtr, const std::string& aNamespace);

   UtScript* Parse(const std::string&   aScriptName,
                   UtStringId           aScriptReturnType,
                   UtScriptStream*      aStreamPtr,
                   UtScriptContext*     aContextPtr,
                   const std::string&   aNamespace,
                   bool                 aIsVariableBlock = false,
                   ImplicitArgumentList aImplicitArgs    = ImplicitArgumentList());

   UtScript* ParseVariables(const std::string& aVariables, UtScriptContext* aContextPtr);

   UtScriptTypes* GetTypes();

private:
   void InitializeP(UtScriptTypes* aTypesPtr);

   UtScript* ParseP(UtScriptStream*      aStreamPtr,
                    UtScriptContext*     aContextPtr,
                    const std::string&   aNamespace,
                    std::string          aImplicitScriptName = std::string(),
                    UtStringId           aImplicitReturnType = {},
                    bool                 aVarDefBlock        = false,
                    ImplicitArgumentList aImplicitArgs       = ImplicitArgumentList());

   UtScriptEnvironment*         mEnvironmentPtr;
   bool                         mInitialized;
   UtScriptLanguage::UsConstCtx mConstCtx;
};

#endif
