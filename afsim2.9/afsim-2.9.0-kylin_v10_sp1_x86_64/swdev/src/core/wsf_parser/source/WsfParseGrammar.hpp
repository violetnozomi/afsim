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

#ifndef WSFPARSEGRAMMAR_HPP
#define WSFPARSEGRAMMAR_HPP

#include "wsf_parser_export.h"

#include "UtCast.hpp"
#include "UtReferenceCount.hpp"
#include "UtTextDocument.hpp"
class WsfParser;
class WsfParseNode;
#include <string>
#include <vector>

// Implements copy-on-write
template<class T>
class UtSharablePointer
{
public:
   //! Create an empty shared pointer.
   //! Must be initialized with an assignment operation using another UtSharedPtr.
   explicit UtSharablePointer()
      : mObjectPtr(nullptr)
      , mCountPtr(nullptr)
   {
   }

   //! Create a shared pointer that takes ownership of the specified object.
   //! @param aObjectPtr A pointer to the object to be managed as a shared object.
   explicit UtSharablePointer(T* aObjectPtr)
      : mObjectPtr(aObjectPtr)
      , mCountPtr(nullptr)
   {
      if (mObjectPtr != nullptr)
      {
         mCountPtr = new UtReferenceCount;
         mCountPtr->AddStrongRef();
      }
   }

   //! Create a copy of shared object pointer.
   UtSharablePointer(const UtSharablePointer<T>& aSrc)
      : mObjectPtr(aSrc.mObjectPtr)
      , mCountPtr(aSrc.mCountPtr)
   {
      if (mCountPtr != nullptr)
      {
         mCountPtr->AddStrongRef();
      }
   }

   //! Destroy this shared object pointer.
   //! If all shared pointers pointing to the underlying a
   ~UtSharablePointer() { ReleaseReference(); }

   //! Assignment operator.
   //!
   //! @note: operator=(T*) is explicitly not provided for safety.
   //! Use lhs = std::shared_ptr<T>(tPtr);
   //! This is for compatibility with the 'shared_ptr'.
   UtSharablePointer<T>& operator=(const UtSharablePointer<T>& aRhs)
   {
      if (this != &aRhs)
      {
         ReleaseReference();
         mObjectPtr = aRhs.mObjectPtr;
         mCountPtr  = aRhs.mCountPtr;
         // Increment reference count if this is a non-null reference
         if (mCountPtr != nullptr)
         {
            mCountPtr->AddStrongRef();
         }
      }
      return *this;
   }

   //! Return 'true' if the pointer to the object is null
   bool IsNull() const { return mObjectPtr == nullptr; }

   //! Returns true if both point to the same object
   bool operator==(const UtSharablePointer<T>& aRHS) const { return mCountPtr == aRHS.mCountPtr; }

   //! Returns false if both point to the same object
   bool operator!=(const UtSharablePointer<T>& aRHS) const { return mCountPtr != aRHS.mCountPtr; }

   //! Provides strict weak ordering based on what the UtSharedPtr is referencing
   bool operator<(const UtSharablePointer<T>& aRHS) const { return mCountPtr < aRHS.mCountPtr; }

   const T* Read()
   {
      if (mCountPtr && mCountPtr->IsValid())
      {
         return mObjectPtr;
      }
      return nullptr;
   }
   T* Write()
   {
      if (mCountPtr && mCountPtr->IsValid())
      {
         if (mCountPtr->GetStrongCount() == 1)
         {
            return mObjectPtr;
         }
         else
         {
            mObjectPtr = new T(*mObjectPtr);
            mCountPtr  = new UtReferenceCount(1);
         }
      }
      return nullptr;
   }

   void Swap(UtSharablePointer<T>& aRhs)
   {
      std::swap(mObjectPtr, aRhs.mObjectPtr);
      std::swap(mCountPtr, aRhs.mCountPtr);
   }

private:
   //! Decrement the reference count (if defined) and release the
   //! shared object memory if the count has gone to zero.
   void ReleaseReference()
   {
      if (mCountPtr != nullptr)
      {
         if (mCountPtr->RemoveStrongRef())
         {
            delete mObjectPtr;
            mObjectPtr = nullptr;
         }
         mCountPtr = nullptr;
      }
   }

   T*                mObjectPtr;
   UtReferenceCount* mCountPtr;
};

namespace std
{
template<typename T>
void swap(UtSharablePointer<T>& lhs, UtSharablePointer<T>& rhs)
{
   lhs.Swap(rhs);
}
} // namespace std

namespace WsfGrammar
{

// The 'M' namespace defines a very simple structure to hold the result of parsing the grammar
// Once all the data is read, it is processed further.
namespace M
{

enum RuleType
{
   cRT_SEQUENCE,
   cRT_ALTERNATE,
   // Expression:   (Type <parameter>*)
   cRT_EXPRESSION,
   cRT_LITERAL,
   cRT_ROOT,
   cRT_ACTION,
   cRT_ACTION_CALL
};

enum Flags
{
   cRF_RECURRENCE_STAR     = 1,
   cRF_RECURRENCE_PLUS     = 2,
   cRF_RECURRENCE_QUESTION = 4
};

// Represents an element of the grammar file
// Expression:
//    (<type>  <words>*)
// Sequence:
//    { <words> }
// Alternate
//    sequence | sequence | sequence      ( each sequence is a word())
// Literal:
//    "text"   or   text
// Action:
//    [word;word;...]

class WSF_PARSER_EXPORT Expr
{
public:
   Expr()
      : mRuleType(cRT_SEQUENCE)
      , mFlags(0)
   {
   }
   ~Expr() = default;
   Expr(const Expr& aSrc);
   Expr&       operator=(const Expr& aRhs);
   static Expr Literal(const std::string& aText)
   {
      Expr lit;
      lit.SetLiteral(aText);
      return lit;
   }

   void Clear()
   {
      if (!mWords.IsNull())
      {
         mWords.Write()->clear();
      }
   }
   void SetLiteral(const std::string& aLiteral)
   {
      mText     = aLiteral;
      mRuleType = cRT_LITERAL;
   }

   void Swap(Expr& aRhs);
   // adds a subexpression efficiently by taking ownership of the input
   void PushMove(Expr& aExp)
   {
      NeedWords();
      ExprList* wordList = mWords.Write();
      wordList->push_back(Expr());
      aExp.Swap(wordList->back());
   }
   Expr& operator+=(const Expr& aExp)
   {
      NeedWords();
      mWords.Write()->push_back(aExp);
      return *this;
   }
   size_t      WordCount() const { return mWords.IsNull() ? 0 : mWords.Read()->size(); }
   const Expr& GetWord(size_t aIndex) const;
   Expr&       GetWord(size_t aIndex);

   std::vector<Expr>& words()
   {
      NeedWords();
      return *mWords.Write();
   }
   const std::vector<Expr>& words() const
   {
      NeedWords();
      return *mWords.Read();
   }
   RuleType mRuleType;
   // Expression type
   std::string mType;
   // parameter name if this is a named parameter
   std::string mLabel;
   // Used for single-word expressions / aka literals
   // Used for actions -- name of function called
   std::string mText;

private:
   void NeedWords() const;
   using ExprList = std::vector<Expr>;
   mutable UtSharablePointer<ExprList> mWords;

public:
   int mFlags;
};


} // namespace M
} // namespace WsfGrammar

#endif
