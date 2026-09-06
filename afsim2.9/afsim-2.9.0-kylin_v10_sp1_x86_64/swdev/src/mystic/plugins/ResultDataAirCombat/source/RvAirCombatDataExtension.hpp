// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef RVAIRCOMBATDATAEXTENSION_HPP
#define RVAIRCOMBATDATAEXTENSION_HPP

#include "RvDataExtension.hpp"
#include "RvResultPlatform.hpp"

namespace rv
{
class MsgSA_EngagementSummaryData;
class MsgSA_FlightKinematicsData;
class MsgSA_FuelData;
class MsgSA_NavData;
class MsgSA_FlightControlsData;
class MsgSA_WeaponsData;
class MsgSA_TrackData;
class MsgSA_PerceivedAssetsData;
class MsgSA_PerceivedBogiesAndBanditsData;
class MsgSA_GroupsData;
class MsgSA_PrioritizedThreatsAndTargetsData;

class AirCombatDataExtension : public DataExtension
{
public:
   bool     HandlesMessage(int aMessageId) const override;
   QVariant HandleMessage(const MsgBase* aMsg, int aColumn, int aRole) const override;

   void        RegisterSerializer(UtPackSerializer& aSerializer) const override;
   const char* GetSchema() const override;

   bool IsOneTimeMessage(int aMessageId) const override;
   bool IsEventMessage(int aMessageId) const { return false; }

   void AddMessage(ResultDb& aResultDb, MsgBase* aMsg, bool aAtBegin) const override;

   void RemoveMessage(ResultDb& aResultDb, MsgBase* aMsg, bool aAtBegin) const override;

private:
   QVariant HandleSA_EngagementSummaryData(const MsgSA_EngagementSummaryData* aMsg, int aColumn, int aRole) const;

   QVariant HandleSA_FlightKinematicsData(const MsgSA_FlightKinematicsData* aMsg, int aColumn, int aRole) const;

   QVariant HandleSA_FuelData(const MsgSA_FuelData* aMsg, int aColumn, int aRole) const;

   QVariant HandleSA_NavData(const MsgSA_NavData* aMsg, int aColumn, int aRole) const;

   QVariant HandleSA_FlightControlsData(const MsgSA_FlightControlsData* aMsg, int aColumn, int aRole) const;

   QVariant HandleSA_WeaponsData(const MsgSA_WeaponsData* aMsg, int aColumn, int aRole) const;

   QVariant HandleSA_TrackData(const MsgSA_TrackData* aMsg, int aColumn, int aRole) const;

   QVariant HandleSA_PerceivedAssetsData(const MsgSA_PerceivedAssetsData* aMsg, int aColumn, int aRole) const;

   QVariant HandleSA_PerceivedBogiesAndBanditsData(const MsgSA_PerceivedBogiesAndBanditsData* aMsg,
                                                   int                                        aColumn,
                                                   int                                        aRole) const;

   QVariant HandleSA_GroupsData(const MsgSA_GroupsData* aMsg, int aColumn, int aRole) const;

   QVariant HandleSA_PrioritizedThreatsAndTargetsData(const MsgSA_PrioritizedThreatsAndTargetsData* aMsg,
                                                      int                                           aColumn,
                                                      int                                           aRole) const;
};
} // namespace rv

#endif // !RVAIRCOMBATDATAEXTENSION_HPP
