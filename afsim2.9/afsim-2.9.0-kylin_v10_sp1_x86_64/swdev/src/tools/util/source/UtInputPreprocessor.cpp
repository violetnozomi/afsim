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

#include "UtInputPreprocessor.hpp"

#include <ctype.h>
#include <iostream>

#include "UtCast.hpp"
#include "UtLog.hpp"


UtInputPreprocessorBuffer::UtInputPreprocessorBuffer(UtInputPreprocessor*           aPreprocessor,
                                                     std::unique_ptr<UtInputBuffer> aBuffer)
   : mData(std::make_shared<Private>(aPreprocessor, std::move(aBuffer)))
{
   mPos = 0;
}

UtInputPreprocessorBuffer::UtInputPreprocessorBuffer(const UtInputPreprocessorBuffer& aSrc)
   : mData(aSrc.mData)
   , mPos(aSrc.mPos)
{
}

std::string UtInputPreprocessorBuffer::GetFileName() const
{
   return mData->mBufferPtr->GetFileName();
}

UtInputBuffer* UtInputPreprocessorBuffer::Clone() const
{
   return new UtInputPreprocessorBuffer(*this);
}

void UtInputPreprocessorBuffer::SeekTo(size_t aOffset)
{
   // scan more of the file if necessary
   while (aOffset >= mData->mPreprocessed.size() && mData->Preprocess())
   {
   }

   if (aOffset < mData->mPreprocessed.size())
   {
      mPos = aOffset;
   }
   else
   {
      // not enough data, set to end
      mPos = mData->mPreprocessed.size();
   }
}

bool UtInputPreprocessorBuffer::UnGet()
{
   if (mPos == 0)
   {
      return false;
   }
   else
   {
      --mPos;
   }
   return true;
}

char UtInputPreprocessorBuffer::Private::PeekNext(size_t i)
{
   if (i < mLookahead.size())
   {
      return mLookahead[i];
   }
   char c;
   while (mBufferPtr->Get(c))
   {
      mLookahead.push_back(c);
      if (mLookahead.size() > i)
      {
         return mLookahead[i];
      }
   }
   return 0;
}

// Preprocess at least one more character of the input
bool UtInputPreprocessorBuffer::Private::Preprocess()
{
   std::string variableName;
   std::string variableValue;
   std::string variableDefault;
   mBytesSkipped = mBytesPushed = 0;
   int outputSize               = (int)mPreprocessed.size();

   // keep processing until we return to normal mode, and have at least output one character
   while (mState != cNORMAL || (int)mPreprocessed.size() == outputSize)
   {
      char c   = PeekNext(0);
      char cn  = PeekNext(1);
      char cnn = PeekNext(2);
      switch (mState)
      {
      case cNORMAL:
         if (c == '\0')
         {
            // We got zero chars of input
            return false;
         }
         else if (c == '/' && cn == '/')
         {
            Passthrough();
            Passthrough();
            mState = cLINE_COMMENT;
         }
         else if (c == '#')
         {
            Passthrough();
            mState = cLINE_COMMENT;
         }
         else if (c == '/' && cn == '*')
         {
            Passthrough();
            Passthrough();
            mState = cBLOCK_COMMENT;
         }
         else if ((c == '\n' && cn == '$' && cnn != '<') || (c == '$' && cn != '<' && mLookaheadOffset == 0))
         {
            if (c == '$' && ScanNext(1, "define"))
            {
               Skip(7);
               mState = cDEFINE_VARIABLE;
            }
            else if (c == '\n' && ScanNext(2, "define"))
            {
               Skip(8);
               mState = cDEFINE_VARIABLE;
            }
            else
            {
               Passthrough();
            }
         }
         else if (c == '$')
         {
            if (cn == '$')
            {
               Skip(1);
               Passthrough();
            }
            else if (cn == '<' && (isalpha(cnn) != 0))
            {
               mState = cVARIABLE_START;
               Skip(2);
            }
            else
            {
               Passthrough();
            }
         }
         else
         {
            Passthrough();
         }
         break;
      case cLINE_COMMENT:
         if (c == '\n' || c == '\0')
         {
            mState = cNORMAL;
         }
         else
         {
            Passthrough();
         }
         break;
      case cBLOCK_COMMENT:
         if (c == '\0')
         {
            return false;
         }
         else if (c == '*' && cn == '/')
         {
            Passthrough();
            Passthrough();
            mState = cNORMAL;
         }
         else
         {
            Passthrough();
         }
         break;
      case cVARIABLE_START:
         if (c == '\0')
         {
            return false;
         }
         else if (c == ':')
         {
            mState = cVARIABLE_DEFAULT_VALUE;
            Skip();
         }
         else if (c == '>' && cn == '$')
         {
            Skip(2);
            Push(Expand(variableName, ""));
            variableDefault = variableName = std::string();
            mState                         = cNORMAL;
         }
         else
         {
            variableName += c;
            Skip();
         }
         break;
      case cVARIABLE_DEFAULT_VALUE:
         if (c == '\0')
         {
            return false;
         }
         else if (c == '>' && cn == '$')
         {
            Skip(2);
            mState = cNORMAL;
            Push(Expand(variableName, variableDefault));
            variableDefault = variableName = std::string();
         }
         else
         {
            variableDefault += c;
            Skip();
         }
         break;
      case cDEFINE_VARIABLE:
         if (c == '\0')
         {
            return false;
         }
         else if (isspace(c) != 0)
         {
            Skip();
            break;
         }
         variableName = "";
         mState       = cDEFINE_VARIABLE_NAME;
         continue;
      case cDEFINE_VARIABLE_NAME:
         if (c == '\n' || c == '\0')
         {
            if (!variableName.empty())
            {
               AddVariableDefine(variableName, "");
            }
            mState = cNORMAL;
         }
         else if (isspace(c) != 0)
         {
            mState = cDEFINE_VARIABLE_NAME_DONE;
         }
         else
         {
            Skip();
            variableName += c;
         }
         break;
      case cDEFINE_VARIABLE_NAME_DONE:
         if (c == '\n' || c == '\0')
         {
            AddVariableDefine(variableName, variableValue);
            Skip();
            mState = cNORMAL;
         }
         else if (c == '\\' && cn == '\n')
         {
            Skip(2);
         }
         else if (isspace(c) != 0)
         {
            Skip();
         }
         else
         {
            mState        = cDEFINE_VARIABLE_VALUE;
            variableValue = std::string();
         }
         break;
      case cDEFINE_VARIABLE_VALUE:
         if (c == '\\' && cn == '\n') // escape newline
         {
            Skip(2);
            variableValue += cn;
         }
         else if (c == '\\' && cn == '\\') // escape backslash
         {
            Skip(2);
            variableValue += cn;
         }
         else if (c == '\n' || c == '\0')
         {
            AddVariableDefine(variableName, variableValue);
            mState = cNORMAL;
         }
         else
         {
            variableValue += c;
            Skip();
         }
         break;
      }
   }
   Synchronize();
   return true;
}

// Determine if the ahead characters are a specific string
bool UtInputPreprocessorBuffer::Private::ScanNext(size_t aOffset, const char* aStringSearch)
{
   size_t      offset = aOffset;
   const char* strPtr = aStringSearch;
   while (*strPtr != 0)
   {
      if (PeekNext(offset) != *strPtr)
      {
         return false;
      }
      ++strPtr;
      ++offset;
   }
   return true;
}

UtInputPreprocessorBuffer::Private::Private(UtInputPreprocessor* aPreprocessor, std::unique_ptr<UtInputBuffer> aBuffer)
   : mBufferPtr(std::move(aBuffer))
   , mPreprocessor(aPreprocessor)
   , mLookaheadOffset(0)
   , mState(cNORMAL)
   , mBytesSkipped(0)
   , mBytesPushed(0)
{
}

bool UtInputPreprocessorBuffer::Private::Get(char& aCh, size_t& aOffset)
{
   do
   {
      if (aOffset < mPreprocessed.size())
      {
         aCh = mPreprocessed[aOffset++];
         return true;
      }
   } while (Preprocess());
   aCh = '\0';
   return false;
}

int UtInputPreprocessorBuffer::Private::SourceToProcessedOffset(int aSourceOffset)
{
   int delta = 0;
   for (OffsetChange& v : mOffsetMapping)
   {
      if (v.mSourceOffset + v.mSourceLength >= aSourceOffset)
      {
         if (v.mSourceOffset <= aSourceOffset)
         {
            return v.mSourceOffset;
         }
         break;
      }
      else
      {
         delta += v.mFinalLength - v.mSourceLength;
      }
   }
   return aSourceOffset + delta;
}

size_t UtInputPreprocessorBuffer::Private::ProcessedToSourceOffset(size_t aProcessedOffset)
{
   size_t delta      = 0;
   size_t srcOffset  = 0;
   size_t procOffset = 0;
   for (OffsetChange& v : mOffsetMapping)
   {
      if (procOffset + v.mFinalLength > aProcessedOffset)
      {
         return srcOffset;
      }
      std::ptrdiff_t change = v.mSourceOffset - srcOffset;
      // before substitution
      if (procOffset + change >= aProcessedOffset)
      {
         return aProcessedOffset - procOffset + srcOffset;
      }
      // within substitution
      if (procOffset + change + v.mFinalLength >= aProcessedOffset)
      {
         return ut::cast_to_size_t(v.mSourceOffset);
      }

      delta += v.mFinalLength - v.mSourceLength;
      srcOffset = v.mSourceOffset + v.mSourceLength;
      procOffset += change + v.mFinalLength;
   }
   return aProcessedOffset - delta;
}

std::string UtInputPreprocessorBuffer::Private::GetSource()
{
   return mBufferPtr->RealGetSource();
}

std::string UtInputPreprocessorBuffer::Private::GetLocation(size_t aPos)
{
   size_t saveOffset = mBufferPtr->GetOffset();
   mBufferPtr->SeekTo(ProcessedToSourceOffset(aPos));
   std::string location = mBufferPtr->RealGetLocation();
   mBufferPtr->SeekTo(saveOffset);
   return location;
}

void UtInputPreprocessorBuffer::Private::GetLineColumn(size_t aProcessedOffset, size_t& aLine, size_t& aColumn)
{
   size_t savedOffset = mBufferPtr->GetOffset();
   size_t srcOffset   = ProcessedToSourceOffset(aProcessedOffset);
   mBufferPtr->SeekTo(srcOffset);
   mBufferPtr->GetLineColumn(aLine, aColumn);
   mBufferPtr->SeekTo(savedOffset);
}

char UtInputPreprocessorBuffer::Private::Peek(size_t aOffset)
{
   do
   {
      if (aOffset < mPreprocessed.size())
      {
         return mPreprocessed[aOffset];
      }
   } while (Preprocess());
   return '\0';
}

//////////////////////////////////////////////////////////////////////////
// Synchronization:
//    A mapping must be kept between input byte offset and processed
//     output byte offset.  Every time a '$define', variable expansions,
//     or escaped character is processed, special attention is made to
//     keep the offset mapping up to date.
//
//     All proceOnly three operations; Push, Skip, and Pass-through
//////////////////////////////////////////////////////////////////////////

// Push a string to the output
void UtInputPreprocessorBuffer::Private::Push(const std::string& aText)
{
   mBytesPushed += (int)aText.size();
   mPreprocessed.insert(mPreprocessed.end(), aText.begin(), aText.end());
}

// Push a character to the output
void UtInputPreprocessorBuffer::Private::Push(char c)
{
   ++mBytesPushed;
   mPreprocessed.push_back(c);
}

// Skip n characters from the input buffer.  These will not be passed to the output
void UtInputPreprocessorBuffer::Private::Skip(int aCount)
{
   if (mBytesPushed != 0)
   {
      Synchronize();
   }
   mBytesSkipped += aCount;
   for (int i = 0; i < aCount; ++i)
   {
      Next();
   }
}

// Pass one byte from input to output
void UtInputPreprocessorBuffer::Private::Passthrough()
{
   Synchronize();
   mPreprocessed.push_back(Next());
}

// Applies skipped and pushed bytes to the offset mapping
void UtInputPreprocessorBuffer::Private::Synchronize()
{
   if (mBytesSkipped + mBytesPushed == 0)
   {
      return;
   }
   OffsetChange e;
   e.mSourceOffset = mLookaheadOffset - mBytesSkipped;
   e.mSourceLength = mBytesSkipped;
   e.mFinalLength  = mBytesPushed;
   mOffsetMapping.push_back(e);
   mBytesSkipped = mBytesPushed = 0;
}

void UtInputPreprocessorBuffer::Private::AddVariableDefine(const std::string& aVariableName,
                                                           const std::string& aVariableValue)
{
   if (!mPreprocessor->VariableExists(aVariableName))
   {
      mPreprocessor->AddVariableDefine(aVariableName, mPreprocessor->SubstituteVariables(aVariableValue));
   }
   else
   {
      auto out = ut::log::warning() << "Preprocessor variable cannot be re-defined.";
      out.AddNote() << "Variable: " << aVariableName;
   }
}

std::string UtInputPreprocessorBuffer::Private::Expand(const std::string& aVariableName, const std::string& aDefaultValue)
{
   std::string expanded = mPreprocessor->Expand(aVariableName, aDefaultValue);
   // note: default value is a single space
   //       this makes sure variable expansions cannot combine into a single token.
   //       Although combining variable expansions into a single token might be nice
   //       in some cases, it would be very difficult to support in the IDE.
   //       This functionality can be implemented with additional defines:
   //          $define  NUMBER 1
   //          $define  NAME   FRED
   //          $define  ID $<NAME>$$<NUMBER>$
   //    $<ID>$               - expands to -      FRED1
   //     where as, using this outside a $define:
   //    $<NAME>$$<NUMBER>$   - expands to -      FRED 1
   if (expanded.empty())
   {
      expanded = " ";
   }
   return expanded;
}


//! Does variable substitution on a string for any preprocessor variables referenced
//! @param aText The text for which substitution is done
//! ex:  SubstituteVariables("Hello $<myvar:default_value>$.");
std::string UtInputPreprocessor::SubstituteVariables(std::string aText)
{
   int substitutionCount = 0;
   for (int i = 0; i + 1 < (int)aText.size(); ++i)
   {
      char c  = aText[i];
      char cn = aText[i + 1];
      if (c == '$' && cn == '<')
      {
         if (i > 0 && aText[i - 1] == '$')
         {
            // escape
            aText.erase(aText.begin() + i);
            ++i;
         }
         else
         {
            std::string name;
            std::string defaultValue;
            bool        namePart = true;
            for (int j = i + 2; j + 1 < (int)aText.size(); ++j)
            {
               if (namePart && aText[j] == ':')
               {
                  namePart = false;
               }
               else if (aText[j] == '>' && aText[j + 1] == '$')
               {
                  int         variableSize = (j + 1) - i;
                  std::string expanded     = Expand(name, defaultValue);
                  int         sizeChange   = (int)expanded.size() - variableSize;
                  aText                    = aText.substr(0, i) + expanded + aText.substr(j + 2);
                  ++substitutionCount;
                  i = j + 2 + sizeChange;
                  break;
               }
               else if (namePart)
               {
                  name += aText[j];
               }
               else
               {
                  defaultValue += aText[j];
               }
            }
         }
      }
   }
   return aText;
}

void UtInputPreprocessor::AddVariableReference(const std::string& aVariableReference, const std::string& aDefaultValue)
{
   mVariableReferences[aVariableReference].insert(aDefaultValue);
}
