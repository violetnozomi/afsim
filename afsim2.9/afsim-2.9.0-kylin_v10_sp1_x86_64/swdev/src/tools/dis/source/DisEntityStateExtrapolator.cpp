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

#include "DisEntityStateExtrapolator.hpp"

#include "DisEntityState.hpp"
#include "UtMath.hpp"

DisEntityStateExtrapolator::DisEntityStateExtrapolator(ExtrapolationTechnique aExtrapolationTechnique)
   : mDisEntityState(nullptr)
   , mNewTime(-99999.0F)
   , mExtrapolationTechnique(aExtrapolationTechnique)
   , mRotationExactValid(0)
   , mRotationSimpleValid(0)
   , mWCStoBodyValid(0)
   , mGotRotation(0)
{
}


DisEntityStateExtrapolator::~DisEntityStateExtrapolator() {}

void DisEntityStateExtrapolator::SetDisEntityState(const DisEntityState* aDisEntityState, double aTime)
{
   mDisEntityState      = aDisEntityState;
   mTime                = aTime;
   mNewTime             = -99999.0F;
   mRotationExactValid  = 0;
   mRotationSimpleValid = 0;
   mWCStoBodyValid      = 0;

   mDisEntityState->GetAngularVelocity(mBodyOmega[0], mBodyOmega[1], mBodyOmega[2]);

   if (mBodyOmega[0] != 0.0F || mBodyOmega[1] != 0.0F || mBodyOmega[2] != 0.0F)
   {
      mGotRotation = 1;
   }
   else
   {
      mGotRotation = 0;
   }
}

void DisEntityStateExtrapolator::Update()
{
   // No need to go any farther, if there isn't any data.
   if (mDisEntityState == nullptr)
   {
      return;
   }

   // No need to go any farther, if frozen or deactivated.
   if (mDisEntityState->GetAppearance() & 0x00A00000)
   {
      mDisEntityState->GetLocation(mPosExtrap[0], mPosExtrap[1], mPosExtrap[2]);
      mDisEntityState->GetOrientation(mPsiExtrap, mThaExtrap, mPhiExtrap);
      return;
   }

   const float        deltaTime = static_cast<float>(mNewTime - mTime);
   const unsigned int algorithm = mDisEntityState->GetDeadreckoningAlgorithm();


   /////////////////////////////
   //                         //
   //  Do The Position Stuff  //
   //                         //
   /////////////////////////////
   mDisEntityState->GetLocation(mPosExtrap[0], mPosExtrap[1], mPosExtrap[2]);

   if (algorithm == 4 || algorithm == 5 || algorithm == 11) // RVW, FVW, DRM
   {
      float velWCS[3];
      float accWCS[3];

      mDisEntityState->GetVelocity(velWCS[0], velWCS[1], velWCS[2]);
      mDisEntityState->GetAcceleration(accWCS[0], accWCS[1], accWCS[2]);

      float deltaTime2 = 0.5F * deltaTime * deltaTime;

      mPosExtrap[0] += velWCS[0] * deltaTime + accWCS[0] * deltaTime2;
      mPosExtrap[1] += velWCS[1] * deltaTime + accWCS[1] * deltaTime2;
      mPosExtrap[2] += velWCS[2] * deltaTime + accWCS[2] * deltaTime2;
   }
   else if (algorithm == 2 || algorithm == 3 || algorithm == 10) // FPW, RPW, RPW
   {
      float velWCS[3];

      mDisEntityState->GetVelocity(velWCS[0], velWCS[1], velWCS[2]);

      mPosExtrap[0] += velWCS[0] * deltaTime;
      mPosExtrap[1] += velWCS[1] * deltaTime;
      mPosExtrap[2] += velWCS[2] * deltaTime;
   }
   else if (algorithm == 8 || algorithm == 9) // RVB,  FVB
   {
      if (mGotRotation)
      {
         if (DoRotationExactInit())
         {
            mBodyOmegaMag3Inv = mBodyOmegaMagInv * mBodyOmegaMag2Inv;
            mBodyOmegaMag4Inv = mBodyOmegaMag2Inv * mBodyOmegaMag2Inv;

            float velBody[3];
            mDisEntityState->GetVelocity(velBody[0], velBody[1], velBody[2]);

            // Compute Centripetal acceleration
            float centripetalAccel[3];
            centripetalAccel[0] = mBodyOmega[1] * velBody[2] - mBodyOmega[2] * velBody[1];
            centripetalAccel[1] = mBodyOmega[2] * velBody[0] - mBodyOmega[0] * velBody[2];
            centripetalAccel[2] = mBodyOmega[0] * velBody[1] - mBodyOmega[1] * velBody[0];

            float accBody[3];
            mDisEntityState->GetAcceleration(accBody[0], accBody[1], accBody[2]);

            mAccelBodyLessCentripetal[0] = accBody[0] - centripetalAccel[0];
            mAccelBodyLessCentripetal[1] = accBody[1] - centripetalAccel[1];
            mAccelBodyLessCentripetal[2] = accBody[2] - centripetalAccel[2];
         }

         const float bodyOmegaMagTime    = mBodyOmegaMag * deltaTime;
         const float sinBodyOmegaMagTime = sinf(bodyOmegaMagTime);
         const float cosBodyOmegaMagTime = cosf(bodyOmegaMagTime);
         float       termA               = (bodyOmegaMagTime - sinBodyOmegaMagTime) * mBodyOmegaMag3Inv;
         float       termB               = sinBodyOmegaMagTime * mBodyOmegaMagInv;
         float       termC               = (1.0F - cosBodyOmegaMagTime) * mBodyOmegaMag2Inv;
         float       termAXY             = termA * mOmegaXOmegaY;
         float       termAXZ             = termA * mOmegaXOmegaZ;
         float       termAYZ             = termA * mOmegaYOmegaZ;
         float       termCX              = termC * mBodyOmega[0];
         float       termCY              = termC * mBodyOmega[1];
         float       termCZ              = termC * mBodyOmega[2];

         UtMat3f matr1;

         matr1[0] = termA * mOmegaXOmegaX + termB;
         matr1[1] = termAXY + termCZ;
         matr1[2] = termAXZ - termCY;
         matr1[3] = termAXY - termCZ;
         matr1[4] = termA * mOmegaYOmegaY + termB;
         matr1[5] = termAYZ + termCX;
         matr1[6] = termAXZ + termCY;
         matr1[7] = termAYZ - termCX;
         matr1[8] = termA * mOmegaZOmegaZ + termB;

         termA = (0.5F * bodyOmegaMagTime * bodyOmegaMagTime - cosBodyOmegaMagTime -
                  bodyOmegaMagTime * sinBodyOmegaMagTime + 1.0F) *
                 mBodyOmegaMag4Inv;
         termB   = (cosBodyOmegaMagTime + bodyOmegaMagTime * sinBodyOmegaMagTime - 1.0F) * mBodyOmegaMag2Inv;
         termC   = (sinBodyOmegaMagTime - bodyOmegaMagTime * cosBodyOmegaMagTime) * mBodyOmegaMag3Inv;
         termAXY = termA * mOmegaXOmegaY;
         termAXZ = termA * mOmegaXOmegaZ;
         termAYZ = termA * mOmegaYOmegaZ;
         termCX  = termC * mBodyOmega[0];
         termCY  = termC * mBodyOmega[1];
         termCZ  = termC * mBodyOmega[2];

         UtMat3f matr2;

         matr2[0] = termA * mOmegaXOmegaX + termB;
         matr2[1] = termAXY + termCZ;
         matr2[2] = termAXZ - termCY;
         matr2[3] = termAXY - termCZ;
         matr2[4] = termA * mOmegaYOmegaY + termB;
         matr2[5] = termAYZ + termCX;
         matr2[6] = termAXZ + termCY;
         matr2[7] = termAYZ - termCX;
         matr2[8] = termA * mOmegaZOmegaZ + termB;

         float velBody[3];
         mDisEntityState->GetVelocity(velBody[0], velBody[1], velBody[2]);

         UtVec3f deltaPosWCS(
            mWCStoBody.TransposeMultiply(matr1.Multiply(UtVec3f(velBody)) + matr2.Multiply(mAccelBodyLessCentripetal)));

         mPosExtrap[0] += deltaPosWCS[0];
         mPosExtrap[1] += deltaPosWCS[1];
         mPosExtrap[2] += deltaPosWCS[2];
      }
      else
      {
         // No rotation, so keep it simple
         float velBody[3];
         mDisEntityState->GetVelocity(velBody[0], velBody[1], velBody[2]);

         float accBody[3];
         mDisEntityState->GetAcceleration(accBody[0], accBody[1], accBody[2]);

         float   termA = 0.5f * deltaTime * deltaTime;
         UtVec3f deltaPosBody;
         deltaPosBody[0] = velBody[0] * deltaTime + accBody[0] * termA;
         deltaPosBody[1] = velBody[1] * deltaTime + accBody[1] * termA;
         deltaPosBody[2] = velBody[2] * deltaTime + accBody[2] * termA;

         DoWCStoBodyInit();
         UtVec3f deltaPosWCS(mWCStoBody.TransposeMultiply(deltaPosBody));

         mPosExtrap[0] += deltaPosWCS[0];
         mPosExtrap[1] += deltaPosWCS[1];
         mPosExtrap[2] += deltaPosWCS[2];
      }
   }
   else if (algorithm == 6 || algorithm == 7) // FPB,  RPB
   {
      if (mGotRotation)
      {
         if (DoRotationExactInit())
         {
            mBodyOmegaMag3Inv = mBodyOmegaMagInv * mBodyOmegaMag2Inv;
         }

         const float bodyOmegaMagTime    = mBodyOmegaMag * deltaTime;
         const float sinBodyOmegaMagTime = sinf(bodyOmegaMagTime);
         const float cosBodyOmegaMagTime = cosf(bodyOmegaMagTime);
         float       termA               = (bodyOmegaMagTime - sinBodyOmegaMagTime) * mBodyOmegaMag3Inv;
         float       termB               = sinBodyOmegaMagTime * mBodyOmegaMagInv;
         float       termC               = 1.0F - cosBodyOmegaMagTime * mBodyOmegaMag2Inv;
         float       termAXY             = termA * mOmegaXOmegaY;
         float       termAXZ             = termA * mOmegaXOmegaZ;
         float       termAYZ             = termA * mOmegaYOmegaZ;
         float       termCX              = termC * mBodyOmega[0];
         float       termCY              = termC * mBodyOmega[1];
         float       termCZ              = termC * mBodyOmega[2];

         UtMat3f matr1;

         matr1[0] = termA * mOmegaXOmegaX + termB;
         matr1[1] = termAXY + termCZ;
         matr1[2] = termAXZ - termCY;
         matr1[3] = termAXY - termCZ;
         matr1[4] = termA * mOmegaYOmegaY + termB;
         matr1[5] = termAYZ + termCX;
         matr1[6] = termAXZ + termCY;
         matr1[7] = termAYZ - termCX;
         matr1[8] = termA * mOmegaZOmegaZ + termB;

         float velBody[3];
         mDisEntityState->GetVelocity(velBody[0], velBody[1], velBody[2]);

         UtVec3f deltaPosWCS(mWCStoBody.TransposeMultiply(matr1.Multiply(UtVec3f(velBody))));

         mPosExtrap[0] += deltaPosWCS[0];
         mPosExtrap[1] += deltaPosWCS[1];
         mPosExtrap[2] += deltaPosWCS[2];
      }
      else
      {
         // No rotation, so keep it simple
         float velBody[3];
         mDisEntityState->GetVelocity(velBody[0], velBody[1], velBody[2]);

         UtVec3f deltaPosBody;
         deltaPosBody[0] = velBody[0] * deltaTime;
         deltaPosBody[1] = velBody[1] * deltaTime;
         deltaPosBody[2] = velBody[2] * deltaTime;

         DoWCStoBodyInit();
         UtVec3f deltaPosWCS(mWCStoBody.TransposeMultiply(deltaPosBody));

         mPosExtrap[0] += deltaPosWCS[0];
         mPosExtrap[1] += deltaPosWCS[1];
         mPosExtrap[2] += deltaPosWCS[2];
      }
   }


   //////////////////////////////
   //                          //
   //  Do The Rotation Stuff   //
   //                          //
   //////////////////////////////
   if (algorithm == 4 || algorithm == 3 || algorithm == 8 || algorithm == 7) // RVW, RPW, RVB, RPB
   {
      if (mGotRotation)
      {
         if (mExtrapolationTechnique == AccurateAngles)
         {
            // The accurate way
            DoRotationExactInit();

            const float bodyOmegaMagTime = mBodyOmegaMag * deltaTime;
            const float termB            = cosf(bodyOmegaMagTime);
            const float termA            = (1.0F - termB) * mBodyOmegaMag2Inv;
            const float termC            = sinf(bodyOmegaMagTime) * mBodyOmegaMagInv;
            const float termAXY          = termA * mOmegaXOmegaY;
            const float termAXZ          = termA * mOmegaXOmegaZ;
            const float termAYZ          = termA * mOmegaYOmegaZ;
            const float termCX           = termC * mBodyOmega[0];
            const float termCY           = termC * mBodyOmega[1];
            const float termCZ           = termC * mBodyOmega[2];

            UtMat3f deadReckon;

            deadReckon[0] = termA * mOmegaXOmegaX + termB;
            deadReckon[1] = termAXY - termCZ;
            deadReckon[2] = termAXZ + termCY;
            deadReckon[3] = termAXY + termCZ;
            deadReckon[4] = termA * mOmegaYOmegaY + termB;
            deadReckon[5] = termAYZ - termCX;
            deadReckon[6] = termAXZ - termCY;
            deadReckon[7] = termAYZ + termCX;
            deadReckon[8] = termA * mOmegaZOmegaZ + termB;

            // One could use the "WCStoBodyFuture" matrix computed in the position calculations for
            // algorithms 7 and 8, but algorithms 7 and 8 are used so infrequently that we will not worry
            // about any optimizations for now.
            UtMat3f WCStoBodyFuture;
            deadReckon.Multiply(WCStoBodyFuture, mWCStoBody);

            mPhiExtrap = WCStoBodyFuture.GetPhiOfTransposed();
            mThaExtrap = WCStoBodyFuture.GetThaOfTransposed();
            mPsiExtrap = WCStoBodyFuture.GetPsiOfTransposed();

            CorrectFutureAngles();
         }
         else
         {
            // The fast way
            DoRotationSimpleInit();

            mDisEntityState->GetOrientation(mPsiExtrap, mThaExtrap, mPhiExtrap);

            mPhiExtrap += mPhiDotWCS * deltaTime;
            mThaExtrap += mThaDotWCS * deltaTime;
            mPsiExtrap += mPsiDotWCS * deltaTime;

            CorrectFutureAngles();
         }
      }
      else
      {
         mDisEntityState->GetOrientation(mPsiExtrap, mThaExtrap, mPhiExtrap);
      }
   }
   else if (algorithm == 10 || algorithm == 11) // RVW, RPW
   {
      mDisEntityState->GetOrientation(mPsiExtrap, mThaExtrap, mPhiExtrap);
      if (mGotRotation)
      {
         DoRotationSimpleInit();

         mPhiExtrap += mPhiDotWCS * deltaTime;
         mThaExtrap += mThaDotWCS * deltaTime;
         mPsiExtrap += mPsiDotWCS * deltaTime;

         CorrectFutureAngles();
      }
   }
   else
   {
      mDisEntityState->GetOrientation(mPsiExtrap, mThaExtrap, mPhiExtrap);
   }
}

unsigned int DisEntityStateExtrapolator::DoRotationExactInit()
{
   if (mRotationExactValid == 0)
   {
      mBodyOmegaMag = sqrtf(mBodyOmega[0] * mBodyOmega[0] + mBodyOmega[1] * mBodyOmega[1] + mBodyOmega[2] * mBodyOmega[2]);
      mBodyOmegaMagInv  = 1.0F / mBodyOmegaMag;
      mBodyOmegaMag2Inv = mBodyOmegaMagInv * mBodyOmegaMagInv;

      mOmegaXOmegaX = mBodyOmega[0] * mBodyOmega[0];
      mOmegaXOmegaY = mBodyOmega[0] * mBodyOmega[1];
      mOmegaXOmegaZ = mBodyOmega[0] * mBodyOmega[2];
      mOmegaYOmegaY = mBodyOmega[1] * mBodyOmega[1];
      mOmegaYOmegaZ = mBodyOmega[1] * mBodyOmega[2];
      mOmegaZOmegaZ = mBodyOmega[2] * mBodyOmega[2];

      DoWCStoBodyInit();

      mRotationExactValid = 1;

      // Did Computations
      return 1;
   }
   else
   {
      // Did nothing
      return 0;
   }
}

unsigned int DisEntityStateExtrapolator::DoRotationSimpleInit()
{
   if (mRotationSimpleValid == 0)
   {
      float phi;
      float tha;
      float psi;

      mDisEntityState->GetOrientation(psi, tha, phi);
      const float sinphi = sinf(phi);
      const float cosphi = cosf(phi);
      const float sintha = sinf(tha);
      float       costha = cosf(tha);

      // Protect for a divide by zero
      if (costha < 1.0e-4F && costha > -1.0e-4F)
      {
         costha = costha > 0.0F ? 1.0e-4F : -1.0e-4F;
      }

      mThaDotWCS = mBodyOmega[1] * cosphi - mBodyOmega[2] * sinphi;
      mPsiDotWCS = (mBodyOmega[1] * sinphi + mBodyOmega[2] * cosphi) / costha;
      mPhiDotWCS = mBodyOmega[0] + mPsiDotWCS * sintha;

      mRotationSimpleValid = 1;

      // Did Computations
      return 1;
   }
   else
   {
      // Did nothing
      return 0;
   }
}

unsigned int DisEntityStateExtrapolator::DoWCStoBodyInit()
{
   if (mWCStoBodyValid == 0)
   {
      float phi;
      float tha;
      float psi;

      mDisEntityState->GetOrientation(psi, tha, phi);
      mWCStoBody.BuildTranspose(phi, tha, psi);
      mWCStoBodyValid = 1;

      // Did Computations
      return 1;
   }
   else
   {
      // Did nothing
      return 0;
   }
}

void DisEntityStateExtrapolator::CorrectFutureAngles()
{
   int   jtemp;
   float temp;

   if (mThaExtrap > static_cast<float>(UtMath::cPI_OVER_2))
   {
      temp  = mThaExtrap / static_cast<float>(UtMath::cPI_OVER_2);
      jtemp = int(temp);
      temp  = temp - static_cast<float>(jtemp);
      jtemp %= 4;

      if (jtemp == 0)
      {
         mThaExtrap = static_cast<float>(UtMath::cPI_OVER_2) * temp;
      }
      else if (jtemp == 1)
      {
         mThaExtrap = static_cast<float>(UtMath::cPI_OVER_2) * (1.0F - temp);
         mPhiExtrap += static_cast<float>(UtMath::cPI);
         mPsiExtrap += static_cast<float>(UtMath::cPI);
      }
      else if (jtemp == 2)
      {
         mThaExtrap = -static_cast<float>(UtMath::cPI_OVER_2) * temp;
         mPhiExtrap += static_cast<float>(UtMath::cPI);
         mPsiExtrap += static_cast<float>(UtMath::cPI);
      }
      else
      {
         mThaExtrap = static_cast<float>(UtMath::cPI_OVER_2) * (temp - 1.0F);
      }
   }
   else if (mThaExtrap < -static_cast<float>(UtMath::cPI_OVER_2))
   {
      temp  = mThaExtrap / static_cast<float>(UtMath::cPI_OVER_2);
      jtemp = -int(temp);
      temp  = temp + static_cast<float>(jtemp);
      jtemp %= 4;

      if (jtemp == 0)
      {
         mThaExtrap = static_cast<float>(UtMath::cPI_OVER_2) * temp;
      }
      else if (jtemp == 1)
      {
         mThaExtrap = -static_cast<float>(UtMath::cPI_OVER_2) * (1.0F + temp);
         mPhiExtrap += static_cast<float>(UtMath::cPI);
         mPsiExtrap += static_cast<float>(UtMath::cPI);
      }
      else if (jtemp == 2)
      {
         mThaExtrap = -static_cast<float>(UtMath::cPI_OVER_2) * temp;
         mPhiExtrap += static_cast<float>(UtMath::cPI);
         mPsiExtrap += static_cast<float>(UtMath::cPI);
      }
      else
      {
         mThaExtrap = static_cast<float>(UtMath::cPI_OVER_2) * (1.0F + temp);
      }
   }

   if (mPhiExtrap > static_cast<float>(UtMath::cPI))
   {
      temp  = mPhiExtrap / static_cast<float>(UtMath::cPI);
      jtemp = int(temp);
      temp  = temp - static_cast<float>(jtemp);
      jtemp %= 2;

      if (jtemp == 0)
      {
         mPhiExtrap = static_cast<float>(UtMath::cPI) * temp;
      }
      else
      {
         mPhiExtrap = static_cast<float>(UtMath::cPI) * (temp - 1.0F);
      }
   }
   else if (mPhiExtrap < -static_cast<float>(UtMath::cPI))
   {
      temp  = mPhiExtrap / static_cast<float>(UtMath::cPI);
      jtemp = -int(temp);
      temp  = temp + static_cast<float>(jtemp);
      jtemp %= 2;

      if (jtemp == 0)
      {
         mPhiExtrap = static_cast<float>(UtMath::cPI) * temp;
      }
      else
      {
         mPhiExtrap = static_cast<float>(UtMath::cPI) * (temp + 1.0F);
      }
   }

   if (mPsiExtrap > UtMath::cTWO_PI)
   {
      temp       = static_cast<float>(mPsiExtrap / UtMath::cTWO_PI);
      jtemp      = int(temp);
      mPsiExtrap = static_cast<float>((temp - static_cast<float>(jtemp)) * UtMath::cTWO_PI);
   }
   else if (mPsiExtrap < 0.0F)
   {
      temp       = static_cast<float>(mPsiExtrap / UtMath::cTWO_PI);
      jtemp      = int(temp);
      mPsiExtrap = static_cast<float>((temp - static_cast<float>(jtemp) + 1.0F) * UtMath::cTWO_PI);
   }
}
