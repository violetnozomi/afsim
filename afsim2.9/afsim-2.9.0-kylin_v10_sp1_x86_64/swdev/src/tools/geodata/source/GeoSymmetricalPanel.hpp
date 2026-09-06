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

#ifndef GeoSymmetricalPanel_hpp
#define GeoSymmetricalPanel_hpp

#include "geodata_export.h"

// This class will take the two corner points of a rectangle and compute
// position and texture points based on the desired resolution.  The index
// points tell what order to draw triangles.

class GEODATA_EXPORT GeoSymmetricalPanel
{
public:
   GeoSymmetricalPanel();

   // All arrays should hold two values
   GeoSymmetricalPanel(const float posLowerLeftA[],
                       const float texLowerLeftA[],
                       const float posUpperRightA[],
                       const float texUpperRightA[],
                       const float posResolutionA[]);

   ~GeoSymmetricalPanel();

   // All arrays should hold two values
   void Init(const float posLowerLeftA[],
             const float texLowerLeftA[],
             const float posUpperRightA[],
             const float texUpperRightA[],
             const float posResolutionA[]);

   void RemovePts();

   unsigned int GetNumIndPts() const { return totalIndPtsM; }
   unsigned int GetNumPosPts() const { return totalPosPtsM; }
   unsigned int GetNumPosPts(unsigned int dimA) const { return numPtsM[dimA]; }
   const float (*GetPosPts() const)[2] { return posM; }
   const float (*GetTexPts() const)[2] { return texM; }
   const unsigned int (*GetIndPts() const)[3] { return indM; }

   const float*        GetPosPt(unsigned int j) const { return posM[j]; }
   const float*        GetTexPt(unsigned int j) const { return texM[j]; }
   const unsigned int* GetIndPt(unsigned int j) const { return indM[j]; }

   const float* GetPosPtFromFirstSet(unsigned int j) const;
   const float* GetPosPtFromLastSet(unsigned int j) const;
   const float* GetTexPtFromFirstSet(unsigned int j) const;
   const float* GetTexPtFromLastSet(unsigned int j) const;


private:
   // Disallow copy and assignment
   GeoSymmetricalPanel(const GeoSymmetricalPanel& aSrc);
   GeoSymmetricalPanel& operator=(const GeoSymmetricalPanel& aRhs);

   float (*posM)[2];
   float (*texM)[2];
   unsigned int (*indM)[3];

   unsigned int totalIndPtsM;
   unsigned int totalPosPtsM;
   unsigned int numPtsM[2];
};

inline const float* GeoSymmetricalPanel::GetPosPtFromFirstSet(unsigned int j) const
{
   return posM[j];
}

inline const float* GeoSymmetricalPanel::GetPosPtFromLastSet(unsigned int j) const
{
   return posM[totalPosPtsM - numPtsM[0] + j];
}

inline const float* GeoSymmetricalPanel::GetTexPtFromFirstSet(unsigned int j) const
{
   return texM[j];
}

inline const float* GeoSymmetricalPanel::GetTexPtFromLastSet(unsigned int j) const
{
   return texM[totalPosPtsM - numPtsM[0] + j];
}

#endif
