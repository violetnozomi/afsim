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

#ifndef WSFDISACTIONRESPONSER_HPP
#define WSFDISACTIONRESPONSER_HPP

#include "wsf_export.h"

#include "DisActionResponseR.hpp"

class DisActionRequestR;
class WsfDisInterface;

class WSF_EXPORT WsfDisActionResponseR : public DisActionResponseR
{
public:
   WsfDisActionResponseR(WsfDisInterface* aInterfacePtr);
   WsfDisActionResponseR(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr);
   ~WsfDisActionResponseR() override;

   //!  There are several possible uses for a WsfDisActionResponseR PDU.
   //!  The following methods are provided as a convenience, and specifically tailored to
   //!  individual uses of the PDU type.

   void MakeJoinExerciseResponse(double       aSimTime,
                                 double       aSimRate,
                                 double       aSimTimeStep,
                                 unsigned int aRequestStatus,
                                 unsigned int aTimeScheme,
                                 unsigned int aFeedbackTime,
                                 unsigned int aTimeLatency);

   /*      void MakeTimeAdvanceRequestResponse(double       aSimTime,
                                             double       aSimRate,
                                             int          aRequestStatus,
                                             int          aTimeScheme,
                                             DisUint32    aFeedbackTime,
                                             DisUint32    aTimeLatency,
                                             DisClockTime aSimTimeStep);

         void MakeResignExerciseResponse(double       aSimTime,
                                         double       aSimRate,
                                         int          aRequestStatus,
                                         int          aTimeScheme,
                                         DisUint32    aFeedbackTime,
                                         DisUint32    aTimeLatency,
                                         DisClockTime aSimTimeStep); */

protected:
   // Disallow assignment
   WsfDisActionResponseR& operator=(const WsfDisActionResponseR& aRhs) = delete;
   bool                   ProcessResponseTo(DisPdu* aInitiatorPtr);
   bool                   ProcessJoinExerciseResponseTo(const DisActionRequestR* aRequestPtr);

private:
   WsfDisInterface* mInterfacePtr;
};

#endif
