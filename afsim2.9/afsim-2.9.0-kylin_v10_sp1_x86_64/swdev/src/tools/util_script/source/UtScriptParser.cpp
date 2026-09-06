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

#include "UtScriptParser.hpp"

#include <cassert>
#include <sstream>

#include "UtInputString.hpp"
#include "UtLog.hpp"
#include "UtScriptEnvironment.hpp"
#include "UtScriptLanguageCtx.hpp"

//! Catches errors from the parser and outputs them to the screen.
//! Overrides the standard COCO/R Errors class.
class UsErrorInterface : public UtScriptLanguage::Errors
{
public:
   UtScriptStream* mStreamPtr;
   size_t          mDocOffset;

   UsErrorInterface();
   ~UsErrorInterface() override;

   void TokenLineCol(UtScriptLanguage::Token* tok, size_t& line, size_t& col);
   void PrintError(const std::string& aMessage, size_t aPos);

   void SynErr(UtScriptLanguage::Token* tok, int n) override;
   void Error(UtScriptLanguage::Token* tok, const cocochar_t* s) override;
   void Warning(UtScriptLanguage::Token* tok, const cocochar_t* s) override;
   void Warning(const cocochar_t* s) override;
   void Exception(const cocochar_t* s) override;

}; // Errors

UtScriptParser::UtScriptParser(UtScriptEnvironment* aEnvironmentPtr)
   : mInitialized(false)
{
   mEnvironmentPtr  = aEnvironmentPtr;
   mConstCtx.mEnv   = aEnvironmentPtr;
   mConstCtx.mTypes = &aEnvironmentPtr->GetTypes();
}

UtScriptParser::~UtScriptParser() {}

void UtScriptParser::InitializeP(UtScriptTypes* aTypesPtr)
{
   if (mInitialized)
   {
      return;
   }
   aTypesPtr->Initialize();

   mConstCtx.tyNull   = aTypesPtr->GetClass("null");
   mConstCtx.tyVoid   = aTypesPtr->GetClass("void");
   mConstCtx.tyVar    = aTypesPtr->GetClass("var");
   mConstCtx.tyBool   = aTypesPtr->GetClass("bool");
   mConstCtx.tyInt    = aTypesPtr->GetClass("int");
   mConstCtx.tyString = aTypesPtr->GetClass("string");
   mConstCtx.tyDouble = aTypesPtr->GetClass("double");
   mConstCtx.tyObject = aTypesPtr->GetClass("Object");
   mConstCtx.mTypes   = aTypesPtr;
   mInitialized       = true;
}

UtScript* UtScriptParser::ParseP(UtScriptStream*      aStreamPtr,
                                 UtScriptContext*     aContextPtr,
                                 const std::string&   aNamespace,
                                 std::string          aImplicitScriptName,
                                 UtStringId           aImplicitReturnType,
                                 bool                 aVarDefBlock,
                                 ImplicitArgumentList aImplicitArgs)
{
   InitializeP(aContextPtr->GetTypes());

   int                       chars;
   const char*               ptr = aStreamPtr->GetText(chars);
   UtScriptLanguage::Scanner s((const unsigned char*)ptr, chars);
   UtScriptLanguage::Parser  p(&s);

   // Replace the error handler in the scanner with our own derived version.
   UsErrorInterface* errorPtr = new UsErrorInterface;
   errorPtr->mStreamPtr       = aStreamPtr;
   errorPtr->mDocOffset       = aStreamPtr->GetOffset();
   delete p.mErrorsPtr;
   p.mErrorsPtr = errorPtr;

   mConstCtx.Reset();
   mConstCtx.mParser = &p;
   if (!aStreamPtr->GetFilePath().empty())
   {
      mConstCtx.mDocumentPath = (UtPath::WorkingDirectory() + aStreamPtr->GetFilePath()).GetSystemPath();
   }

   mConstCtx.mDocumentOffset = aStreamPtr->GetOffset();
   size_t line(0), col;
   aStreamPtr->OffsetToLineColumn(mConstCtx.mDocumentOffset, line, col);
   mConstCtx.mDocumentLineOffset = line;
   // mConstCtx.mScriptContext = aContextPtr;
   mConstCtx.mInstancePtr = aContextPtr;
   mConstCtx.mScopePtr    = aContextPtr->ModifyScope();

   p.mRootCx                = new UtScriptLanguage::UsCtx(&mConstCtx);
   mConstCtx.mTopCtx        = p.mRootCx;
   mConstCtx.mNamespace     = aNamespace;
   mConstCtx.mGlobalVarMode = aVarDefBlock;
   p.InitParse();
   if (aImplicitScriptName.empty())
   {
      p.func_def(*p.mRootCx);
   }
   else
   {
      UtScriptClass* rtypePtr = mConstCtx.mTypes->GetClass(aImplicitReturnType);
      if (rtypePtr)
      {
         UtScriptFunction fn;
         p.mRootCx->SetRType(fn, rtypePtr);
         for (size_t i = 0; i < aImplicitArgs.size(); ++i)
         {
            fn.mArgs.emplace_back(aImplicitArgs[i].mTypePtr->GetClassName());
         }
         UtScriptLanguage::UsCtx fncx(*p.mRootCx);
         UtScriptLanguage::UsVal fnVal = p.mRootCx->AddEntry(aImplicitScriptName, 0, fn, false, &fncx);
         if (fnVal.IsValid())
         {
            fncx.SetBlockType(UtScriptLanguage::UsCtx::cSCRIPT_BLOCK);
            for (size_t i = 0; i < aImplicitArgs.size(); ++i)
            {
               fncx.AddImplicitParam(aImplicitArgs[i].mTypePtr, aImplicitArgs[i].mName);
               fncx.mArgNames.push_back(aImplicitArgs[i].mName);
            }
            fncx.FuncDefStart(fnVal);
            p.block_func(fncx);
            p.mRootCx->FuncComplete(fnVal, fncx);
         }
      }
   }

   UtScript* regScript = p.mRootCx->mRegisteredScriptPtr;
   mConstCtx.mParser   = nullptr;
   mConstCtx.mScopePtr = nullptr;
   mConstCtx.mTypes    = aContextPtr->GetTypes();
   delete p.mRootCx;
   if (errorPtr->count > 0)
   {
      return nullptr;
   }
   else
   {
      return regScript;
   }
}

UtScript* UtScriptParser::Parse(UtScriptStream* aStreamPtr, UtScriptContext* aContextPtr, const std::string& aNamespace)
{
   return ParseP(aStreamPtr, aContextPtr, aNamespace);
}

UtScript* UtScriptParser::Parse(const std::string&   aScriptName,
                                UtStringId           aScriptReturnType,
                                UtScriptStream*      aStreamPtr,
                                UtScriptContext*     aContextPtr,
                                const std::string&   aNamespace,
                                bool                 aIsVariableBlock,
                                ImplicitArgumentList aImplicitArgs)
{
   return ParseP(aStreamPtr, aContextPtr, aNamespace, aScriptName, aScriptReturnType, aIsVariableBlock, aImplicitArgs);
}

UtScript* UtScriptParser::ParseVariables(const std::string& aVariables, UtScriptContext* aContextPtr)
{
   static int        varDefCount = 0;
   std::stringstream ss;
   ss << "__var_defs_" << ++varDefCount;

   UtInput tmpInput;
   tmpInput.PushInput(ut::make_unique<UtInputString>(aVariables));
   UtScriptStream scriptStream(tmpInput, "");
   return ParseP(&scriptStream, aContextPtr, "", ss.str(), "void", true);
}

UtScriptTypes* UtScriptParser::GetTypes()
{
   return mConstCtx.mTypes;
}

UsErrorInterface::UsErrorInterface()
   : mStreamPtr(nullptr)
   , mDocOffset(0)
{
}

UsErrorInterface::~UsErrorInterface() {}

void UsErrorInterface::TokenLineCol(UtScriptLanguage::Token* tok, size_t& line, size_t& col)
{
   size_t pos = tok->pos + mDocOffset;
   mStreamPtr->OffsetToLineColumn(pos, line, col);
   ++line;
   ++col;
}

void UsErrorInterface::PrintError(const std::string& aMessage, size_t aPos)
{
   aPos += mDocOffset;
   auto out =
      ut::log::error() << aMessage; // not sure what to put for summary here, seems the 'aMessage' arg provides the summary.
   std::string filePath = mStreamPtr->GetFilePath();
   if (!filePath.empty())
   {
      out.AddNote() << "File: " << filePath;
   }
   else
   {
      out.AddNote() << "In <string>.";
   }
   size_t line, col;
   mStreamPtr->OffsetToLineColumn(aPos, line, col);
   ++line;
   ++col;
   out.AddNote() << "Line: " << line;
   out.AddNote() << "Column: " << col;

   std::string lineText(mStreamPtr->GetLineStringFromOffset(aPos));
   if (!lineText.empty())
   {
      out.AddNote() << lineText;
      std::string location;
      for (size_t i = 1; i < std::min(col, lineText.size()); ++i)
      {
         location += ' ';
      }
      location += '^';
      out.AddNote() << location;
   }
}

void UsErrorInterface::SynErr(UtScriptLanguage::Token* tok, int n)
{
   cocochar_t* s = GetStaticError(n);
   if (!s)
   {
      // Fallback code: should never enter here
      assert(false);
      std::stringstream ss;
      ss << "Error " << n;
      PrintError(s, tok->pos);
   }
   else
   {
      PrintError(s, tok->pos);
   }
   UtScriptLanguage::coco_string_delete(s);
   count++;
}

void UsErrorInterface::Error(UtScriptLanguage::Token* tok, const cocochar_t* s)
{
   PrintError(s, tok->pos);
   count++;
}

void UsErrorInterface::Warning(UtScriptLanguage::Token* tok, const cocochar_t* s)
{
   PrintError(s, tok->pos);
}

void UsErrorInterface::Warning(const cocochar_t* s)
{
   // This should be unused.
   assert(false);
}

void UsErrorInterface::Exception(const cocochar_t* s)
{
   // This should be unused.
   assert(false);
}
