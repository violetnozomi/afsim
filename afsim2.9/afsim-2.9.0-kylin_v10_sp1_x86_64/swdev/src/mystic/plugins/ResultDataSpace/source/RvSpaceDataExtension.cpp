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
#include "RvSpaceDataExtension.hpp"

#include "RvEnvironment.hpp"
#include "RvResultData.hpp"
#include "RvResultDb.hpp"
#include "RvResultPlatform.hpp"
#include "RvSpaceEventPipeClasses.hpp"
#include "RvSpaceEventPipeClassesRegister.hpp"
#include "RvSpaceEventPipeSchema.hpp"
#include "UtEllipsoidalEarth.hpp"

bool rv::SpaceDataExtension::HandlesMessage(int aMessageId) const
{
   switch (aMessageId)
   {
   case (MsgOrbitalElements::cMESSAGE_ID):
      return true;
   default:
      return false;
   }
}

QVariant rv::SpaceDataExtension::HandleMessage(const MsgBase* aMsg, int aColumn, int aRole) const
{
   if (aMsg->GetMessageId() == MsgOrbitalElements::cMESSAGE_ID)
   {
      auto oeMsg = dynamic_cast<const MsgOrbitalElements*>(aMsg);
      if (oeMsg)
      {
         switch (aColumn)
         {
         case (rv::MessageBase::cFIELD_OWNER):
         {
            if (aRole == Qt::ToolTipRole)
            {
               return "owner";
            }
            return rvEnv.LookupPlatformVariant(oeMsg->platformIndex(), aRole);
         }
         case (rv::MessageBase::cFIELD_DATA):
         {
            if (aRole == Qt::ToolTipRole)
            {
               return "semi-major axis";
            }
            return oeMsg->semiMajorAxis();
         }
         case (rv::MessageBase::cFIELD_DATA + 1):
         {
            if (aRole == Qt::ToolTipRole)
            {
               return "eccentricity";
            }
            return oeMsg->eccentricity();
         }
         case (rv::MessageBase::cFIELD_DATA + 2):
         {
            if (aRole == Qt::ToolTipRole)
            {
               return "true anomaly";
            }
            return oeMsg->trueAnomaly();
         }
         case (rv::MessageBase::cFIELD_DATA + 3):
         {
            if (aRole == Qt::ToolTipRole)
            {
               return "right ascension of ascending node";
            }
            return oeMsg->raan();
         }
         case (rv::MessageBase::cFIELD_DATA + 4):
         {
            if (aRole == Qt::ToolTipRole)
            {
               return "inclination";
            }
            return oeMsg->inclination();
         }
         case (rv::MessageBase::cFIELD_DATA + 5):
         {
            if (aRole == Qt::ToolTipRole)
            {
               return "argument of periapsis";
            }
            return oeMsg->argumentOfPeriapsis();
         }
         case (rv::MessageBase::cFIELD_DATA + 6):
         {
            if (aRole == Qt::ToolTipRole)
            {
               return "orbit color";
            }
            QString orbitColorStr{"INVALID"};
            if (oeMsg->orbitColorValid())
            {
               QString commaStr{", "};
               orbitColorStr = QString::number(oeMsg->orbitColor().x()) + commaStr +
                               QString::number(oeMsg->orbitColor().y()) + commaStr +
                               QString::number(oeMsg->orbitColor().z());
            }
            return orbitColorStr;
         }
         }
      }
   }
   return QVariant();
}

void rv::SpaceDataExtension::RegisterSerializer(UtPackSerializer& aSerializer) const
{
   UtPack_register_all_wsf_space_events_types(aSerializer);
}

const char* rv::SpaceDataExtension::GetSchema() const
{
   return cRvSpaceEventPipe_SCHEMA;
}

bool rv::SpaceDataExtension::IsOneTimeMessage(int aMessageId) const
{
   if (aMessageId == MsgOrbitalElements::cMESSAGE_ID)
   {
      return true;
   }
   return false;
}

bool rv::SpaceDataExtension::IsEventMessage(int aMessageId) const
{
   if (aMessageId == MsgOrbitalElements::cMESSAGE_ID)
   {
      return true;
   }
   return false;
}

void rv::SpaceDataExtension::AddMessage(ResultDb& aResultDb, MsgBase* aMsg) const
{
   if (aMsg->GetMessageId() == MsgOrbitalElements::cMESSAGE_ID)
   {
      auto            msgPtr  = dynamic_cast<const MsgOrbitalElements*>(aMsg);
      ResultPlatform* platPtr = aResultDb.FindPlatform(msgPtr->platformIndex());
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->AddBaseMessage(aMsg, false);
         platPtr->EndWrite();
      }
   }
}

QString rv::SpaceDataExtension::OrbitEventHandler::GetInstanceLabel(rv::MsgBase* aMsg) const
{
   auto msgPtr = dynamic_cast<const MsgOrbitalElements*>(aMsg);
   if (msgPtr != nullptr)
   {
      rv::ResultPlatform* plat = rvEnv.GetData()->FindPlatform(msgPtr->platformIndex());
      if (plat)
      {
         return QString::fromStdString(plat->GetName());
      }
   }
   return "unknown";
}

vespa::VaPosition rv::SpaceDataExtension::OrbitEventHandler::GetPosition(rv::MsgBase* aMsg) const
{
   auto msgPtr = dynamic_cast<const MsgOrbitalElements*>(aMsg);
   if (msgPtr != nullptr)
   {
      rv::ResultPlatform* plat = rvEnv.GetData()->FindPlatform(msgPtr->platformIndex());
      if (plat)
      {
         auto statemsg = plat->FindFirstBefore<rv::MsgEntityState>(aMsg->simTime());

         if (statemsg != nullptr)
         {
            double xyz[3];
            double dtime = aMsg->simTime() - statemsg->simTime();
            xyz[0]       = statemsg->state().locationWCS().x() + dtime * statemsg->state().velocityWCS().x() +
                     0.5 * dtime * dtime * statemsg->state().accelerationWCS().x();
            xyz[1] = statemsg->state().locationWCS().y() + dtime * statemsg->state().velocityWCS().y() +
                     0.5 * dtime * dtime * statemsg->state().accelerationWCS().y();
            xyz[2] = statemsg->state().locationWCS().z() + dtime * statemsg->state().velocityWCS().z() +
                     0.5 * dtime * dtime * statemsg->state().accelerationWCS().z();

            double lla[3];
            UtEllipsoidalEarth::ConvertECEFToLLA(xyz, lla[0], lla[1], lla[2]);
            return vespa::VaPosition(lla[0], lla[1], lla[2], xyz[0], xyz[1], xyz[2]);
         }
      }
   }
   return vespa::VaPosition(0, 0, 0);
}

int rv::SpaceDataExtension::OrbitEventHandler::GetEventId() const
{
   return MsgOrbitalElements::cMESSAGE_ID;
}

std::vector<std::unique_ptr<rv::DataExtension::EventHandler>> rv::SpaceDataExtension::GetEventList() const
{
   std::vector<std::unique_ptr<rv::DataExtension::EventHandler>> list;
   list.emplace_back(new OrbitEventHandler());
   return list;
}
