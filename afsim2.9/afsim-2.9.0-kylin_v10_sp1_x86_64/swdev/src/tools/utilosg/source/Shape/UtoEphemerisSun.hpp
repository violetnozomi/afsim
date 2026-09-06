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

#ifndef _UTOEPHEMERISSUN_HPP_
#define _UTOEPHEMERISSUN_HPP_

#include "utilosg_export.h"

#include <vector>

#include <osg/Drawable>

class UtCalendar;

class UTILOSG_EXPORT UtoEphemerisSun : public osg::Drawable
{
public:
   UtoEphemerisSun();

   /** Copy constructor using CopyOp to manage deep vs shallow copy.*/
   UtoEphemerisSun(const UtoEphemerisSun& drawer, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);

   UtoEphemerisSun& operator=(const UtoEphemerisSun& aRHS) = delete;


   META_Object(StealthView, UtoEphemerisSun)

      // the draw immediate mode method is where the OSG wraps up the drawing of
      // of OpenGL primitives.
      void drawImplementation(osg::RenderInfo&) const override;

   void SetTime(const UtCalendar& aTime);

   const osg::Vec3& GetVector() const { return mDirection; }

   void EnableVisualization(const bool aState) { mDraw = aState; }

protected:
   ~UtoEphemerisSun() override = default;

private:
   double    mAz;
   double    mEl;
   osg::Vec3 mDirection;
   bool      mDraw;

public:
};

#endif
