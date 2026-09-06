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

#ifndef UTINPUTPREPROCESSOR_HPP
#define UTINPUTPREPROCESSOR_HPP

#include <cassert>
#include <deque>
#include <map>
#include <memory>
#include <set>
#include <stack>
#include <vector>

#include "UtInputBuffer.hpp"

// processes variable definitions
//    $define X 123
// substitutes variables
//    $<var:default value>$

//! Stores the variable definitions discovered and used by UtInputPreprocessorBuffer instances
class UtInputPreprocessor
{
public:
   //! Adds a new preprocessor variable
   void AddVariableDefine(const std::string& aVariableName, const std::string& aVariableValue)
   {
      mVariables[aVariableName] = aVariableValue;
   }

   //! Record the use of a variable.  Because variables have default values,
   //! not all variables need be defined, and the set of variables referenced
   //! is important information
   void AddVariableReference(const std::string& aVariableReference, const std::string& aDefaultValue = std::string{});

   //! Returns the value of a preprocessor variable if it exists, otherwise returns the default value provided
   std::string Expand(const std::string& aVariableName, const std::string& aVariableDefault)
   {
      AddVariableReference(aVariableName, aVariableDefault);
      std::map<std::string, std::string>::iterator i = mVariables.find(aVariableName);
      if (i != mVariables.end())
      {
         return i->second;
      }
      return aVariableDefault;
   }

   //! returns 'true' if the variable exists
   bool VariableExists(const std::string& aVariableName) const
   {
      return mVariables.find(aVariableName) != mVariables.end();
   }

   std::string SubstituteVariables(std::string aText);

   const std::map<std::string, std::string>& GetVariables() const { return mVariables; }

   const std::map<std::string, std::set<std::string>>& GetVariableReferences() const { return mVariableReferences; }

private:
   std::map<std::string, std::string>           mVariables;
   std::map<std::string, std::set<std::string>> mVariableReferences;
};


//! Encapsulates another UtInputBuffer, and provides
//! Preprocessing as that buffer is read for the first time.
//! Clones of this object will share the underlying preprocessed file to prevent
//! multiple passes of the preprocessor and needless processing
class UtInputPreprocessorBuffer : public UtInputBuffer
{
   //! Data shared between clones of UtInputPreprocessorBuffers
   class Private
   {
   public:
      enum State
      {
         cNORMAL,
         cLINE_COMMENT,
         cBLOCK_COMMENT,
         cVARIABLE_START,
         cVARIABLE_DEFAULT_VALUE,
         cDEFINE_VARIABLE,
         cDEFINE_VARIABLE_NAME,
         cDEFINE_VARIABLE_NAME_DONE,
         cDEFINE_VARIABLE_VALUE
      };

      Private(UtInputPreprocessor* aPreprocessor, std::unique_ptr<UtInputBuffer> aBuffer);
      ~Private() = default;
      char Next()
      {
         assert(!mLookahead.empty());
         char c = mLookahead[0];
         mLookahead.pop_front();
         ++mLookaheadOffset;
         return c;
      }
      void Next(int aCount)
      {
         assert((int)mLookahead.size() >= aCount);
         mLookahead.erase(mLookahead.begin(), mLookahead.begin() + aCount);
         mLookaheadOffset += aCount;
      }
      bool Get(char& aCh, size_t& aOffset);
      char Peek(size_t aOffset);
      bool Preprocess();
      void Passthrough();
      void Skip(int aCount = 1);
      void Push(char c);
      void Push(const std::string& aText);

      void Synchronize();

      char        PeekNext(size_t i);
      bool        ScanNext(size_t aOffset, const char* aStringSearch);
      std::string GetLocation(size_t aPos);
      std::string GetSource();
      std::string Expand(const std::string& aVariableName, const std::string& aDefaultValue);
      int         SourceToProcessedOffset(int aSourceOffset);
      size_t      ProcessedToSourceOffset(size_t aProcessedOffset);
      void        GetLineColumn(size_t aProcessedOffset, size_t& aLine, size_t& aColumn);
      void        AddVariableDefine(const std::string& aVariableName, const std::string& aVariableValue);

      //! The input buffer that is being preprocessed
      std::unique_ptr<UtInputBuffer> mBufferPtr;
      //! The preprocessor containing the shared variable definitions
      UtInputPreprocessor* mPreprocessor;
      // a hand-full of lookahead characters are needed to lex the file
      std::deque<char> mLookahead;
      // current offset of the source input buffer
      int mLookaheadOffset;
      //! Lexer state machine's state
      State mState;
      //! As the file is preprocessed, the output is stored here
      //! eventually holds the entire preprocessed file text
      std::vector<char> mPreprocessed;

      // A mapping between byte offsets of the source and preprocessed file texts
      // is kept with these variables:

      //! Records differences between source and destination offsets
      struct OffsetChange
      {
         int mSourceOffset;
         int mSourceLength;
         int mFinalLength;
      };

      std::vector<OffsetChange> mOffsetMapping;
      int                       mBytesSkipped;
      int                       mBytesPushed;
   };

public:
   UtInputPreprocessorBuffer(UtInputPreprocessor* aPreprocessor, std::unique_ptr<UtInputBuffer> aBuffer);
   UtInputPreprocessorBuffer(const UtInputPreprocessorBuffer& aSrc);
   ~UtInputPreprocessorBuffer() override = default;

   std::string GetFileName() const override;

   UtInputBuffer* Clone() const override;

   void SeekTo(size_t aOffset) override;

   size_t GetOffset() override { return mPos; }

   bool Get(char& aCh) override { return mData->Get(aCh, mPos); }

   bool UnGet() override;

   char Peek() override { return mData->Peek(mPos); }

   std::string RealGetSource() override { return mData->GetSource(); }

   std::string RealGetLocation() override { return mData->GetLocation(mPos); }

   void GetLineColumn(size_t& aLine, size_t& aColumn) override { return mData->GetLineColumn(mPos, aLine, aColumn); }

private:
   std::shared_ptr<Private> mData;
   //! The current seek position
   size_t mPos;
};

#endif
