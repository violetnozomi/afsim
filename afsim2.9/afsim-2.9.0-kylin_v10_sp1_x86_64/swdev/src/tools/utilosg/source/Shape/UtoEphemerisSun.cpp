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

#include "UtoEphemerisSun.hpp"

#include <iostream>

#include <UtMath.hpp>
#include <UtSun.hpp>

UtoEphemerisSun::UtoEphemerisSun()
   : mDirection(1.0F, 0.0F, 0.0F)
   , mDraw(true)
{
   setSupportsDisplayList(false);
   osg::StateSet* stateSet = getOrCreateStateSet();
   stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
   stateSet->setRenderBinDetails(-4, "RenderBin");
   stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
   stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
   setStateSet(stateSet);
}

UtoEphemerisSun::UtoEphemerisSun(const UtoEphemerisSun& drawer, const osg::CopyOp& copyop)
   : osg::Drawable(drawer, copyop)
   , mDirection(drawer.GetVector())
{
   setSupportsDisplayList(false);
   osg::StateSet* stateSet = getOrCreateStateSet();
   stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
   stateSet->setRenderBinDetails(-4, "RenderBin");
   stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
   stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
   setStateSet(stateSet);
}

// virtual
void UtoEphemerisSun::drawImplementation(osg::RenderInfo& aRenderInfo) const
{
   if (!mDraw)
   {
      return;
   }

   double leftp;
   double rightp;
   double bottomp;
   double topp;
   double nearp;
   double farp;
   aRenderInfo.getCurrentCamera()->getProjectionMatrixAsFrustum(leftp, rightp, bottomp, topp, nearp, farp);

   float fard;
   fard = 0.5 * (nearp + farp); // I probably shouldn't assume the order of the planes...

   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   osg::Matrix mv = aRenderInfo.getState()->getModelViewMatrix();
   mv(3, 0)       = 0;
   mv(3, 1)       = 0;
   mv(3, 2)       = 0; // zero out the translations

   double size = fard / 216.0; // 7e8 : 1.5e11 :: x : neard (make the sun an appropriate size)

   osg::Matrix sunMat =
      osg::Matrixd::scale(size, size, size) *
      osg::Matrixd::translate(osg::Vec3(fard, 0, 0)) * // put the sun out there somewhere where it won't usually be clipped
      osg::Matrixd::rotate(-mEl, 0.0, 1.0, 0.0) *      // elevation
      osg::Matrixd::rotate(-mAz, 0.0, 0.0, 1.0) *      // azimuth
      mv;

   glLoadMatrix(sunMat.ptr());

   glColor3f(1.0f, 1.0f, 1.0f);
   glBegin(GL_POLYGON);
   glVertex3f(0.0f, 0.00f, 1.00f);
   glVertex3f(0.0f, 0.38f, 0.92f);
   glVertex3f(0.0f, 0.71f, 0.71f);
   glVertex3f(0.0f, 0.92f, 0.38f);
   glVertex3f(0.0f, 1.00f, 0.00f);
   glVertex3f(0.0f, 0.92f, -0.38f);
   glVertex3f(0.0f, 0.71f, -0.71f);
   glVertex3f(0.0f, 0.38f, -0.92f);
   glVertex3f(0.0f, 0.00f, -1.00f);
   glVertex3f(0.0f, -0.38f, -0.92f);
   glVertex3f(0.0f, -0.71f, -0.71f);
   glVertex3f(0.0f, -0.92f, -0.38f);
   glVertex3f(0.0f, -1.00f, 0.00f);
   glVertex3f(0.0f, -0.92f, 0.38f);
   glVertex3f(0.0f, -0.71f, 0.71f);
   glVertex3f(0.0f, -0.38f, 0.92f);
   glEnd();
   glBegin(GL_TRIANGLE_STRIP);
   glColor3f(1.0f, 1.0f, 1.0f);
   glVertex3f(0.0f, 0.00f, 1.00f);
   glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
   glVertex3f(0.0f, 0.00f, 1.20f); // scale
   glColor3f(1.0f, 1.0f, 1.0f);
   glVertex3f(0.0f, 0.38f, 0.92f);
   glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
   glVertex3f(0.0f, 0.46f, 1.10f); // scale
   glColor3f(1.0f, 1.0f, 1.0f);
   glVertex3f(0.0f, 0.71f, 0.71f);
   glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
   glVertex3f(0.0f, 0.85f, 0.85f); // scale
   glColor3f(1.0f, 1.0f, 1.0f);
   glVertex3f(0.0f, 0.92f, 0.38f);
   glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
   glVertex3f(0.0f, 1.10f, 0.46f); // scale
   glColor3f(1.0f, 1.0f, 1.0f);
   glVertex3f(0.0f, 1.00f, 0.00f);
   glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
   glVertex3f(0.0f, 1.20f, 0.00f); // scale@
   glColor3f(1.0f, 1.0f, 1.0f);
   glVertex3f(0.0f, 0.92f, -0.38f);
   glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
   glVertex3f(0.0f, 1.10f, -0.46f); // scale
   glColor3f(1.0f, 1.0f, 1.0f);
   glVertex3f(0.0f, 0.71f, -0.71f);
   glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
   glVertex3f(0.0f, 0.85f, -0.85f); // scale
   glColor3f(1.0f, 1.0f, 1.0f);
   glVertex3f(0.0f, 0.38f, -0.92f);
   glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
   glVertex3f(0.0f, 0.46f, -1.10f); // scale
   glColor3f(1.0f, 1.0f, 1.0f);
   glVertex3f(0.0f, 0.00f, -1.00f);
   glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
   glVertex3f(0.0f, 0.00f, -1.20f); // scale
   glColor3f(1.0f, 1.0f, 1.0f);
   glVertex3f(0.0f, -0.38f, -0.92f);
   glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
   glVertex3f(0.0f, -0.46f, -1.10f); // scale
   glColor3f(1.0f, 1.0f, 1.0f);
   glVertex3f(0.0f, -0.71f, -0.71f);
   glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
   glVertex3f(0.0f, -0.85f, -0.85f); // scale
   glColor3f(1.0f, 1.0f, 1.0f);
   glVertex3f(0.0f, -0.92f, -0.38f);
   glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
   glVertex3f(0.0f, -1.10f, -0.46f); // scale
   glColor3f(1.0f, 1.0f, 1.0f);
   glVertex3f(0.0f, -1.00f, 0.00f);
   glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
   glVertex3f(0.0f, -1.20f, 0.00f); // scale
   glColor3f(1.0f, 1.0f, 1.0f);
   glVertex3f(0.0f, -0.92f, 0.38f);
   glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
   glVertex3f(0.0f, -1.10f, 0.46f); // scale
   glColor3f(1.0f, 1.0f, 1.0f);
   glVertex3f(0.0f, -0.71f, 0.71f);
   glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
   glVertex3f(0.0f, -0.85f, 0.85f); // scale
   glColor3f(1.0f, 1.0f, 1.0f);
   glVertex3f(0.0f, -0.38f, 0.92f);
   glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
   glVertex3f(0.0f, -0.46f, 1.10f); // scale
   glColor3f(1.0f, 1.0f, 1.0f);
   glVertex3f(0.0f, 0.00f, 1.00f);
   glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
   glVertex3f(0.0f, 0.00f, 1.20f); // scale
   glEnd();

   glPopMatrix();
}

void UtoEphemerisSun::SetTime(const UtCalendar& aTime)
{
   double dir[3];
   UtSun::GetSunVecWCS(aTime, dir);
   mDirection.set(dir[0], dir[1], dir[2]);
   mAz = atan2(-dir[1], dir[0]);
   mEl = atan2(dir[2], sqrt(dir[1] * dir[1] + dir[0] * dir[0]));
}
