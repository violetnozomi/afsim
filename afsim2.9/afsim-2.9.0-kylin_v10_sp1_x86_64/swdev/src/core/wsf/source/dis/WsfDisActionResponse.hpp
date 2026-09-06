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

#ifndef WSFDISACTIONRESPONSE_HPP
#define WSFDISACTIONRESPONSE_HPP

#include "wsf_export.h"

#include "DisActionResponse.hpp"
class DisActionRequest;
class WsfDisInterface;

class WSF_EXPORT WsfDisActionResponse : public DisActionResponse
{
public:
   WsfDisActionResponse(WsfDisInterface* aInterfacePtr);
   WsfDisActionResponse(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr);
   ~WsfDisActionResponse() override;

   //!  There are several possible uses for a WsfDisActionResponse PDU.
   //!  The following methods are provided as a convenience, and specifically
   //!  tailored to individual uses of the PDU type.
   void MakeJoinExerciseResponse(const DisEntityId& aRequestorDisId,
                                 int                aRequestorSerialId,
                                 int                aRequestStatus,
                                 int                aTimeScheme,
                                 DisUint32          aFeedbackTime,
                                 DisUint32          aTimeLatency,
                                 double             aSimTime,
                                 double             aSimRate,
                                 double             aSimTimeStep);

protected:
   // Disallow assignment
   WsfDisActionResponse& operator=(const WsfDisActionResponse& aRhs) = delete;
   // bool GetTimeParams(int&    aTimeScheme,
   //                    double& aFeedbackTime,
   //                    double& aTimeLatency,
   //                    double& aStartTime,
   //                    double& aSimulationRate,
   //                    double& aSimTimeStep) const;
   // void FillInTimeParams(int    aTimeScheme,
   //                       double aStartTime,
   //                       double aSimulationRate,
   //                       double aSimTimeStep);
   bool ProcessResponseTo(DisPdu* aInitiatorPtr);
   bool ProcessJoinExerciseResponseTo(const DisActionRequest* aRequestPtr);

private:
   WsfDisInterface* mInterfacePtr;
};

#endif
