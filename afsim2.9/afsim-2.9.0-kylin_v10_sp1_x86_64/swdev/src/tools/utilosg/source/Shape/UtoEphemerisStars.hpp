// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef _UTOEPHEMERISSTARS_HPP_
#define _UTOEPHEMERISSTARS_HPP_

#include "utilosg_export.h"

#include <vector>

#include <osg/Drawable>

#include "UtCalendar.hpp"
#include "UtSolarSystem.hpp"

class UTILOSG_EXPORT UtoEphemerisStars : public osg::Drawable
{
public:
   UtoEphemerisStars();
   UtoEphemerisStars(const char* aFilename);

   /** Copy constructor using CopyOp to manage deep vs shallow copy.*/
   UtoEphemerisStars(const UtoEphemerisStars& drawer, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);

   UtoEphemerisStars& operator=(const UtoEphemerisStars& aRHS) = delete;


   META_Object(StealthView, UtoEphemerisStars)

      // the draw immediate mode method is where the OSG wraps up the drawing of
      // of OpenGL primitives.
      void drawImplementation(osg::RenderInfo&) const override;

   void SetTime(const UtCalendar& aTime);
   void EnableVisualization(const bool aState) { mDraw = aState; }

protected:
   ~UtoEphemerisStars() override;

private:
   void ReadHeader(std::istream& aInput);
   void ReadEntry(std::istream& aInput);
   template<class T>
   void RevRead(T* aBuf, int aSize, std::istream& aInput);
   void BuildDisplayList(const UtCalendar& aTime) const;
   void BuildSolarSystemDisplayList(const UtCalendar& aTime) const;

   struct StarPoint
   {
      int   mNum;
      float mRA;
      float mDec;
      float mRA_ProperMotion;
      float mDec_ProperMotion;
      float mColor[4];
   };
   std::vector<StarPoint> mStarCatalog;
   int                    mNumberStars;
   static float           mColor[21][3];
   mutable unsigned int   mListId;
   mutable bool           mListIdDirty;
   mutable bool           mListLoaded;
   float                  mAz;
   UtCalendar             mEpoch;
   int                    mYear;
   int                    mMonth;
   int                    mDay;
   int                    mHour;
   bool                   mDraw;

   bool mUseStarTexture;

   int                   mPlanetsIndex[5];
   float                 mPlanetLocation[5][3];
   mutable UtSolarSystem mSolarSystem;
   float                 mPlanetColor[5][3];
   mutable unsigned int  mSolarSystemListId;
   mutable bool          mSolarSystemListIdDirty;
   mutable bool          mSolarSystemListLoaded;
   UtCalendar            mTime;
};

#endif
