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

#ifndef DIS_ENTITY_STATE_EXTRAPOLATOR_HPP
#define DIS_ENTITY_STATE_EXTRAPOLATOR_HPP

#include "dis_export.h"

#include "UtMat3.hpp"
#include "UtVec3.hpp"

class DisEntityState;
class DIS_EXPORT DisEntityStateExtrapolator
{
public:
   enum ExtrapolationTechnique
   {
      AccurateAngles = 0,
      FastAngles     = 1
   };

   DisEntityStateExtrapolator(ExtrapolationTechnique aExtrapolationTechnique);

   ~DisEntityStateExtrapolator();

   void SetDisEntityState(const DisEntityState* aDisEntityState, double aTime);


   // Accessors

   // Returned vector is in meters
   // Returned angles are in radians
   // aNewTime should be in seconds
   void GetFutureValuesWCS(double aPosExtrap[3], float& aPsiExtrap, float& aThaExtrap, float& aPhiExtrap, double aNewTime);
   // Returned vector is in meters
   // aNewTime should be in seconds
   void GetFutureLocationWCS(double aPosExtrap[3], double aNewTime);

   // Returned angles are in radians
   // aNewTime should be in seconds
   void GetFutureOrientationWCS(float& aPsiExtrap, float& aThaExtrap, float& aPhiExtrap, double aNewTime);

   const DisEntityState* GetDisEntityState() const;

private:
   DisEntityStateExtrapolator();

   void         CorrectFutureAngles();
   void         EnsureCurrentValues(double aNewTime);
   unsigned int DoRotationExactInit();
   unsigned int DoRotationSimpleInit();
   unsigned int DoWCStoBodyInit();
   void         Update();

   UtMat3f mWCStoBody;
   UtVec3f mAccelBodyLessCentripetal;

   const DisEntityState* mDisEntityState;
   double                mTime;
   double                mNewTime;
   double                mPosExtrap[3];
   int                   mExtrapolationTechnique;
   unsigned int          mRotationExactValid;
   unsigned int          mRotationSimpleValid;
   unsigned int          mWCStoBodyValid;
   unsigned int          mGotRotation;
   float                 mBodyOmega[3];
   float                 mBodyOmegaMag;
   float                 mBodyOmegaMagInv;
   float                 mBodyOmegaMag2Inv;
   float                 mBodyOmegaMag3Inv;
   float                 mBodyOmegaMag4Inv;
   float                 mOmegaXOmegaX;
   float                 mOmegaXOmegaY;
   float                 mOmegaXOmegaZ;
   float                 mOmegaYOmegaY;
   float                 mOmegaYOmegaZ;
   float                 mOmegaZOmegaZ;
   float                 mPhiExtrap;
   float                 mThaExtrap;
   float                 mPsiExtrap;
   float                 mPhiDotWCS;
   float                 mThaDotWCS;
   float                 mPsiDotWCS;
};

void inline DisEntityStateExtrapolator::GetFutureValuesWCS(double aPosExtrap[3],
                                                           float& aPsiExtrap,
                                                           float& aThaExtrap,
                                                           float& aPhiExtrap,
                                                           double aNewTime)
{
   EnsureCurrentValues(aNewTime);
   aPosExtrap[0] = mPosExtrap[0];
   aPosExtrap[1] = mPosExtrap[1];
   aPosExtrap[2] = mPosExtrap[2];
   aPsiExtrap    = mPsiExtrap;
   aThaExtrap    = mThaExtrap;
   aPhiExtrap    = mPhiExtrap;
}

void inline DisEntityStateExtrapolator::GetFutureLocationWCS(double aPosExtrap[3], double aNewTime)
{
   EnsureCurrentValues(aNewTime);
   aPosExtrap[0] = mPosExtrap[0];
   aPosExtrap[1] = mPosExtrap[1];
   aPosExtrap[2] = mPosExtrap[2];
}

void inline DisEntityStateExtrapolator::GetFutureOrientationWCS(float& aPsiExtrap,
                                                                float& aThaExtrap,
                                                                float& aPhiExtrap,
                                                                double aNewTime)
{
   EnsureCurrentValues(aNewTime);
   aPsiExtrap = mPsiExtrap;
   aThaExtrap = mThaExtrap;
   aPhiExtrap = mPhiExtrap;
}

void inline DisEntityStateExtrapolator::EnsureCurrentValues(double aNewTime)
{
   if (mNewTime != aNewTime)
   {
      // Update needs the new time so set it now
      mNewTime = aNewTime;
      Update();
   }
}

const inline DisEntityState* DisEntityStateExtrapolator::GetDisEntityState() const
{
   return mDisEntityState;
}

#endif
