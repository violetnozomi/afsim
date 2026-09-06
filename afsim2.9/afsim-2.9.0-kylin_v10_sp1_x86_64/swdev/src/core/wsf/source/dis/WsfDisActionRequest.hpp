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

#ifndef WSFDISACTIONREQUEST_HPP
#define WSFDISACTIONREQUEST_HPP

#include "wsf_export.h"

#include "DisActionRequest.hpp"
class WsfDisInterface;

class WSF_EXPORT WsfDisActionRequest : public DisActionRequest
{
public:
   WsfDisActionRequest(WsfDisInterface* aInterfacePtr);
   WsfDisActionRequest(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr);
   ~WsfDisActionRequest() override;

   int Process() override;

   //! There are several possible uses for a WsfDisActionRequest PDU.
   //! The following methods are provided as a convenience, and
   //! specifically tailored to individual uses of the PDU type.

   //! 1) Application requesting of his System Manager to join an exercise
   void MakeJoinExerciseRequest(const DisEntityId& aSimulationManager,
                                double             aAppJoinTime, // Zero = join immediately
                                double             aMaxAppRate,
                                double             aMaxAppTimeStep,
                                const std::string& aExerciseToJoinId);

   void MakeTimeAdvanceRequest(const DisEntityId& aSimulationManager, double aSimTime, double aAppRate, double aAppTimeStep);

   void MakeResignExerciseRequest(double aSimTime);

protected:
   // Disallow assignment
   WsfDisActionRequest& operator=(const WsfDisActionRequest& aRhs);
   bool                 ProcessJoinExercise(double aSimTime);
   bool                 ParseJoinExerciseRequest(double&      aAppJoinTime,
                                                 double&      aMaxAppRate,
                                                 double&      aMaxAppTimeStep,
                                                 std::string& aExerciseToJoinId) const;

private:
   WsfDisInterface* mInterfacePtr;
};

#endif
