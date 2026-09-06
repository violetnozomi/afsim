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

#ifndef WSFDISACTIONREQUESTR_HPP
#define WSFDISACTIONREQUESTR_HPP

#include "wsf_export.h"

#include <string>

#include "DisActionRequestR.hpp"
#include "DisEntityId.hpp"
class WsfDisInterface;
class WsfDisActionResponseR;

class WSF_EXPORT WsfDisActionRequestR : public DisActionRequestR
{
public:
   WsfDisActionRequestR(WsfDisInterface* aInterfacePtr);
   WsfDisActionRequestR(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr);
   ~WsfDisActionRequestR() override;

   void TimedOut() override;

   //!  There are several possible uses for a WsfDisActionRequestR PDU.
   //!  The following methods are provided as a convenience, and are
   //!  specifically tailored to individual uses of the PDU type.

   void MakeJoinExerciseRequest(const DisEntityId& aSimulationManager,
                                double             aSimJoinTime, // Zero or negative = join immediately
                                double             aMaxAppRate,
                                double             aMaxAppTimeStep,
                                const std::string& aExerciseToJoinId);

   void MakeTimeAdvanceRequest(const DisEntityId& aSimulationManager, double aSimTime, double aAppRate, double aAppTimeStep);

   void MakeResignExerciseRequest(double aSimTime);

protected:
   // Disallow assignment
   WsfDisActionRequestR& operator=(const WsfDisActionRequestR& aRhs) = delete;
   DisPdu*               ProduceResponse(bool aComplete);
   bool                  ProcessJoinExercise();
   bool                  ProcessResignExercise();
   bool                  ProcessTimeAdvance(double aSimTime, double aSimRate, double aSimTimeStep);
   bool                  ParseData(double&       aSimTime,
                                   double&       aSimRate,
                                   double&       aSimTimeStep,
                                   unsigned int& aTimeScheme,
                                   std::string&  aExerciseToJoinId) const;

private:
   WsfDisInterface* mInterfacePtr;
};

#endif
