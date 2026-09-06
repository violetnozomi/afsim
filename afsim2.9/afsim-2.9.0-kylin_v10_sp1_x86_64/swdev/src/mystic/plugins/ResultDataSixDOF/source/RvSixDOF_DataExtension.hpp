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

#ifndef RVSIXDOFDATAEXTENSION_HPP
#define RVSIXDOFDATAEXTENSION_HPP

#include "RvDataExtension.hpp"
#include "RvResultDb.hpp"
#include "RvResultPlatform.hpp"

namespace rv
{
class MsgSixDOF_CoreData;
class MsgSixDOF_Kinematic;
class MsgSixDOF_EngineFuel;
class MsgSixDOF_Autopilot;
class MsgSixDOF_AutopilotLimits;
class MsgSixDOF_ControlInputs;
class MsgSixDOF_ForceMoment;

class SixDOF_DataExtension : public DataExtension
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
   QVariant HandleSixDOF_CoreData(const MsgSixDOF_CoreData* aMsg, int aColumn, int aRole) const;
   QVariant HandleSixDOF_Kinematic(const MsgSixDOF_Kinematic* aMsg, int aColumn, int aRole) const;
   QVariant HandleSixDOF_EngineFuel(const MsgSixDOF_EngineFuel* aMsg, int aColumn, int aRole) const;
   QVariant HandleSixDOF_Autopilot(const MsgSixDOF_Autopilot* aMsg, int aColumn, int aRole) const;
   QVariant HandleSixDOF_AutopilotLimits(const MsgSixDOF_AutopilotLimits* aMsg, int aColumn, int aRole) const;
   QVariant HandleSixDOF_ControlInputs(const MsgSixDOF_ControlInputs* aMsg, int aColumn, int aRole) const;
   QVariant HandleSixDOF_ForceMoment(const MsgSixDOF_ForceMoment* aMsg, int aColumn, int aRole) const;
};

template<class T>
void SixDOF_DataExtension::AddTypedMessage(MsgBase* aMsg, ResultDb& aResultDb, bool aAtBegin) const
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
void SixDOF_DataExtension::RemoveTypedMessage(MsgBase* aMsg, ResultDb& aResultDb, bool aAtBegin) const
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
