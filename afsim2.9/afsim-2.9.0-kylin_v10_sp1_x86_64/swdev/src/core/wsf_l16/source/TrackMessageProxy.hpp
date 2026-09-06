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
#ifndef L16_TRACKMESSAGEPROXY_HPP
#define L16_TRACKMESSAGEPROXY_HPP

#include <string>

class DisEntityId;
#include "MessagesPreDef.hpp"

namespace WsfL16
{
class Interface;

//! The track message proxy is a "stand-in" for the various L16 track messages,
//! which can have varying and confusing formats.  Using the proxy allows access to the
//! J3 message in a generic, understandable format.
class TrackMessageProxy
{
public:
   enum Environment
   {
      cAIR,
      cSURFACE,
      cSUBSURFACE,
      cLAND,
      cSPACE
   };

   virtual ~TrackMessageProxy() {}

   virtual bool GetLocationLLA(double& aLat, double& aLon, double& aAlt) const = 0;

   virtual double GetSpeed() const = 0;

   virtual double GetCourse() const = 0;

   virtual bool GetVelocityWCS(double aWCS[3]) const { return false; }

   virtual int GetTrackNumber() const = 0;

   virtual unsigned GetSourceTrackNumber() const = 0;

   virtual std::string GetStringTrackNumber() const = 0;

   virtual void GetTrackDisEntityId(WsfL16::Interface* aL16_InterfacePtr, DisEntityId& aEntityId) const = 0;

   virtual double GetTrackQuality() const = 0;

   virtual double GetTrackQualityErrorDistance95CEP() const { return 0.0; }

   virtual int GetIdentity() const = 0;

   virtual int GetSpecificType() const = 0;

   virtual void SetLocationLLA(double aLat, double aLon, double aAlt) = 0;

   //! Returns the environment -- determined by the message type
   virtual Environment GetEnvironment() const = 0;

   static TrackMessageProxy*       Create(Messages::InitialBase* aMsg);
   static const TrackMessageProxy* Create(const Messages::InitialBase* aMsg);

private:
};
} // namespace WsfL16

#endif
