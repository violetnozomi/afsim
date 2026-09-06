// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef LINK16_MESSAGEDEFINES_HPP
#define LINK16_MESSAGEDEFINES_HPP

#include "FieldAccessorTypes.hpp"
#include "FieldTypes.hpp"
#include "MessageSizeChecker.hpp"
#include "Messages.hpp"
#include "UtMemory.hpp"

//! This file should be included prior to defining a message.  Currently there is a limit of 32 fields per message.
namespace WsfL16
{
namespace Messages
{
class MessageReader;
class MessageWriter;
} // namespace Messages
} // namespace WsfL16

//! Define a virtual TransferI() for each message.
#define DEFINE_VIRTUAL_TRANSERS__()                                 \
   virtual void TransferI(MessageBitInput& aMR) { Transfer(aMR); }  \
   virtual void TransferO(MessageBitOutput& aMW) { Transfer(aMW); } \
   virtual void CreateAccessor(MessageAccessorFiller& aMP) { Transfer(aMP); }

//! This should be used at the top of the public: section of each Initial message
#define DEFINE_INITIAL(_LABEL_, _SUB_LABEL_)                                        \
public:                                                                             \
   virtual InitialBase* Clone() const { return new Initial(*this); }                \
                                                                                    \
protected:                                                                          \
   static const unsigned int ___RequiredMessageSize = 57;                           \
                                                                                    \
public:                                                                             \
   virtual void GetLabel(int& aLabel, int& aSubLabel) const                         \
   {                                                                                \
      aLabel    = _LABEL_;                                                          \
      aSubLabel = _SUB_LABEL_;                                                      \
   }                                                                                \
   virtual int      GetUID() const { return (_LABEL_ * 100 + _SUB_LABEL_) * 1000; } \
   static const int cWORD_LABEL     = _LABEL_;                                      \
   static const int cWORD_SUB_LABEL = _SUB_LABEL_;                                  \
   DEFINE_VIRTUAL_TRANSERS__()

//! This should be used at the top of the public: section of each Extension message
#define DEFINE_EXTENSION(_LABEL_, _SUB_LABEL_, _EXT_NUM_)                                                    \
public:                                                                                                      \
   virtual ExtensionBase* Clone() const { return new Extension##_EXT_NUM_(*this); }                          \
                                                                                                             \
protected:                                                                                                   \
   static const unsigned int ___RequiredMessageSize = 68;                                                    \
                                                                                                             \
public:                                                                                                      \
   static const unsigned int cEXTENSION_NUMBER = _EXT_NUM_;                                                  \
   virtual int               GetExtensionNumber() const { return cEXTENSION_NUMBER; }                        \
   virtual int               GetUID() const { return (_LABEL_ * 100 + _SUB_LABEL_) * 1000 + _EXT_NUM_ + 1; } \
   static const int          cWORD_LABEL     = _LABEL_;                                                      \
   static const int          cWORD_SUB_LABEL = _SUB_LABEL_;                                                  \
   DEFINE_VIRTUAL_TRANSERS__()

//! This should be used at the top of the public: section of each Continuation message
#define DEFINE_CONTINUATION(_LABEL_, _SUB_LABEL_, _CONT_NUM_)                                                  \
public:                                                                                                        \
   virtual ContinuationBase* Clone() const { return new Continuation##_CONT_NUM_(*this); }                     \
                                                                                                               \
protected:                                                                                                     \
   static const unsigned int ___RequiredMessageSize = 63;                                                      \
                                                                                                               \
public:                                                                                                        \
   static const unsigned int cCONTINUATION_NUMBER = _CONT_NUM_;                                                \
   virtual int               GetLabel() const { return _CONT_NUM_; }                                           \
   virtual int               GetUID() const { return (_LABEL_ * 100 + _SUB_LABEL_) * 1000 + _CONT_NUM_ * 10; } \
   static const int          cWORD_LABEL     = _LABEL_;                                                        \
   static const int          cWORD_SUB_LABEL = _SUB_LABEL_;                                                    \
   DEFINE_VIRTUAL_TRANSERS__()

//
//! A DEFINE_MEMBERS() macro should be used in each message type.  Look at
//! any message type for an example of how to use.  This will define how
//! the message reads and writes data, and check the message size to ensure
//! it is 70 bits.
//!  !! You will get a compiler error if the message isn't 70 bits !!
//
//! MessageDefines_Details.hpp contains code generated using a script: MessageDefines_Generate.py
#include "MessageDefines_Details.hpp"

//
//! The following \#define's make type - specific functions for getting
//! the continuation and extension words in a message.
//

// Helper definition...
#define DEFINE_CONT_CASE__(NUM) \
   case NUM:                    \
      return ut::make_unique<Continuation##NUM>();
#define DEFINE_CONT_CASE2__(N1, N2) DEFINE_CONT_CASE__(N1) DEFINE_CONT_CASE__(N2)
#define DEFINE_CONT_CASE4__(N1, N2, N3, N4) DEFINE_CONT_CASE2__(N1, N2) DEFINE_CONT_CASE2__(N3, N4)
#define DEFINE_CONT_CASE8__(N1, N2, N3, N4, N5, N6, N7, N8) \
   DEFINE_CONT_CASE4__(N1, N2, N3, N4) DEFINE_CONT_CASE4__(N5, N6, N7, N8)

#define DEFINE_CONT_DEFS(NUM) using Continuation##NUM##Type = Continuation##NUM;
#define DEFINE_CONT_DEFS2(N1, N2) DEFINE_CONT_DEFS(N1) DEFINE_CONT_DEFS(N2)
#define DEFINE_CONT_DEFS3(N1, N2, N3) DEFINE_CONT_DEFS2(N1, N2) DEFINE_CONT_DEFS(N3)
#define DEFINE_CONT_DEFS4(N1, N2, N3, N4) DEFINE_CONT_DEFS2(N1, N2) DEFINE_CONT_DEFS2(N3, N4)
#define DEFINE_CONT_DEFS8(N1, N2, N3, N4, N5, N6, N7, N8) \
   DEFINE_CONT_DEFS4(N1, N2, N3, N4) DEFINE_CONT_DEFS4(N5, N6, N7, N8)

// One of these should be in each Initial word definitions
#define DEFINE_CONTINUATIONS12(N1, N2, N3, N4, N5, N6, N7, N8, N9, N10, N11, N12)                                  \
   DEFINE_CONT_DEFS8(N1, N2, N3, N4, N5, N6, N7, N8)                                                               \
   DEFINE_CONT_DEFS4(N9, N10, N11, N12) std::unique_ptr<ContinuationBase> CreateContinuationWord(int lbl) override \
   {                                                                                                               \
      switch (lbl)                                                                                                 \
      {                                                                                                            \
         DEFINE_CONT_CASE8__(N1, N2, N3, N4, N5, N6, N7, N8) DEFINE_CONT_CASE4__(N9, N10, N11, N12)                \
      }                                                                                                            \
      return nullptr;                                                                                              \
   }
#define DEFINE_CONTINUATIONS11(N1, N2, N3, N4, N5, N6, N7, N8, N9, N10, N11)                                      \
   DEFINE_CONT_DEFS8(N1, N2, N3, N4, N5, N6, N7, N8)                                                              \
   DEFINE_CONT_DEFS3(N9, N10, N11) std::unique_ptr<ContinuationBase> CreateContinuationWord(int lbl) override     \
   {                                                                                                              \
      switch (lbl)                                                                                                \
      {                                                                                                           \
         DEFINE_CONT_CASE8__(N1, N2, N3, N4, N5, N6, N7, N8) DEFINE_CONT_CASE2__(N9, N10) DEFINE_CONT_CASE__(N11) \
      }                                                                                                           \
      return nullptr;                                                                                             \
   }
#define DEFINE_CONTINUATIONS10(N1, N2, N3, N4, N5, N6, N7, N8, N9, N10)                                  \
   DEFINE_CONT_DEFS8(N1, N2, N3, N4, N5, N6, N7, N8)                                                     \
   DEFINE_CONT_DEFS2(N9, N10) std::unique_ptr<ContinuationBase> CreateContinuationWord(int lbl) override \
   {                                                                                                     \
      switch (lbl)                                                                                       \
      {                                                                                                  \
         DEFINE_CONT_CASE8__(N1, N2, N3, N4, N5, N6, N7, N8) DEFINE_CONT_CASE2__(N9, N10)                \
      }                                                                                                  \
      return nullptr;                                                                                    \
   }
#define DEFINE_CONTINUATIONS9(N1, N2, N3, N4, N5, N6, N7, N8, N9)                                  \
   DEFINE_CONT_DEFS8(N1, N2, N3, N4, N5, N6, N7, N8)                                               \
   DEFINE_CONT_DEFS(N9) std::unique_ptr<ContinuationBase> CreateContinuationWord(int lbl) override \
   {                                                                                               \
      switch (lbl)                                                                                 \
      {                                                                                            \
         DEFINE_CONT_CASE8__(N1, N2, N3, N4, N5, N6, N7, N8) DEFINE_CONT_CASE__(N9)                \
      }                                                                                            \
      return nullptr;                                                                              \
   }
#define DEFINE_CONTINUATIONS8(N1, N2, N3, N4, N5, N6, N7, N8)                 \
   DEFINE_CONT_DEFS8(N1, N2, N3, N4, N5, N6, N7, N8)                          \
   std::unique_ptr<ContinuationBase> CreateContinuationWord(int lbl) override \
   {                                                                          \
      switch (lbl)                                                            \
      {                                                                       \
         DEFINE_CONT_CASE8__(N1, N2, N3, N4, N5, N6, N7, N8)                  \
      }                                                                       \
      return nullptr;                                                         \
   }
#define DEFINE_CONTINUATIONS7(N1, N2, N3, N4, N5, N6, N7)                                                   \
   DEFINE_CONT_DEFS4(N1, N2, N3, N4)                                                                        \
   DEFINE_CONT_DEFS3(N5, N6, N7) std::unique_ptr<ContinuationBase> CreateContinuationWord(int lbl) override \
   {                                                                                                        \
      switch (lbl)                                                                                          \
      {                                                                                                     \
         DEFINE_CONT_CASE4__(N1, N2, N3, N4) DEFINE_CONT_CASE2__(N5, N6) DEFINE_CONT_CASE__(N7)             \
      }                                                                                                     \
      return nullptr;                                                                                       \
   }
#define DEFINE_CONTINUATIONS6(N1, N2, N3, N4, N5, N6)                                                   \
   DEFINE_CONT_DEFS4(N1, N2, N3, N4)                                                                    \
   DEFINE_CONT_DEFS2(N5, N6) std::unique_ptr<ContinuationBase> CreateContinuationWord(int lbl) override \
   {                                                                                                    \
      switch (lbl)                                                                                      \
      {                                                                                                 \
         DEFINE_CONT_CASE4__(N1, N2, N3, N4) DEFINE_CONT_CASE2__(N5, N6)                                \
      }                                                                                                 \
      return nullptr;                                                                                   \
   }
#define DEFINE_CONTINUATIONS5(N1, N2, N3, N4, N5)                                                  \
   DEFINE_CONT_DEFS4(N1, N2, N3, N4)                                                               \
   DEFINE_CONT_DEFS(N5) std::unique_ptr<ContinuationBase> CreateContinuationWord(int lbl) override \
   {                                                                                               \
      switch (lbl)                                                                                 \
      {                                                                                            \
         DEFINE_CONT_CASE4__(N1, N2, N3, N4) DEFINE_CONT_CASE__(N5)                                \
      }                                                                                            \
      return nullptr;                                                                              \
   }
#define DEFINE_CONTINUATIONS4(N1, N2, N3, N4)                                                                   \
   DEFINE_CONT_DEFS4(N1, N2, N3, N4) std::unique_ptr<ContinuationBase> CreateContinuationWord(int lbl) override \
   {                                                                                                            \
      switch (lbl)                                                                                              \
      {                                                                                                         \
         DEFINE_CONT_CASE4__(N1, N2, N3, N4)                                                                    \
      }                                                                                                         \
      return nullptr;                                                                                           \
   }
#define DEFINE_CONTINUATIONS3(N1, N2, N3)                                                          \
   DEFINE_CONT_DEFS2(N1, N2)                                                                       \
   DEFINE_CONT_DEFS(N3) std::unique_ptr<ContinuationBase> CreateContinuationWord(int lbl) override \
   {                                                                                               \
      switch (lbl)                                                                                 \
      {                                                                                            \
         DEFINE_CONT_CASE2__(N1, N2) DEFINE_CONT_CASE__(N3)                                        \
      }                                                                                            \
      return nullptr;                                                                              \
   }
#define DEFINE_CONTINUATIONS2(N1, N2)                                                                   \
   DEFINE_CONT_DEFS2(N1, N2) std::unique_ptr<ContinuationBase> CreateContinuationWord(int lbl) override \
   {                                                                                                    \
      switch (lbl)                                                                                      \
      {                                                                                                 \
         DEFINE_CONT_CASE2__(N1, N2)                                                                    \
      }                                                                                                 \
      return nullptr;                                                                                   \
   }
#define DEFINE_CONTINUATIONS1(N1)                                                                  \
   DEFINE_CONT_DEFS(N1) std::unique_ptr<ContinuationBase> CreateContinuationWord(int lbl) override \
   {                                                                                               \
      switch (lbl)                                                                                 \
      {                                                                                            \
         DEFINE_CONT_CASE__(N1)                                                                    \
      }                                                                                            \
      return nullptr;                                                                              \
   }
#define DEFINE_CONTINUATIONS0() \
   std::unique_ptr<ContinuationBase> CreateContinuationWord(int lbl) override { return nullptr; }

// One of these should be inside each Initial word definition
#define DEFINE_EXTENSIONS0() \
   std::unique_ptr<ExtensionBase> CreateExtensionWord(int /*aEn*/) override { return nullptr; }
#define DEFINE_EXTENSIONS1()                                            \
   typedef Extension0             Extension0Type;                       \
   std::unique_ptr<ExtensionBase> CreateExtensionWord(int aEn) override \
   {                                                                    \
      switch (aEn)                                                      \
      {                                                                 \
      case 0:                                                           \
         return ut::make_unique<Extension0>();                          \
      default:                                                          \
         return nullptr;                                                \
      }                                                                 \
   }
#define DEFINE_EXTENSIONS2()                                            \
   typedef Extension0             Extension0Type;                       \
   typedef Extension1             Extension1Type;                       \
   std::unique_ptr<ExtensionBase> CreateExtensionWord(int aEn) override \
   {                                                                    \
      switch (aEn)                                                      \
      {                                                                 \
      case 0:                                                           \
         return ut::make_unique<Extension0>();                          \
      case 1:                                                           \
         return ut::make_unique<Extension1>();                          \
      default:                                                          \
         return nullptr;                                                \
      }                                                                 \
   }
#define DEFINE_EXTENSIONS3()                                            \
   typedef Extension0             Extension0Type;                       \
   typedef Extension1             Extension1Type;                       \
   typedef Extension2             Extension2Type;                       \
   std::unique_ptr<ExtensionBase> CreateExtensionWord(int aEn) override \
   {                                                                    \
      switch (aEn)                                                      \
      {                                                                 \
      case 0:                                                           \
         return ut::make_unique<Extension0>();                          \
      case 1:                                                           \
         return ut::make_unique<Extension1>();                          \
      case 2:                                                           \
         return ut::make_unique<Extension2>();                          \
      default:                                                          \
         return nullptr;                                                \
      }                                                                 \
   }

namespace WsfL16
{
namespace Messages
{

template<typename INIT_TYPE, typename SUB_WORD, typename WORD_TYPE_TAG, unsigned CORRECT_SUB_TYPE>
struct WordFinder
{
};

template<typename INIT_TYPE, typename SUB_WORD>
struct WordFinder<INIT_TYPE, SUB_WORD, ContinuationBase, 1>
{
   static bool FindWord(INIT_TYPE& aInitial, SUB_WORD*& aContinuation)
   {
      aContinuation = static_cast<SUB_WORD*>(aInitial.FindContinuation(SUB_WORD::cCONTINUATION_NUMBER));
      return aContinuation != nullptr;
   }
};

template<typename INIT_TYPE, typename SUB_WORD>
struct WordFinder<INIT_TYPE, SUB_WORD, ExtensionBase, 1>
{
   static bool FindWord(INIT_TYPE& aInitial, SUB_WORD*& aExtension)
   {
      if (aInitial.GetExtensionCount() > SUB_WORD::cEXTENSION_NUMBER)
      {
         aExtension = static_cast<SUB_WORD*>(aInitial.GetExtension(SUB_WORD::cEXTENSION_NUMBER));
      }
      else
      {
         aExtension = nullptr;
      }
      return aExtension != nullptr;
   }
};

template<typename INIT_TYPE, typename SUB_WORD>
inline bool FindWordT(INIT_TYPE& aInitialWord, SUB_WORD*& aSubWord)
{
   const bool correctSubType =
      (INIT_TYPE::cWORD_LABEL == SUB_WORD::cWORD_LABEL) && (INIT_TYPE::cWORD_SUB_LABEL == SUB_WORD::cWORD_SUB_LABEL);
   typedef WordFinder<INIT_TYPE, SUB_WORD, typename SUB_WORD::subword_type, correctSubType> Finder;
   return Finder::FindWord(aInitialWord, aSubWord);
}

} // namespace Messages
} // namespace WsfL16

#endif
