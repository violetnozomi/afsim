// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef WSFDISSETDATAR_HPP
#define WSFDISSETDATAR_HPP

#include "wsf_export.h"

#include "DisEntityId.hpp"
#include "DisSetDataR.hpp"
class WsfDisInterface;
class WsfDisDataR;

class WSF_EXPORT WsfDisSetDataR : public DisSetDataR
{
public:
   WsfDisSetDataR(WsfDisInterface* aInterfacePtr);
   WsfDisSetDataR(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr);
   ~WsfDisSetDataR() override;

   void TimedOut() override;

   //!  There are several possible uses for a WsfDisSetDataR PDU.
   //!  The following methods are provided as a convenience, and
   //!  specifically tailored to individual uses of the PDU type.

   // This method prepares the PDU to be used as either/or a
   // 'Set Time Parameters' or a 'Time Advance Grant" service request:
   void Make_STP_or_TAG(double aSimTime, double aSimTimeStep, double aSimRate, int aTimeScheme);

protected:
   // Disallow assignment <--authors comment. code must predate c++ 11...
   WsfDisSetDataR& operator=(const WsfDisSetDataR& aRhs) = delete;
   WsfDisDataR*    ProduceResponse(bool& aIsDone);
   bool            ProcessSetDataRequest();
   bool            PopulateResponse(WsfDisDataR* aResponsePduPtr) const;
   bool ParseTimeParameters(double& aSimTime, double& aSimTimeStep, double& aSimRate, int& aTimeScheme) const;

private:
   WsfDisInterface* mInterfacePtr;
};

#endif
