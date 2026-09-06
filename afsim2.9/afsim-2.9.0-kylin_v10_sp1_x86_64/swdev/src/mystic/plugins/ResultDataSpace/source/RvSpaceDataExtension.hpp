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
#ifndef RVSPACEDATAEXTENSION_HPP
#define RVSPACEDATAEXTENSION_HPP

#include <QVariant>

#include "RvDataExtension.hpp"

namespace rv
{
class SpaceDataExtension : public DataExtension
{
public:
   bool        HandlesMessage(int aMessageId) const override;
   QVariant    HandleMessage(const MsgBase* aMsg, int aColumn, int aRole) const override;
   void        RegisterSerializer(UtPackSerializer& aSerializer) const override;
   const char* GetSchema() const override;
   bool        IsOneTimeMessage(int aMessageId) const override;
   bool        IsEventMessage(int aMessageId) const override;
   void        AddMessage(ResultDb& aResultDb, MsgBase* aMsg) const override;

   class OrbitEventHandler : public DataExtension::EventHandler
   {
      std::string       GetName() const override { return "Orbital Elements Changed"; }
      QString           GetInstanceLabel(rv::MsgBase* aMsg) const override;
      bool              EventFilter(rv::MsgBase* aMsg) const override { return true; }
      bool              ColorIsConfigurable() const override { return true; }
      QColor            GetColor(rv::MsgBase* aMsg) const override { return Qt::red; }
      vespa::VaPosition GetPosition(rv::MsgBase* aMsg) const override;
      int               GetEventId() const override;
   };
   std::vector<std::unique_ptr<EventHandler>> GetEventList() const override;
};

} // namespace rv

#endif
