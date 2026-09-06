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
#ifndef RVDATAEXTENSION_HPP
#define RVDATAEXTENSION_HPP

#include <QColor>
#include <QVariant>

#include "RvExport.hpp"
#include "UtPack.hpp"
#include "VaPosition.hpp"

namespace rv
{
class MsgBase;
class ResultDb;
class ResultPlatform;

class RV_EXPORT DataExtension
{
public:
   //! this will inform that this extension handles messages with this ID
   virtual bool HandlesMessage(int aMessageId) const = 0;

   //! this converts an index (aColumn) and a message (aMsg) into a qvariant for display
   //! this is used by the event list display
   virtual QVariant HandleMessage(const MsgBase* aMsg, int aColumn, int aRole) const = 0;

   //! this will register the low level message reading capability with the application
   virtual void RegisterSerializer(UtPackSerializer& aSerializer) const = 0;

   //! this will return the utpack schema definition for this extension's messages
   virtual const char* GetSchema() const = 0;

   //! identifies a message type as "one-time" to the applciation
   //! one time messages are expected to be held in memory regardless of what
   //! time-window is paged in, so that they are always accessible
   virtual bool IsOneTimeMessage(int aMessageId) const = 0;

   //! identifies a message type as an event message
   //! event messages are accessible from the RvResultDb by type
   //! the intention here is that these are messages that are of interest at a
   //! general capacity (as opposed to something that might better be scoped (only) to a platform or interaction)
   virtual bool IsEventMessage(int aMessageId) const = 0;

   //! processes a one-time message
   virtual void AddMessage(ResultDb& aResultDb, MsgBase* aMsg) const {}

   //! processes a message (non-one-time)
   //! aAtbegin identifies if this message is being
   //! added to the start of the paged window, or end
   virtual void AddMessage(ResultDb& aResultDb, MsgBase* aMsg, bool aAtBegin) const {}
   //! removes a message (non-one-time)
   //! aAtbegin identifies if this message is being
   //! removed from the start of the paged window, or end
   virtual void RemoveMessage(ResultDb& aResultDb, MsgBase* aMsg, bool aAtBegin) const {}

   //! @note this will not work for non-event messages
   class EventHandler
   {
   public:
      //! usually the event name
      virtual std::string GetName() const { return ""; }
      //! use this to filter multiple markers created by one event
      virtual bool EventFilter(rv::MsgBase* aMsg) const { return false; }
      virtual bool ColorIsConfigurable() const { return true; }
      //! this is the default color
      virtual QColor GetColor(rv::MsgBase* aMsg) const { return Qt::white; }
      //! the position
      //! @note maybe read right from the event or maybe derived from a platform
      virtual vespa::VaPosition GetPosition(rv::MsgBase* aMsg) const { return vespa::VaPosition(); }
      //! @note for example, a platform name
      virtual QString GetInstanceLabel(rv::MsgBase* aMsg) const { return "???"; }
      //! the event ID
      //! @note THIS IS IMPORTANT!
      virtual int GetEventId() const { return 0; }
   };

   struct StateCard
   {
      StateCard(const QString& aName, const QString& aIcon, const QString& aGroup)
         : mName(aName)
         , mGroup(aGroup)
         , mIcon(aIcon)
      {
      }
      QString mName;
      QString mGroup;
      QString mIcon;
   };

   struct Interaction
   {
      Interaction(const QString& aName, const QString& aGroup, const QString& aHoverText, const QColor& aColor)
         : mName(aName)
         , mGroup(aGroup)
         , mHoverText(aHoverText)
         , mColor(aColor)
      {
      }
      QString mName;
      QString mGroup;
      QString mHoverText;
      QColor  mColor;
   };

   //! this allows event processing plug-ins to handle events defined by this extension
   virtual std::vector<std::unique_ptr<EventHandler>> GetEventList() const
   {
      return std::vector<std::unique_ptr<EventHandler>>();
   }

   //! this allows state processing plug-ins to handle states defined by this extension
   virtual std::vector<std::unique_ptr<StateCard>> GetStateCardList() const
   {
      return std::vector<std::unique_ptr<StateCard>>();
   }

   //! this allows interaction processing plug-ins to handle interactions defined by this extension
   virtual std::vector<std::unique_ptr<Interaction>> GetInteractionList() const
   {
      return std::vector<std::unique_ptr<Interaction>>();
   }
};

} // namespace rv

#endif
