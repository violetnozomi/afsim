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

#ifndef WSFANGLEOFATTACK_HPP
#define WSFANGLEOFATTACK_HPP

#include "wsf_export.h"

#include <memory>
#include <vector>

class UtInput;

//! WsfAngleOfAttack uses a table to compute a platform's pitch given the altitude and speed
class WSF_EXPORT WsfAngleOfAttack
{
public:
   WsfAngleOfAttack();

   bool ProcessInput(UtInput& aInput);

   double GetPitch(double aFlightPathAngle, double aAltitude, double aSpeed);

   class AOA_Row
   {
   public:
      float              mAltitude;
      std::vector<float> mSpeeds;
      std::vector<float> mAngles;
   };

   class AOA_Table
   {
   public:
      std::vector<AOA_Row> mRows;
   };

private:
   double GetAngleOfAttack(double aAltitude, double aSpeed);

   double GetAngleOfAttack(double aSpeed, const AOA_Row& aRow) const;

   std::shared_ptr<AOA_Table> mAOA_TablePtr;
   double                     mLastAOA_Altitude;
   double                     mLastAOA_Speed;
   double                     mLastAOA;
};

#endif
