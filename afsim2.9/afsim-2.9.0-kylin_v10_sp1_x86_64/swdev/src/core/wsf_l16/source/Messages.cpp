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
#include "Messages.hpp"

#include <algorithm>
#include <cstdarg>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "Exception.hpp"
#include "FieldTypes.hpp"
#include "MessageAccessor.hpp"
#include "MessagePacking.hpp"
#include "MessagesFactory.hpp"

namespace WsfL16
{
namespace Messages
{

Header::Header()
   : mSourceTrackNumber(0)
   , mTimeSlotType(0)
   , mRelayTransmission(false)
   , mSecureData(0)
{
}

void Header::Read(MessageBitInput& aInput)
{
   unsigned relay;
   aInput.Read(mTimeSlotType, 3);
   aInput.Read(relay, 1);
   aInput.Read(mSourceTrackNumber, 15);
   aInput.Read(mSecureData, 16);
   aInput.Skip(13);
   mRelayTransmission = relay != 0;
}

void Header::Write(MessageBitOutput& aOutput) const
{
   unsigned relay = (mRelayTransmission ? 1U : 0U);
   aOutput.Write(mTimeSlotType, 3);
   aOutput.Write(relay, 1);
   aOutput.Write(mSourceTrackNumber, 15);
   aOutput.Write(mSecureData, 16);
   aOutput.Skip(13);
}


Base::Base() {}

// virtual
Base::~Base() {}

// Output the contents of the J-Message word to a UtMessageStream
void Base::PrintWord(ut::log::MessageStream& aStream) const
{
   auto            className = aStream.AddNote() << GetClassName() << ":";
   MessageAccessor msgAccess(this);
   for (int i = 0; i < msgAccess.FieldCount(); ++i)
   {
      FieldAccessor field     = msgAccess.GetField(i);
      auto          fieldNote = className.AddNote() << std::left << field.GetName();
      if (field.IsNoStatement())
      {
         fieldNote.AddNote() << "No Statement";
      }
      else if (field.IsSpare())
      {
         fieldNote.AddNote() << "Spare";
      }
      else if (field.IsDisused())
      {
         fieldNote.AddNote() << "Disused";
      }
      else
      {
         std::string unitsName;
         field.GetUnits(unitsName);

         switch (field.GetType())
         {
         case FieldAccessor::cBOOLEAN:
            fieldNote.AddNote() << std::boolalpha << field.GetBool() << ' ' << unitsName;
            break;
         case FieldAccessor::cDOUBLE:
            fieldNote.AddNote() << field.GetDouble() << ' ' << unitsName;
            break;
         case FieldAccessor::cENUMERATION:
         {
            std::string enumValue;
            if (field.GetEnumeration(enumValue))
            {
               fieldNote.AddNote() << enumValue << ' ' << unitsName;
            }
            else
            {
               fieldNote.AddNote() << "INVALID VALUE: " << field.GetRawData() << ' ' << unitsName;
            }
         }
         break;
         case FieldAccessor::cINTEGER:
            fieldNote.AddNote() << field.GetInt() << ' ' << unitsName;
            break;
         case FieldAccessor::cSTRING:
            fieldNote.AddNote() << field.GetString() << ' ' << unitsName;
            break;
         }
      }
   }
}

InitialBase::InitialBase(const InitialBase& aSrc)
   : mHeader(aSrc.mHeader)
{
   for (const auto& cw : aSrc.mContinuationWords)
   {
      mContinuationWords.emplace_back(cw->Clone());
   }
   for (const auto& ew : aSrc.mExtensionWords)
   {
      mExtensionWords.emplace_back(ew->Clone());
   }
}

ContinuationBase* InitialBase::AddContinuationWord(int label)
{
   ContinuationBase* contWord = FindContinuation(label);
   if (contWord == nullptr)
   {
      auto cw  = CreateContinuationWord(label);
      contWord = cw.get();
      if (cw)
      {
         mContinuationWords.push_back(std::move(cw));
      }
   }
   return contWord;
}

void InitialBase::InsertContinuationWord(std::unique_ptr<ContinuationBase> aContWord)
{
   bool foundWord = false;
   int  label     = aContWord->GetLabel();
   for (unsigned i = 0; i < mContinuationWords.size() && !foundWord; ++i)
   {
      if (mContinuationWords[i]->GetLabel() == label)
      {
         mContinuationWords[i] = std::move(aContWord);
         foundWord             = true;
      }
   }

   if (!foundWord)
   {
      mContinuationWords.push_back(std::move(aContWord));
   }
}

ExtensionBase* InitialBase::AddExtensionWord(int aExtensionNumber)
{
   auto           newWord    = CreateExtensionWord(aExtensionNumber);
   ExtensionBase* extWordPtr = newWord.get();

   if (newWord)
   {
      InsertExtensionWord(std::move(newWord));
   }
   return extWordPtr;
}

void InitialBase::InsertExtensionWord(std::unique_ptr<ExtensionBase> aExtensionWord)
{
   int ext = aExtensionWord->GetExtensionNumber();
   mExtensionWords.resize(std::max(ext + 1, (int)mExtensionWords.size()));
   for (int i = 0; i < ext; ++i)
   {
      if (mExtensionWords[i] == nullptr)
      {
         mExtensionWords[i] = CreateExtensionWord(i);
      }
   }
   mExtensionWords[ext] = std::move(aExtensionWord);
}

Base* InitialBase::GetSubWord(int index) const
{
   int nExt  = static_cast<int>(mExtensionWords.size());
   int nCont = static_cast<int>(mContinuationWords.size());
   if (index < nExt)
   {
      return mExtensionWords[index].get();
   }
   index -= nExt;
   if (nCont < index)
   {
      return mContinuationWords[index].get();
   }
   return nullptr;
}

void InitialBase::WriteMessage(MessageBitOutput& aMW, bool aWriteHeader) const
{
   if (aWriteHeader)
   {
      mHeader.Write(aMW);
   }

   int label, sublabel;
   GetLabel(label, sublabel);

   aMW.Write(cINITIAL, 2);
   aMW.Write(label, 5);
   aMW.Write(sublabel, 3);
   aMW.Write(static_cast<unsigned int>(GetMessageLength()), 3);

   // This is really a const method
   const_cast<InitialBase*>(this)->TransferO(aMW);

   aMW.Skip(10);
   for (const auto& ewPtr : mExtensionWords)
   {
      ewPtr->WriteMessage(aMW);
      aMW.Skip(10);
   }
   for (const auto& cwPtr : mContinuationWords)
   {
      cwPtr->WriteMessage(aMW);
      aMW.Skip(10);
   }
}

ContinuationBase* InitialBase::FindContinuation(int aContinuation) const
{
   for (const auto& cwPtr : mContinuationWords)
   {
      if (cwPtr->GetLabel() == aContinuation)
      {
         return cwPtr.get();
      }
   }
   return nullptr;
}

//! Returns the human-readable class name for the Initial word
std::string InitialBase::GetClassName() const
{
   int lbl, sublbl;
   GetLabel(lbl, sublbl);
   std::stringstream ss;
   ss << "WsfTadilJ" << lbl << "_" << sublbl << "I";
   return ss.str();
}

void InitialBase::PrintMessage(ut::log::MessageStream& aStream) const
{
   PrintWord(aStream);
   for (const auto& cwPtr : mContinuationWords)
   {
      if (cwPtr != nullptr)
      {
         cwPtr->PrintWord(aStream);
      }
   }
   for (const auto& ewPtr : mExtensionWords)
   {
      if (ewPtr != nullptr)
      {
         ewPtr->PrintWord(aStream);
      }
   }
}

void ContinuationBase::WriteMessage(MessageBitOutput& aMW) const
{
   int label = GetLabel();
   aMW.Write((int)cCONTINUATION, 2);
   aMW.Write(label, 5);
   const_cast<ContinuationBase*>(this)->TransferO(aMW);
}
//! Returns the human-readable class name for the Continuation word
std::string ContinuationBase::GetClassName() const
{
   int               uid     = GetUID();
   int               lbl     = uid / 100 / 1000;
   int               sublbl  = (uid / 1000) % 100;
   int               contNum = (uid / 10) % 100;
   std::stringstream ss;
   ss << "WsfTadilJ" << lbl << "_" << sublbl << "C" << contNum;
   return ss.str();
}

void ExtensionBase::WriteMessage(MessageBitOutput& aMW) const
{
   aMW.Write((int)cEXTENSION, 2);
   const_cast<ExtensionBase*>(this)->TransferO(aMW);
}

//! Returns the human-readable class name for the Extension word
std::string ExtensionBase::GetClassName() const
{
   int               uid    = GetUID();
   int               lbl    = uid / 100 / 1000;
   int               sublbl = (uid / 1000) % 100;
   int               extNum = (uid % 10) - 1;
   std::stringstream ss;
   ss << "WsfTadilJ" << lbl << "_" << sublbl << "E" << extNum;
   return ss.str();
}


} // namespace Messages
} // namespace WsfL16
