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
#ifndef L16_PPLI_MESSAGEPROXY_HPP
#define L16_PPLI_MESSAGEPROXY_HPP

#include <string>

#include "MessagesPreDef.hpp"

namespace WsfL16
{
//! Allows access to PPLI message in a generic format
class PPLI_MessageProxy
{
public:
   virtual ~PPLI_MessageProxy() {}

   enum Environment
   {
      cAIR,
      cSURFACE,
      cSUBSURFACE,
      cLAND
   };

   virtual bool GetLocationLLA(double& aLat, double& aLon, double& aAlt) const = 0;

   virtual double GetSpeed() const = 0;

   virtual double GetCourse() const = 0;

   virtual std::string GetCallsign() const = 0;

   virtual void SetLocationLLA(double aLat, double aLon, double aAlt) = 0;


   //! Returns the environment -- determined by the message type
   virtual Environment GetEnvironment() const = 0;

   static PPLI_MessageProxy*       Create(Messages::InitialBase* aMsg);
   static const PPLI_MessageProxy* Create(const Messages::InitialBase* aMsg);

private:
};
} // namespace WsfL16
#endif
