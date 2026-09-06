// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef RVP6DOFDATAEXTENSION_HPP
#define RVP6DOFDATAEXTENSION_HPP

#include "RvDataExtension.hpp"
#include "RvResultDb.hpp"
#include "RvResultPlatform.hpp"

namespace rv
{
class MsgP6dofCoreData;
class MsgP6dofKinematic;
class MsgP6dofEngineFuel;
class MsgP6dofAutopilot;
class MsgP6dofAutopilotLimits;
class MsgP6dofControlInputs;
class MsgP6dofForceMoment;

class P6DofDataExtension : public DataExtension
{
public:
   bool        HandlesMessage(int aMessageId) const override;
   QVariant    HandleMessage(const MsgBase* aMsg, int aColumn, int aRole) const override;
   void        RegisterSerializer(UtPackSerializer& aSerializer) const override;
   const char* GetSchema() const override;
   bool        IsOneTimeMessage(int aMessageId) const override;
   bool        IsEventMessage(int aMessageId) const override { return false; }
   void        AddMessage(ResultDb& aResultDb, MsgBase* aMsg, bool aAtBegin) const override;
   void        RemoveMessage(ResultDb& aResultDb, MsgBase* aMsg, bool aAtBegin) const override;

private:
   template<class T>
   void AddTypedMessage(MsgBase* aMsg, ResultDb& aResultDb, bool aAtBegin) const;
   template<class T>
   void     RemoveTypedMessage(MsgBase* aMsg, ResultDb& aResultDb, bool aAtBegin) const;
   QVariant HandleP6DofCoreData(const MsgP6dofCoreData* aMsg, int aColumn, int aRole) const;
   QVariant HandleP6DofKinematic(const MsgP6dofKinematic* aMsg, int aColumn, int aRole) const;
   QVariant HandleP6DofEngineFuel(const MsgP6dofEngineFuel* aMsg, int aColumn, int aRole) const;
   QVariant HandleP6DofAutopilot(const MsgP6dofAutopilot* aMsg, int aColumn, int aRole) const;
   QVariant HandleP6DofAutopilotLimits(const MsgP6dofAutopilotLimits* aMsg, int aColumn, int aRole) const;
   QVariant HandleP6DofControlInputs(const MsgP6dofControlInputs* aMsg, int aColumn, int aRole) const;
   QVariant HandleP6DofForceMoment(const MsgP6dofForceMoment* aMsg, int aColumn, int aRole) const;
};

template<class T>
void P6DofDataExtension::AddTypedMessage(MsgBase* aMsg, ResultDb& aResultDb, bool aAtBegin) const
{
   auto            msgPtr  = static_cast<T*>(aMsg);
   ResultPlatform* platPtr = aResultDb.FindPlatform(msgPtr->platformIndex());
   if (platPtr)
   {
      platPtr->BeginWrite();
      platPtr->AddBaseMessage(msgPtr, aAtBegin);
      platPtr->EndWrite();
   }
   else
   {
      msgPtr->platformIndex(0);
   }
}

template<class T>
void P6DofDataExtension::RemoveTypedMessage(MsgBase* aMsg, ResultDb& aResultDb, bool aAtBegin) const
{
   auto            msgPtr  = static_cast<T*>(aMsg);
   ResultPlatform* platPtr = aResultDb.FindPlatform(msgPtr->platformIndex());
   if (platPtr)
   {
      platPtr->BeginWrite();
      platPtr->RemoveBaseMessage(msgPtr, aAtBegin);
      platPtr->EndWrite();
   }
}

} // namespace rv

#endif
