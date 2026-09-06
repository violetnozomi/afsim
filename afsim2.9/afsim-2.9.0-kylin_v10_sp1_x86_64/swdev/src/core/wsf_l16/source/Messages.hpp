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
#ifndef L16_LINK16MESSAGES_HPP
#define L16_LINK16MESSAGES_HPP


#include "wsf_l16_export.h"

#include <memory>
#include <string>
#include <vector>

#include "UtLog.hpp"

namespace WsfL16
{
namespace Messages
{
class Base;
class InitialBase;
class ExtensionBase;
class ContinuationBase;
class MessageBitInput;
class MessageBitOutput;
class MessageAccessorFiller;

class WSF_L16_EXPORT Header
{
public:
   Header();
   void                 Read(MessageBitInput& aInput);
   void                 Write(MessageBitOutput& aOutput) const;
   unsigned             mSourceTrackNumber;
   mutable unsigned int mTimeSlotType;
   bool                 mRelayTransmission;
   unsigned int         mSecureData;
};

class WSF_L16_EXPORT Base
{
public:
   Base(char* aData);
   Base();

   enum WordType
   {
      cINITIAL,
      cCONTINUATION,
      cEXTENSION,
      cUNDEFINED
   };

   //! Returns the base type of the word
   virtual WordType GetWordType() const = 0;

   virtual ~Base();

   //! Returns the unique identifier of a message word type.
   virtual int GetUID() const = 0;
   //! Reads the message from a MessageBitInput
   virtual void TransferI(MessageBitInput& aMR) = 0;
   //! Reads the message from a MessageBitOutput
   virtual void TransferO(MessageBitOutput& aMW) = 0;
   //! Used once to provide generic access to messages and fields.
   virtual void CreateAccessor(MessageAccessorFiller& aMP) = 0;
   //! Returns the name of the field with a given index
   virtual const char* GetFieldName(int i) = 0;
   //! Returns the class name of a field (used in scripting and debug output)
   virtual std::string GetClassName() const = 0;

   void PrintWord(ut::log::MessageStream& aStream) const;

protected:
};

class WSF_L16_EXPORT InitialBase : public Base
{
public:
   virtual InitialBase* Clone() const = 0;
   ~InitialBase() override            = default;
   InitialBase(const InitialBase& aSrc);
   const InitialBase& operator=(const InitialBase& aRhs) = delete;

   WordType GetWordType() const override { return cINITIAL; }

   //! Returns the number of extra words in the message
   size_t       GetMessageLength() const { return mContinuationWords.size() + mExtensionWords.size(); }
   virtual void GetLabel(int& aLabel, int& aSubLabel) const = 0;
   bool         IsLabelEqual(int aLabel, int aSublabel) const
   {
      int label, subLabel;
      GetLabel(label, subLabel);
      return aLabel == label && aSublabel == subLabel;
   }
   int GetLabel() const
   {
      int label, subLabel;
      GetLabel(label, subLabel);
      return label;
   }
   int GetSubLabel() const
   {
      int label, subLabel;
      GetLabel(label, subLabel);
      return subLabel;
   }
   //! Returns the sub-word with the given index number
   Base* GetSubWord(int index) const;

   ContinuationBase*       GetContinuation(int aIndex) { return mContinuationWords[aIndex].get(); }
   const ContinuationBase* GetContinuation(int aIndex) const { return mContinuationWords[aIndex].get(); }
   size_t                  GetContinuationCount() const { return mContinuationWords.size(); }
   ContinuationBase*       FindContinuation(int aContinuation) const;

   size_t         GetExtensionCount() const { return mExtensionWords.size(); }
   ExtensionBase* GetExtension(int aIndex) const { return mExtensionWords[aIndex].get(); }

   ContinuationBase* AddContinuationWord(int aWordNumber);

   ExtensionBase* AddExtensionWord(int aWordNumber);

   //! Inserts a new Continuation word into this word
   void InsertContinuationWord(std::unique_ptr<ContinuationBase>);

   //! Inserts a new Extension word into this word
   void InsertExtensionWord(std::unique_ptr<ExtensionBase>);

   const Header& GetHeader() const { return mHeader; }

   Header& GetHeader() { return mHeader; }

   void WriteMessage(MessageBitOutput& aMW, bool aWriteHeader = true) const;

   //! Adds a Continuation word with the given label to this message
   virtual std::unique_ptr<ContinuationBase> CreateContinuationWord(int label) = 0;

   //! Adds an Extension word with the given ExtensionBase number to this message
   virtual std::unique_ptr<ExtensionBase> CreateExtensionWord(int aExtensionBaseNumber) = 0;

   std::string GetClassName() const override;

   void PrintMessage(ut::log::MessageStream& aStream) const;

protected:
   // prevent construction without the factory
   friend class Factory;

   InitialBase() = default;

   //! Initialize the message by reading from the data buffer
   bool Initialize(const char* aData);

   std::vector<std::unique_ptr<ContinuationBase>> mContinuationWords;
   std::vector<std::unique_ptr<ExtensionBase>>    mExtensionWords;
   Header                                         mHeader;
};

//! The base class for J-Message ContinuationBase words
class WSF_L16_EXPORT ContinuationBase : public Base
{
public:
   typedef ContinuationBase subword_type;
   friend class InitialBase;
   friend class Factory;

   virtual ContinuationBase* Clone() const = 0;
   ~ContinuationBase() override {}
   virtual int GetLabel() const = 0;
   WordType    GetWordType() const override { return cCONTINUATION; }
   void        WriteMessage(MessageBitOutput& aMW) const;
   std::string GetClassName() const override;
};

class WSF_L16_EXPORT ExtensionBase : public Base
{
public:
   typedef ExtensionBase subword_type;
   friend class InitialBase;
   virtual ExtensionBase* Clone() const = 0;
   WordType               GetWordType() const override { return cEXTENSION; }
   void                   WriteMessage(MessageBitOutput& aMW) const;
   virtual int            GetExtensionNumber() const = 0;
   std::string            GetClassName() const override;
};
} // namespace Messages
} // namespace WsfL16

#endif
