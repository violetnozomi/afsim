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

#include "GeoSymmetricalPanel.hpp"

#include "UtCast.hpp"

GeoSymmetricalPanel::GeoSymmetricalPanel()
   : posM(nullptr)
   , texM(nullptr)
   , indM(nullptr)
   , totalIndPtsM(0)
   , totalPosPtsM(0)

{
   numPtsM[0] = 0;
   numPtsM[1] = 0;
}

// All arrays should hold two values
GeoSymmetricalPanel::GeoSymmetricalPanel(const float posLowerLeftA[],
                                         const float texLowerLeftA[],
                                         const float posUpperRightA[],
                                         const float texUpperRightA[],
                                         const float posResolutionA[])
   : posM(nullptr)
   , texM(nullptr)
   , indM(nullptr)
{
   Init(posLowerLeftA, texLowerLeftA, posUpperRightA, texUpperRightA, posResolutionA);
}

GeoSymmetricalPanel::~GeoSymmetricalPanel()
{
   RemovePts();
}

// All arrays should hold two values
void GeoSymmetricalPanel::Init(const float posLowerLeftA[],
                               const float texLowerLeftA[],
                               const float posUpperRightA[],
                               const float texUpperRightA[],
                               const float posResolutionA[])

{
   float posDis[2];
   float texDis[2];
   float posInvDis[2];
   float texInvDis[2];


   // Clean up from before
   RemovePts();

   texDis[0]  = texUpperRightA[0] - texLowerLeftA[0];
   posDis[0]  = posUpperRightA[0] - posLowerLeftA[0];
   numPtsM[0] = static_cast<unsigned int>(posDis[0] / posResolutionA[0] + 1.5F);
   if (numPtsM[0] < 2U)
      numPtsM[0] = 2U;

   texDis[1]  = texUpperRightA[1] - texLowerLeftA[1];
   posDis[1]  = posUpperRightA[1] - posLowerLeftA[1];
   numPtsM[1] = static_cast<unsigned int>(posDis[1] / posResolutionA[1] + 1.5F);
   if (numPtsM[1] < 3U)
      numPtsM[1] = 3U;
   else if (!(numPtsM[1] % 2U))
      ++numPtsM[1];

   totalPosPtsM = (numPtsM[0] * 2U + 1U) * ((numPtsM[1] - 1U) / 2U) + numPtsM[0];

   posM = new float[totalPosPtsM][2];
   texM = new float[totalPosPtsM][2];

   totalIndPtsM = (numPtsM[0] * 2U - 1U) * (numPtsM[1] - 1U);
   indM         = new unsigned int[totalIndPtsM][3];

   // Calculate some intermediate variablse
   posInvDis[0] = posDis[0] / (numPtsM[0] - 1U);
   posInvDis[1] = posDis[1] / (numPtsM[1] - 1U);
   texInvDis[0] = texDis[0] / (numPtsM[0] - 1U);
   texInvDis[1] = texDis[1] / (numPtsM[1] - 1U);


   // Calculate all the postion and texture points
   // Start in the upper left hand corner and compute points
   // row by row
   unsigned int index = 0U;
   for (unsigned int j1 = 0U; j1 < numPtsM[1]; ++j1)
   {
      const float posStepOne = j1 * posInvDis[1];
      const float texStepOne = j1 * texInvDis[1];
      if (j1 % 2U)
      {
         // Odd lines have an extra point
         for (unsigned int j0 = 0U; j0 < numPtsM[0] + 1U; ++j0)
         {
            if (j0 == 0U)
            {
               posM[index][0] = posLowerLeftA[0];
               texM[index][0] = texLowerLeftA[0];
            }
            else if (j0 == numPtsM[0])
            {
               posM[index][0] = posUpperRightA[0];
               texM[index][0] = texUpperRightA[0];
            }
            else
            {
               posM[index][0] = posLowerLeftA[0] + (j0 - 0.5F) * posInvDis[0];
               texM[index][0] = texLowerLeftA[0] + (j0 - 0.5F) * texInvDis[0];
            }
            posM[index][1] = posUpperRightA[1] - posStepOne;
            texM[index][1] = texUpperRightA[1] - texStepOne;
            ++index;
         }
      }
      else
      {
         // Even lines are straight through
         for (unsigned int j0 = 0U; j0 < numPtsM[0]; ++j0)
         {
            posM[index][0] = posLowerLeftA[0] + j0 * posInvDis[0];
            texM[index][0] = texLowerLeftA[0] + j0 * texInvDis[0];
            posM[index][1] = posUpperRightA[1] - posStepOne;
            texM[index][1] = texUpperRightA[1] - texStepOne;
            ++index;
         }
      }
   }

   // Calculate the indexs; that is the order of the points to draw triangles
   // The indexs should should draw triangles row by row
   // starting in the upper left hand corner
   int rowBeg[3][3];
   int step[3][3];

   unsigned int ptsPerRow       = numPtsM[0] * 2U + 1U;
   unsigned int trianglesPerRow = numPtsM[0] * 2U - 1U;

   rowBeg[0][0] = 0;
   rowBeg[0][1] = ut::safe_cast<int, unsigned int>(numPtsM[0]);
   rowBeg[0][2] = rowBeg[0][1] + 1;

   rowBeg[1][0] = ut::safe_cast<int, unsigned int>(numPtsM[0]);
   rowBeg[1][1] = ut::safe_cast<int, unsigned int>(ptsPerRow);
   rowBeg[1][2] = rowBeg[1][0] + 1;

   step[0][0] = 1;
   step[0][1] = 0;
   step[0][2] = ut::safe_cast<int, unsigned int>(numPtsM[0] + 1U);

   step[1][0] = 0;
   step[1][1] = 1;
   step[1][2] = -ut::safe_cast<int, unsigned int>(numPtsM[0]);

   index = 0U;
   for (unsigned int jr = 0U; jr < numPtsM[1] - 1U; ++jr)
   {
      unsigned int evenOddRow = jr % 2U;

      indM[index][0] = ut::safe_cast<unsigned int, int>(rowBeg[evenOddRow][0]);
      indM[index][1] = ut::safe_cast<unsigned int, int>(rowBeg[evenOddRow][1]);
      indM[index][2] = ut::safe_cast<unsigned int, int>(rowBeg[evenOddRow][2]);
      ++index;

      for (unsigned int jt = 1U; jt < trianglesPerRow; ++jt)
      {
         unsigned int evenOddTri = index % 2U;
         indM[index][0]          = indM[index - 1][0] + ut::safe_cast<unsigned int, int>(step[evenOddTri][0]);
         indM[index][1]          = indM[index - 1][1] + ut::safe_cast<unsigned int, int>(step[evenOddTri][1]);
         indM[index][2]          = indM[index - 1][2] + ut::safe_cast<unsigned int, int>(step[evenOddTri][2]);
         ++index;
      }

      rowBeg[evenOddRow][0] += ut::safe_cast<int, unsigned int>(ptsPerRow);
      rowBeg[evenOddRow][1] += ut::safe_cast<int, unsigned int>(ptsPerRow);
      rowBeg[evenOddRow][2] += ut::safe_cast<int, unsigned int>(ptsPerRow);
   }
}

void GeoSymmetricalPanel::RemovePts()
{
   delete[] posM;
   delete[] texM;
   delete[] indM;

   totalIndPtsM = 0;
   totalPosPtsM = 0;
   numPtsM[0]   = 0;
   numPtsM[1]   = 0;
}
