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

#include "UtoEphemerisStars.hpp"

#include <iostream>
#include <stdio.h>

#include <UtCalendar.hpp>
#include <UtMath.hpp>
#include <UtSolarSystem.hpp>
#include <UtSun.hpp>
#include <osg/PolygonMode>
#include <osg/Texture2D>
#include <osgDB/ReadFile>

#include "UtPathParser.hpp"

float UtoEphemerisStars::mColor[21][3] = {{0.62f, 0.71f, 1.0f},  // O5
                                          {0.64f, 0.73f, 1.0f},  // B1
                                          {0.65f, 0.74f, 1.0f},  // B3
                                          {0.67f, 0.75f, 1.0f},  // B5
                                          {0.69f, 0.76f, 1.0f},  // B8
                                          {0.73f, 0.80f, 1.0f},  // A1
                                          {0.75f, 0.82f, 1.0f},  // A3
                                          {0.79f, 0.85f, 1.0f},  // A5
                                          {0.89f, 0.91f, 1.0f},  // F0
                                          {0.93f, 0.93f, 1.0f},  // F2
                                          {0.98f, 0.97f, 1.0f},  // F5
                                          {1.0f, 0.98f, 0.98f},  // F8
                                          {1.0f, 0.96f, 0.93f},  // G2
                                          {1.0f, 0.96f, 0.91f},  // G5
                                          {1.0f, 0.95f, 0.87f},  // G8
                                          {1.0f, 0.92f, 0.82f},  // K0
                                          {1.0f, 0.84f, 0.68f},  // K4
                                          {1.0f, 0.78f, 0.56f},  // K7
                                          {1.0f, 0.75f, 0.50f},  // M2
                                          {1.0f, 0.73f, 0.48f},  // M4
                                          {1.0f, 0.73f, 0.48f}}; // M6

UtoEphemerisStars::UtoEphemerisStars()
   : mNumberStars(0)
   , mListIdDirty(true)
   , mYear(1950)
   , mMonth(1)
   , mDay(1)
   , mHour(0)
   , mDraw(true)
   , mUseStarTexture(false)
   , mSolarSystem()
   , mSolarSystemListIdDirty(true)
   , mTime()
{
   mEpoch.SetDate(mYear, mMonth, mDay);
   mEpoch.SetTime(mHour, 0, 0);

   mListLoaded            = false;
   mSolarSystemListLoaded = false;
   mStarCatalog.clear();

   setSupportsDisplayList(false);
   osg::StateSet* stateSet = getOrCreateStateSet();
   stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
   stateSet->setRenderBinDetails(-5, "RenderBin");
   stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
   stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
   stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
   //   stateSet->setMode(GL_POINT_SMOOTH, osg::StateAttribute::ON);
   setStateSet(stateSet);

   mPlanetsIndex[0]   = UtSolarSystem::Planet::cMERCURY;
   mPlanetColor[0][0] = 0.5f;
   mPlanetColor[0][1] = 0.5f;
   mPlanetColor[0][2] = 0.5f;
   mPlanetsIndex[1]   = UtSolarSystem::Planet::cVENUS;
   mPlanetColor[1][0] = 1.0f;
   mPlanetColor[1][1] = 1.0f;
   mPlanetColor[1][2] = 1.0f;
   mPlanetsIndex[2]   = UtSolarSystem::Planet::cMARS;
   mPlanetColor[2][0] = 1.0f;
   mPlanetColor[2][1] = 0.5f;
   mPlanetColor[2][2] = 0.5f;
   mPlanetsIndex[3]   = UtSolarSystem::Planet::cJUPITER;
   mPlanetColor[3][0] = 1.0f;
   mPlanetColor[3][1] = 0.8f;
   mPlanetColor[3][2] = 0.3f;
   mPlanetsIndex[4]   = UtSolarSystem::Planet::cSATURN;
   mPlanetColor[4][0] = 0.8f;
   mPlanetColor[4][1] = 0.8f;
   mPlanetColor[4][2] = 0.5f;
}

UtoEphemerisStars::UtoEphemerisStars(const char* aFilename)
   : mNumberStars(0)
   , mYear(1950)
   , mMonth(1)
   , mDay(1)
   , mHour(0)
   , mDraw(true)
   , mUseStarTexture(false)
   , mSolarSystem()
   , mSolarSystemListIdDirty(true)
   , mTime()
{
   mEpoch.SetDate(mYear, mMonth, mDay);
   mEpoch.SetTime(mHour, 0, 0);

   mListLoaded            = false;
   mSolarSystemListLoaded = false;
   int i;

   mStarCatalog.clear();

   setSupportsDisplayList(false);
   osg::StateSet* stateSet = getOrCreateStateSet();
   stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
   stateSet->setRenderBinDetails(-5, "RenderBin");
   stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
   stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
   stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
   stateSet->setMode(GL_POINT_SMOOTH, osg::StateAttribute::ON);

   std::string filenamestr(aFilename);
   std::size_t lst = filenamestr.find_last_of('/');
   filenamestr.resize(lst + 1);
   filenamestr += "starmap.tif";
   if (UtPathParser::FileExists(filenamestr))
   {
      mUseStarTexture           = true;
      osg::Texture2D* mytexture = new osg::Texture2D;
      mytexture->setImage(osgDB::readImageFile(filenamestr));

      mytexture->setWrap(osg::Texture::WRAP_R, osg::Texture::REPEAT);
      mytexture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
      mytexture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
      mytexture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
      mytexture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
      stateSet->setTextureAttributeAndModes(0, mytexture, osg::StateAttribute::OFF);
   }

   setStateSet(stateSet);
   std::ifstream catalog(aFilename, std::ios::in | std::ios::binary);
   if (catalog)
   {
      ReadHeader(catalog);
      for (i = 0; i < mNumberStars; ++i)
      {
         ReadEntry(catalog);
      }
   }

   mPlanetsIndex[0]   = UtSolarSystem::Planet::cMERCURY;
   mPlanetColor[0][0] = 0.5f;
   mPlanetColor[0][1] = 0.5f;
   mPlanetColor[0][2] = 0.5f;
   mPlanetsIndex[1]   = UtSolarSystem::Planet::cVENUS;
   mPlanetColor[1][0] = 1.0f;
   mPlanetColor[1][1] = 1.0f;
   mPlanetColor[1][2] = 1.0f;
   mPlanetsIndex[2]   = UtSolarSystem::Planet::cMARS;
   mPlanetColor[2][0] = 1.0f;
   mPlanetColor[2][1] = 0.5f;
   mPlanetColor[2][2] = 0.5f;
   mPlanetsIndex[3]   = UtSolarSystem::Planet::cJUPITER;
   mPlanetColor[3][0] = 1.0f;
   mPlanetColor[3][1] = 0.8f;
   mPlanetColor[3][2] = 0.3f;
   mPlanetsIndex[4]   = UtSolarSystem::Planet::cSATURN;
   mPlanetColor[4][0] = 0.8f;
   mPlanetColor[4][1] = 0.8f;
   mPlanetColor[4][2] = 0.5f;

   BuildDisplayList(mEpoch);
}

UtoEphemerisStars::UtoEphemerisStars(const UtoEphemerisStars& drawer, const osg::CopyOp& copyop)
   : osg::Drawable(drawer, copyop)
   , mStarCatalog(drawer.mStarCatalog)
   , mNumberStars(drawer.mNumberStars)
   , mYear(1950)
   , mMonth(1)
   , mDay(1)
   , mHour(0)
   , mDraw(true)
   , mUseStarTexture(false)
   , mSolarSystem()
   , mSolarSystemListIdDirty(true)
   , mTime()
{
   mEpoch.SetDate(mYear, mMonth, mDay);
   mEpoch.SetTime(mHour, 0, 0);
   mListLoaded            = false;
   mSolarSystemListLoaded = false;
   setSupportsDisplayList(false);
   osg::StateSet* stateSet = getOrCreateStateSet();
   stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
   stateSet->setRenderBinDetails(-2, "RenderBin");
   stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
   stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
   stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
   setStateSet(stateSet);

   mPlanetsIndex[0]   = UtSolarSystem::Planet::cMERCURY;
   mPlanetColor[0][0] = 0.5f;
   mPlanetColor[0][1] = 0.5f;
   mPlanetColor[0][2] = 0.5f;
   mPlanetsIndex[1]   = UtSolarSystem::Planet::cVENUS;
   mPlanetColor[1][0] = 1.0f;
   mPlanetColor[1][1] = 1.0f;
   mPlanetColor[1][2] = 1.0f;
   mPlanetsIndex[2]   = UtSolarSystem::Planet::cMARS;
   mPlanetColor[2][0] = 1.0f;
   mPlanetColor[2][1] = 0.5f;
   mPlanetColor[2][2] = 0.5f;
   mPlanetsIndex[3]   = UtSolarSystem::Planet::cJUPITER;
   mPlanetColor[3][0] = 1.0f;
   mPlanetColor[3][1] = 0.8f;
   mPlanetColor[3][2] = 0.3f;
   mPlanetsIndex[4]   = UtSolarSystem::Planet::cSATURN;
   mPlanetColor[4][0] = 0.8f;
   mPlanetColor[4][1] = 0.8f;
   mPlanetColor[4][2] = 0.5f;
}

// virtual
UtoEphemerisStars::~UtoEphemerisStars()
{
   if (mListLoaded)
   {
      glDeleteLists(mListId, 1);
   }
   if (mSolarSystemListLoaded)
   {
      glDeleteLists(mSolarSystemListId, 1);
   }
}

// virtual
void UtoEphemerisStars::drawImplementation(osg::RenderInfo& aRenderInfo) const
{
   if (!mDraw)
   {
      return;
   }

   if (mListIdDirty)
   {
      BuildDisplayList(mTime);
   }
   if (mSolarSystemListIdDirty)
   {
      BuildSolarSystemDisplayList(mTime);
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

   osg::Matrix starMat = osg::Matrixd::scale(fard, fard, fard) *     // put the stars pretty far out
                         osg::Matrixd::rotate(-mAz, 0.0, 0.0, 1.0) * // azimuth
                         mv;

   glLoadMatrix(starMat.ptr());

   if (mListLoaded)
   {
      glCallList(mListId);
   }

   if (mSolarSystemListLoaded)
   {
      glCallList(mSolarSystemListId);
   }

   glPopMatrix();
}

void UtoEphemerisStars::ReadHeader(std::istream& aInput)
{
   long star0;
   long star1;
   long starn;
   long stnum;
   long mprop;
   long nmag;
   long nbent;

   RevRead(&star0, 4, aInput);
   RevRead(&star1, 4, aInput);
   RevRead(&starn, 4, aInput);
   mNumberStars = starn;
   RevRead(&stnum, 4, aInput);
   RevRead(&mprop, 4, aInput);
   RevRead(&nmag, 4, aInput);
   RevRead(&nbent, 4, aInput);
}

void UtoEphemerisStars::ReadEntry(std::istream& aInput)
{
   float  num;
   double ra;
   double dec;
   char   spectralType[2];
   short  vMagnitude;
   float  raMotion;
   float  decMotion;

   RevRead(&num, 4, aInput);
   RevRead(&ra, 8, aInput);
   RevRead(&dec, 8, aInput);
   aInput.read(spectralType, 2);
   RevRead(&vMagnitude, 2, aInput);
   RevRead(&raMotion, 4, aInput);
   RevRead(&decMotion, 4, aInput);

   StarPoint sp;
   sp.mRA               = ra;
   sp.mDec              = dec;
   sp.mRA_ProperMotion  = raMotion;
   sp.mDec_ProperMotion = decMotion;
   sp.mNum              = static_cast<int>(num);

   float* spectrumColor;
   switch (spectralType[0])
   {
   case ('O'):
      spectrumColor = mColor[0];
      break;
   case ('B'):
      if (spectralType[1] <= '1') // 0, 1
      {
         spectrumColor = mColor[1];
      }
      else if (spectralType[1] >= '8') // 8+
      {
         spectrumColor = mColor[4];
      }
      else if (spectralType[1] >= '5') // 5, 6, 7
      {
         spectrumColor = mColor[3];
      }
      else // 2, 3, 4
      {
         spectrumColor = mColor[2];
      }
      break;
   case ('A'):
      if (spectralType[1] <= '1') // 0, 1
      {
         spectrumColor = mColor[5];
      }
      else if (spectralType[1] >= '5') // 5+
      {
         spectrumColor = mColor[7];
      }
      else // 2, 3, 4
      {
         spectrumColor = mColor[6];
      }
      break;
   case ('F'):
      if (spectralType[1] <= '0') // 0, 1
      {
         spectrumColor = mColor[8];
      }
      else if (spectralType[1] >= '8') // 8+
      {
         spectrumColor = mColor[11];
      }
      else if (spectralType[1] >= '5') // 5, 6, 7
      {
         spectrumColor = mColor[10];
      }
      else // 2, 3, 4
      {
         spectrumColor = mColor[9];
      }
      break;
   case ('G'):
      if (spectralType[1] <= '2') // 0, 1, 2
      {
         spectrumColor = mColor[12];
      }
      else if (spectralType[1] >= '8') // 8+
      {
         spectrumColor = mColor[14];
      }
      else // 3, 4, 5, 6, 7
      {
         spectrumColor = mColor[13];
      }
      break;
   case ('K'):
      if (spectralType[1] <= '0') // 0
      {
         spectrumColor = mColor[15];
      }
      else if (spectralType[1] >= '7') // 7+
      {
         spectrumColor = mColor[17];
      }
      else // 1, 2, 3, 4, 5, 6
      {
         spectrumColor = mColor[16];
      }
      break;
   case ('M'):
      if (spectralType[1] <= '2') // 0, 1, 2
      {
         spectrumColor = mColor[18];
      }
      else if (spectralType[1] >= '6') // 6+
      {
         spectrumColor = mColor[20];
      }
      else // 3, 4, 5
      {
         spectrumColor = mColor[19];
      }
      break;
   default:
      spectrumColor = mColor[0];
      break;
   }

   sp.mColor[0] = 1.0f * spectrumColor[0];
   sp.mColor[1] = 1.0f * spectrumColor[1];
   sp.mColor[2] = 1.0f * spectrumColor[2];
   //   sp.mColor[3] = pow(0.4, vMagnitude / 100.0); // this is how it should work
   sp.mColor[3] = std::pow(0.6, vMagnitude / 100.0) * 8; // this is what I do to make things less dim.

   mStarCatalog.push_back(sp);
}

template<class T>
void UtoEphemerisStars::RevRead(T* aBuf, int aSize, std::istream& aInput)
{
   for (int i = 0; i < aSize; ++i)
   {
      aInput.read(&(reinterpret_cast<char*>(aBuf)[aSize - 1 - i]), 1);
   }
}

void UtoEphemerisStars::SetTime(const UtCalendar& aTime)
{
   mTime = aTime;
   if (mDraw)
   {
      int hour  = aTime.GetHour();
      int day   = aTime.GetDay();
      int month = aTime.GetMonth();
      int year  = aTime.GetYear();
      if ((hour != mHour) || (day != mDay) || (month != mMonth) || (year != mYear) || (!mListLoaded))
      {
         mHour                   = hour;
         mDay                    = day;
         mMonth                  = month;
         mSolarSystemListIdDirty = true;
         if ((year != mYear) || (!mListLoaded))
         {
            mListIdDirty = true;
            mYear        = year;
         }
      }

      mAz = aTime.GetGMST() / 3600.0 * 15.0 * UtMath::cRAD_PER_DEG; // + UtMath::cPI;
   }
}

void UtoEphemerisStars::BuildSolarSystemDisplayList(const UtCalendar& aTime) const
{
   mSolarSystemListIdDirty = false;

   float  cosdec;
   double ra;
   double dec;
   int    i;

   if (mSolarSystemListLoaded)
   {
      glDeleteLists(mSolarSystemListId, 1);
   }

   mSolarSystemListId = glGenLists(1);
   if (0 == mSolarSystemListId)
   {
      mSolarSystemListLoaded = false;
      return;
   }
   glNewList(mSolarSystemListId, GL_COMPILE);
   glPointSize(5);
   glBegin(GL_POINTS);
   for (i = 0; i < 5; ++i)
   {
      mSolarSystem.GetLocationRA_Dec(UtSolarSystem::GetPlanetName(mPlanetsIndex[i]), aTime, ra, dec);

      cosdec = cos(dec);
      glColor3fv(mPlanetColor[i]);
      glVertex3f(cos(ra) * cosdec, sin(ra) * cosdec, sin(dec));
   }
   glEnd();
   glEndList();
   mSolarSystemListLoaded = true;
}

void UtoEphemerisStars::BuildDisplayList(const UtCalendar& aTime) const
{
   mListIdDirty = false;

   if (mListLoaded)
   {
      glDeleteLists(mListId, 1);
   }

   std::vector<StarPoint>::const_iterator it;
   mListId = glGenLists(1);
   if (0 == mListId)
   {
      mListLoaded = false;
      return;
   }
   glNewList(mListId, GL_COMPILE);
   int         yearsSinceEpoch = mYear - mEpoch.GetYear();
   double      precessionRate  = 0.00024386; // 50.3 arcseconds a year in radians
   double      precessionAngle = precessionRate * yearsSinceEpoch;
   osg::Matrix epochToNow      = osg::Matrixd::rotate(-UtSun::GetObliquity(mEpoch), 1.0, 0.0, 0.0) *
                            osg::Matrixd::rotate(-precessionAngle, 0.0, 0.0, 0.1) *
                            osg::Matrixd::rotate(UtSun::GetObliquity(aTime), 1.0, 0.0, 0.0);

   if (mUseStarTexture)
   {
      glEnable(GL_TEXTURE_2D);
      glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
      for (int j = -9; j < 9; ++j)
      {
         glBegin(GL_TRIANGLE_STRIP);
         for (int i = 0; i <= 360; i += 10)
         {
            float     ti     = 540.0f - i;
            double    ra     = i * UtMath::cRAD_PER_DEG;
            double    dec    = UtMath::cPI_OVER_2 * j / 9.0;
            double    cosdec = cos(dec);
            osg::Vec3 epochCelestialPositionWithProperMotion(cos(ra) * cosdec, sin(ra) * cosdec, sin(dec));
            osg::Vec3 celestialPosition = epochToNow * epochCelestialPositionWithProperMotion;
            glTexCoord2f(ti / 360.0f, 1.0f - ((j + 9.0f) / 18.0f));
            glVertex3f(celestialPosition[0], celestialPosition[1], celestialPosition[2]);
            ra     = i * UtMath::cRAD_PER_DEG;
            dec    = UtMath::cPI_OVER_2 * (j + 1) / 9.0;
            cosdec = cos(dec);
            osg::Vec3 epochCelestialPositionWithProperMotion2(cos(ra) * cosdec, sin(ra) * cosdec, sin(dec));
            osg::Vec3 celestialPosition2 = epochToNow * epochCelestialPositionWithProperMotion2;
            glTexCoord2f(ti / 360.0f, 1.0f - ((j + 1 + 9.0f) / 18.0f));
            glVertex3f(celestialPosition2[0], celestialPosition2[1], celestialPosition2[2]);
         }
         glEnd();
      }
      glDisable(GL_TEXTURE_2D);
   }
   else
   {
      glPointSize(1);
      glBegin(GL_POINTS);
      for (it = mStarCatalog.begin(); it != mStarCatalog.end(); ++it)
      {
         double ra  = it->mRA + it->mRA_ProperMotion * yearsSinceEpoch;
         double dec = it->mDec + it->mDec_ProperMotion * yearsSinceEpoch;

         double    cosdec = cos(dec);
         osg::Vec3 epochCelestialPositionWithProperMotion(cos(ra) * cosdec, sin(ra) * cosdec, sin(dec));

         osg::Vec3 celestialPosition = epochToNow * epochCelestialPositionWithProperMotion;

         glColor4fv(it->mColor);

         glVertex3f(celestialPosition[0], celestialPosition[1], celestialPosition[2]);
      }
      glEnd();
   }
   glEndList();
   mListLoaded = true;
}
