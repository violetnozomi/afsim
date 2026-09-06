// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017-2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "GeometryGLWidget.hpp"

#include <iomanip>
#include <sstream>

#include <QApplication>
#include <QMouseEvent>
#include <QSettings>
#include <QTreeWidgetItem>

#include "AeroBody.hpp"
#include "AeroDish.hpp"
#include "AeroLandingGear.hpp"
#include "AeroNacelle.hpp"
#include "AeroObject.hpp"
#include "AeroSpeedBrake.hpp"
#include "AeroSurface.hpp"
#include "AeroWing.hpp"
#include "GeometryBody.hpp"
#include "GeometryDish.hpp"
#include "GeometryEngine.hpp"
#include "GeometryFuselage.hpp"
#include "GeometryLandingGear.hpp"
#include "GeometryMassProperties.hpp"
#include "GeometryNacelle.hpp"
#include "GeometryPropulsionData.hpp"
#include "GeometrySpeedBrake.hpp"
#include "GeometrySurface.hpp"
#include "GeometryWing.hpp"
#include "GraphicalPointCollection.hpp"
#include "MoverCreatorMainWindow.hpp"
#include "MoverCreatorWidget.hpp"
#include "P6DofUtils.hpp"
#include "Vehicle.hpp"
#include "VehicleGeometry.hpp"
#include "UtMath.hpp"
#include "UtQtTextureFont.hpp"

namespace Designer
{

std::array<double, 7> GeometryGLWidget::mAirfoilPtsX = { 0.0, 0.05, 0.150, 0.25, 0.500, 0.750, 1.0 };
std::array<double, 7> GeometryGLWidget::mAirfoilPtsY = { 0.0, 0.50, 0.875, 1.00, 0.875, 0.625, 0.0 };
std::array<double, 5> GeometryGLWidget::mOgivePtsX   = { 0.0, 0.4375, 0.6875, 0.875, 1.0 };
std::array<double, 5> GeometryGLWidget::mOgivePtsY   = { 1.0, 0.7500, 0.5000, 0.250, 0.0 };

void MaterialColorData::SetProperties(GLfloat aBaseAmbient[4],
                                      GLfloat aBaseDiffuse[4],
                                      GLfloat aBaseSpecular[4],
                                      GLfloat aCurrentAmbient[4],
                                      GLfloat aCurrentDiffuse[4],
                                      GLfloat aCurrentSpecular[4],
                                      GLfloat aShininess,
                                      GLfloat aBrightnessFactor)
{
   for (int i = 0; i < 4; ++i)
   {
      mBaseAmbient[i] = aBaseAmbient[i];
      mBaseDiffuse[i] = aBaseDiffuse[i];
      mBaseSpecular[i] = aBaseSpecular[i];
      mCurrentAmbient[i] = aCurrentAmbient[i];
      mCurrentDiffuse[i] = aCurrentDiffuse[i];
      mCurrentSpecular[i] = aCurrentSpecular[i];
   }

   mShininess        = aShininess;
   mBrightnessFactor = aBrightnessFactor;
}

void MaterialColorData::GetProperties(GLfloat  aBaseAmbient[4],
                                      GLfloat  aBaseDiffuse[4],
                                      GLfloat  aBaseSpecular[4],
                                      GLfloat  aCurrentAmbient[4],
                                      GLfloat  aCurrentDiffuse[4],
                                      GLfloat  aCurrentSpecular[4],
                                      GLfloat& aShininess,
                                      GLfloat& aBrightnessFactor) const
{
   for (int i = 0; i < 4; ++i)
   {
      aBaseAmbient[i] = mBaseAmbient[i];
      aBaseDiffuse[i] = mBaseDiffuse[i];
      aBaseSpecular[i] = mBaseSpecular[i];
      aCurrentAmbient[i] = mCurrentAmbient[i];
      aCurrentDiffuse[i] = mCurrentDiffuse[i];
      aCurrentSpecular[i] = mCurrentSpecular[i];
   }

   aShininess        = mShininess;
   aBrightnessFactor = mBrightnessFactor;
}

GeometryGLWidget::GeometryGLWidget(QWidget* aParent)
   : UtQtGLWidgetBase(aParent)
   , mEyePt(0.0, 0.0, 0.0)
   , mLightVec(0.0, 0.0, 0.0)
   , mViewingFocusPoint(0.0, 0.0, 0.0)
   , mObjectCenter(0.0, 0.0, 0.0)
{
   SetNominalMaterialProperties();
   ReCalcEyePoint();
   ReCalcLightGeometry();
   SetBackgroundColor(16, 16, 32);
}

GeometryGLWidget::~GeometryGLWidget()
{
   if (mSaveOptions)
   {
      SaveToggleSettings();
   }

   if (mTextFont != nullptr)
   {
      delete mTextFont;
   }

   while (!mMaterialColorStack.empty())
   {
      mMaterialColorStack.pop();
   }
}

void GeometryGLWidget::initializeGL()
{
   UtQtGLWidgetBase::initializeGL();

   mTextFont = new TextureFont(":/images/256_32x16_Font.tga");
}

void GeometryGLWidget::Update()
{
   UtQtGLWidgetBase::Update();
}

void GeometryGLWidget::mousePressEvent(QMouseEvent* event)
{
   mMouseX = event->x();
   mMouseY = event->y();

   Qt::MouseButton button = event->button();

   if (button & Qt::MouseButton::LeftButton)
   {
      mMouseDown = true;
   }
   else if (button & Qt::MouseButton::RightButton)
   {
      mRgtMouseDown    = true;
      mShowLightVector = true;

      MoverCreatorMainWindow& mainWindow = MoverCreatorMainWindow::GetInstance();
      MoverCreatorWidget* mcWidget = mainWindow.GetMoverCreatorWidget();
      mcWidget->ClearSelections();
   }
   else if (button & Qt::MouseButton::MiddleButton)
   {
      mWheelDown = true;
   }
}

void GeometryGLWidget::mouseReleaseEvent(QMouseEvent* event)
{
   Qt::MouseButton button = event->button();

   if (button & Qt::MouseButton::LeftButton)
   {
      mMouseDown = false;

      Update();
   }
   else if (button & Qt::MouseButton::RightButton)
   {
      mRgtMouseDown    = false;
      mShowLightVector = false;

      Update();
   }
   else if (button & Qt::MouseButton::MiddleButton)
   {
      mWheelDown = false;

      Update();
   }
}

void GeometryGLWidget::ReCalcEyePoint()
{
   if (mViewAz_deg < 0.0) { mViewAz_deg += 360.0; }
   if (mViewAz_deg > 360.0) { mViewAz_deg -= 360.0; }

   if (mViewEl_deg < -89.0) { mViewEl_deg = -89.0; }
   if (mViewEl_deg > 89.0) { mViewEl_deg = 89.0; }

   mEyePt.Set(mEyeDist * cos(mViewEl_deg / 57.3)*sin(mViewAz_deg / 57.3),
              mEyeDist * sin(mViewEl_deg / 57.3),
              mEyeDist * cos(mViewEl_deg / 57.3)*cos(mViewAz_deg / 57.3));

   mEyePt = mEyePt + mObjectCenter;

   mEyePt += mViewingFocusPoint;
}

void GeometryGLWidget::ReCalcLightGeometry()
{
   if (mLightAz_deg < 0.0) { mLightAz_deg += 360.0; }
   if (mLightAz_deg > 360.0) { mLightAz_deg -= 360.0; }

   if (mLightEl_deg < -89.0) { mLightEl_deg = -89.0; }
   if (mLightEl_deg > 89.0) { mLightEl_deg = 89.0; }

   mLightVec.Set(cos(mLightEl_deg / 57.3)*sin(mLightAz_deg / 57.3),
                 sin(mLightEl_deg / 57.3),
                 cos(mLightEl_deg / 57.3)*cos(mLightAz_deg / 57.3));
}

void GeometryGLWidget::mouseMoveEvent(QMouseEvent* event)
{
   if (mMouseDown || mWheelDown || mRgtMouseDown)
   {
      int mx = event->x();
      int my = event->y();

      int deltaX = mx - mMouseX;
      int deltaY = my - mMouseY;

      double fractionX = (double)deltaX / (double)width();
      double fractionY = (double)deltaY / (double)height();

      mMouseX = mx;
      mMouseY = my;

      if (mMouseDown)
      {
         mViewAz_deg -= fractionX * 120.0;
         if (mViewAz_deg < 0.0) { mViewAz_deg += 360.0; }
         if (mViewAz_deg > 360.0) { mViewAz_deg -= 360.0; }

         mViewEl_deg += fractionY * 120.0;
         if (mViewEl_deg < -89.0) { mViewEl_deg = -89.0; }
         if (mViewEl_deg > 89.0) { mViewEl_deg = 89.0; }

         ReCalcEyePoint();
      }
      else if (mWheelDown)
      {
         double sizeFactor = (mMinExtent + 2.0 * mExtentX + mExtentY + mExtentZ) / 5.0;
         double delta = -25.0 * fractionY * (sizeFactor * 0.5);
         AdjustEyeDataZoomInOut(delta);
      }
      else if (mRgtMouseDown)
      {
         mLightAz_deg += fractionX * 120.0;
         if (mLightAz_deg < 0.0) { mLightAz_deg += 360.0; }
         if (mLightAz_deg > 360.0) { mLightAz_deg -= 360.0; }

         mLightEl_deg -= fractionY * 120.0;
         if (mLightEl_deg < -89.0) { mLightEl_deg = -89.0; }
         if (mLightEl_deg > 89.0) { mLightEl_deg = 89.0; }

         ReCalcLightGeometry();
      }

      Update();
   }
}

void GeometryGLWidget::wheelEvent(QWheelEvent* event)
{
   // Negative is back towards user
   QPoint numDegrees = event->angleDelta() / 8;
   int numTicks = numDegrees.y() / 15;

   if (!numDegrees.isNull())
   {
      double sizeFactor = (mMinExtent + 2.0 * mExtentX + mExtentY + mExtentZ) / 5.0;
      double delta = (double)numTicks * (sizeFactor * 0.5);
      AdjustEyeDataZoomInOut(delta);

      Update();
   }
}

void GeometryGLWidget::CalcViewRegionCharacteristics()
{
   CalcVehicleExtents();

   MoverCreatorMainWindow& mainWindow = MoverCreatorMainWindow::GetInstance();
   Vehicle* vehicle = mainWindow.GetMoverCreatorWidget()->GetVehicle();
   VehicleGeometry& vehicleGeometry = vehicle->GetGeometry();
   GeometryBody* body = vehicleGeometry.GetBody();
   GeometryFuselage* fuselage = vehicleGeometry.GetFuselage();

   // Find the max extent length in any direction
   double maxLength = mExtentX;
   if (maxLength < mExtentY) { maxLength = mExtentY; }
   if (maxLength < mExtentZ) { maxLength = mExtentZ; }

   // Set nominal arrow diameter
   mArrowDiameter = maxLength * 0.03;

   // Adjust arrow diam, based on body/fuselage, if present
   if (body != nullptr)
   {
      double averageDiam = (body->GetWidth_ft() + body->GetHeight_ft()) * 0.5;
      mArrowDiameter = averageDiam * 0.2;
   }
   else if (fuselage != nullptr)
   {
      double averageDiam = (fuselage->GetWidth_ft() + fuselage->GetHeight_ft()) * 0.5;
      mArrowDiameter = averageDiam * 0.15;
   }

   // Set arrowhead length, based on diameter
   mArrowheadLength = mArrowDiameter * 2.0;

   // Arrow data is in vehicle coords
   double additionalLengthForArrowhead = 3.0 * mArrowheadLength;
   mArrowX_Length = std::abs(mExtentMaxX) + additionalLengthForArrowhead;
   mArrowY_Length = std::abs(mExtentMaxY) + additionalLengthForArrowhead;
   mArrowZ_Length = std::abs(mExtentMaxZ) + additionalLengthForArrowhead;

   // Set wall distance based on the larger extent of x or y
   double maxWall = mExtentX;
   if (mExtentY > maxWall) { maxWall = mExtentY; }

   // Region walls (vehicle x and y maps to region x and z)
   mRegionX = (maxWall + additionalLengthForArrowhead) * 2.0;
   mRegionZ = (maxWall + additionalLengthForArrowhead) * 2.0;

   // Calc extent-based floor and ceiling
   mRegionCeilingY = (std::abs(mExtentZ) + additionalLengthForArrowhead) * 2.0;
   mRegionFloorY = -(std::abs(mExtentMaxZ) + additionalLengthForArrowhead * 1.5);

   // Re-adjust ceiling to match walls, if appropriate
   if (std::abs(mExtentZ) < maxWall)
   {
      mRegionCeilingY = (maxWall + additionalLengthForArrowhead) * 2.0;
   }

   // Re-adjust floor and z-arrow length if landing gear are present
   if (LandingGearArePresent())
   {
      mRegionFloorY = -mExtentMaxZ;
      mArrowZ_Length = mExtentMaxZ * 0.95;
   }

   int intLength = (int)maxLength;
   mRegionStepSize = intLength / 10;

   if (mRegionStepSize < 1)
   {
      mRegionStepSize = 1;
   }
}

void GeometryGLWidget::CalcVehicleExtents()
{
   MoverCreatorMainWindow& mainWindow = MoverCreatorMainWindow::GetInstance();
   Vehicle* vehicle = mainWindow.GetMoverCreatorWidget()->GetVehicle();
   VehicleGeometry& vehicleGeometry = vehicle->GetGeometry();

   double totalExtentMinX = 0.0;
   double totalExtentMaxX = 0.0;
   double totalExtentMinY = 0.0;
   double totalExtentMaxY = 0.0;
   double totalExtentMinZ = 0.0;
   double totalExtentMaxZ = 0.0;

   const std::map<size_t, GeometryObject*>& geometryObjMap =
      vehicleGeometry.GetGeometryObjectMap();
   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;

      double extentMinX = 0.0;
      double extentMaxX = 0.0;
      double extentMinY = 0.0;
      double extentMaxY = 0.0;
      double extentMinZ = 0.0;
      double extentMaxZ = 0.0;
      if (geometryObject->CalcExtents(extentMinX, extentMaxX,
                                      extentMinY, extentMaxY,
                                      extentMinZ, extentMaxZ))
      {
         if (extentMinX < totalExtentMinX) { totalExtentMinX = extentMinX; }
         if (extentMaxX > totalExtentMaxX) { totalExtentMaxX = extentMaxX; }
         if (extentMinY < totalExtentMinY) { totalExtentMinY = extentMinY; }
         if (extentMaxY > totalExtentMaxY) { totalExtentMaxY = extentMaxY; }
         if (extentMinZ < totalExtentMinZ) { totalExtentMinZ = extentMinZ; }
         if (extentMaxZ > totalExtentMaxZ) { totalExtentMaxZ = extentMaxZ; }
      }
   }

   mExtentMinX = totalExtentMinX;
   mExtentMaxX = totalExtentMaxX;
   mExtentMinY = totalExtentMinY;
   mExtentMaxY = totalExtentMaxY;
   mExtentMinZ = totalExtentMinZ;
   mExtentMaxZ = totalExtentMaxZ;

   // Extent data is in vehicle coords

   mExtentX = std::abs(totalExtentMaxX);
   if (mExtentX < std::abs(totalExtentMinX)) { mExtentX = std::abs(totalExtentMinX); }

   mExtentY = std::abs(totalExtentMaxY);
   if (mExtentY < std::abs(totalExtentMinY)) { mExtentY = std::abs(totalExtentMinY); }

   mExtentZ = std::abs(totalExtentMaxZ);
   if (mExtentZ < std::abs(totalExtentMinZ)) { mExtentZ = std::abs(totalExtentMinZ); }
}

bool GeometryGLWidget::LandingGearArePresent()
{
   MoverCreatorMainWindow& mainWindow = MoverCreatorMainWindow::GetInstance();
   Vehicle* vehicle = mainWindow.GetMoverCreatorWidget()->GetVehicle();
   VehicleGeometry& vehicleGeometry = vehicle->GetGeometry();

   const std::map<size_t, GeometryObject*>& geometryObjMap = vehicleGeometry.GetGeometryObjectMap();
   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;

      std::string type = geometryObject->GeometryObjectType();
      if (type == "GeometryLandingGear")
      {
         return true;
      }
   }

   return false;
}

void GeometryGLWidget::AdjustEyeDataZoomInOut(double aFactor)
{
   double tempDist = mEyeDist;
   tempDist -= aFactor;
   AdjustEyeData(tempDist);
}

void GeometryGLWidget::AdjustEyeData(double aDesiredViewingDistance, bool aInitialize)
{
   if (aInitialize)
   {
      mViewAz_deg = mDefaultViewAz_deg;
      mViewEl_deg = mDefaultViewEl_deg;
      mLightAz_deg = mDefaultLightAz_deg;
      mLightEl_deg = mDefaultLightEl_deg;
      ReCalcLightGeometry();

      mViewingFocusPoint.Set(0.0, 0.0, 0.0);
   }

   // Determine the maximum distance from the center of the region to the corner
   double maxDiagonalToCornerFloor = sqrt(mRegionX * mRegionX + mRegionFloorY * mRegionFloorY + mRegionZ * mRegionZ);
   double maxDiagonalToCornerCeiling = sqrt(mRegionX * mRegionX + mRegionCeilingY * mRegionCeilingY + mRegionZ * mRegionZ);
   mMaxDiagonalToCorner = maxDiagonalToCornerFloor;
   if (maxDiagonalToCornerCeiling > maxDiagonalToCornerFloor)
   {
      mMaxDiagonalToCorner = maxDiagonalToCornerCeiling;
   }

   // Determine the minimum extent
   mMinExtent = mExtentX;
   if (mExtentY < mMinExtent)
   {
      mMinExtent = mExtentY;
   }
   if (mExtentZ < mMinExtent)
   {
      mMinExtent = mExtentZ;
   }

   // Determine the maximum extent
   mMaxExtent = mExtentX;
   if (mExtentY > mMaxExtent)
   {
      mMaxExtent = mExtentY;
   }
   if (mExtentZ > mMaxExtent)
   {
      mMaxExtent = mExtentZ;
   }

   // Determine the horizontal maximum extent
   double horizontalMaxExtent = mExtentX;
   if (mExtentY > horizontalMaxExtent)
   {
      horizontalMaxExtent = mExtentY;
   }

   double halfWidth = (double)width() * 0.5;
   double halfHeight = (double)height() * 0.5;
   double horizontalFovFactor = halfWidth / halfHeight;

   // Now, determine the distance to see the full max extent with the current FOV
   double halfFovY_rad = (mFullFovY_deg * 0.5) * UtMath::cRAD_PER_DEG;
   double halfHorizontalFovY_rad = halfFovY_rad * horizontalFovFactor;

   double distForMaxExtentVertical = (mExtentZ * 1.5) / tan(halfFovY_rad);
   double distForMaxExtentHorizontal = (horizontalMaxExtent * 1.5) / tan(halfHorizontalFovY_rad);
   mDistForMaxExtent = distForMaxExtentVertical;
   if (distForMaxExtentHorizontal > mDistForMaxExtent)
   {
      mDistForMaxExtent = distForMaxExtentHorizontal;
   }

   // The farthest distance should be the greater of the max extent or mDistForMaxExtent
   double farthestDist = mMaxExtent;
   if (farthestDist < mDistForMaxExtent)
   {
      farthestDist = mDistForMaxExtent;
   }

   // Double the value to provide a "cushion"
   farthestDist *= 2.0;

   // Determine the minimum viewing distance
   mMinViewDist = mMinExtent;

   // Determine the maximum viewing distance
   mMaxViewDist = farthestDist;

   // Limit desired view distance to max view distance
   if (aDesiredViewingDistance > mMaxViewDist)
   {
      aDesiredViewingDistance = mMaxViewDist;
   }

   // Limit desired view distance to min view distance
   if (aDesiredViewingDistance < mMinViewDist)
   {
      aDesiredViewingDistance = mMinViewDist;
   }

   // Set the eye distance, limited by max/min (above)
   mEyeDist = aDesiredViewingDistance;

   // if we are initializing, use the mDistForMaxExtent
   if (aInitialize)
   {
      mEyeDist = mDistForMaxExtent;
   }

   ReCalcEyePoint();
}

void GeometryGLWidget::AdjustViewingDistance(bool aInitialize)
{
   CalcViewRegionCharacteristics();
   AdjustEyeData(mEyeDist, aInitialize);
}

void GeometryGLWidget::CalcClippingPlaneDistances()
{
   // Determine near clipping plane distance
   mNearDist = 0.9 * (mEyeDist - mMaxExtent);

   // The farthest near distance should be no more than 0.5 feet
   if (mNearDist > 0.5)
   {
      mNearDist = 0.5;
   }

   // Do not get closer than 0.01 feet
   if (mNearDist < 0.01)
   {
      mNearDist = 0.01;
   }

   // Determine far clipping plane distance
   mFarDist = 1.1 * (mEyeDist + mMaxDiagonalToCorner * 2.0);
}

void GeometryGLWidget::LimitFOV(double& aFov_deg)
{
   if (aFov_deg <  10.0) { aFov_deg = 10.0; }
   if (aFov_deg > 120.0) { aFov_deg = 120.0; }
}

void GeometryGLWidget::SetLightProperties()
{
   // Set light properties
   GLfloat lightPos[4]      = { 1.0f, 1.0f, -1.0f, 0.0f }; // 4th element must be 0 for directional light
   GLfloat lightAmbient[4]  = { 1.0f, 1.0f,  1.0,  1.0f };
   GLfloat lightDiffuse[4]  = { 1.0f, 1.0f,  1.0f, 1.0f };
   GLfloat lightSpecular[4] = { 1.0f, 1.0f,  1.0f, 1.0f };

   lightPos[0] = (float)mLightVec.X();
   lightPos[1] = (float)mLightVec.Y();
   lightPos[2] = (float)mLightVec.Z();

   glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
   glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
   glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
}

void GeometryGLWidget::SetDrawingSelectedMode()
{
   mDrawingSelectedMode = true;
}

void GeometryGLWidget::SetDrawingNonSelectedMode()
{
   mDrawingSelectedMode = false;
}

void GeometryGLWidget::SetDrawingNothingSelectedMode()
{
   mDrawingSelectedMode = true;
}

void GeometryGLWidget::SetNominalMaterialProperties(double aAlpha)
{
   GLfloat alpha = (GLfloat)aAlpha;

   if (!mDrawingSelectedMode)
   {
      alpha *= 0.4f;
   }

   SetAmbientMaterialProperties(0.2f, 0.2f, 0.2f, alpha);
   SetDiffuseMaterialProperties(0.8f, 0.8f, 0.8f, alpha);
   SetSpecularMaterialProperties(0.8f, 0.8f, 0.8f, alpha);

   mShininess        = 100.0f;
   mBrightnessFactor = 1.0f;

   SetMaterialProperties();
}

void GeometryGLWidget::SetHighlightedMaterialProperties(double aAlpha)
{
   GLfloat alpha = (GLfloat)aAlpha;

   SetAmbientMaterialProperties(0.4f, 0.1f, 0.1f, alpha);
   SetDiffuseMaterialProperties(0.9f, 0.4f, 0.4f, alpha);
   SetSpecularMaterialProperties(0.9f, 0.4f, 0.4f, alpha);

   mShininess        = 100.0f;
   mBrightnessFactor = 1.0f;

   SetMaterialProperties();
}

void GeometryGLWidget::SetEngineMaterialProperties(double aAlpha)
{
   GLfloat alpha = (GLfloat)aAlpha;

   if (!mDrawingSelectedMode)
   {
      alpha *= 0.4f;

      SetAmbientMaterialProperties(0.2f, 0.2f, 0.2f, alpha);
      SetDiffuseMaterialProperties(0.8f, 0.8f, 0.8f, alpha);
      SetSpecularMaterialProperties(0.8f, 0.8f, 0.8f, alpha);
   }
   else
   {
      SetAmbientMaterialProperties(0.2f, 0.2f, 0.0f, alpha);
      SetDiffuseMaterialProperties(0.8f, 0.8f, 0.0f, alpha);
      SetSpecularMaterialProperties(0.8f, 0.8f, 0.0f, alpha);
   }

   mShininess        = 100.0f;
   mBrightnessFactor = 1.0f;

   SetMaterialProperties();
}

void GeometryGLWidget::SetRedMaterialProperties()
{
   SetAmbientMaterialProperties(0.2f, 0.0f, 0.0f, 1.0f);
   SetDiffuseMaterialProperties(0.8f, 0.1f, 0.1f, 1.0f);
   SetSpecularMaterialProperties(0.8f, 0.1f, 0.1f, 1.0f);

   mShininess        = 100.0f;
   mBrightnessFactor = 1.0f;

   SetMaterialProperties();
}

void GeometryGLWidget::SetRedOrangeMaterialProperties()
{
   SetAmbientMaterialProperties(0.2f, 0.05f, 0.0f, 1.0f);
   SetDiffuseMaterialProperties(0.8f, 0.2f, 0.0f, 1.0f);
   SetSpecularMaterialProperties(0.8f, 0.2f, 0.0f, 1.0f);

   mShininess        = 100.0f;
   mBrightnessFactor = 1.0f;

   SetMaterialProperties();
}

void GeometryGLWidget::SetOrangeMaterialProperties()
{
   SetAmbientMaterialProperties(0.2f, 0.1f, 0.0f, 1.0f);
   SetDiffuseMaterialProperties(0.8f, 0.4f, 0.0f, 1.0f);
   SetSpecularMaterialProperties(0.8f, 0.4f, 0.0f, 1.0f);

   mShininess        = 100.0f;
   mBrightnessFactor = 1.0f;

   SetMaterialProperties();
}

void GeometryGLWidget::SetYellowOrangeMaterialProperties()
{
   SetAmbientMaterialProperties(0.2f, 0.15f, 0.0f, 1.0f);
   SetDiffuseMaterialProperties(0.8f, 0.6f, 0.0f, 1.0f);
   SetSpecularMaterialProperties(0.8f, 0.6f, 0.0f, 1.0f);

   mShininess        = 100.0f;
   mBrightnessFactor = 1.0f;

   SetMaterialProperties();
}

void GeometryGLWidget::SetYellowMaterialProperties()
{
   SetAmbientMaterialProperties(0.2f, 0.2f, 0.0f, 1.0f);
   SetDiffuseMaterialProperties(0.8f, 0.8f, 0.0f, 1.0f);
   SetSpecularMaterialProperties(0.8f, 0.8f, 0.0f, 1.0f);
   mShininess = 100.0f;
   mBrightnessFactor = 1.0f;
   SetMaterialProperties();
}

void GeometryGLWidget::SetYellowGreenMaterialProperties()
{
   SetAmbientMaterialProperties(0.1f, 0.2f, 0.0f, 1.0f);
   SetDiffuseMaterialProperties(0.4f, 0.8f, 0.0f, 1.0f);
   SetSpecularMaterialProperties(0.4f, 0.8f, 0.0f, 1.0f);

   mShininess        = 100.0f;
   mBrightnessFactor = 1.0f;

   SetMaterialProperties();
}

void GeometryGLWidget::SetGreenMaterialProperties()
{
   SetAmbientMaterialProperties(0.0f, 0.2f, 0.0f, 1.0f);
   SetDiffuseMaterialProperties(0.0f, 0.8f, 0.2f, 1.0f);
   SetSpecularMaterialProperties(0.0f, 0.8f, 0.2f, 1.0f);

   mShininess        = 100.0f;
   mBrightnessFactor = 1.0f;

   SetMaterialProperties();
}

void GeometryGLWidget::SetGreenCyanMaterialProperties()
{
   SetAmbientMaterialProperties(0.0f, 0.2f, 0.1f, 1.0f);
   SetDiffuseMaterialProperties(0.0f, 0.8f, 0.4f, 1.0f);
   SetSpecularMaterialProperties(0.0f, 0.8f, 0.4f, 1.0f);

   mShininess        = 100.0f;
   mBrightnessFactor = 1.0f;

   SetMaterialProperties();
}

void GeometryGLWidget::SetCyanMaterialProperties()
{
   SetAmbientMaterialProperties(0.0f, 0.2f, 0.2f, 1.0f);
   SetDiffuseMaterialProperties(0.0f, 0.8f, 0.8f, 1.0f);
   SetSpecularMaterialProperties(0.0f, 0.8f, 0.8f, 1.0f);

   mShininess        = 100.0f;
   mBrightnessFactor = 1.0f;

   SetMaterialProperties();
}

void GeometryGLWidget::SetBlueCyanMaterialProperties()
{
   SetAmbientMaterialProperties(0.0f, 0.1f, 0.2f, 1.0f);
   SetDiffuseMaterialProperties(0.0f, 0.4f, 0.8f, 1.0f);
   SetSpecularMaterialProperties(0.0f, 0.4f, 0.8f, 1.0f);

   mShininess        = 100.0f;
   mBrightnessFactor = 1.0f;

   SetMaterialProperties();
}
void GeometryGLWidget::SetBlueMaterialProperties()
{
   SetAmbientMaterialProperties(0.0f, 0.0f, 0.2f, 1.0f);
   SetDiffuseMaterialProperties(0.0f, 0.2f, 0.8f, 1.0f);
   SetSpecularMaterialProperties(0.0f, 0.2f, 0.8f, 1.0f);

   mShininess        = 100.0f;
   mBrightnessFactor = 1.0f;

   SetMaterialProperties();
}

void GeometryGLWidget::SetBlueVioletMaterialProperties()
{
   SetAmbientMaterialProperties(0.1f, 0.0f, 0.2f, 1.0f);
   SetDiffuseMaterialProperties(0.4f, 0.0f, 0.8f, 1.0f);
   SetSpecularMaterialProperties(0.4f, 0.0f, 0.8f, 1.0f);

   mShininess        = 100.0f;
   mBrightnessFactor = 1.0f;

   SetMaterialProperties();
}

void GeometryGLWidget::SetVioletMaterialProperties()
{
   SetAmbientMaterialProperties(0.2f, 0.0f, 0.2f, 1.0f);
   SetDiffuseMaterialProperties(0.8f, 0.0f, 0.8f, 1.0f);
   SetSpecularMaterialProperties(0.8f, 0.0f, 0.8f, 1.0f);

   mShininess        = 100.0f;
   mBrightnessFactor = 1.0f;

   SetMaterialProperties();
}

void GeometryGLWidget::SetRedVioletMaterialProperties()
{
   SetAmbientMaterialProperties(0.2f, 0.0f, 0.1f, 1.0f);
   SetDiffuseMaterialProperties(0.8f, 0.0f, 0.4f, 1.0f);
   SetSpecularMaterialProperties(0.8f, 0.0f, 0.4f, 1.0f);

   mShininess        = 100.0f;
   mBrightnessFactor = 1.0f;

   SetMaterialProperties();
}

void GeometryGLWidget::SetDarkGrayMaterialProperties()
{
   SetAmbientMaterialProperties(0.05f, 0.05f, 0.05f, 1.0f);
   SetDiffuseMaterialProperties(0.25f, 0.25f, 0.25f, 1.0f);
   SetSpecularMaterialProperties(0.25f, 0.25f, 0.25f, 1.0f);

   mShininess        = 100.0f;
   mBrightnessFactor = 1.0f;

   SetMaterialProperties();
}

void GeometryGLWidget::SetGrayMaterialProperties()
{
   SetAmbientMaterialProperties(0.1f, 0.1f, 0.1f, 1.0f);
   SetDiffuseMaterialProperties(0.5f, 0.5f, 0.5f, 1.0f);
   SetSpecularMaterialProperties(0.5f, 0.5f, 0.5f, 1.0f);

   mShininess        = 100.0f;
   mBrightnessFactor = 1.0f;

   SetMaterialProperties();
}

void GeometryGLWidget::SetLightGrayMaterialProperties()
{
   SetAmbientMaterialProperties(0.2f, 0.2f, 0.2f, 1.0f);
   SetDiffuseMaterialProperties(0.75f, 0.75f, 0.75f, 1.0f);
   SetSpecularMaterialProperties(0.75f, 0.75f, 0.75f, 1.0f);

   mShininess        = 100.0f;
   mBrightnessFactor = 1.0f;

   SetMaterialProperties();
}

void GeometryGLWidget::SetMaterialProperties()
{
   // Can specify GL_FRONT or GL_FRONT_AND_BACK
   glMaterialfv(GL_FRONT, GL_AMBIENT, mCurrentAmbient);
   glMaterialfv(GL_FRONT, GL_DIFFUSE, mCurrentDiffuse);
   glMaterialfv(GL_FRONT, GL_SPECULAR, mCurrentSpecular);
   glMaterialf(GL_FRONT, GL_SHININESS, mShininess);
}

void GeometryGLWidget::SetAmbientMaterialProperties(GLfloat aRed, GLfloat aGreen, GLfloat aBlue, GLfloat aAlpha)
{
   mBaseAmbient[0]    = aRed;
   mBaseAmbient[1]    = aGreen;
   mBaseAmbient[2]    = aBlue;
   mBaseAmbient[3]    = aAlpha;
   mCurrentAmbient[0] = aRed;
   mCurrentAmbient[1] = aGreen;
   mCurrentAmbient[2] = aBlue;
   mCurrentAmbient[3] = aAlpha;

   EnforceLimitsForProperties(mBaseAmbient);
   EnforceLimitsForProperties(mCurrentAmbient);
}

void GeometryGLWidget::SetDiffuseMaterialProperties(GLfloat aRed, GLfloat aGreen, GLfloat aBlue, GLfloat aAlpha)
{
   mBaseDiffuse[0]    = aRed;
   mBaseDiffuse[1]    = aGreen;
   mBaseDiffuse[2]    = aBlue;
   mBaseDiffuse[3]    = aAlpha;
   mCurrentDiffuse[0] = aRed;
   mCurrentDiffuse[1] = aGreen;
   mCurrentDiffuse[2] = aBlue;
   mCurrentDiffuse[3] = aAlpha;

   EnforceLimitsForProperties(mBaseDiffuse);
   EnforceLimitsForProperties(mCurrentDiffuse);
}

void GeometryGLWidget::SetSpecularMaterialProperties(GLfloat aRed, GLfloat aGreen, GLfloat aBlue, GLfloat aAlpha)
{
   mBaseSpecular[0]    = aRed;
   mBaseSpecular[1]    = aGreen;
   mBaseSpecular[2]    = aBlue;
   mBaseSpecular[3]    = aAlpha;
   mCurrentSpecular[0] = aRed;
   mCurrentSpecular[1] = aGreen;
   mCurrentSpecular[2] = aBlue;
   mCurrentSpecular[3] = aAlpha;

   EnforceLimitsForProperties(mBaseSpecular);
   EnforceLimitsForProperties(mCurrentSpecular);
}

void GeometryGLWidget::EnforceLimitsForProperties(GLfloat aAmbient[4])
{
   if (aAmbient[0] < 0.0f) { aAmbient[0] = 0.0f; }
   if (aAmbient[0] > 1.0f) { aAmbient[0] = 1.0f; }
   if (aAmbient[1] < 0.0f) { aAmbient[1] = 0.0f; }
   if (aAmbient[1] > 1.0f) { aAmbient[1] = 1.0f; }
   if (aAmbient[2] < 0.0f) { aAmbient[2] = 0.0f; }
   if (aAmbient[2] > 1.0f) { aAmbient[2] = 1.0f; }
   if (aAmbient[3] < 0.0f) { aAmbient[3] = 0.0f; }
   if (aAmbient[3] > 1.0f) { aAmbient[3] = 1.0f; }
}

void GeometryGLWidget::DarkenMaterialProperties(GLfloat aFactor)
{
   if (aFactor > 1.0) { return; }

   mBrightnessFactor = aFactor;
   if (mBrightnessFactor < 0.0f) { mBrightnessFactor = 0.0f; }

   mCurrentAmbient[0] = mBrightnessFactor * mBaseAmbient[0];
   mCurrentAmbient[1] = mBrightnessFactor * mBaseAmbient[1];
   mCurrentAmbient[2] = mBrightnessFactor * mBaseAmbient[2];
   mCurrentAmbient[3] = mBaseAmbient[3];
   EnforceLimitsForProperties(mCurrentAmbient);

   mCurrentDiffuse[0] = mBrightnessFactor * mBaseDiffuse[0];
   mCurrentDiffuse[1] = mBrightnessFactor * mBaseDiffuse[1];
   mCurrentDiffuse[2] = mBrightnessFactor * mBaseDiffuse[2];
   mCurrentDiffuse[3] = mBaseDiffuse[3];
   EnforceLimitsForProperties(mCurrentDiffuse);

   mCurrentSpecular[0] = mBrightnessFactor * mBaseSpecular[0];
   mCurrentSpecular[1] = mBrightnessFactor * mBaseSpecular[1];
   mCurrentSpecular[2] = mBrightnessFactor * mBaseSpecular[2];
   mCurrentSpecular[3] = mBaseSpecular[3];
   EnforceLimitsForProperties(mCurrentSpecular);

   SetMaterialProperties();
}

void GeometryGLWidget::LightenMaterialProperties(GLfloat aFactor)
{
   if (aFactor < 1.0) { return; }

   mBrightnessFactor = aFactor;
   if (mBrightnessFactor > 10.0f) { mBrightnessFactor = 10.0f; }

   mCurrentAmbient[0] = mBrightnessFactor * mBaseAmbient[0];
   mCurrentAmbient[1] = mBrightnessFactor * mBaseAmbient[1];
   mCurrentAmbient[2] = mBrightnessFactor * mBaseAmbient[2];
   mCurrentAmbient[3] = mBaseAmbient[3];
   EnforceLimitsForProperties(mCurrentAmbient);

   mCurrentDiffuse[0] = mBrightnessFactor * mBaseDiffuse[0];
   mCurrentDiffuse[1] = mBrightnessFactor * mBaseDiffuse[1];
   mCurrentDiffuse[2] = mBrightnessFactor * mBaseDiffuse[2];
   mCurrentDiffuse[3] = mBaseDiffuse[3];
   EnforceLimitsForProperties(mCurrentDiffuse);

   mCurrentSpecular[0] = mBrightnessFactor * mBaseSpecular[0];
   mCurrentSpecular[1] = mBrightnessFactor * mBaseSpecular[1];
   mCurrentSpecular[2] = mBrightnessFactor * mBaseSpecular[2];
   mCurrentSpecular[3] = mBaseSpecular[3];
   EnforceLimitsForProperties(mCurrentSpecular);

   SetMaterialProperties();
}

void GeometryGLWidget::RestoreMaterialProperties()
{
   mBrightnessFactor = 1.0f;

   mCurrentAmbient[0] = mBaseAmbient[0];
   mCurrentAmbient[1] = mBaseAmbient[1];
   mCurrentAmbient[2] = mBaseAmbient[2];
   mCurrentAmbient[3] = mBaseAmbient[3];
   EnforceLimitsForProperties(mCurrentAmbient);

   mCurrentDiffuse[0] = mBaseDiffuse[0];
   mCurrentDiffuse[1] = mBaseDiffuse[1];
   mCurrentDiffuse[2] = mBaseDiffuse[2];
   mCurrentDiffuse[3] = mBaseDiffuse[3];
   EnforceLimitsForProperties(mCurrentDiffuse);

   mCurrentSpecular[0] = mBaseSpecular[0];
   mCurrentSpecular[1] = mBaseSpecular[1];
   mCurrentSpecular[2] = mBaseSpecular[2];
   mCurrentSpecular[3] = mBaseSpecular[3];
   EnforceLimitsForProperties(mCurrentSpecular);

   SetMaterialProperties();
}

void GeometryGLWidget::PushMaterialProperties()
{
   MaterialColorData materialColor;
   materialColor.SetProperties(mBaseAmbient, mBaseDiffuse, mBaseSpecular,
                               mCurrentAmbient, mCurrentDiffuse, mCurrentSpecular,
                               mShininess, mBrightnessFactor);
   mMaterialColorStack.push(materialColor);
}

void GeometryGLWidget::PopMaterialProperties()
{
   if (mMaterialColorStack.size() > 0)
   {
      MaterialColorData currentColor = mMaterialColorStack.top();
      currentColor.GetProperties(mBaseAmbient, mBaseDiffuse, mBaseSpecular,
                                 mCurrentAmbient, mCurrentDiffuse, mCurrentSpecular,
                                 mShininess, mBrightnessFactor);
      if (mBrightnessFactor > 1.0)
      {
         LightenMaterialProperties(mBrightnessFactor);
      }
      else if (mBrightnessFactor < 1.0)
      {
         DarkenMaterialProperties(mBrightnessFactor);
      }
      else
      {
         SetMaterialProperties();
      }

      mMaterialColorStack.pop(); // Pop the current color
   }
}

void GeometryGLWidget::SetRedArrowLabelColor()
{
   glColor4d(0.8, 0.0, 0.0, 1.0);
}

void GeometryGLWidget::SetGreenArrowLabelColor()
{
   glColor4d(0.0, 0.8, 0.0, 1.0);
}

void GeometryGLWidget::SetBlueArrowLabelColor()
{
   glColor4d(0.0, 0.0, 0.8, 1.0);
}

void GeometryGLWidget::SetNominalWireframeColor()
{
   glColor4d(0.2, 0.7, 1.0, 0.9);
}

void GeometryGLWidget::SetSelectedWireframeColor()
{
   glColor4d(0.9, 0.15, 0.3, 1.0);
}

void GeometryGLWidget::paintGL()
{
   if (mResetView)
   {
      AdjustViewingDistance(true);
      mResetView = false;
   }
   UtQtGLWidgetBase::paintGL();
}

void GeometryGLWidget::Draw()
{
   Draw3dView();
   Draw2dOverlay();
}

void GeometryGLWidget::QtDraw()
{
   // Do nothing
}

void GeometryGLWidget::Draw3dView()
{
   SetBackgroundColor(16, 16, 32);

   double halfWidth  = (double)(width() / 2);
   double halfHeight = (double)(height() / 2);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   int viewWidth = halfWidth;
   int viewHeight = halfHeight;
   double xFactor = (double)viewWidth / (double)viewHeight;

   double tgtHalfFovY_deg = 0.5 * mFullFovY_deg;
   double tgtHalfFovY_rad = tgtHalfFovY_deg / 57.3;

   // Determine the "near" dist for a unity delta with the desired angle
   // L * sin(a) = 1
   // L = 1/sin(a)
   double tempLength = 1.0 / sin(tgtHalfFovY_rad);
   double tempNear = tempLength * cos(tgtHalfFovY_rad);

   // Now, use the relative window sizes to calc the x angle
   double xAngle_rad = atan(xFactor / tempNear);
   double angleY_rad = tgtHalfFovY_rad;
   double angleX_rad = xAngle_rad;

   // Determine the size of the enclosing box region and arrow sizes
   CalcViewRegionCharacteristics();

   CalcClippingPlaneDistances();

   double frustrumY = mNearDist * tan(angleY_rad);
   double frustrumX = mNearDist * tan(angleX_rad);

   glFrustum(-frustrumX, frustrumX, -frustrumY, frustrumY, mNearDist, mFarDist);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   glClear(GL_DEPTH_BUFFER_BIT |
           GL_COLOR_BUFFER_BIT |
           GL_STENCIL_BUFFER_BIT);  // Clear the depth, color, and stencil buffers

   glPushAttrib(GL_COLOR_BUFFER_BIT |
                GL_CURRENT_BIT |
                GL_DEPTH_BUFFER_BIT |
                GL_ENABLE_BIT |
                GL_LIGHTING_BIT |
                GL_POLYGON_BIT |
                GL_TEXTURE_BIT);

   glEnable(GL_ALPHA_TEST);
   glAlphaFunc(GL_GREATER, 0.01f);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   glCullFace(GL_BACK);
   glDepthFunc(GL_LESS);
   glEnable(GL_LINE_SMOOTH);

   // Note: Do not use GL_POLYGON_SMOOTH, since it can cause problems on some graphics
   // cards when using certain GL_BLEND modes

   glDisable(GL_LIGHTING);
   glEnable(GL_CULL_FACE);
   glEnable(GL_DEPTH_TEST);

   // Set line width to default (1.0)
   glLineWidth(1.0f);

   // Only draw the reflected image if we are not showing aero vectors
   if (!mShowAllAeroVectors && !mShowSelectedAeroVectors)
   {
      DrawReflectedImage(mRegionX, mRegionZ, mRegionFloorY);
   }

   DrawDirectImage(mRegionX, mRegionZ, mRegionCeilingY, mRegionFloorY, mRegionStepSize);

   glPushMatrix();
   glRotated(mViewEl_deg, 1.0, 0.0, 0.0);
   glRotated(-mViewAz_deg, 0.0, 1.0, 0.0);
   glTranslated(-mEyePt.X(), -mEyePt.Y(), -mEyePt.Z());
   DrawLightVector();
   glPopMatrix();

   // Reset line width
   glLineWidth(1.0f);

   // Pop the attributes
   glPopAttrib();
}

void GeometryGLWidget::Draw2dOverlay()
{
   double halfWidth = (double)(width() / 2);
   double halfHeight = (double)(height() / 2);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   // Determine the "near" dist for a unity delta with the desired angle
   // L * sin(a) = 1
   // L = 1/sin(a)

   // We want a near plane of 0.5 meters
   // Horizontal pixels (half FOV):
   //   dist = Length * sin(FOV_half_angle)
   //   Length * cos(FOV_half_angle) = 0.5
   //   This implies, Length = 0.5/cos(FOV_half_angle)

   // double halfFovY_rad = (0.5 * aFullFovY_deg)*UtMath::cRAD_PER_DEG;

   // Determine the size of the enclosing box region and arrow sizes
   CalcViewRegionCharacteristics();

   CalcClippingPlaneDistances();

   double left = -halfWidth;
   double right = halfWidth;
   double bottom = -halfHeight;
   double top = halfHeight;
   double nearDist = -10.0;
   double farDist = 10.0;
   glOrtho(left, right, bottom, top, nearDist, farDist);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   // We only need to clear the depth buffer, since we are overlaying the 3d scene
   // in the background
   glClear(GL_DEPTH_BUFFER_BIT);

   glPushAttrib(GL_COLOR_BUFFER_BIT |
                GL_CURRENT_BIT |
                GL_DEPTH_BUFFER_BIT |
                GL_ENABLE_BIT |
                GL_LIGHTING_BIT |
                GL_POLYGON_BIT |
                GL_TEXTURE_BIT);

   glEnable(GL_ALPHA_TEST);
   glAlphaFunc(GL_GREATER, 0.01f);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   glCullFace(GL_BACK);
   glDepthFunc(GL_LESS);
   glEnable(GL_LINE_SMOOTH);
   // Note: Do not use GL_POLYGON_SMOOTH, since it can cause problems on some
   // graphics cards when using certain GL_BLEND modes

   glDisable(GL_LIGHTING);
   glEnable(GL_CULL_FACE);
   glDisable(GL_DEPTH_TEST);

   // Set line width to default (1.0)
   glLineWidth(1.0f);

   mTextFont->SetScale(0.5); // Fonts are nominally 1.0, which is 32x16

   // We explicitly enable/disable GL_TEXTURE_2D, which allows use of the
   // simple version of RenderText
   glEnable(GL_TEXTURE_2D);
   glPushMatrix();
   Draw2dOverlayTopLeft();

   // Only draw the top center if we are showing aero vectors
   if (mShowAllAeroVectors || mShowSelectedAeroVectors)
   {
      Draw2dOverlayTopCenter();
   }

   Draw2dOverlayTopRight();
   Draw2dOverlayBottomLeft();
   Draw2dOverlayBottomRight();
   glPopMatrix();
   glDisable(GL_TEXTURE_2D);

   // Reset line width
   glLineWidth(1.0f);

   // Pop the attributes
   glPopAttrib();
}

namespace
{
   std::string formatValue(double value, int width = 0, int precision = 6, std::ios::fmtflags align = std::ios::right)
   {
      std::stringstream ss;
      ss.setf(align);
      ss << std::fixed << std::setprecision(precision) << std::setw(width) << value;
      return ss.str();
   }

   std::string formatValue(int value, int width = 0)
   {
      std::stringstream ss;
      ss << std::setw(width) << value;
      return ss.str();
   }

   std::string join(const std::string& str) {
      return str;
   }

   template<class... Args>
   std::string join(const std::string& str, Args&&... strings) {
      return str + join(std::forward<Args>(strings)...);
   }

}

void GeometryGLWidget::Draw2dOverlayTopLeft()
{
   MoverCreatorMainWindow& mainWindow = MoverCreatorMainWindow::GetInstance();
   Designer::Vehicle* vehicle = mainWindow.GetMoverCreatorWidget()->GetVehicle();
   Designer::VehicleGeometry& vehicleGeometry = vehicle->GetGeometry();

   auto halfWidth = static_cast<double>(width() / 2);
   auto halfHeight = static_cast<double>(height() / 2);

   double xCoord = -halfWidth + 20.0;
   double yCoord = halfHeight - 40.0;

   // Top Left - Vehicle Summary

   SetColorOverlayNominal(); // Use SetColorOverlayWarning() for warning conditions

   // Simple lambda for rendering text
   auto renderText = [&](const std::string& str) {
      mTextFont->RenderText(xCoord, yCoord, str.c_str(), TextureFont::eLeftTextJustification, TextureFont::eBottomTextJustification);
      yCoord -= 20.0;
   };
   // Simple lambda for formatting values
   auto formatString = [&](const std::string& prefix, const std::string& suffix, int width, int precision, double value) {
     std::stringstream ss;
     ss << std::setw(14) << std::left << prefix << formatValue(value, width, precision, std::ios::left) << std::right << ' ' << suffix;
     return ss.str();
   };
   auto name = mainWindow.GetCurrentBaseName().toStdString();
   name = name.substr(0, name.length() - 4); // Strip off ".amc"
   name.insert(0, "Vehicle: ");
   renderText(name);

   // Determine length, width, height -- based on extents
   double length_ft = mExtentMaxX - mExtentMinX;
   double width_ft = mExtentMaxY - mExtentMinY;
   double height_ft = mExtentMaxZ - mExtentMinZ;
   renderText(formatString("Length:", "ft", 6, 1, length_ft));
   renderText(formatString("Width:", "ft", 6, 1, width_ft));
   renderText(formatString("Height:", "ft", 6, 1, height_ft));

   // If the vehicle has a wing, we present wing data, otherwise we present body and fin data
   Designer::GeometryWing* wing = vehicleGeometry.GetWing();
   if (wing != nullptr)
   {
      renderText(formatString("Wingspan:", "ft", 6, 1, wing->GetSpan_ft()));
      renderText(formatString("Wing Area:", "ft2", 6, 1, wing->GetPlanformArea_ft2()));
      renderText(formatString("Aspect Ratio:", "", 6, 2, wing->GetAspectRatio()));
   }
   else
   {
      double maxFinSpan_ft = 0.0;
      double bodyDiam_ft = 0.0;
      double frontalArea_ft = 0.0;

      Designer::GeometryBody* body = vehicleGeometry.GetBody();
      if (body != nullptr)
      {
         double width = body->GetWidth_ft();
         double height = body->GetHeight_ft();
         bodyDiam_ft = 0.5 * (width + height);
         double radius = bodyDiam_ft * 0.5;
         frontalArea_ft = (radius * radius) * UtMath::cPI;
      }

      if (bodyDiam_ft < 1.0)
      {
         renderText(formatString("Body Diam:", "in", 6, 1, bodyDiam_ft * 12.0));
      }
      else
      {
         renderText(formatString("Body Diam:", "ft", 6, 2, bodyDiam_ft));
      }

      renderText(formatString("Frontal Area:", "ft2", 6, 2, frontalArea_ft));

      if (MaxFinSpan(maxFinSpan_ft))
      {
         renderText(formatString("Max Fin Span:", "ft", 6, 2, maxFinSpan_ft));
      }
   }
}

void GeometryGLWidget::Draw2dOverlayTopCenter()
{
   MoverCreatorMainWindow& mainWindow = MoverCreatorMainWindow::GetInstance();
   Designer::Vehicle* vehicle = mainWindow.GetMoverCreatorWidget()->GetVehicle();
   Designer::VehicleGeometry& vehicleGeometry = vehicle->GetGeometry();
   Designer::VehicleAero* vehicleAero = vehicle->GetAero();

   if (vehicleAero == nullptr)
   {
      return;
   }

   auto halfHeight = static_cast<double>(height() / 2);

   double xCoord = -200.0;
   double yCoord = halfHeight - 40.0;
   double deltaY = 20.0;

   // Top Center - Aero Data Summary

   SetColorOverlayNominal(); // Use SetColorOverlayWarning() for warning conditions

   const std::map<size_t, GeometryObject*>& geometryObjMap = vehicleGeometry.GetGeometryObjectMap();
   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;
      if (TreeWidgetItemSelected(geometryObject->GetTreeWidgetItem()))
      {
         // Convenience lambdas
         auto renderText = [&](const std::string& str){
            mTextFont->RenderText(xCoord, yCoord, str.c_str(), TextureFont::eLeftTextJustification, TextureFont::eBottomTextJustification);
            yCoord -= deltaY;
         };
         auto fmt = [](double value) {
            // Width 7, precision 2 seems to be common in this function.
            return formatValue(value, 7, 2);
         };

         // Get the name of part
         std::string name = geometryObject->GetName();

         // Replace any spaces with underscores
         std::replace(name.begin(), name.end(), ' ', '_');

         std::vector<AeroObject*> aeroObjectsList = vehicleAero->GetAeroObjectsGeneratedFrom(name);

         if (aeroObjectsList.empty())
         {
            return;
         }

         // Get the vehicle attitude
         double vehicleAlpha_deg = 0.0;
         double vehicleBeta_deg  = 0.0;
         vehicle->GetVehicleAlphaBeta(vehicleAlpha_deg, vehicleBeta_deg);

         renderText(join("Vehicle  Alpha: ", fmt(vehicleAlpha_deg), "  Beta: ", fmt(vehicleBeta_deg)));

         double vehicleYawRate_dps = 0.0;
         double vehiclePitchRate_dps = 0.0;
         double vehicleRollRate_dps = 0.0;
         vehicle->GetVehicleYawPitchRollRates(vehicleYawRate_dps, vehiclePitchRate_dps, vehicleRollRate_dps);

         double speed_fps = 0.0;
         double mach = 0.0;
         double KTAS = 0.0;
         double KCAS = 0.0;
         double dynamicPressure_psf = 0.0;
         double airDensity_slugs_ft3 = 0.0;
         vehicle->GetVehicleSpeedParameters(speed_fps, mach, KTAS, KCAS, dynamicPressure_psf, airDensity_slugs_ft3);

         renderText(join("Vehicle  Mach: ", fmt(mach), "   Speed: ", fmt(speed_fps)));
         renderText(join("YPR Rates (", fmt(vehicleYawRate_dps), ",", fmt(vehiclePitchRate_dps), ",", fmt(vehicleRollRate_dps), ") deg/sec"));

         // Loop through the list
         for (auto aeroObj : aeroObjectsList)
         {
            double alpha_deg = 0.0;
            double beta_deg = 0.0;
            aeroObj->CalcAlphaBeta(alpha_deg, beta_deg);

            // Get the name of part
            std::string objectName = aeroObj->GetName();

            // Get the type of part
            std::string objectType = aeroObj->AeroObjType();

            bool isSurface = false;
            bool invertVectors = false;
            double sweepAngle_deg = 0.0;
            double controlAngle_deg = 0.0;

            if ((objectType == "AeroSurface") || 
                (objectType == "AeroWing") ||
                (objectType == "AeroFullFlyingSurface") || 
                (objectType == "AeroHingedSurface"))
            {
               auto surface = dynamic_cast<AeroSurface*>(aeroObj);
               invertVectors  = surface->InvertVectors();
               sweepAngle_deg = surface->CalcSweep_deg();
               controlAngle_deg = surface->GetControlAngle_deg();
               isSurface      = true;
            }

            if (isSurface)
            {
               renderText(join("  Object: ", objectName, "  [", objectType, " (", (invertVectors ? "INVERTED" : "NORMAL"), ")]"));
               renderText(join("    Alpha: ", fmt(alpha_deg), "   Beta: ", fmt(beta_deg), "   Sweep: ", fmt(sweepAngle_deg)));
               renderText(join("    Control Angle: ", fmt(controlAngle_deg), " deg"));
            }
            else
            {
               renderText(join("  Object: ", objectName, "  [", objectType, "]"));
               renderText(join("    Alpha = ", fmt(alpha_deg), "  Beta  = ", fmt(beta_deg)));
            }
         }
      }
   }
}

bool GeometryGLWidget::MaxFinSpan(double& aMaxSpan_ft)
{
   MoverCreatorMainWindow& mainWindow = MoverCreatorMainWindow::GetInstance();
   Vehicle* vehicle = mainWindow.GetMoverCreatorWidget()->GetVehicle();
   VehicleGeometry& vehicleGeometry = vehicle->GetGeometry();

   double maxFinSpan = 0.0;
   bool   response   = false;

   // Loop through objects to find surfaces and determine maximum span from tip to tip
   const std::map<size_t, GeometryObject*>& geometryObjMap = vehicleGeometry.GetGeometryObjectMap();
   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;
      if (geometryObject->GeometryObjectType() == "GeometrySurface")
      {
         GeometrySurface* surface = (GeometrySurface*)geometryObject;
         double span = surface->GetFinSpan();

         if (span > maxFinSpan)
         {
            maxFinSpan = span;
            response = true;
         }
      }
   }

   aMaxSpan_ft = maxFinSpan;

   return response;
}

void GeometryGLWidget::DrawCgSymbol(UtVec3dX aCgVector, double aRadius, bool aIsPart)
{
   const int numSides = 20;  // The amount of segment to create the circle

   glPushMatrix();
   glTranslated(aCgVector.X(), -aCgVector.Z(), aCgVector.Y());
   glRotated(mViewAz_deg, 0.0, 1.0, 0.0);
   glRotated(mViewEl_deg, -1.0, 0.0, 0.0);

   // Draw pie wedge
   SetLightCgColor(aIsPart);
   glBegin(GL_POLYGON);
   glVertex2d(0, 0);
   for (int a = 0; a <= 90; a += 90 / numSides)
   {
      double heading = a * UtMath::cRAD_PER_DEG;
      glVertex2d(cos(heading) * aRadius, sin(heading) * aRadius);
   }
   glVertex2d(0, 0);
   glEnd();

   SetBorderCgColor(aIsPart);
   glBegin(GL_LINE_STRIP);
   glVertex2d(0, 0);
   for (int a = 0; a <= 90; a += 90 / numSides)
   {
      double heading = a * UtMath::cRAD_PER_DEG;
      glVertex2d(cos(heading) * aRadius, sin(heading) * aRadius);
   }
   glVertex2d(0, 0);
   glEnd();

   // Draw pie wedge
   SetDarkCgColor(aIsPart);
   glBegin(GL_POLYGON);
   glVertex2d(0, 0);
   for (int a = 90; a <= 180; a += 90 / numSides)
   {
      double heading = a * UtMath::cRAD_PER_DEG;
      glVertex2d(cos(heading) * aRadius, sin(heading) * aRadius);
   }
   glVertex2d(0, 0);
   glEnd();

   SetBorderCgColor(aIsPart);
   glBegin(GL_LINE_STRIP);
   glVertex2d(0, 0);
   for (int a = 90; a <= 180; a += 90 / numSides)
   {
      double heading = a * UtMath::cRAD_PER_DEG;
      glVertex2d(cos(heading) * aRadius, sin(heading) * aRadius);
   }
   glVertex2d(0, 0);
   glEnd();

   // Draw pie wedge
   SetLightCgColor(aIsPart);
   glBegin(GL_POLYGON);
   glVertex2d(0, 0);
   for (int a = 180; a <= 270; a += 90 / numSides)
   {
      double heading = a * UtMath::cRAD_PER_DEG;
      glVertex2d(cos(heading) * aRadius, sin(heading) * aRadius);
   }
   glVertex2d(0, 0);
   glEnd();

   SetBorderCgColor(aIsPart);
   glBegin(GL_LINE_STRIP);
   glVertex2d(0, 0);
   for (int a = 180; a <= 270; a += 90 / numSides)
   {
      double heading = a * UtMath::cRAD_PER_DEG;
      glVertex2d(cos(heading) * aRadius, sin(heading) * aRadius);
   }
   glVertex2d(0, 0);
   glEnd();

   // Draw pie wedge
   SetDarkCgColor(aIsPart);
   glBegin(GL_POLYGON);
   glVertex2d(0, 0);
   for (int a = 270; a <= 360; a += 90 / numSides)
   {
      double heading = a * UtMath::cRAD_PER_DEG;
      glVertex2d(cos(heading) * aRadius, sin(heading) * aRadius);
   }
   glVertex2d(0, 0);
   glEnd();

   SetBorderCgColor(aIsPart);
   glBegin(GL_LINE_STRIP);
   glVertex2d(0, 0);
   for (int a = 270; a <= 360; a += 90 / numSides)
   {
      double heading = a * UtMath::cRAD_PER_DEG;
      glVertex2d(cos(heading) * aRadius, sin(heading) * aRadius);
   }
   glVertex2d(0, 0);
   glEnd();

   // Draw the cross
   SetBorderCgColor(aIsPart);
   glBegin(GL_LINES);
   glVertex2d(0, 0);
   glVertex2d(0, aRadius * 1.3);

   glVertex2d(0, 0);
   glVertex2d(0, -aRadius * 1.3);

   glVertex2d(0, 0);
   glVertex2d(aRadius * 1.3, 0);

   glVertex2d(0, 0);
   glVertex2d(-aRadius * 1.3, 0);
   glEnd();

   glPopMatrix(); // End draw cg circle
}

void GeometryGLWidget::Draw2dOverlayTopRight()
{
   MoverCreatorMainWindow& mainWindow = MoverCreatorMainWindow::GetInstance();
   Designer::Vehicle* vehicle = mainWindow.GetMoverCreatorWidget()->GetVehicle();
   Designer::VehicleGeometry& vehicleGeometry = vehicle->GetGeometry();
   Designer::GeometryPropulsionData* geomPropulsionData = vehicleGeometry.GetPropulsionData();

   double halfWidth = static_cast<double>(width() / 2);
   double halfHeight = static_cast<double>(height() / 2);

   // Top Right - Mass Properties Summary

   SetColorOverlayNominal(); // Use SetColorOverlayWarning() for warning conditions

   double xCoord = halfWidth - 20.0;
   double yCoord = halfHeight - 40.0;

   Designer::GeometryMassProperties* massProperties = vehicleGeometry.GetMassProperties();

   // Convenience lambdas
   auto renderText = [&](const std::string& str) {
      mTextFont->RenderText(xCoord, yCoord, str.c_str(), TextureFont::eRightTextJustification, TextureFont::eBottomTextJustification);
      yCoord -= 20.0;
   };
   auto fmt91 = [](double value) { return formatValue(value, 9, 1); };
   auto fmt72 = [](double value) { return formatValue(value, 7, 2); };
   auto fmt101 = [](double value) { return formatValue(value, 10, 1); };

   if (massProperties != nullptr)
   {
      double emptyMass_lbs = massProperties->GetEmptyMass_lbs();
      double totalMass_lbs = massProperties->GetTotalMass_lbs();
      renderText(join("Empty Weight:  ", fmt91(emptyMass_lbs), " lbs"));

      // Determine fuel quantity
      double maxFuelQty_lbs = 0.0;
      if (geomPropulsionData != nullptr)
      {
         maxFuelQty_lbs = totalMass_lbs - emptyMass_lbs;
      }
      renderText(join("Internal Fuel: ", fmt91(maxFuelQty_lbs), " lbs"));
      renderText(join("Gross Weight:  ", fmt91(totalMass_lbs), " lbs"));

      renderText(join("CG (X):          ", fmt72(massProperties->GetCgX()), "  ft"));
      renderText(join("CG (Y):          ", fmt72(massProperties->GetCgY()), "  ft"));
      renderText(join("CG (Z):          ", fmt72(massProperties->GetCgZ()), "  ft"));

      renderText(join("Ixx:     ", fmt101(massProperties->GetIxx()), " slug-ft2"));
      renderText(join("Iyy:     ", fmt101(massProperties->GetIyy()), " slug-ft2"));
      renderText(join("Izz:     ", fmt101(massProperties->GetIzz()), " slug-ft2"));
   }
   else
   {
      renderText("MASS PROPERTIES SUMMARY    ");
      renderText("  *** ERROR ***            ");
      renderText("  No MassProperties exist  ");
   }
}

void GeometryGLWidget::Draw2dOverlayBottomLeft()
{
   MoverCreatorMainWindow& mainWindow = MoverCreatorMainWindow::GetInstance();
   Designer::Vehicle* vehicle = mainWindow.GetMoverCreatorWidget()->GetVehicle();
   Designer::VehicleGeometry& vehicleGeometry = vehicle->GetGeometry();

   double halfWidth = static_cast<double>(width() / 2);
   double halfHeight = static_cast<double>(height() / 2);

   // Bottom Left - Selected Part Summary

   SetColorOverlayNominal(); // Use SetColorOverlayWarning() for warning conditions

   double xCoord = -halfWidth + 20.0;
   double yCoord = -halfHeight + 20.0;

   // Convenience lambdas
   auto renderText = [&](const std::string& str) {
      mTextFont->RenderText(xCoord, yCoord, str.c_str(), TextureFont::eLeftTextJustification, TextureFont::eBottomTextJustification);
      yCoord += 20.0; // Output starts at bottom and works up ...
   };
   auto fmtDot1 = [](double value) { return formatValue(value, 0, 1); };
   auto fmt91 = [](double value) { return formatValue(value, 9, 1); };

   const std::map<size_t, GeometryObject*>& geometryObjMap = vehicleGeometry.GetGeometryObjectMap();
   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;
      if (TreeWidgetItemSelected(geometryObject->GetTreeWidgetItem()))
      {
         // Get the type of part
         std::string geometryType = geometryObject->GeometryObjectType();

         // Get mass properties
         UtVec3dX cgLocation(0.0, 0.0, 0.0);
         double mass_lbs = 0.0;
         UtVec3dX rotInertia(0.0, 0.0, 0.0);
         geometryObject->GetCalculatedMassProperties(cgLocation, mass_lbs, rotInertia);

         UtVec3dX refPt = geometryObject->GetRefPoint_ft();
         double aPosX = refPt.X();
         double aPosY = refPt.Y();
         double aPosZ = refPt.Z();

         bool drawCommonData = true;

         // Handle based on type of part
         if ((geometryType == "GeometryWing") || (geometryType == "GeometrySurface"))
         {
            Designer::GeometrySurface* surface = (Designer::GeometrySurface*)geometryObject;

            if (surface != nullptr)
            {
               double span = surface->GetSpan_ft();
               double sweep_deg = surface->GetSweepAngle_deg();
               double rootChord = surface->GetRootChord_ft();
               double tipChord = surface->GetTipChord_ft();
               double thickness = rootChord * surface->GetThicknessRatio();
               double dihedral_deg = surface->GetDihedralAngle_deg();
               double incidence_deg = surface->GetIncidenceAngle_deg();
               double area = surface->GetPlanformArea_ft2();

               // Starting at bottom and working up
               renderText(join("CG:      (", fmtDot1(cgLocation.X()), ", ", fmtDot1(cgLocation.Y()), ", ", fmtDot1(cgLocation.Z()), ") ft"));
               renderText(join("Ref Pt:  (", fmtDot1(aPosX), ", ", fmtDot1(aPosY), ", ", fmtDot1(aPosZ), ") ft"));
               renderText(join("Inertia: (", fmtDot1(rotInertia.X()), ", ", fmtDot1(rotInertia.Y()), ", ", fmtDot1(rotInertia.Z()), ") slug-ft2"));

               if (thickness < 0.000000001) { SetColorOverlayWarning(); }
               renderText(join("Thickness:    ", fmt91(thickness), " ft"));
               SetColorOverlayNominal();

               renderText(join("Incidence:    ", fmt91(incidence_deg), " deg"));
               renderText(join("Dihedral:     ", fmt91(dihedral_deg), " deg"));
               renderText(join("Sweep:        ", fmt91(sweep_deg), " deg"));
               renderText(join("Area:         ", fmt91(area), " ft2"));

               if (tipChord < 0.000000001) { SetColorOverlayWarning(); }
               renderText(join("Chord (Tip):  ", fmt91(tipChord), " ft"));
               SetColorOverlayNominal();

               if (rootChord < 0.000000001) { SetColorOverlayWarning(); }
               renderText(join("Chord (Root): ", fmt91(rootChord), " ft"));
               SetColorOverlayNominal();

               if (span < 0.000000001) { SetColorOverlayWarning(); }
               renderText(join("Span:         ", fmt91(span), " ft"));
               SetColorOverlayNominal();

               if (mass_lbs < 0.000000001) { SetColorOverlayWarning(); }
               renderText(join("Mass:         ", fmt91(mass_lbs), " lbs"));
               SetColorOverlayNominal();
            }
         }
         else if ((geometryType == "GeometryFuselage") || (geometryType == "GeometryBody"))
         {
            auto body = dynamic_cast<Designer::GeometryBody*>(geometryObject);

            double length = body->GetLength_ft();
            double width = body->GetWidth_ft();
            double height = body->GetHeight_ft();

            // Starting at bottom and working up

            renderText(join("Ref Pt:  (", fmtDot1(aPosX), ", ", fmtDot1(aPosY), ", ", fmtDot1(aPosZ), ") ft"));
            renderText(join("Inertia: (", fmtDot1(rotInertia.X()), ", ", fmtDot1(rotInertia.Y()), ", ", fmtDot1(rotInertia.Z()), ") slug-ft2"));

            if (height < 0.000000001) { SetColorOverlayWarning(); }
            renderText(join("Height:   ", fmt91(height), " ft"));
            SetColorOverlayNominal();

            if (width < 0.000000001) { SetColorOverlayWarning(); }
            renderText(join("Width:    ", fmt91(width), " ft"));
            SetColorOverlayNominal();

            if (length < 0.000000001) { SetColorOverlayWarning(); }
            renderText(join("Length:   ", fmt91(length), " ft"));
            SetColorOverlayNominal();

            if (mass_lbs < 0.000000001) { SetColorOverlayWarning(); }
            renderText(join("Mass:     ", fmt91(mass_lbs), " lbs"));
            SetColorOverlayNominal();
         }
         else if (geometryType == "GeometryNacelle")
         {
            auto nacelle = dynamic_cast<Designer::GeometryNacelle*>(geometryObject);

            double length = nacelle->GetLength_ft();
            double width = nacelle->GetWidth_ft();
            double height = nacelle->GetHeight_ft();
            double height_inner = nacelle->GetHeightInner_ft();
            double height_outer = nacelle->GetHeightOuter_ft();
            std::string shapeString = nacelle->GetOverallShapeString().toStdString();

            // Starting at bottom and working up

            renderText(join("Ref Pt:  (", fmtDot1(aPosX), ", ", fmtDot1(aPosY), ", ", fmtDot1(aPosZ), ") ft"));
            renderText(join("Inertia: (", fmtDot1(rotInertia.X()), ", ", fmtDot1(rotInertia.Y()), ", ", fmtDot1(rotInertia.Z()), ") slug-ft2"));

            if ((shapeString == "Flat-Swept-Right") || (shapeString == "Flat-Swept-Left"))
            {
               if (height_outer < 0.000000001) { SetColorOverlayWarning(); }
               renderText(join("Height (Outer): ", fmt91(height_outer), " ft"));
               SetColorOverlayNominal();

               if (height_inner < 0.000000001) { SetColorOverlayWarning(); }
               renderText(join("Height (Inner): ", fmt91(height_inner), " ft"));
               SetColorOverlayNominal();

               if (width < 0.000000001) { SetColorOverlayWarning(); }
               renderText(join("Width:          ", fmt91(width), " ft"));
               SetColorOverlayNominal();
            }
            else
            {
               if (height < 0.000000001) { SetColorOverlayWarning(); }
               renderText(join("Height:   ", fmt91(height), " ft"));
               SetColorOverlayNominal();

               if (width < 0.000000001) { SetColorOverlayWarning(); }
               renderText(join("Width:    ", fmt91(width), " ft"));
               SetColorOverlayNominal();
            }

            if (length < 0.000000001) { SetColorOverlayWarning(); }
            renderText(join("Length:   ", fmt91(length), " ft"));
            SetColorOverlayNominal();

            if (mass_lbs < 0.000000001) { SetColorOverlayWarning(); }
            renderText(join("Mass:     ", fmt91(mass_lbs), " lbs"));
            SetColorOverlayNominal();
         }
         else if (geometryType == "GeometryDish")
         {
            auto dish = dynamic_cast<Designer::GeometryDish*>(geometryObject);

            double diam = dish->GetDiameter_ft();
            double thickness = dish->GetThickness_ft();

            // Starting at bottom and working up

            renderText(join("CG:      (", fmtDot1(cgLocation.X()), ", ", fmtDot1(cgLocation.Y()), ", ", fmtDot1(cgLocation.Z()), ") ft"));
            renderText(join("Ref Pt:  (", fmtDot1(aPosX), ", ", fmtDot1(aPosY), ", ", fmtDot1(aPosZ), ") ft"));
            renderText(join("Inertia: (", fmtDot1(rotInertia.X()), ", ", fmtDot1(rotInertia.Y()), ", ", fmtDot1(rotInertia.Z()), ") slug-ft2"));

            if (thickness < 0.000000001) { SetColorOverlayWarning(); }
            renderText(join("Thickness: ", fmt91(thickness), " ft"));
            SetColorOverlayNominal();

            if (diam < 0.000000001) { SetColorOverlayWarning(); }
            renderText(join("Diameter:  ", fmt91(diam), " ft"));
            SetColorOverlayNominal();

            if (mass_lbs < 0.000000001) { SetColorOverlayWarning(); }
            renderText(join("Mass:      ", fmt91(mass_lbs), " lbs"));
            SetColorOverlayNominal();
         }
         else if (geometryType == "GeometryLandingGear")
         {
            Designer::GeometryLandingGear* gear = (Designer::GeometryLandingGear*)geometryObject;

            double cdMult = gear->GetCdMultiplier();
            double strutDiam = gear->GetStrutDiam_ft();
            double tireDiam = gear->GetTireDiam_ft();
            double tireWidth = gear->GetTireWidth_ft();
            double length = gear->GetUncompressedLength_ft();

            // Starting at bottom and working up

            renderText(join("Ref Pt:  (", fmtDot1(aPosX), ", ", fmtDot1(aPosY), ", ", fmtDot1(aPosZ), ") ft"));

            if (cdMult < 0.000000001) { SetColorOverlayWarning(); }
            renderText(join("CD Mult:    ", fmt91(cdMult)));
            SetColorOverlayNominal();

            if (tireWidth < 0.000000001) { SetColorOverlayWarning(); }
            renderText(join("Tire Width: ", fmt91(tireWidth), " ft"));
            SetColorOverlayNominal();

            if (tireDiam < 0.000000001) { SetColorOverlayWarning(); }
            renderText(join("Tire Diam:  ", fmt91(tireDiam), " ft"));
            SetColorOverlayNominal();

            if (strutDiam < 0.000000001) { SetColorOverlayWarning(); }
            renderText(join("Strut Diam: ", fmt91(strutDiam), " ft"));
            SetColorOverlayNominal();

            if (length < 0.000000001) { SetColorOverlayWarning(); }
            renderText(join("Length:     ", fmt91(length), " ft"));
            SetColorOverlayNominal();

            if (mass_lbs < 0.000000001) { SetColorOverlayWarning(); }
            renderText(join("Mass:       ", fmt91(mass_lbs), " lbs"));
            SetColorOverlayNominal();
         }
         else if (geometryType == "GeometryPointMass")
         {
            // Starting at bottom and working up
            renderText(join("Ref Pt:  (", fmtDot1(aPosX), ", ", fmtDot1(aPosY), ", ", fmtDot1(aPosZ), ") ft"));

            if (mass_lbs < 0.000000001) { SetColorOverlayWarning(); }
            renderText(join("Mass:          ", fmtDot1(mass_lbs), " lbs"));
            SetColorOverlayNominal();
         }
         else if (geometryType == "GeometryPropulsionData")
         {
            auto propulsion = dynamic_cast<Designer::GeometryPropulsionData*>(geometryObject);

            bool tankPresent = propulsion->FuelTankPresent();
            double maxQty = propulsion->GetFuelQtyMax();

            UtVec3dX cgVec(0.0, 0.0, 0.0);
            if (propulsion->GetCG_ft(cgVec))
            {
               // Starting at bottom and working up

               if (tankPresent)
               {
                  renderText(join("Ref Pt:  (", fmtDot1(cgVec.X()), ", ", fmtDot1(cgVec.Y()), ", ", fmtDot1(cgVec.Z()), ") ft"));

                  if (maxQty < 0.000000001) { SetColorOverlayWarning(); }
                  renderText(join("Max Fuel Qty:  ", fmtDot1(maxQty), " lbs"));
                  SetColorOverlayNominal();

                  renderText("Fuel Tank:     Installed");
               }
               else
               {
                  renderText("Fuel Tank:     Not Installed");
               }
            }
         }
         else if (geometryType == "GeometrySpeedBrake")
         {
            auto brake = dynamic_cast<Designer::GeometrySpeedBrake*>(geometryObject);

            double length = brake->GetLength_ft();
            double width = brake->GetWidth_ft();
            double maxAngle = brake->GetMaxAngle_deg();
            double cdMult = brake->GetCdMultiplier();

            // Starting at bottom and working up

            renderText(join("Ref Pt:  (", fmtDot1(aPosX), ", ", fmtDot1(aPosY), ", ", fmtDot1(aPosZ), ") ft"));

            if (cdMult < 0.000000001) { SetColorOverlayWarning(); }
            renderText(join("CD Mult:   ", fmt91(cdMult)));
            SetColorOverlayNominal();

            if (maxAngle < 0.000000001) { SetColorOverlayWarning(); }
            renderText(join("Max Angle: ", fmt91(maxAngle), " deg"));
            SetColorOverlayNominal();

            if (width < 0.000000001) { SetColorOverlayWarning(); }
            renderText(join("Width:     ", fmt91(width), " ft"));
            SetColorOverlayNominal();

            if (length < 0.000000001) { SetColorOverlayWarning(); }
            renderText(join("Length:    ", fmt91(length), " ft"));
            SetColorOverlayNominal();
         }
         else if (geometryType == "GeometryMassProperties")
         {
            auto massProperties = dynamic_cast<Designer::GeometryMassProperties*>(geometryObject);

            // Starting at bottom and working up
            double emptyMass_lbs           = massProperties->GetEmptyMass_lbs();
            double fuselageMass_lbs        = massProperties->GetFuselageBodyMass_lbs();
            double podsMass_lbs            = massProperties->GetPodsMass_lbs();
            double wingMass_lbs            = massProperties->GetWingMass_lbs();
            double surfacesMass_lbs        = massProperties->GetSurfacesMass_lbs();
            double gearMass_lbs            = massProperties->GetLandingGearMasses_lbs();
            double nacellesMass_lbs        = massProperties->GetNacelleMasses_lbs();
            double dishMass_lbs            = massProperties->GetDishMasses_lbs();
            double pointMassesMass_lbs     = massProperties->GetPointMasses_lbs();
            double enginesMass_lbs         = massProperties->GetEngineMasses_lbs();

            double fuselageMass_percent    = 100.0 * (fuselageMass_lbs / emptyMass_lbs);
            double wingMass_percent        = 100.0 * (wingMass_lbs / emptyMass_lbs);
            double surfacesMass_percent    = 100.0 * (surfacesMass_lbs / emptyMass_lbs);
            double gearMass_percent        = 100.0 * (gearMass_lbs / emptyMass_lbs);
            double otherMass_percent       = 100.0 * ((nacellesMass_lbs + dishMass_lbs + podsMass_lbs) / emptyMass_lbs);
            double enginesMass_percent     = 100.0 * (enginesMass_lbs / emptyMass_lbs);

            double structureMass_lbs       = fuselageMass_lbs + podsMass_lbs + wingMass_lbs +
                                             surfacesMass_lbs + gearMass_lbs + nacellesMass_lbs +
                                             dishMass_lbs;
            double structureMass_percent   = 100.0 * (structureMass_lbs / emptyMass_lbs);

            auto fmt81 = [](double value) { return formatValue(value, 8, 1); };
            auto fmt41 = [](double value) { return formatValue(value, 4, 1); };
            if (enginesMass_lbs > 0)
            {
               renderText(join("Engines:             ", fmt81(enginesMass_lbs), " lbs (", fmt41(enginesMass_percent), "%)"));
            }

            if (pointMassesMass_lbs > 0)
            {
               renderText(join("Systems:             ", fmt81(pointMassesMass_lbs), " lbs (", fmt41(enginesMass_percent), "%)"));
            }

            if ((nacellesMass_lbs + dishMass_lbs + podsMass_lbs) > 0)
            {
               renderText(join("  Other Structures:  ", fmt81(nacellesMass_lbs + dishMass_lbs + podsMass_lbs),
                               " lbs (", fmt41(otherMass_percent), "%)"));
            }

            if (gearMass_lbs > 0)
            {
               renderText(join("  Landing Gear:      ", fmt81(gearMass_lbs), " lbs (", fmt41(gearMass_percent), "%)"));
            }

            if (surfacesMass_lbs > 0)
            {
               renderText(join("  Aero Surfaces:     ", fmt81(surfacesMass_lbs), " lbs (", fmt41(surfacesMass_percent), "%)"));
            }

            if (wingMass_lbs > 0)
            {
               renderText(join("  Wing:              ", fmt81(wingMass_lbs), " lbs (", fmt41(wingMass_percent), "%)"));
            }

            if (fuselageMass_lbs > 0)
            {
               renderText(join("  Fuselage/Body:     ", fmt81(fuselageMass_lbs), " lbs (", fmt41(fuselageMass_percent), "%)"));
            }

            renderText(join("Structure:           ", fmt81(structureMass_lbs), " lbs (", fmt41(structureMass_percent), "%)"));
            renderText(join("Empty Mass:          ", fmt81(emptyMass_lbs), " lbs"));

            drawCommonData = false;
         }
         else if (geometryType == "GeometryEngine")
         {
            auto engine = dynamic_cast<Designer::GeometryEngine*>(geometryObject);

            double length_ft = engine->GetLength_ft();
            double diam_ft = engine->GetDiameter_ft();
            double offset_ft = engine->GetThrustOffset_ft();

            // Output starts at bottom and works up ...
            renderText(join("Ref Pt:  (", fmtDot1(aPosX), ", ", fmtDot1(aPosY), ", ", fmtDot1(aPosZ), ") ft"));
            renderText(join("Inertia: (", fmtDot1(rotInertia.X()), ", ", fmtDot1(rotInertia.Y()), ", ", fmtDot1(rotInertia.Z()), ") slug-ft2"));
            renderText(join("Thrust Offset: ", fmt91(offset_ft), " ft"));
            renderText(join("Diam:          ", fmt91(diam_ft), " ft"));
            renderText(join("Length:        ", fmt91(length_ft), " ft"));
            renderText(join("Mass:          ", fmt91(mass_lbs), " lbs"));
            renderText(join("Engine Model:    ", engine->GetEngineModel()));
            renderText(join("Engine Type:     ", engine->GetEngineTypeString()));
            renderText(join("Selected Engine: ", engine->GetName()));
         }
         else
         {
            drawCommonData = false;
         }

         // Common data
         if (drawCommonData)
         {
            renderText(join("Object Type:   ", geometryObject->GeometryObjectType()));
            renderText(join("Selected Item: ", geometryObject->GetName()));
         }
      }
   }
}

void GeometryGLWidget::Draw2dOverlayBottomRight()
{
   double halfWidth = static_cast<double>(width() / 2);
   double halfHeight = static_cast<double>(height() / 2);

   // Bottom Right - Sizing and Observer Summary ..................................................

   SetColorOverlayNominal(); // Use SetColorOverlayWarning() for warning conditions

   double xCoord = halfWidth - 20.0;
   double yCoord = -halfHeight + 20.0;

   // Starting at bottom and working up
   auto renderText = [&](const std::string& str) {
      mTextFont->RenderText(xCoord, yCoord, str.c_str(), TextureFont::eRightTextJustification, TextureFont::eBottomTextJustification);
      yCoord += 20.0;
   };
   auto fmt61 = [](double value) { return formatValue(value, 6, 1); };

   renderText(join("Grid Size: ", formatValue(mRegionStepSize, 6), "  ft"));
   renderText(join("Observer Distance: ", fmt61(mEyeDist), "  ft"));
   renderText(join("Observer Elevation: ", fmt61(mViewEl_deg), " deg"));

   double az_deg = 90.0 - mViewAz_deg;
   if (az_deg < -180.0) { az_deg += 360.0; }
   if (az_deg >  180.0) { az_deg -= 360.0; }

   renderText(join("Observer Azimuth: ", fmt61(az_deg), " deg"));
}

void GeometryGLWidget::DrawStenciledFloor(double aLength, double aWidth, double aFloorHeight)
{
   // Don't update color or depth
   glDisable(GL_DEPTH_TEST);
   glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
   glDepthMask(GL_FALSE);

   // Draw 1 into the stencil buffer
   glEnable(GL_STENCIL_TEST);
   glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
   glStencilFunc(GL_ALWAYS, 1, 0xffffffff);

   glPushMatrix();

   glRotated(mViewEl_deg, 1.0, 0.0, 0.0);
   glRotated(-mViewAz_deg, 0.0, 1.0, 0.0);
   glTranslated(-mEyePt.X(), -mEyePt.Y(), -mEyePt.Z());

   glBegin(GL_POLYGON);
   glVertex3d(-aLength, aFloorHeight,  aWidth);
   glVertex3d(aLength, aFloorHeight,  aWidth);
   glVertex3d(aLength, aFloorHeight, -aWidth);
   glVertex3d(-aLength, aFloorHeight, -aWidth);
   glEnd();

   glPopMatrix();

   // Re-enable update of color and depth
   glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
   glEnable(GL_DEPTH_TEST);
   glDepthMask(GL_TRUE);
}

void GeometryGLWidget::DrawDirectImage(double aLength, double aWidth, double aCeilingHeight, double aFloorHeight, int aStepSize)
{
   glPushMatrix();
   glRotated(mViewEl_deg, 1.0, 0.0, 0.0);
   glRotated(-mViewAz_deg, 0.0, 1.0, 0.0);
   glTranslated(-mEyePt.X(), -mEyePt.Y(), -mEyePt.Z());

   SetLightProperties();

   // Disable depth test while drawing walls and grids of enclosure box
   glDisable(GL_DEPTH_TEST);

   glLineWidth(1.0f);
   DrawEnclosureBox(aLength, aWidth, aCeilingHeight, aFloorHeight, aStepSize);
   glLineWidth(1.0f);

   // Re-enable depth test
   glEnable(GL_DEPTH_TEST);

   DrawItems();

   DrawBillboardItems();

   glPopMatrix();
}

void GeometryGLWidget::DrawReflectedImage(double aLength, double aWidth, double aFloorHeight)
{
   DrawStenciledFloor(aLength, aWidth, aFloorHeight);

   // Now, only render where stencil is set to 1
   glStencilFunc(GL_EQUAL, 1, 0xffffffff);
   glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

   // Reverse winding order
   glFrontFace(GL_CW);

   glPushMatrix();
   glRotated(mViewEl_deg, 1.0, 0.0, 0.0);
   glRotated(-mViewAz_deg, 0.0, 1.0, 0.0);
   glTranslated(-mEyePt.X(), -mEyePt.Y(), -mEyePt.Z());
   glTranslated(0.0, 2.0 * aFloorHeight, 0.0);
   glScaled(1.0, -1.0, 1.0);

   SetLightProperties();

   DrawItems();

   glPopMatrix();

   // Restore winding order
   glFrontFace(GL_CCW);

   // Stop using stencil buffer
   glDisable(GL_STENCIL_TEST);
}

void GeometryGLWidget::DrawLightVector()
{
   glLineWidth(2.0f);
   if (mShowLightVector)
   {
      glDisable(GL_CULL_FACE);
      glColor4d(1.0, 1.0, 0.0, 0.3);
      glBegin(GL_POLYGON);
      glVertex3d(0.0, 0.0, 0.0);
      glVertex3d(200.0 * mLightVec.X(), 0.0, 200.0 * mLightVec.Z());
      glVertex3d(200.0 * mLightVec.X(), 200.0 * mLightVec.Y(), 200.0 * mLightVec.Z());
      glEnd();
      glEnable(GL_CULL_FACE);

      glColor4d(1.0, 1.0, 0.0, 1.0);
      glBegin(GL_LINES);
      glVertex3d(0.0, 0.0, 0.0);
      glVertex3d(200.0 * mLightVec.X(), 200.0 * mLightVec.Y(), 200.0 * mLightVec.Z());
      glEnd();
   }
}

GeometryObject* GeometryGLWidget::GetSelectedGeometryObject()
{
   MoverCreatorMainWindow& mainWindow = MoverCreatorMainWindow::GetInstance();
   Designer::Vehicle* vehicle = mainWindow.GetMoverCreatorWidget()->GetVehicle();
   Designer::VehicleGeometry& vehicleGeometry = vehicle->GetGeometry();

   // Loop through geometry objects
   const std::map<size_t, GeometryObject*>& geometryObjMap = vehicleGeometry.GetGeometryObjectMap();
   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;
      QTreeWidgetItem* tmpItem = geometryObject->GetTreeWidgetItem();
      if (tmpItem != nullptr)
      {
         if (TreeWidgetItemSelected(tmpItem))
         {
            return geometryObject;
         }
      }
   }

   return nullptr;
}

bool GeometryGLWidget::GeometryObjectIsRenderable(GeometryObject* aObject)
{
   if (aObject == nullptr) { return false; }

   if ((aObject->GeometryObjectType() == "GeometryBody") ||
       (aObject->GeometryObjectType() == "GeometryDish") ||
       (aObject->GeometryObjectType() == "GeometryFuselage") ||
       (aObject->GeometryObjectType() == "GeometryLandingGear") ||
       (aObject->GeometryObjectType() == "GeometryNacelle") ||
       (aObject->GeometryObjectType() == "GeometryPointMass") ||
       (aObject->GeometryObjectType() == "GeometryPropulsionData") ||
       (aObject->GeometryObjectType() == "GeometrySpeedBrake") ||
       (aObject->GeometryObjectType() == "GeometrySurface") ||
       (aObject->GeometryObjectType() == "GeometryWing") ||
       (aObject->GeometryObjectType() == "GeometryEngine"))
   {
      return true;
   }

   return false;
}

QTreeWidgetItem* GeometryGLWidget::GetSelectedTreeWidgetItem()
{
   GeometryObject* geometryObject = GetSelectedGeometryObject();
   if (geometryObject != nullptr)
   {
      QTreeWidgetItem* tmpItem = geometryObject->GetTreeWidgetItem();
      if (tmpItem != nullptr)
      {
         if (TreeWidgetItemSelected(tmpItem))
         {
            return tmpItem;
         }
      }
   }

   // No match was found so return null
   return nullptr;
}

bool GeometryGLWidget::TreeWidgetItemSelected(QTreeWidgetItem* aItem) const
{
   if (aItem != nullptr)
   {
      // If this item is selected, return true
      if (aItem->isSelected())
      {
         return true;
      }

      // Check if this item is the "current item", if so, return true
      QTreeWidgetItem* currentItem = aItem->treeWidget()->currentItem();
      if (currentItem == aItem) { return true; }

      // Get the focus widget
      QWidget* focusWidget = QApplication::focusWidget();

      // Check if this item is the "focus widget", if so, return true
      if (focusWidget == (QWidget*)aItem) { return true; }

      // Loop through children
      int num = aItem->childCount();
      for (int i = 0; i < num; ++i)
      {
         QTreeWidgetItem* item = aItem->child(i);
         // Check if this child item is selected, if so, return true
         if (TreeWidgetItemSelected(item)) { return true; }
         // Check if this child item is the "focus widget", if so, return true
         if (focusWidget == (QWidget*)item) { return true; }
         // Check if this child item is the "current item", if so, return true
         if (currentItem == item) { return true; }
      }
   }

   return false;
}

void GeometryGLWidget::DrawAxisArrows()
{
   if (!mShowAxis)
   {
      return;
   }

   glPushMatrix();
   glTranslated(mObjectCenter.X(), mObjectCenter.Y(), mObjectCenter.Z());
   UtVec3dX ptVec(0.0, 0.0, 0.0);

   // Draw x-arrow in x-render space
   glPushMatrix();
   SetRedMaterialProperties();
   DrawArrow(ptVec, mArrowheadLength, mArrowX_Length, mArrowDiameter); //Length/Diam
   glPopMatrix();

   // Draw y-arrow in z-render space
   glPushMatrix();
   SetGreenMaterialProperties();
   glRotated(90.0, 0.0, -1.0, 0.0);
   DrawArrow(ptVec, mArrowheadLength, mArrowY_Length, mArrowDiameter); //Length/Diam
   glPopMatrix();

   // Draw z-arrow in y-render space
   glPushMatrix();
   SetBlueMaterialProperties();
   glRotated(90.0, 0.0, 0.0, -1.0);
   DrawArrow(ptVec, mArrowheadLength, mArrowZ_Length, mArrowDiameter); //Length/Diam
   glPopMatrix();

   glPopMatrix();
}

void GeometryGLWidget::DrawAxisArrowLabels()
{
   if (!mShowAxis)
   {
      return;
   }

   glPushAttrib(GL_COLOR_BUFFER_BIT |
                GL_CURRENT_BIT |
                GL_DEPTH_BUFFER_BIT |
                GL_ENABLE_BIT |
                GL_LIGHTING_BIT |
                GL_POLYGON_BIT |
                GL_TEXTURE_BIT);
   glPushMatrix();
   glTranslated(mObjectCenter.X(), mObjectCenter.Y(), mObjectCenter.Z());
   UtVec3dX ptVec(0.0, 0.0, 0.0);

   glDisable(GL_LIGHTING);
   glDisable(GL_CULL_FACE);

   UtVec3dX labelPt(ptVec);

   // Draw +X
   glPushMatrix();
   SetRedArrowLabelColor();
   labelPt.Set(ptVec.X() + mArrowX_Length + mArrowDiameter * 2.0, ptVec.Y(), ptVec.Z());
   DrawAxisLabel(labelPt, 0.0, mArrowDiameter, "X");
   glPopMatrix();

   // Draw +Y
   glPushMatrix();
   SetGreenArrowLabelColor();
   labelPt.Set(ptVec.X(), ptVec.Y(), ptVec.Z() + mArrowY_Length + mArrowDiameter * 2.0);
   DrawAxisLabel(labelPt, 0.0, mArrowDiameter, "Y");
   glPopMatrix();

   // Draw +Z
   glPushMatrix();
   SetBlueArrowLabelColor();
   labelPt.Set(ptVec.X(), ptVec.Y() - mArrowZ_Length - mArrowDiameter * 2.0, ptVec.Z());
   DrawAxisLabel(labelPt, 0.0, mArrowDiameter, "Z");
   glPopMatrix();

   glEnable(GL_CULL_FACE);

   glPopMatrix();
   glPopAttrib();
}

void GeometryGLWidget::DrawItems()
{
   MoverCreatorMainWindow& mainWindow = MoverCreatorMainWindow::GetInstance();
   Designer::Vehicle* vehicle = mainWindow.GetMoverCreatorWidget()->GetVehicle();

   // Set flags
   const bool nonSelectedObjects    = false;
   const bool onlyTheSelectedObject = true;

   // Find the selected object (if any)
   bool viableObjectSelected = false;
   GeometryObject* selectedObject = GetSelectedGeometryObject();
   if (selectedObject)
   {
      viableObjectSelected = GeometryObjectIsRenderable(selectedObject);
   }

   CalcViewRegionCharacteristics();

   // Only draw the primary axes if we are not showing aero vectors
   if (!mShowAllAeroVectors && !mShowSelectedAeroVectors)
   {
      DrawAxisArrows();
   }

   if (viableObjectSelected)
   {
      SetDrawingSelectedMode();
      DrawVehicle(vehicle, onlyTheSelectedObject); // Draw the selected object

      SetDrawingNonSelectedMode();
      DrawVehicle(vehicle, nonSelectedObjects); // Draw the non-selected objects
   }
   else
   {
      SetDrawingNothingSelectedMode();
      DrawVehicle(vehicle, nonSelectedObjects); // Draw the non-selected objects
   }

   // Draw focus point
   if (vehicle->DebugMode())
   {
      DrawSphereAtPoint(GetViewingFocusPointX(), GetViewingFocusPointY(), GetViewingFocusPointZ(), 1);
      DrawFocusPoints();
   }
}

void GeometryGLWidget::DrawBillboardItems()
{
   MoverCreatorMainWindow& mainWindow = MoverCreatorMainWindow::GetInstance();
   Designer::Vehicle* vehicle = mainWindow.GetMoverCreatorWidget()->GetVehicle();
   Designer::VehicleGeometry& vehicleGeometry = vehicle->GetGeometry();

   CalcViewRegionCharacteristics();

   if (mShowAxis)
   {
      // Only draw the primary axes labels if we are not showing aero vectors
      if (!mShowAllAeroVectors && !mShowSelectedAeroVectors)
      {
         DrawAxisArrowLabels(); // The axis labels are drawn last due to alpha effects
      }
   }

   // Setup for drawing cg billboards
   glClear(GL_DEPTH_BUFFER_BIT);
   glPushAttrib(GL_COLOR_BUFFER_BIT |
                GL_CURRENT_BIT |
                GL_ENABLE_BIT |
                GL_LIGHTING_BIT |
                GL_POLYGON_BIT |
                GL_TEXTURE_BIT);
   glDisable(GL_LIGHTING);
   glDisable(GL_CULL_FACE);
   glDisable(GL_DEPTH_TEST);

   if (mShowVehicleCG)
   {
      Designer::GeometryMassProperties* massProperties = vehicleGeometry.GetMassProperties();

      // Fueled cg
      UtVec3dX cgVec = massProperties->GetFueledCg();
      glPushMatrix();
      glTranslated(mObjectCenter.X(), mObjectCenter.Y(), mObjectCenter.Z());

      // Draw fueled cg, which is larger than the empty cg circle radius
      float radius = mArrowDiameter * 1.2; // Fueled cg circle is larger than empty cg circle
      DrawCgSymbol(cgVec, radius, false);
      glPopMatrix();

      // Empty cg
      cgVec = massProperties->GetCg();
      glPushMatrix();
      glTranslated(mObjectCenter.X(), mObjectCenter.Y(), mObjectCenter.Z());

      // Empty cg circle is smaller than fueled cg circle radius
      radius = mArrowDiameter;
      DrawCgSymbol(cgVec, radius, false);
      glPopMatrix();
   }

   // Loop through geometry objects
   const std::map<size_t, GeometryObject*>& geometryObjMap = vehicleGeometry.GetGeometryObjectMap();
   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;

      bool partIsSelected = false;
      if (TreeWidgetItemSelected(geometryObject->GetTreeWidgetItem()))
      {
         SetHighlightedMaterialProperties();
         partIsSelected = true;
      }

      // Draw cg locations
      if (partIsSelected && mShowSelectedObjectCG)
      {
         UtVec3dX cgVec(0.0, 0.0, 0.0);
         if (geometryObject->GetCG_ft(cgVec))
         {
            glPushMatrix();
            glTranslated(mObjectCenter.X(), mObjectCenter.Y(), mObjectCenter.Z());
            float radius = mArrowDiameter * 0.5;
            DrawCgSymbol(cgVec, radius, true);
            glPopMatrix();
         }
      }
   }

   // Restore after setting cg billboards
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_CULL_FACE);
   glPopAttrib();
}

void GeometryGLWidget::DrawVehicle(Designer::Vehicle* aVehicle, bool aOnlySelectedObject)
{
   glPushMatrix();
   glTranslated(mObjectCenter.X(), mObjectCenter.Y(), mObjectCenter.Z());

   //bool wireframe = false;
   //if (mShowAllAeroVectors || mShowSelectedAeroVectors)
   //{
   //   wireframe = true;
   //}

   DrawEngines(aVehicle, aOnlySelectedObject, mShowWireframe);
   DrawWingsAndSurfaces(aVehicle, aOnlySelectedObject, mShowWireframe);
   DrawSpeedBrakes(aVehicle, aOnlySelectedObject, mShowWireframe);
   DrawLandingGear(aVehicle, aOnlySelectedObject, mShowWireframe);
   DrawDishes(aVehicle, aOnlySelectedObject, mShowWireframe);
   DrawBodiesAndNacelles(aVehicle, aOnlySelectedObject, mShowWireframe);
   DrawCanopy(aVehicle, aOnlySelectedObject, mShowWireframe);
   DrawFuelTanks(aVehicle, aOnlySelectedObject, mShowWireframe);
   DrawPointMasses(aVehicle, aOnlySelectedObject, mShowWireframe);

   if (mShowAllAeroVectors || mShowSelectedAeroVectors)
   {
      DrawAeroData(aVehicle, aOnlySelectedObject);
   }

   glPopMatrix();
}

void GeometryGLWidget::DrawEngines(Designer::Vehicle* aVehicle, bool aOnlySelectedObject, bool aWireframe)
{
   VehicleGeometry& vehicleGeometry = aVehicle->GetGeometry();
   const std::map<size_t, GeometryObject*>& geometryObjMap = vehicleGeometry.GetGeometryObjectMap();

   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;

      if (geometryObject->GeometryObjectType() == "GeometryEngine")
      {
         Designer::GeometryEngine* engine = dynamic_cast<Designer::GeometryEngine*>(geometryObject);

         if (engine != nullptr)
         {
            double length_ft = engine->GetLength_ft();
            double diam_ft = engine->GetDiameter_ft();
            double offset_ft = engine->GetThrustOffset_ft();

            UtVec3dX refPt_ft = engine->GetRefPoint_ft();

            double symmetryHorizontalY = engine->GetSymmetryHorizontalY();

            bool highlighted = false;
            SetEngineMaterialProperties();
            bool skipDrawingItem = false;
            if (TreeWidgetItemSelected(engine->GetTreeWidgetItem()))
            {
               SetHighlightedMaterialProperties();
               highlighted = true;
               if (!aOnlySelectedObject)
               {
                  skipDrawingItem = true;
               }
            }
            else
            {
               if (aOnlySelectedObject)
               {
                  skipDrawingItem = true;
               }
            }
            if (!skipDrawingItem)
            {
               if (mShowEngines || highlighted)
               {
                  if (aWireframe)
                  {
                     if (highlighted)
                     {
                        SetSelectedWireframeColor();
                     }
                     else
                     {
                        SetNominalWireframeColor();
                     }
                  }

                  DrawEngine(refPt_ft.X(), refPt_ft.Y(), refPt_ft.Z(), diam_ft, length_ft, offset_ft,
                             engine->GetYaw_deg(), engine->GetPitch_deg(), engine->GetRoll_deg(),
                             mShowThrustVectors, highlighted, aWireframe);

                  if (engine->IsSymmetrical())
                  {
                     // Now, determine location of other engine
                     double currentEngineY = refPt_ft.Y();
                     double delta = currentEngineY - symmetryHorizontalY;
                     double otherEngineY = currentEngineY - 2.0 * delta;

                     DrawEngine(refPt_ft.X(), otherEngineY, refPt_ft.Z(), diam_ft, length_ft, offset_ft,
                                -engine->GetYaw_deg(), engine->GetPitch_deg(), -engine->GetRoll_deg(),
                                mShowThrustVectors, highlighted, aWireframe);
                  }
               }
            }

         }
      }
   }
}

void GeometryGLWidget::DrawBodiesAndNacelles(Designer::Vehicle* aVehicle, bool aOnlySelectedObject, bool aWireframe)
{
   VehicleGeometry& vehicleGeometry = aVehicle->GetGeometry();
   const std::map<size_t, GeometryObject*>& geometryObjMap = vehicleGeometry.GetGeometryObjectMap();

   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;
      std::string geometryType = geometryObject->GeometryObjectType();
      if ((geometryType == "GeometryFuselage") || (geometryType == "GeometryBody"))
      {
         Designer::GeometryBody* body = (Designer::GeometryBody*)geometryObject;
         if (body != nullptr)
         {
            UtVec3dX refPt                            = body->GetRefPoint_ft();
            double posX                               = refPt.X();
            double posY                               = refPt.Y();
            double posZ                               = refPt.Z();
            double length                             = body->GetLength_ft();
            double height                             = body->GetHeight_ft();
            double width                              = body->GetWidth_ft();
            double frontLength                        = body->GetForwardShapeLength_ft();
            double rearLength                         = body->GetAftShapeLength_ft();
            double rearDiam                           = body->GetAftShapeDiam_ft();
            double yaw_deg                            = body->GetYawAngle_deg();
            double pitch_deg                          = body->GetPitchAngle_deg();
            double roll_deg                           = body->GetRollAngle_deg();
            double symmetryHorizontalY                = body->GetSymmetryHorizontalY();
            GeometryBody::ForwardShapeType frontShape = body->GetForwardShapeType();
            GeometryBody::AftShapeType aftShape       = body->GetAftShapeType();

            if (aftShape == GeometryBody::AftShapeType::cBLUNT)
            {
               rearLength = 0;
            }

            if (!aWireframe)
            {
               SetNominalMaterialProperties(0.85);
            }

            bool skipDrawingItem = false;
            bool highlighted = false;
            if (TreeWidgetItemSelected(body->GetTreeWidgetItem()))
            {
               SetHighlightedMaterialProperties(0.85);
               highlighted = true;

               if (!aOnlySelectedObject)
               {
                  skipDrawingItem = true;
               }
            }
            else
            {
               if (aOnlySelectedObject)
               {
                  skipDrawingItem = true;
               }
            }

            if (!skipDrawingItem)
            {
               if (aWireframe)
               {
                  if (highlighted)
                  {
                     SetSelectedWireframeColor();
                  }
                  else
                  {
                     SetNominalWireframeColor();
                  }
               }

               DrawBody(posX, posY, posZ, length, height, width, frontShape, frontLength, aftShape, rearLength, rearDiam, yaw_deg, pitch_deg, roll_deg, aWireframe);

               if (body->IsSymmetrical())
               {
                  // Consider the other fuselage/body
                  double currentBodyY = refPt.Y();
                  double delta = currentBodyY - symmetryHorizontalY;
                  double otherBodyY = currentBodyY - 2.0 * delta;

                  DrawBody(posX, otherBodyY, posZ, length, height, width, frontShape, frontLength, aftShape, rearLength, rearDiam, -yaw_deg, pitch_deg, -roll_deg, aWireframe);
               }
            }
         }
      }
      else if (geometryType == "GeometryNacelle")
      {
         Designer::GeometryNacelle* nacelle = (Designer::GeometryNacelle*)geometryObject;
         if (nacelle != nullptr)
         {
            UtVec3dX refPt_ft = nacelle->GetRefPoint_ft();
            double posX = refPt_ft.X();
            double posY = refPt_ft.Y();
            double posZ = refPt_ft.Z();
            double length = nacelle->GetLength_ft();
            double height = nacelle->GetHeight_ft();

            double heightInner = nacelle->GetHeightInner_ft();
            double heightOuter = nacelle->GetHeightOuter_ft();
            double forwardSweepLength_ft = nacelle->GetForwardSweepLength_ft();

            double width = nacelle->GetWidth_ft();
            double thickness = nacelle->GetThickness_ft();
            double yaw_deg = nacelle->GetYawAngle_deg();
            double pitch_deg = nacelle->GetPitchAngle_deg();
            double roll_deg = nacelle->GetRollAngle_deg();

            double symmetryHorizontalY = nacelle->GetSymmetryHorizontalY();

            std::string overallShapeString = nacelle->GetOverallShapeString().toStdString();
            std::string aftSectionString = nacelle->GetAftSectionString().toStdString();
            double aftSectionLength_ft = nacelle->GetAftSectionLength_ft();

            SetNominalMaterialProperties(0.85);
            bool skipDrawingItem = false;
            bool highlighted = false;
            if (TreeWidgetItemSelected(nacelle->GetTreeWidgetItem()))
            {
               SetHighlightedMaterialProperties(0.85);
               highlighted = true;

               if (!aOnlySelectedObject)
               {
                  skipDrawingItem = true;
               }
            }
            else
            {
               if (aOnlySelectedObject)
               {
                  skipDrawingItem = true;
               }
            }

            if (!skipDrawingItem)
            {
               if (aWireframe)
               {
                  if (highlighted)
                  {
                     SetSelectedWireframeColor();
                  }
                  else
                  {
                     SetNominalWireframeColor();
                  }
               }

               DrawNacelle(posX, posY, posZ, length, height, heightInner, heightOuter, width, thickness, forwardSweepLength_ft,
                           overallShapeString, aftSectionString, aftSectionLength_ft, yaw_deg, pitch_deg, roll_deg, aWireframe);
               if (nacelle->IsSymmetrical())
               {
                  // "Rounded" << "Half-Round-Right" << "Half-Round-Left" << "Half-Round-Top" << "Half-Round-Bottom"
                  // << "Flat-Sided" << "Flat-Swept-Right" << "Flat-Swept-Left"

                  // Consider the other nacelle/body
                  double currentBodyY = refPt_ft.Y();
                  double delta = currentBodyY - symmetryHorizontalY;
                  double otherBodyY = currentBodyY - 2.0 * delta;

                  if ((overallShapeString == "Half-Round-Right") || (overallShapeString == "Half-Round-Left") ||
                      (overallShapeString == "Flat-Swept-Right") || (overallShapeString == "Flat-Swept-Left"))
                  {
                     yaw_deg  = -yaw_deg;
                     roll_deg = 180.0 - roll_deg;
                  }
                  else
                  {
                     yaw_deg  = -yaw_deg;
                     roll_deg = -roll_deg;
                  }

                  DrawNacelle(posX, otherBodyY, posZ, length, height, heightInner, heightOuter, width, thickness, forwardSweepLength_ft,
                              overallShapeString, aftSectionString, aftSectionLength_ft, yaw_deg, pitch_deg, roll_deg, aWireframe);
               }
            }
         }
      }
   }
}

void GeometryGLWidget::DrawAeroData(Designer::Vehicle* aVehicle, bool aOnlySelectedObject)
{
   // First, find the selected geometry object
   QTreeWidgetItem* treeWidgetItem = GetSelectedTreeWidgetItem();

   VehicleGeometry& vehicleGeometry = aVehicle->GetGeometry();

   VehicleAero* vehicleAero = aVehicle->GetAero();
   if (vehicleAero == nullptr) { return; }

   // Determine size of arrows
   double arrowLength = 0.10 * mMaxExtent;
   double arrowDiam   = 0.02 * mMaxExtent;

   GeometryObject* selectedGeometryObj = vehicleGeometry.GetGeometryObjectWithWidget(treeWidgetItem);

   if (aOnlySelectedObject && (selectedGeometryObj == nullptr)) { return; }

   if (mShowSelectedAeroVectors && aOnlySelectedObject)
   {
      // Only work with the selected object
      std::string selectedObjectName = selectedGeometryObj->GetName();

      // Replace all spaces with underscores
      std::replace(selectedObjectName.begin(), selectedObjectName.end(), ' ', '_');

      // Get list of objects containing name
      const std::vector<AeroObject*> aeroObjList = vehicleAero->GetAeroObjectsGeneratedFrom(selectedObjectName);
      
      for (auto& objIter : aeroObjList)
      {
         AeroObject* aeroObject = objIter;
         DrawAeroDataForSingleObject(aVehicle, aeroObject, arrowLength, arrowDiam);
      }
   }
   else if (mShowAllAeroVectors && !mShowSelectedAeroVectors && !aOnlySelectedObject)
   {
      // Using all objects
      const std::map<size_t, AeroObject*>& aeroObjMap = vehicleAero->GetAeroObjectMap();
      for (auto& objIter : aeroObjMap)
      {
         AeroObject* aeroObject = objIter.second;
         DrawAeroDataForSingleObject(aVehicle, aeroObject, arrowLength, arrowDiam);
      }
   }
}

void GeometryGLWidget::DetermineYawPitchForVector(UtVec3dX aVector, double& aYaw_deg, double& aPitch_deg)
{
   double yaw_rad = 0.0;
   double pitch_rad = 0.0;
   double x = aVector.X();
   double y = aVector.Y();
   double z = aVector.Z();

   if (std::abs(x) < mEpsilon)
   {
      if (std::abs(y) < mEpsilon)
      {
         yaw_rad = 0.0;
      }
      else if (y > 0.0)
      {
         yaw_rad = UtMath::cPI_OVER_2;
      }
      else
      {
         yaw_rad = -UtMath::cPI_OVER_2;
      }
   }
   else
   {
      yaw_rad = atan2(y, x);
   }

   double xy = sqrt(x * x + y * y);

   if (std::abs(xy) < mEpsilon)
   {
      if (std::abs(z) < mEpsilon)
      {
         yaw_rad = 0.0;
         pitch_rad = 0.0;
      }
      else if (z < 0.0)
      {
         pitch_rad = UtMath::cPI_OVER_2;
      }
      else
      {
         pitch_rad = -UtMath::cPI_OVER_2;
      }
   }
   else
   {
      pitch_rad = atan(-z / xy);
   }

   aYaw_deg = yaw_rad * UtMath::cDEG_PER_RAD;
   aPitch_deg = pitch_rad * UtMath::cDEG_PER_RAD;
}

void GeometryGLWidget::DrawAeroVectors(UtVec3dX& aAeroCenterLowMach,
                                       UtVec3dX& aAeroCenterHighMach,
                                       UtVec3dX& aUpVec,
                                       UtVec3dX& aSideVec,
                                       UtVec3dX& aForwardVec,
                                       UtVec3dX& aVelocityVec,
                                       UtVec3dX& aDragForceVec,
                                       UtVec3dX& aSideForceVec,
                                       UtVec3dX& aLiftForceVec,
                                       double    aArrowLength,
                                       double    aArrowDiam)
{
   UtVec3dX zeroVec(0.0, 0.0, 0.0);

   // Draw up vector at low speed aero force point (Green Cyan)
   double yaw_deg = 0.0;
   double pitch_deg = 0.0;
   DetermineYawPitchForVector(aUpVec, yaw_deg, pitch_deg);
   PushMaterialProperties();
   SetGreenCyanMaterialProperties();
   glPushMatrix();
   glTranslated(aAeroCenterLowMach.X(), -aAeroCenterLowMach.Z(), aAeroCenterLowMach.Y());
   zeroVec.Set(0.0, 0.0, 0.0);
   glPushMatrix();
   glRotated(yaw_deg, 0.0, -1.0, 0.0);
   glRotated(pitch_deg, 0.0, 0.0, 1.0);
   DrawArrow(zeroVec, aArrowDiam * 1.2 * 0.75, aArrowLength * 1.5, aArrowDiam * 0.7 * 0.5);
   glPopMatrix();
   glPopMatrix();
   PopMaterialProperties();

   // Draw up vector at high speed aero force point (Blue-Cyan)
   yaw_deg = 0.0;
   pitch_deg = 0.0;
   DetermineYawPitchForVector(aUpVec, yaw_deg, pitch_deg);
   PushMaterialProperties();
   SetBlueCyanMaterialProperties();
   glPushMatrix();
   glTranslated(aAeroCenterHighMach.X(), -aAeroCenterHighMach.Z(), aAeroCenterHighMach.Y());
   zeroVec.Set(0.0, 0.0, 0.0);
   glPushMatrix();
   glRotated(yaw_deg, 0.0, -1.0, 0.0);
   glRotated(pitch_deg, 0.0, 0.0, 1.0);
   DrawArrow(zeroVec, aArrowDiam * 1.2 * 0.75, aArrowLength * 1.5, aArrowDiam * 0.7 * 0.5);
   glPopMatrix();
   glPopMatrix();
   PopMaterialProperties();

   // Only draw the rest if the vehicle is in wireframe mode
   if (mShowWireframe)
   {
      // Draw side vector at low speed aero force point (Cyan)
      yaw_deg = 0.0;
      pitch_deg = 0.0;
      DetermineYawPitchForVector(aSideVec, yaw_deg, pitch_deg);
      PushMaterialProperties();
      SetCyanMaterialProperties();
      glPushMatrix();
      glTranslated(aAeroCenterLowMach.X(), -aAeroCenterLowMach.Z(), aAeroCenterLowMach.Y());
      zeroVec.Set(0.0, 0.0, 0.0);
      glPushMatrix();
      glRotated(yaw_deg, 0.0, -1.0, 0.0);
      glRotated(pitch_deg, 0.0, 0.0, 1.0);
      DrawArrow(zeroVec, aArrowDiam * 1.2 * 0.75, aArrowLength * 1.5, aArrowDiam * 0.7 * 0.5);
      glPopMatrix();
      glPopMatrix();
      PopMaterialProperties();

      // Draw forward vector at low speed aero force point (Cyan)
      yaw_deg = 0.0;
      pitch_deg = 0.0;
      DetermineYawPitchForVector(aForwardVec, yaw_deg, pitch_deg);
      PushMaterialProperties();
      SetCyanMaterialProperties();
      glPushMatrix();
      glTranslated(aAeroCenterLowMach.X(), -aAeroCenterLowMach.Z(), aAeroCenterLowMach.Y());
      zeroVec.Set(0.0, 0.0, 0.0);
      glPushMatrix();
      glRotated(yaw_deg, 0.0, -1.0, 0.0);
      glRotated(pitch_deg, 0.0, 0.0, 1.0);
      DrawArrow(zeroVec, aArrowDiam * 1.2 * 0.75, aArrowLength * 1.5, aArrowDiam * 0.7 * 0.5);
      glPopMatrix();
      glPopMatrix();
      PopMaterialProperties();

      // Draw velocity vector at low speed aero force point (Orange)
      yaw_deg = 0.0;
      pitch_deg = 0.0;
      DetermineYawPitchForVector(aVelocityVec, yaw_deg, pitch_deg);
      PushMaterialProperties();
      SetOrangeMaterialProperties();
      glPushMatrix();
      glTranslated(aAeroCenterLowMach.X(), -aAeroCenterLowMach.Z(), aAeroCenterLowMach.Y());
      zeroVec.Set(0.0, 0.0, 0.0);
      glPushMatrix();
      glRotated(yaw_deg, 0.0, -1.0, 0.0);
      glRotated(pitch_deg, 0.0, 0.0, 1.0);
      DrawArrow(zeroVec, aArrowDiam * 1.2, aArrowLength * 0.5, aArrowDiam * 0.7);
      glPopMatrix();
      glPopMatrix();
      PopMaterialProperties();

      // Draw drag vector at low speed aero force point (Red)
      yaw_deg = 0.0;
      pitch_deg = 0.0;
      DetermineYawPitchForVector(aDragForceVec, yaw_deg, pitch_deg);
      PushMaterialProperties();
      SetRedMaterialProperties();
      glPushMatrix();
      glTranslated(aAeroCenterLowMach.X(), -aAeroCenterLowMach.Z(), aAeroCenterLowMach.Y());
      zeroVec.Set(0.0, 0.0, 0.0);
      glPushMatrix();
      glRotated(yaw_deg, 0.0, -1.0, 0.0);
      glRotated(pitch_deg, 0.0, 0.0, 1.0);
      DrawArrow(zeroVec, aArrowDiam * 1.2, aArrowLength, aArrowDiam * 0.7);
      glPopMatrix();
      glPopMatrix();
      PopMaterialProperties();

      // Draw side force vector at low speed aero force point (Blue)
      yaw_deg = 0.0;
      pitch_deg = 0.0;
      DetermineYawPitchForVector(aSideForceVec, yaw_deg, pitch_deg);
      PushMaterialProperties();
      SetBlueMaterialProperties();
      glPushMatrix();
      glTranslated(aAeroCenterLowMach.X(), -aAeroCenterLowMach.Z(), aAeroCenterLowMach.Y());
      zeroVec.Set(0.0, 0.0, 0.0);
      glPushMatrix();
      glRotated(yaw_deg, 0.0, -1.0, 0.0);
      glRotated(pitch_deg, 0.0, 0.0, 1.0);
      DrawArrow(zeroVec, aArrowDiam * 1.2, aArrowLength, aArrowDiam * 0.7);
      glPopMatrix();
      glPopMatrix();
      PopMaterialProperties();

      // Draw lift vector at low speed aero force point (Green)
      yaw_deg = 0.0;
      pitch_deg = 0.0;
      DetermineYawPitchForVector(aLiftForceVec, yaw_deg, pitch_deg);
      PushMaterialProperties();
      SetGreenMaterialProperties();
      glPushMatrix();
      glTranslated(aAeroCenterLowMach.X(), -aAeroCenterLowMach.Z(), aAeroCenterLowMach.Y());
      zeroVec.Set(0.0, 0.0, 0.0);
      glPushMatrix();
      glRotated(yaw_deg, 0.0, -1.0, 0.0);
      glRotated(pitch_deg, 0.0, 0.0, 1.0);
      DrawArrow(zeroVec, aArrowDiam * 1.2, aArrowLength, aArrowDiam * 0.7);
      glPopMatrix();
      glPopMatrix();
      PopMaterialProperties();
   }
}

void GeometryGLWidget::DrawAeroVectors(UtVec3dX& aAeroCenter,
                                       UtVec3dX& aUpVec,
                                       UtVec3dX& aSideVec,
                                       UtVec3dX& aForwardVec,
                                       UtVec3dX& aVelocityVec,
                                       UtVec3dX& aDragForceVec,
                                       UtVec3dX& aSideForceVec,
                                       UtVec3dX& aLiftForceVec,
                                       double    aArrowLength,
                                       double    aArrowDiam)
{
   UtVec3dX zeroVec(0.0, 0.0, 0.0);

   // Draw up vector at aero force point (Green-Cyan)
   double yaw_deg = 0.0;
   double pitch_deg = 0.0;
   DetermineYawPitchForVector(aUpVec, yaw_deg, pitch_deg);
   PushMaterialProperties();
   SetGreenCyanMaterialProperties();
   glPushMatrix();
   glTranslated(aAeroCenter.X(), -aAeroCenter.Z(), aAeroCenter.Y());
   zeroVec.Set(0.0, 0.0, 0.0);
   glPushMatrix();
   glRotated(yaw_deg, 0.0, -1.0, 0.0);
   glRotated(pitch_deg, 0.0, 0.0, 1.0);
   DrawArrow(zeroVec, aArrowDiam * 1.2 * 0.75, aArrowLength * 1.5, aArrowDiam * 0.7 * 0.5);
   glPopMatrix();
   glPopMatrix();
   PopMaterialProperties();

   // Draw side vector at aero force point (Cyan)
   yaw_deg = 0.0;
   pitch_deg = 0.0;
   DetermineYawPitchForVector(aSideVec, yaw_deg, pitch_deg);
   PushMaterialProperties();
   SetCyanMaterialProperties();
   glPushMatrix();
   glTranslated(aAeroCenter.X(), -aAeroCenter.Z(), aAeroCenter.Y());
   zeroVec.Set(0.0, 0.0, 0.0);
   glPushMatrix();
   glRotated(yaw_deg, 0.0, -1.0, 0.0);
   glRotated(pitch_deg, 0.0, 0.0, 1.0);
   DrawArrow(zeroVec, aArrowDiam * 1.2 * 0.75, aArrowLength * 1.5, aArrowDiam * 0.7 * 0.5);
   glPopMatrix();
   glPopMatrix();
   PopMaterialProperties();

   // Draw forward vector at aero force point (Cyan)
   yaw_deg = 0.0;
   pitch_deg = 0.0;
   DetermineYawPitchForVector(aForwardVec, yaw_deg, pitch_deg);
   PushMaterialProperties();
   SetCyanMaterialProperties();
   glPushMatrix();
   glTranslated(aAeroCenter.X(), -aAeroCenter.Z(), aAeroCenter.Y());
   zeroVec.Set(0.0, 0.0, 0.0);
   glPushMatrix();
   glRotated(yaw_deg, 0.0, -1.0, 0.0);
   glRotated(pitch_deg, 0.0, 0.0, 1.0);
   DrawArrow(zeroVec, aArrowDiam * 1.2 * 0.75, aArrowLength * 1.5, aArrowDiam * 0.7 * 0.5);
   glPopMatrix();
   glPopMatrix();
   PopMaterialProperties();

   // Draw forward vector at aero force point (Orange)
   yaw_deg = 0.0;
   pitch_deg = 0.0;
   DetermineYawPitchForVector(aVelocityVec, yaw_deg, pitch_deg);
   PushMaterialProperties();
   SetOrangeMaterialProperties();
   glPushMatrix();
   glTranslated(aAeroCenter.X(), -aAeroCenter.Z(), aAeroCenter.Y());
   zeroVec.Set(0.0, 0.0, 0.0);
   glPushMatrix();
   glRotated(yaw_deg, 0.0, -1.0, 0.0);
   glRotated(pitch_deg, 0.0, 0.0, 1.0);
   DrawArrow(zeroVec, aArrowDiam * 1.2, aArrowLength * 0.5, aArrowDiam * 0.7);
   glPopMatrix();
   glPopMatrix();
   PopMaterialProperties();

   // Draw drag vector at aero force point (Red)
   yaw_deg = 0.0;
   pitch_deg = 0.0;
   DetermineYawPitchForVector(aDragForceVec, yaw_deg, pitch_deg);
   PushMaterialProperties();
   SetRedMaterialProperties();
   glPushMatrix();
   glTranslated(aAeroCenter.X(), -aAeroCenter.Z(), aAeroCenter.Y());
   zeroVec.Set(0.0, 0.0, 0.0);
   glPushMatrix();
   glRotated(yaw_deg, 0.0, -1.0, 0.0);
   glRotated(pitch_deg, 0.0, 0.0, 1.0);
   DrawArrow(zeroVec, aArrowDiam * 1.2, aArrowLength, aArrowDiam * 0.7);
   glPopMatrix();
   glPopMatrix();
   PopMaterialProperties();

   // Draw side force vector at aero force point (Blue)
   yaw_deg = 0.0;
   pitch_deg = 0.0;
   DetermineYawPitchForVector(aSideForceVec, yaw_deg, pitch_deg);
   PushMaterialProperties();
   SetBlueMaterialProperties();
   glPushMatrix();
   glTranslated(aAeroCenter.X(), -aAeroCenter.Z(), aAeroCenter.Y());
   zeroVec.Set(0.0, 0.0, 0.0);
   glPushMatrix();
   glRotated(yaw_deg, 0.0, -1.0, 0.0);
   glRotated(pitch_deg, 0.0, 0.0, 1.0);
   DrawArrow(zeroVec, aArrowDiam * 1.2, aArrowLength, aArrowDiam * 0.7);
   glPopMatrix();
   glPopMatrix();
   PopMaterialProperties();

   // Draw lift vector at aero force point (Green)
   yaw_deg = 0.0;
   pitch_deg = 0.0;
   DetermineYawPitchForVector(aLiftForceVec, yaw_deg, pitch_deg);
   PushMaterialProperties();
   SetGreenMaterialProperties();
   glPushMatrix();
   glTranslated(aAeroCenter.X(), -aAeroCenter.Z(), aAeroCenter.Y());
   zeroVec.Set(0.0, 0.0, 0.0);
   glPushMatrix();
   glRotated(yaw_deg, 0.0, -1.0, 0.0);
   glRotated(pitch_deg, 0.0, 0.0, 1.0);
   DrawArrow(zeroVec, aArrowDiam * 1.2, aArrowLength, aArrowDiam * 0.7);
   glPopMatrix();
   glPopMatrix();
   PopMaterialProperties();
}

void GeometryGLWidget::DrawAeroVectors(UtVec3dX& aAeroCenter,
                                       UtVec3dX& aDragForceVec,
                                       double    aArrowLength,
                                       double    aArrowDiam)
{
   UtVec3dX zeroVec(0.0, 0.0, 0.0);

   // Draw drag vector at low speed aero force point (Red)
   double yaw_deg = 0.0;
   double pitch_deg = 0.0;
   DetermineYawPitchForVector(aDragForceVec, yaw_deg, pitch_deg);
   PushMaterialProperties();
   SetRedMaterialProperties();
   glPushMatrix();
   glTranslated(aAeroCenter.X(), -aAeroCenter.Z(), aAeroCenter.Y());
   zeroVec.Set(0.0, 0.0, 0.0);
   glPushMatrix();
   glRotated(yaw_deg, 0.0, -1.0, 0.0);
   glRotated(pitch_deg, 0.0, 0.0, 1.0);
   DrawArrow(zeroVec, aArrowDiam * 1.2, aArrowLength, aArrowDiam * 0.7);
   glPopMatrix();
   glPopMatrix();
   PopMaterialProperties();
}

void GeometryGLWidget::DrawAeroDataForSingleObject(Designer::Vehicle* aVehicle,
                                                   AeroObject*        aAeroObject,
                                                   double             aArrowLength,
                                                   double             aArrowDiam)

{
   VehicleAero* vehicleAero = aVehicle->GetAero();
   if (vehicleAero == nullptr) { return; }

   if (vehicleAero)
   {
      std::string objectType = aAeroObject->AeroObjType();

      const double MachZero = 0.0;
      const double MachTwo  = 2.0;

      if ((objectType == "AeroSurface") || 
          (objectType == "AeroWing") || 
          (objectType == "AeroFullFlyingSurface") || 
          (objectType == "AeroHingedSurface"))
      {
         AeroSurface* aeroSurface = dynamic_cast<AeroSurface*>(aAeroObject);
         if (aeroSurface == nullptr) { return; }

         UtVec3dX aeroCenterMachZero_ft = aeroSurface->CalcAeroForcePoint_ft(MachZero);
         UtVec3dX aeroCenterMachTwo_ft  = aeroSurface->CalcAeroForcePoint_ft(MachTwo);
         UtVec3dX upVec        = aeroSurface->GetUpUnitVector();
         UtVec3dX sideVec      = aeroSurface->GetSideUnitVector();
         UtVec3dX forwardVec   = aeroSurface->GetForwardUnitVector();
         UtVec3dX velVec       = aeroSurface->GetVelocityUnitVector();
         UtVec3dX sweepVec     = aeroSurface->GetSweepUnitVector();
         UtVec3dX dragForceVec = aeroSurface->GetDragForceUnitVector();
         UtVec3dX sideForceVec = aeroSurface->GetSideForceUnitVector();
         UtVec3dX liftForceVec = aeroSurface->GetLiftForceUnitVector();

         DrawAeroVectors(aeroCenterMachZero_ft, aeroCenterMachTwo_ft, upVec, sideVec, forwardVec,
                         velVec, dragForceVec, sideForceVec, liftForceVec, aArrowLength, aArrowDiam);

         // Draw sweep vector
         if (mShowWireframe)
         {
            double yaw_deg = 0.0;
            double pitch_deg = 0.0;
            DetermineYawPitchForVector(sweepVec, yaw_deg, pitch_deg);
            PushMaterialProperties();
            SetYellowMaterialProperties();
            glPushMatrix();
            glTranslated(aeroCenterMachZero_ft.X(), -aeroCenterMachZero_ft.Z(), aeroCenterMachZero_ft.Y());
            UtVec3dX zeroVec(0.0, 0.0, 0.0);
            glPushMatrix();
            glRotated(yaw_deg, 0.0, -1.0, 0.0);
            glRotated(pitch_deg, 0.0, 0.0, 1.0);
            DrawArrow(zeroVec, aArrowDiam * 1.2 * 0.75, aArrowLength * 1.25, aArrowDiam * 0.7 * 0.5);
            glPopMatrix();
            glPopMatrix();
            PopMaterialProperties();
         }
      }
      else if (objectType == "AeroBody" && mShowWireframe)
      {
         AeroBody* aeroBody = dynamic_cast<AeroBody*>(aAeroObject);
         if (aeroBody == nullptr) { return; }

         UtVec3dX aeroCenter_ft = aeroBody->CalcAeroForcePoint_ft(MachZero);
         UtVec3dX upVec         = aeroBody->GetUpUnitVector();
         UtVec3dX sideVec       = aeroBody->GetSideUnitVector();
         UtVec3dX forwardVec    = aeroBody->GetForwardUnitVector();
         UtVec3dX velVec        = aeroBody->GetVelocityUnitVector();
         UtVec3dX dragForceVec  = aeroBody->GetDragForceUnitVector();
         UtVec3dX sideForceVec  = aeroBody->GetSideForceUnitVector();
         UtVec3dX liftForceVec  = aeroBody->GetLiftForceUnitVector();

         DrawAeroVectors(aeroCenter_ft, upVec, sideVec, forwardVec, velVec, dragForceVec, sideForceVec,
                         liftForceVec, aArrowLength, aArrowDiam);
      }
      else if (objectType == "AeroNacelle" && mShowWireframe)
      {
         AeroNacelle* aeroNacelle = dynamic_cast<AeroNacelle*>(aAeroObject);
         if (aeroNacelle == nullptr) { return; }

         UtVec3dX aeroCenter_ft = aeroNacelle->CalcAeroForcePoint_ft(MachZero);
         UtVec3dX upVec         = aeroNacelle->GetUpUnitVector();
         UtVec3dX sideVec       = aeroNacelle->GetSideUnitVector();
         UtVec3dX forwardVec    = aeroNacelle->GetForwardUnitVector();
         UtVec3dX velVec        = aeroNacelle->GetVelocityUnitVector();
         UtVec3dX dragForceVec  = aeroNacelle->GetDragForceUnitVector();
         UtVec3dX sideForceVec  = aeroNacelle->GetSideForceUnitVector();
         UtVec3dX liftForceVec  = aeroNacelle->GetLiftForceUnitVector();

         DrawAeroVectors(aeroCenter_ft, upVec, sideVec, forwardVec, velVec, dragForceVec, sideForceVec,
                         liftForceVec, aArrowLength, aArrowDiam);
      }
      else if (objectType == "AeroDish" && mShowWireframe)
      {
         AeroDish* aeroDish = dynamic_cast<AeroDish*>(aAeroObject);
         if (aeroDish == nullptr) { return; }

         UtVec3dX aeroCenter_ft = aeroDish->CalcAeroForcePoint_ft(MachZero);
         UtVec3dX upVec         = aeroDish->GetUpUnitVector();
         UtVec3dX sideVec       = aeroDish->GetSideUnitVector();
         UtVec3dX forwardVec    = aeroDish->GetForwardUnitVector();
         UtVec3dX velVec        = aeroDish->GetVelocityUnitVector();
         UtVec3dX dragForceVec  = aeroDish->GetDragForceUnitVector();
         UtVec3dX sideForceVec  = aeroDish->GetSideForceUnitVector();
         UtVec3dX liftForceVec  = aeroDish->GetLiftForceUnitVector();

         DrawAeroVectors(aeroCenter_ft, upVec, sideVec, forwardVec, velVec, dragForceVec, sideForceVec,
                         liftForceVec, aArrowLength, aArrowDiam);
      }
      else if (objectType == "AeroLandingGear" && mShowWireframe)
      {
         AeroLandingGear* aeroLandingGear = dynamic_cast<AeroLandingGear*>(aAeroObject);
         if (aeroLandingGear == nullptr) { return; }

         double maxAngle_deg        = aeroLandingGear->GetMaxAngle_deg();
         UtVec3dX aeroCenterUp_ft   = aeroLandingGear->CalcAngleBasedAeroForcePoint_ft(MachZero, 0.0);
         UtVec3dX aeroCenterDown_ft = aeroLandingGear->CalcAngleBasedAeroForcePoint_ft(MachZero, maxAngle_deg);
         UtVec3dX dragForceVec      = aeroLandingGear->GetDragForceUnitVector();

         DrawAeroVectors(aeroCenterDown_ft, dragForceVec, aArrowLength, aArrowDiam);
         DrawAeroVectors(aeroCenterUp_ft, dragForceVec, aArrowLength, aArrowDiam);
      }
      else if (objectType == "AeroSpeedBrake" && mShowWireframe)
      {
         AeroSpeedBrake* aeroSpeedBrake = dynamic_cast<AeroSpeedBrake*>(aAeroObject);
         if (aeroSpeedBrake == nullptr) { return; }

         // Set the speed brake position to full extension
         double vehicleAlpha_deg = 0.0;
         double vehicleBeta_deg  = 0.0;
         aVehicle->GetVehicleAlphaBeta(vehicleAlpha_deg, vehicleBeta_deg);
         aeroSpeedBrake->SetNormalizedControlValue(1.0);
         aeroSpeedBrake->SetVehicleAlphaBeta(vehicleAlpha_deg, vehicleBeta_deg);

         double maxAngle_deg            = aeroSpeedBrake->GetMaxAngle_deg();
         UtVec3dX aeroCenterClosed_ft   = aeroSpeedBrake->CalcAngleBasedAeroForcePoint_ft(MachZero, 0.0);
         UtVec3dX aeroCenterOpen_ft     = aeroSpeedBrake->CalcAngleBasedAeroForcePoint_ft(MachZero, maxAngle_deg);
         UtVec3dX upVec                 = aeroSpeedBrake->GetUpUnitVector();
         UtVec3dX sideVec               = aeroSpeedBrake->GetSideUnitVector();
         UtVec3dX forwardVec            = aeroSpeedBrake->GetForwardUnitVector();
         UtVec3dX velVec                = aeroSpeedBrake->GetVelocityUnitVector();
         UtVec3dX dragForceVec          = aeroSpeedBrake->GetDragForceUnitVector();
         UtVec3dX sideForceVec          = aeroSpeedBrake->GetSideForceUnitVector();
         UtVec3dX liftForceVec          = aeroSpeedBrake->GetLiftForceUnitVector();

         DrawAeroVectors(aeroCenterOpen_ft, upVec, sideVec, forwardVec, velVec, dragForceVec, sideForceVec,
                         liftForceVec, aArrowLength, aArrowDiam);
         DrawAeroVectors(aeroCenterClosed_ft, dragForceVec, aArrowLength, aArrowDiam);
      }
   }
}

void GeometryGLWidget::DrawWingsAndSurfaces(Designer::Vehicle* aVehicle, bool aOnlySelectedObject, bool aWireframe)
{
   VehicleGeometry& vehicleGeometry = aVehicle->GetGeometry();
   const std::map<size_t, GeometryObject*>& geometryObjMap = vehicleGeometry.GetGeometryObjectMap();

   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;
      std::string geometryType = geometryObject->GeometryObjectType();

      if (geometryType == "GeometryWing")
      {
         Designer::GeometryWing* wing = dynamic_cast<GeometryWing*>(geometryObject);

         if (wing != nullptr)
         {
            UtVec3dX refPt             = wing->GetRefPoint_ft();
            double aPosX               = refPt.X();
            double aPosY               = refPt.Y();
            double aPosZ               = refPt.Z();
            double aSpan               = wing->GetSpan_ft() * 0.5; // Note that a wingspan is different from other surfaces span values
            double aSweep_deg          = wing->GetSweepAngle_deg();
            double aRootChord          = wing->GetRootChord_ft();
            double aTipChord           = wing->GetTipChord_ft();
            double aThicknessRatio     = wing->GetThicknessRatio();
            double aDihedral_deg       = wing->GetDihedralAngle_deg();
            double aIncidence_deg      = wing->GetIncidenceAngle_deg();
            double symmetryHorizontalY = wing->GetSymmetryHorizontalY();

            // Control Surfaces
            std::vector<GeometrySurface::GeometryPanel> controlSurfaces = wing->GetPanels();

            SetNominalMaterialProperties();
            bool skipDrawingItem = false;
            bool highlighted = false;
            if (TreeWidgetItemSelected(wing->GetTreeWidgetItem()))
            {
               SetHighlightedMaterialProperties();
               highlighted = true;
               if (!aOnlySelectedObject)
               {
                  skipDrawingItem = true;
               }
            }
            else
            {
               if (aOnlySelectedObject)
               {
                  skipDrawingItem = true;
               }
            }

            if (!skipDrawingItem)
            {
               if (aWireframe)
               {
                  if (highlighted)
                  {
                     SetSelectedWireframeColor();
                  }
                  else
                  {
                     SetNominalWireframeColor();
                  }
               }

               DrawSurface(aPosX, aPosY, aPosZ, aSpan, aSweep_deg, aRootChord, aTipChord, aThicknessRatio,
                           aDihedral_deg, aIncidence_deg, aWireframe, 20, controlSurfaces);
               if (wing->IsSymmetrical())
               {
                  // Now, determine location of other surface
                  double currentSurfaceY = refPt.Y();
                  double delta = currentSurfaceY - symmetryHorizontalY;
                  double otherSurfaceY = currentSurfaceY - 2.0 * delta;

                  DrawSurface(aPosX, otherSurfaceY, aPosZ, aSpan, aSweep_deg, aRootChord, aTipChord, aThicknessRatio,
                              -180.0 - aDihedral_deg, -aIncidence_deg, aWireframe, 20, controlSurfaces);
               }
            }
         }
      }
      else if (geometryType == "GeometrySurface")
      {
         Designer::GeometrySurface* surface = dynamic_cast<Designer::GeometrySurface*>(geometryObject);

         if (surface != nullptr)
         {
            UtVec3dX refPt = surface->GetRefPoint_ft();
            double aPosX = refPt.X();
            double aPosY = refPt.Y();
            double aPosZ = refPt.Z();
            double aSpan = surface->GetSpan_ft();
            double aSweep_deg = surface->GetSweepAngle_deg();
            double aRootChord = surface->GetRootChord_ft();
            double aTipChord = surface->GetTipChord_ft();
            double aThicknessRatio = surface->GetThicknessRatio();
            double aDihedral_deg = surface->GetDihedralAngle_deg();
            double aIncidence_deg = surface->GetIncidenceAngle_deg();
            double aAltPosRefRadius = surface->GetFinRefRadius_ft();
            double symmetryHorizontalY = surface->GetSymmetryHorizontalY();
            double symmetryVerticalZ = surface->GetSymmetryVerticalZ();

            // Control Surfaces
            std::vector<GeometrySurface::GeometryPanel> controlSurfaces = surface->GetPanels();

            // Get data for the control angle(s)
            double controlAngle1_deg = 0.0;
            double controlAngle2_deg = 0.0;
            double controlAngle3_deg = 0.0;
            double controlAngle4_deg = 0.0;

            // Check for the control angle
            VehicleAero* vehicleAero = aVehicle->GetAero();
            if (vehicleAero != nullptr)
            {
               const std::vector<AeroObject*> list = vehicleAero->GetAeroObjectsGeneratedFrom(surface->GetName());
               size_t numControls = list.size();
               if (numControls > 0)
               {
                  controlAngle1_deg = list.at(0)->GetControlAngle_deg();
                  if (numControls > 1)
                  {
                     controlAngle2_deg = list.at(1)->GetControlAngle_deg();
                  }
                  if (numControls > 2)
                  {
                     controlAngle3_deg = list.at(2)->GetControlAngle_deg();
                  }
                  if (numControls > 3)
                  {
                     controlAngle4_deg = list.at(3)->GetControlAngle_deg();
                  }
               }
            }
            else
            {
               MoverCreatorMainWindow().CreateErrorBox("Vehicle does not include a valid aero component!");
            }

            std::string symmetryString = surface->GetSymmetryString().toStdString();

            // Default pattern is + pattern
            double finAngle_rad = 0.0;
            if (symmetryString == "X Pattern") { finAngle_rad = UtMath::cPI_OVER_4; }

            double finPos1X = refPt.X();
            double finPos1Y = refPt.Y() + aAltPosRefRadius * cos(finAngle_rad);
            double finPos1Z = refPt.Z() - aAltPosRefRadius * sin(finAngle_rad);
            double finDihedral1_deg = finAngle_rad * UtMath::cDEG_PER_RAD;

            finAngle_rad += UtMath::cPI_OVER_2;
            double finPos2X = refPt.X();
            double finPos2Y = refPt.Y() + aAltPosRefRadius * cos(finAngle_rad);
            double finPos2Z = refPt.Z() - aAltPosRefRadius * sin(finAngle_rad);
            double finDihedral2_deg = finAngle_rad * UtMath::cDEG_PER_RAD;

            finAngle_rad += UtMath::cPI_OVER_2;
            double finPos3X = refPt.X();
            double finPos3Y = refPt.Y() + aAltPosRefRadius * cos(finAngle_rad);
            double finPos3Z = refPt.Z() - aAltPosRefRadius * sin(finAngle_rad);
            double finDihedral3_deg = finAngle_rad * UtMath::cDEG_PER_RAD;

            finAngle_rad += UtMath::cPI_OVER_2;
            double finPos4X = refPt.X();
            double finPos4Y = refPt.Y() + aAltPosRefRadius * cos(finAngle_rad);
            double finPos4Z = refPt.Z() - aAltPosRefRadius * sin(finAngle_rad);
            double finDihedral4_deg = finAngle_rad * UtMath::cDEG_PER_RAD;

            SetNominalMaterialProperties();
            bool skipDrawingItem = false;
            bool highlighted = false;
            if (TreeWidgetItemSelected(surface->GetTreeWidgetItem()))
            {
               SetHighlightedMaterialProperties();

               highlighted = true;
               if (!aOnlySelectedObject)
               {
                  skipDrawingItem = true;
               }
            }
            else
            {
               if (aOnlySelectedObject)
               {
                  skipDrawingItem = true;
               }
            }

            if (!skipDrawingItem)
            {
               if (aWireframe)
               {
                  if (highlighted)
                  {
                     SetSelectedWireframeColor();
                  }
                  else
                  {
                     SetNominalWireframeColor();
                  }
               }

               if ((symmetryString == "Single") || (symmetryString == "Horizontal") || (symmetryString == "Vertical"))
               {
                  double incidence_deg = aIncidence_deg + controlAngle1_deg;
                  DrawSurface(aPosX, aPosY, aPosZ, aSpan, aSweep_deg, aRootChord, aTipChord, aThicknessRatio,
                              aDihedral_deg, incidence_deg, aWireframe, 5, controlSurfaces);
               }
               else if ((symmetryString == "+ Pattern") || (symmetryString == "X Pattern"))
               {
                  double incidence_deg = aIncidence_deg + controlAngle1_deg;
                  DrawSurface(finPos1X, finPos1Y, finPos1Z, aSpan, aSweep_deg, aRootChord, aTipChord, aThicknessRatio,
                              finDihedral1_deg, incidence_deg, aWireframe, 5, controlSurfaces);
               }

               if (surface->IsSymmetrical())
               {
                  if (symmetryString == "Horizontal")
                  {
                     // Now, determine location of other surface
                     double currentSurfaceY = refPt.Y();
                     double delta = currentSurfaceY - symmetryHorizontalY;
                     double otherSurfaceY = currentSurfaceY - 2.0 * delta;

                     double incidence_deg = -aIncidence_deg - controlAngle2_deg;
                     DrawSurface(aPosX, otherSurfaceY, aPosZ, aSpan, aSweep_deg, aRootChord, aTipChord, aThicknessRatio,
                                 -180.0 - aDihedral_deg, incidence_deg, aWireframe, 5, controlSurfaces);
                  }
                  else if (symmetryString == "Vertical")
                  {
                     // Now, determine location of other surface
                     double currentSurfaceZ = refPt.Z();
                     double delta = currentSurfaceZ - symmetryVerticalZ;
                     double otherSurfaceZ = currentSurfaceZ - 2.0 * delta;

                     double incidence_deg = -aIncidence_deg - controlAngle2_deg;
                     DrawSurface(aPosX, aPosY, otherSurfaceZ, aSpan, aSweep_deg, aRootChord, aTipChord, aThicknessRatio,
                                 -aDihedral_deg, incidence_deg, aWireframe, 5, controlSurfaces);
                  }
                  else if ((symmetryString == "+ Pattern") || (symmetryString == "X Pattern"))
                  {
                     double incidence_deg = aIncidence_deg + controlAngle2_deg;
                     DrawSurface(finPos2X, finPos2Y, finPos2Z, aSpan, aSweep_deg, aRootChord, aTipChord, aThicknessRatio,
                                 finDihedral2_deg, incidence_deg, aWireframe, 5, controlSurfaces);

                     incidence_deg = aIncidence_deg + controlAngle3_deg;
                     DrawSurface(finPos3X, finPos3Y, finPos3Z, aSpan, aSweep_deg, aRootChord, aTipChord, aThicknessRatio,
                                 finDihedral3_deg, incidence_deg, aWireframe, 5, controlSurfaces);

                     incidence_deg = aIncidence_deg + controlAngle4_deg;
                     DrawSurface(finPos4X, finPos4Y, finPos4Z, aSpan, aSweep_deg, aRootChord, aTipChord, aThicknessRatio,
                                 finDihedral4_deg, incidence_deg, aWireframe, 5, controlSurfaces);
                  }
               }
            }
         }
      }
   }
}

void GeometryGLWidget::DrawSpeedBrakes(Designer::Vehicle* aVehicle, bool aOnlySelectedObject, bool aWireframe)
{
   VehicleGeometry& vehicleGeometry = aVehicle->GetGeometry();
   const std::map<size_t, GeometryObject*>& geometryObjMap = vehicleGeometry.GetGeometryObjectMap();

   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;
      std::string geometryType = geometryObject->GeometryObjectType();
      if (geometryType == "GeometrySpeedBrake")
      {
         Designer::GeometrySpeedBrake* brake = (Designer::GeometrySpeedBrake*)geometryObject;

         if (brake != nullptr)
         {
            UtVec3dX refPt = brake->GetRefPoint_ft();
            double aPosX = refPt.X();
            double aPosY = refPt.Y();
            double aPosZ = refPt.Z();

            double aLength_ft = brake->GetLength_ft();
            double aWidth_ft = brake->GetWidth_ft();
            double aRoll_deg = brake->GetRollAngle_deg();
            double aMaxAngle_deg = brake->GetMaxAngle_deg();

            double symmetryHorizontalY = brake->GetSymmetryHorizontalY();
            double symmetryVerticalZ = brake->GetSymmetryVerticalZ();

            bool partIsSelected = false;
            SetNominalMaterialProperties();
            bool skipDrawingItem = false;
            bool highlighted = false;
            if (TreeWidgetItemSelected(brake->GetTreeWidgetItem()))
            {
               SetHighlightedMaterialProperties();
               highlighted = true;
               partIsSelected = true;
               if (!aOnlySelectedObject)
               {
                  skipDrawingItem = true;
               }
            }
            else
            {
               if (aOnlySelectedObject)
               {
                  skipDrawingItem = true;
               }
            }

            if (!skipDrawingItem)
            {
               if (mShowSpeedBrakes || partIsSelected)
               {
                  if (aWireframe)
                  {
                     if (highlighted)
                     {
                        SetSelectedWireframeColor();
                     }
                     else
                     {
                        SetNominalWireframeColor();
                     }
                  }

                  DrawSpeedBrake(aPosX, aPosY, aPosZ, aLength_ft, aWidth_ft, aRoll_deg, aMaxAngle_deg, aWireframe);

                  if (brake->IsHorizontallySymmetrical())
                  {
                     // Now, determine location of other speed brake
                     double currentBrakeY = refPt.Y();
                     double delta = currentBrakeY - symmetryHorizontalY;
                     double otherBrakeY = currentBrakeY - 2.0 * delta;

                     DrawSpeedBrake(aPosX, otherBrakeY, aPosZ, aLength_ft, aWidth_ft, -aRoll_deg, aMaxAngle_deg, aWireframe);
                  }
                  if (brake->IsVerticallySymmetrical())
                  {
                     // Now, determine location of other speed brake
                     double currentBrakeZ = refPt.Z();
                     double delta = currentBrakeZ - symmetryVerticalZ;
                     double otherBrakeZ = currentBrakeZ - 2.0 * delta;

                     DrawSpeedBrake(aPosX, aPosY, otherBrakeZ, aLength_ft, aWidth_ft, 180.0 - aRoll_deg, aMaxAngle_deg, aWireframe);
                  }
               }
            }
         }
      }
   }
}

void GeometryGLWidget::DrawLandingGear(Designer::Vehicle* aVehicle, bool aOnlySelectedObject, bool aWireframe)
{
   VehicleGeometry& vehicleGeometry = aVehicle->GetGeometry();
   const std::map<size_t, GeometryObject*>& geometryObjMap = vehicleGeometry.GetGeometryObjectMap();

   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;
      std::string geometryType = geometryObject->GeometryObjectType();
      if (geometryType == "GeometryLandingGear")
      {
         Designer::GeometryLandingGear* gear = (Designer::GeometryLandingGear*)geometryObject;

         if (gear != nullptr)
         {
            UtVec3dX refPt = gear->GetRefPoint_ft();
            double aPosX = refPt.X();
            double aPosY = refPt.Y();
            double aPosZ = refPt.Z();

            double aLength_ft = gear->GetUncompressedLength_ft();
            double aStrutDiam_ft = gear->GetStrutDiam_ft();
            double aTireDiam_ft = gear->GetTireDiam_ft();
            double aTireWidth_ft = gear->GetTireWidth_ft();
            double aExtendedAngle_deg = gear->GetMaxAngle_deg();

            double symmetryHorizontalY = gear->GetSymmetryHorizontalY();

            SetNominalMaterialProperties();
            bool partIsSelected = false;
            bool skipDrawingItem = false;
            bool highlighted = false;
            if (TreeWidgetItemSelected(gear->GetTreeWidgetItem()))
            {
               SetHighlightedMaterialProperties();
               highlighted = true;
               partIsSelected = true;
               if (!aOnlySelectedObject)
               {
                  skipDrawingItem = true;
               }
            }
            else
            {
               if (aOnlySelectedObject)
               {
                  skipDrawingItem = true;
               }
            }

            if (!skipDrawingItem)
            {
               if (mShowLandingGear || partIsSelected)
               {
                  if (aWireframe)
                  {
                     if (highlighted)
                     {
                        SetSelectedWireframeColor();
                     }
                     else
                     {
                        SetNominalWireframeColor();
                     }
                  }

                  DrawLandingGear(aPosX, aPosY, aPosZ, aLength_ft, aStrutDiam_ft, aTireDiam_ft, aTireWidth_ft, aExtendedAngle_deg, aWireframe);
                  if (gear->IsSymmetrical())
                  {
                     double currentGearY = refPt.Y();
                     double delta = currentGearY - symmetryHorizontalY;
                     double otherGearY = currentGearY - 2.0 * delta;

                     DrawLandingGear(aPosX, otherGearY, aPosZ, aLength_ft, aStrutDiam_ft, aTireDiam_ft, aTireWidth_ft, aExtendedAngle_deg, aWireframe);
                  }
               }
            }
         }
      }
   }
}

void GeometryGLWidget::DrawDishes(Designer::Vehicle* aVehicle, bool aOnlySelectedObject, bool aWireframe)
{
   VehicleGeometry& vehicleGeometry = aVehicle->GetGeometry();
   const std::map<size_t, GeometryObject*>& geometryObjMap = vehicleGeometry.GetGeometryObjectMap();

   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;
      std::string geometryType = geometryObject->GeometryObjectType();
      if (geometryType == "GeometryDish")
      {
         Designer::GeometryDish* dish = (Designer::GeometryDish*)geometryObject;

         if (dish != nullptr)
         {
            UtVec3dX refPt = dish->GetRefPoint_ft();
            double aPosX = refPt.X();
            double aPosY = refPt.Y();
            double aPosZ = refPt.Z();

            double aDiameter_ft = dish->GetDiameter_ft();
            double aThickness_ft = dish->GetThickness_ft();

            SetNominalMaterialProperties();
            bool skipDrawingItem = false;

            bool highlighted = false;
            if (TreeWidgetItemSelected(dish->GetTreeWidgetItem()))
            {
               SetHighlightedMaterialProperties();
               highlighted = true;
               if (!aOnlySelectedObject)
               {
                  skipDrawingItem = true;
               }
            }
            else
            {
               if (aOnlySelectedObject)
               {
                  skipDrawingItem = true;
               }
            }

            if (!skipDrawingItem)
            {
               if (aWireframe)
               {
                  if (highlighted)
                  {
                     SetSelectedWireframeColor();
                  }
                  else
                  {
                     SetNominalWireframeColor();
                  }
               }

               DrawDish(aPosX, aPosY, aPosZ, aDiameter_ft, aThickness_ft, aWireframe);
            }
         }
      }
   }
}

void GeometryGLWidget::DrawCanopy(Designer::Vehicle* aVehicle, bool aOnlySelectedObject, bool aWireframe)
{
   VehicleGeometry& vehicleGeometry = aVehicle->GetGeometry();
   const std::map<size_t, GeometryObject*>& geometryObjMap = vehicleGeometry.GetGeometryObjectMap();

   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;
      std::string geometryType = geometryObject->GeometryObjectType();
      if (geometryType == "GeometryFuselage")
      {
         Designer::GeometryFuselage* fuselage = (Designer::GeometryFuselage*)geometryObject;
         if (fuselage != nullptr)
         {
            double posX = fuselage->GetRefPoint_ft().X();
            double posY = fuselage->GetRefPoint_ft().Y();
            double posZ = fuselage->GetRefPoint_ft().Z();
            double length = fuselage->CanopyTotalLength();
            double height = fuselage->CanopyHeight();
            double width = fuselage->CanopyWidth();
            GeometryBody::ForwardShapeType frontShape = GeometryBody::ForwardShapeType::cOGIVE;
            double frontLength = fuselage->CanopyForwardLength();
            GeometryBody::AftShapeType aftShape = GeometryBody::AftShapeType::cOGIVE;
            double rearLength = fuselage->CanopyAftLength();
            double yaw_deg = fuselage->GetYawAngle_deg();
            double pitch_deg = fuselage->GetPitchAngle_deg();
            double roll_deg = fuselage->GetRollAngle_deg();

            // Create position vector for fuselage
            UtVec3dX pos(posX, posY, posZ);

            // Create dcm for fuselage
            double yaw_rad = yaw_deg * UtMath::cRAD_PER_DEG;
            double pitch_rad = pitch_deg * UtMath::cRAD_PER_DEG;
            double roll_rad = roll_deg * UtMath::cRAD_PER_DEG;
            UtDCM dcm(yaw_rad, pitch_rad, roll_rad);

            // Set relative location vector
            UtVec3dX vec(fuselage->CanopyRefX(), fuselage->CanopyRefY(), fuselage->CanopyRefZ());

            // Get location in world coords
            vec = dcm.InverseTransform(vec);
            UtVec3dX posVec = vec + pos;

            if (fuselage->CanopyPresent())
            {
               SetNominalMaterialProperties(0.85);
               bool skipDrawingItem = false;
               bool highlighted = false;
               if (TreeWidgetItemSelected(fuselage->GetTreeWidgetItem()))
               {
                  SetHighlightedMaterialProperties(0.85);
                  highlighted = true;
                  if (!aOnlySelectedObject)
                  {
                     skipDrawingItem = true;
                  }
               }
               else
               {
                  if (aOnlySelectedObject)
                  {
                     skipDrawingItem = true;
                  }
               }
               if (!skipDrawingItem)
               {
                  if (aWireframe)
                  {
                     if (highlighted)
                     {
                        SetSelectedWireframeColor();
                     }
                     else
                     {
                        SetNominalWireframeColor();
                     }
                  }

                  DrawBody(posVec.X(), posVec.Y(), posVec.Z(), length, height, width, frontShape, frontLength, aftShape, rearLength, 0.0, yaw_deg, pitch_deg, roll_deg, aWireframe);
               }
            }
         }
      }
   }
}

void GeometryGLWidget::DrawFuelTanks(Designer::Vehicle* aVehicle, bool aOnlySelectedObject, bool aWireframe)
{
   VehicleGeometry& vehicleGeometry = aVehicle->GetGeometry();
   const std::map<size_t, GeometryObject*>& geometryObjMap = vehicleGeometry.GetGeometryObjectMap();

   // Determine minimum dimensions
   double minDimension_ft = 10000000000.0;

   GeometryBody* body = vehicleGeometry.GetBody();
   if (body != nullptr)
   {
      double length = body->GetLength_ft();
      if (length < minDimension_ft) { minDimension_ft = length; }
      length = body->GetHeight_ft();
      if (length < minDimension_ft) { minDimension_ft = length; }
      length = body->GetWidth_ft();
      if (length < minDimension_ft) { minDimension_ft = length; }
   }

   GeometryFuselage* fuselage = vehicleGeometry.GetFuselage();
   if (fuselage != nullptr)
   {
      double length = fuselage->GetLength_ft();
      if (length < minDimension_ft) { minDimension_ft = length; }
      length = fuselage->GetHeight_ft();
      if (length < minDimension_ft) { minDimension_ft = length; }
      length = fuselage->GetWidth_ft();
      if (length < minDimension_ft) { minDimension_ft = length; }
   }

   GeometryWing* wing = vehicleGeometry.GetWing();
   if (wing != nullptr)
   {
      double length = wing->GetSpan_ft();
      if (length < minDimension_ft) { minDimension_ft = length; }
   }

   double iconDiameter_ft = minDimension_ft * 0.35;
   if (iconDiameter_ft < 0.01) { iconDiameter_ft = 0.01; }

   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;
      std::string geometryType = geometryObject->GeometryObjectType();
      if (geometryType == "GeometryPropulsionData")
      {
         Designer::GeometryPropulsionData* propulsion = (Designer::GeometryPropulsionData*)geometryObject;

         if (propulsion != nullptr)
         {
            UtVec3dX refPt(0.0, 0.0, 0.0);

            if (propulsion->GetCG_ft(refPt))
            {
               double posX = refPt.X();
               double posY = refPt.Y();
               double posZ = refPt.Z();

               SetNominalMaterialProperties();
               bool highlighted = false;
               if (TreeWidgetItemSelected(propulsion->GetTreeWidgetItem()))
               {
                  SetHighlightedMaterialProperties();
                  highlighted = true;
               }

               if (mShowFuelTanks || highlighted)
               {
                  if (aWireframe)
                  {
                     if (highlighted)
                     {
                        SetSelectedWireframeColor();
                     }
                     else
                     {
                        SetNominalWireframeColor();
                     }
                  }

                  DrawFuelTank(posX, posY, posZ, iconDiameter_ft, aWireframe);
               }
            }
         }
      }
   }
}

void GeometryGLWidget::DrawPointMasses(Designer::Vehicle* aVehicle, bool aOnlySelectedObject, bool aWireframe)
{
   VehicleGeometry& vehicleGeometry = aVehicle->GetGeometry();
   const std::map<size_t, GeometryObject*>& geometryObjMap = vehicleGeometry.GetGeometryObjectMap();

   // Determine minimum dimensions
   double minDimension_ft = 10000000000.0;

   GeometryBody* body = vehicleGeometry.GetBody();
   if (body != nullptr)
   {
      double length = body->GetLength_ft();
      if (length < minDimension_ft) { minDimension_ft = length; }
      length = body->GetHeight_ft();
      if (length < minDimension_ft) { minDimension_ft = length; }
      length = body->GetWidth_ft();
      if (length < minDimension_ft) { minDimension_ft = length; }
   }

   GeometryFuselage* fuselage = vehicleGeometry.GetFuselage();
   if (fuselage != nullptr)
   {
      double length = fuselage->GetLength_ft();
      if (length < minDimension_ft) { minDimension_ft = length; }
      length = fuselage->GetHeight_ft();
      if (length < minDimension_ft) { minDimension_ft = length; }
      length = fuselage->GetWidth_ft();
      if (length < minDimension_ft) { minDimension_ft = length; }
   }

   GeometryWing* wing = vehicleGeometry.GetWing();
   if (wing != nullptr)
   {
      double length = wing->GetSpan_ft();
      if (length < minDimension_ft) { minDimension_ft = length; }
   }

   double iconDiameter_ft = minDimension_ft * 0.35;
   if (iconDiameter_ft < 0.01) { iconDiameter_ft = 0.01; }

   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;
      std::string geometryType = geometryObject->GeometryObjectType();
      if (geometryType == "GeometryPointMass")
      {
         Designer::GeometryPropulsionData* propulsion = (Designer::GeometryPropulsionData*)geometryObject;

         if (propulsion != nullptr)
         {
            UtVec3dX refPt(0.0, 0.0, 0.0);

            if (propulsion->GetCG_ft(refPt))
            {
               double posX = refPt.X();
               double posY = refPt.Y();
               double posZ = refPt.Z();

               SetNominalMaterialProperties();
               bool highlighted = false;
               if (TreeWidgetItemSelected(propulsion->GetTreeWidgetItem()))
               {
                  SetHighlightedMaterialProperties();
                  highlighted = true;
               }

               if (mShowPointMasses || highlighted)
               {
                  if (aWireframe)
                  {
                     if (highlighted)
                     {
                        SetSelectedWireframeColor();
                     }
                     else
                     {
                        SetNominalWireframeColor();
                     }
                  }

                  DrawPointMass(posX, posY, posZ, iconDiameter_ft, aWireframe);
               }
            }
         }
      }
   }
}

void GeometryGLWidget::DrawCube(double aLength)
{
   double size = aLength * 0.5;

   glBegin(GL_LINE_LOOP);
   glVertex3d(-size, -size, size);
   glVertex3d(size, -size, size);
   glVertex3d(size, size, size);
   glVertex3d(-size, size, size);
   glEnd();
   glBegin(GL_LINE_LOOP);
   glVertex3d(size, -size, -size);
   glVertex3d(-size, -size, -size);
   glVertex3d(-size, size, -size);
   glVertex3d(size, size, -size);
   glEnd();

   glBegin(GL_LINE_LOOP);
   glVertex3d(-size, size, size);
   glVertex3d(size, size, size);
   glVertex3d(size, size, -size);
   glVertex3d(-size, size, -size);
   glEnd();
   glBegin(GL_LINE_LOOP);
   glVertex3d(-size, -size, -size);
   glVertex3d(size, -size, -size);
   glVertex3d(size, -size, size);
   glVertex3d(-size, -size, size);
   glEnd();

   glBegin(GL_LINE_LOOP);
   glVertex3d(-size, -size, -size);
   glVertex3d(-size, -size, size);
   glVertex3d(-size, size, size);
   glVertex3d(-size, size, -size);
   glEnd();
   glBegin(GL_LINE_LOOP);
   glVertex3d(size, -size, size);
   glVertex3d(size, -size, -size);
   glVertex3d(size, size, -size);
   glVertex3d(size, size, size);
   glEnd();
}

void GeometryGLWidget::DrawFocusPoints()
{
   for (auto& curPt : mFocusPoints)
   {
      curPt.Draw();
   }
}

void GeometryGLWidget::DrawSphereAtPoint(double aX, double aY, double aZ, double aRadius)
{
   glPushMatrix();
   glDisable(GL_LIGHTING);
   glDisable(GL_CULL_FACE);
   glColor4d(0.0, 1.0, 0.0, 1.0);
   glTranslated(aX, aY, aZ);


   double x, y, z, i, j;
   int horizontalStripes = 10;
   int virticalStripes = 10;
   double radius = aRadius;

   for (j = 0; j < UtMath::cPI; j += UtMath::cPI / (horizontalStripes + 1))
   {
      glBegin(GL_LINE_LOOP);
      y = (float)(radius * cos(j));
      for (i = 0; i < 2 * UtMath::cPI; i += UtMath::cPI / 60)
      {
         x = (radius * cos(i) * sin(j));
         z = (radius * sin(i) * sin(j));
         glVertex3f(x, y, z);
      }
      glEnd();
   }

   for (j = 0; j < UtMath::cPI; j += UtMath::cPI / virticalStripes)
   {
      glBegin(GL_LINE_LOOP);
      for (i = 0; i < 2 * UtMath::cPI; i += UtMath::cPI / 60)
      {
         x = (radius * sin(i) * cos(j));
         y = (radius * cos(i));
         z = (radius * sin(j) * sin(i));
         glVertex3f(x, y, z);
      }
      glEnd();
   }
   glPopMatrix();

   glDisable(GL_LIGHTING);
   glDisable(GL_LIGHT0);
   glDisable(GL_NORMALIZE);
}


void GeometryGLWidget::DrawEnclosureBox(double aHalfLength, double aHalfWidth, double aCeilingHeight, double aFloorHeight, int aStepSize)
{
   UtVec3dX posLengthWallNormal(-1.0, 0.0, 0.0);
   UtVec3dX negLengthWallNormal(1.0, 0.0, 0.0);
   UtVec3dX posWidthWallNormal(0.0, 0.0, -1.0);
   UtVec3dX negWidthWallNormal(0.0, 0.0, 1.0);
   UtVec3dX ceilingNormal(0.0, -1.0, 0.0);
   UtVec3dX floorNormal(0.0, 1.0, 0.0);

   UtVec3dX posLengthWallCenter(aHalfLength, 0.0, 0.0);
   UtVec3dX negLengthWallCenter(-aHalfLength, 0.0, 0.0);
   UtVec3dX posWidthWallCenter(0.0, 0.0, aHalfWidth);
   UtVec3dX negWidthWallCenter(0.0, 0.0, -aHalfWidth);
   UtVec3dX ceilingCenter(0.0, aCeilingHeight, 0.0);
   UtVec3dX floorCenter(0.0, aFloorHeight, 0.0);

   UtVec3dX eyeToCenterVec(0.0, 0.0, 0.0);

   eyeToCenterVec = posLengthWallCenter - mEyePt;
   if (eyeToCenterVec.Dot(posLengthWallNormal) <= 0.0)
   {
      DrawWall(cPosLengthWall, aHalfLength, aHalfWidth, aCeilingHeight, aFloorHeight, aStepSize);
   }

   eyeToCenterVec = negLengthWallCenter - mEyePt;
   if (eyeToCenterVec.Dot(negLengthWallNormal) <= 0.0)
   {
      DrawWall(cNegLengthWall, aHalfLength, aHalfWidth, aCeilingHeight, aFloorHeight, aStepSize);
   }

   eyeToCenterVec = posWidthWallCenter - mEyePt;
   if (eyeToCenterVec.Dot(posWidthWallNormal) <= 0.0)
   {
      DrawWall(cPosWidthWall, aHalfLength, aHalfWidth, aCeilingHeight, aFloorHeight, aStepSize);
   }

   eyeToCenterVec = negWidthWallCenter - mEyePt;
   if (eyeToCenterVec.Dot(negWidthWallNormal) <= 0.0)
   {
      DrawWall(cNegWidthWall, aHalfLength, aHalfWidth, aCeilingHeight, aFloorHeight, aStepSize);
   }

   eyeToCenterVec = ceilingCenter - mEyePt;
   if (eyeToCenterVec.Dot(ceilingNormal) <= 0.0)
   {
      DrawCeiling(aHalfLength, aHalfWidth, aCeilingHeight, aStepSize);
   }

   eyeToCenterVec = floorCenter - mEyePt;
   if (eyeToCenterVec.Dot(floorNormal) <= 0.0)
   {
      DrawFloor(aHalfLength, aHalfWidth, aFloorHeight, aStepSize);
   }
}

void GeometryGLWidget::DrawWall(wallType aType, double aHalfLength, double aHalfWidth, double aCeilingHeight, double aFloorHeight, int aStepSize)
{
   glPushMatrix();

   double wallLength = 0.0;

   switch (aType)
   {
      case cPosLengthWall:
         glTranslated(aHalfLength, 0.0, 0.0);
         glRotated(-90.0, 0.0, 1.0, 0.0);
         wallLength = aHalfWidth;
         break;
      case cNegLengthWall:
         glTranslated(-aHalfLength, 0.0, 0.0);
         glRotated(90.0, 0.0, 1.0, 0.0);
         wallLength = aHalfWidth;
         break;
      case cPosWidthWall:
         glTranslated(0.0, 0.0, aHalfWidth);
         glRotated(180.0, 0.0, 1.0, 0.0);
         wallLength = aHalfLength;
         break;
      case cNegWidthWall:
         glTranslated(0.0, 0.0, -aHalfWidth);
         glRotated(0.0, 0.0, 1.0, 0.0);
         wallLength = aHalfLength;
         break;
   }

   // Draw wall
   SetColorWalls();
   glBegin(GL_POLYGON);
   glVertex3d(-wallLength, aFloorHeight, 0.0);
   glVertex3d(wallLength, aFloorHeight, 0.0);
   glVertex3d(wallLength, aCeilingHeight, 0.0);
   glVertex3d(-wallLength, aCeilingHeight, 0.0);
   glEnd();

   int iWallLength = (int)wallLength;

   SetColorWallGrid();
   glBegin(GL_LINES);

   for (int i = 0; i <= iWallLength; i += aStepSize)
   {
      glVertex3d((double)i, aFloorHeight, 0.0);
      glVertex3d((double)i, aCeilingHeight, 0.0);

      if (i > 0)
      {
         glVertex3d((double) - i, aFloorHeight, 0.0);
         glVertex3d((double) - i, aCeilingHeight, 0.0);
      }
   }

   for (int i = (int)aFloorHeight; i <= (int)aCeilingHeight; i += aStepSize)
   {
      glVertex3d(-wallLength, (double)i, 0.0);
      glVertex3d(wallLength, (double)i, 0.0);
   }
   glEnd();

   glPopMatrix();
}

void GeometryGLWidget::DrawFloor(double aHalfLength, double aHalfWidth, double aHeight, int aStepSize)
{
   double length = (double)aHalfLength;
   double width  = (double)aHalfWidth;
   double height = (double)aHeight;

   SetColorFloor();
   glBegin(GL_POLYGON);
   glVertex3d(-length, height,  width);
   glVertex3d(length, height,  width);
   glVertex3d(length, height, -width);
   glVertex3d(-length, height, -width);
   glEnd();

   SetColorFloorGrid();
   glBegin(GL_LINES);

   for (int i = 0; i <= aHalfWidth; i += aStepSize)
   {
      glVertex3d((double) - length, height, (double)i);
      glVertex3d((double)length, height, (double)i);
      if (i > 0)
      {
         glVertex3d((double) - length, height, (double) - i);
         glVertex3d((double)length, height, (double) - i);
      }
   }
   for (int i = 0; i <= aHalfLength; i += aStepSize)
   {
      glVertex3d((double)i, height, (double) - width);
      glVertex3d((double)i, height, (double)width);
      if (i > 0)
      {
         glVertex3d((double) - i, height, (double) - width);
         glVertex3d((double) - i, height, (double)width);
      }
   }

   glEnd();
}

void GeometryGLWidget::DrawCeiling(double aHalfLength, double aHalfWidth, double aHeight, int aStepSize)
{
   double length = (double)aHalfLength;
   double width = (double)aHalfWidth;
   double height = (double)aHeight;

   SetColorCeiling();
   glBegin(GL_POLYGON);
   glVertex3d(-length, height, -width);
   glVertex3d(length, height, -width);
   glVertex3d(length, height, width);
   glVertex3d(-length, height, width);
   glEnd();

   SetColorCeilingGrid();
   glBegin(GL_LINES);
   for (int i = 0; i <= aHalfWidth; i += aStepSize)
   {
      glVertex3d((double) - length, height, (double)i);
      glVertex3d((double)length, height, (double)i);
      if (i > 0)
      {
         glVertex3d((double) - length, height, (double) - i);
         glVertex3d((double)length, height, (double) - i);
      }
   }
   for (int i = 0; i <= aHalfLength; i += aStepSize)
   {
      glVertex3d((double)i, height, (double) - width);
      glVertex3d((double)i, height, (double)width);
      if (i > 0)
      {
         glVertex3d((double) - i, height, (double) - width);
         glVertex3d((double) - i, height, (double)width);
      }
   }
   glEnd();
}

void GeometryGLWidget::SetColorWalls()
{
   glColor4d(0.0, 0.4, 0.8, 1.0);
}

void GeometryGLWidget::SetColorWallGrid()
{
   glColor4d(0.0, 0.5, 1.0, 1.0);
}

void GeometryGLWidget::SetColorFloor()
{
   //   glColor4d(0.0, 0.3, 0.6, 0.5);
   glColor4d(0.0, 0.4, 0.8, 0.8);
}

void GeometryGLWidget::SetColorFloorGrid()
{
   //   glColor4d(0.0, 0.4, 0.8, 0.5);
   glColor4d(0.0, 0.5, 1.0, 0.8);
}

void GeometryGLWidget::SetColorCeiling()
{
   glColor4d(0.0, 0.45, 0.9, 1.0);
}

void GeometryGLWidget::SetColorCeilingGrid()
{
   glColor4d(0.0, 0.5, 1.0, 1.0);
}

void GeometryGLWidget::SetColorOverlayNominal()
{
   glColor4d(0.3, 0.7, 1.0, 0.9);
}

void GeometryGLWidget::SetColorOverlayWarning()
{
   glColor4d(1.0, 0.1, 0.3, 0.9);
}

void GeometryGLWidget::SetDarkCgColor(bool aIsPart)
{
   if (aIsPart)
   {
      glColor4d(0.6, 0.0, 0.0, 1.0);

   }
   else
   {
      glColor4d(0.0, 0.2, 0.4, 1.0);
   }
}

void GeometryGLWidget::SetLightCgColor(bool aIsPart)
{
   if (aIsPart)
   {
      glColor4d(1.0, 0.9, 0.9, 1.0);
   }
   else
   {
      glColor4d(0.6, 0.9, 1.0, 1.0);
   }
}

void GeometryGLWidget::SetBorderCgColor(bool aIsPart)
{
   if (aIsPart)
   {
      glColor4d(0.3, 0.0, 0.0, 1.0);
   }
   else
   {
      glColor4d(0.1, 0.15, 0.3, 1.0);
   }
}

void GeometryGLWidget::DrawPointMass(double aPosX,
                                     double aPosY,
                                     double aPosZ,
                                     double aDiameter_ft,
                                     bool   aWireframe)
{
   // Check data
   if (aDiameter_ft <= 0.0) { return; }

   if (!aWireframe)
   {
      glEnable(GL_LIGHTING);
      glEnable(GL_LIGHT0);
      glEnable(GL_NORMALIZE);
   }

   glPushMatrix();
   glTranslated(aPosX, -aPosZ, aPosY);

   double xFront = aDiameter_ft *  0.5;
   double xBack = aDiameter_ft * -0.5;
   double yTop = aDiameter_ft *  0.5;
   double yBottom = aDiameter_ft * -0.5;
   double zRight = aDiameter_ft  *  0.5;
   double zLeft = aDiameter_ft  * -0.5;

   if (!aWireframe)
   {
      // Right
      glBegin(GL_POLYGON);
      glNormal3d(0.0, 0.0, 1.0);
      glVertex3d(xBack, yBottom, zRight);
      glVertex3d(xFront, yBottom, zRight);
      glVertex3d(xFront, yTop, zRight);
      glVertex3d(xBack, yTop, zRight);
      glEnd();

      // Left
      glBegin(GL_POLYGON);
      glNormal3d(0.0, 0.0, -1.0);
      glVertex3d(xFront, yBottom, zLeft);
      glVertex3d(xBack, yBottom, zLeft);
      glVertex3d(xBack, yTop, zLeft);
      glVertex3d(xFront, yTop, zLeft);
      glEnd();

      // Front
      glBegin(GL_POLYGON);
      glNormal3d(1.0, 0.0, 0.0);
      glVertex3d(xFront, yBottom, zRight);
      glVertex3d(xFront, yBottom, zLeft);
      glVertex3d(xFront, yTop, zLeft);
      glVertex3d(xFront, yTop, zRight);
      glEnd();

      // Back
      glBegin(GL_POLYGON);
      glNormal3d(-1.0, 0.0, 0.0);
      glVertex3d(xBack, yBottom, zLeft);
      glVertex3d(xBack, yBottom, zRight);
      glVertex3d(xBack, yTop, zRight);
      glVertex3d(xBack, yTop, zLeft);
      glEnd();

      // Top
      glBegin(GL_POLYGON);
      glNormal3d(0.0, 1.0, 0.0);
      glVertex3d(xFront, yTop, zLeft);
      glVertex3d(xBack, yTop, zLeft);
      glVertex3d(xBack, yTop, zRight);
      glVertex3d(xFront, yTop, zRight);
      glEnd();

      // Bottom
      glBegin(GL_POLYGON);
      glNormal3d(0.0, -1.0, 0.0);
      glVertex3d(xFront, yBottom, zLeft);
      glVertex3d(xFront, yBottom, zRight);
      glVertex3d(xBack, yBottom, zRight);
      glVertex3d(xBack, yBottom, zLeft);
      glEnd();
   }
   else
   {
      glLineWidth(2.0);

      // Right
      glBegin(GL_LINE_LOOP);
      glVertex3d(xBack, yBottom, zRight);
      glVertex3d(xFront, yBottom, zRight);
      glVertex3d(xFront, yTop, zRight);
      glVertex3d(xBack, yTop, zRight);
      glEnd();

      // Left
      glBegin(GL_LINE_LOOP);
      glVertex3d(xFront, yBottom, zLeft);
      glVertex3d(xBack, yBottom, zLeft);
      glVertex3d(xBack, yTop, zLeft);
      glVertex3d(xFront, yTop, zLeft);
      glEnd();

      // Front
      glBegin(GL_LINE_LOOP);
      glVertex3d(xFront, yBottom, zRight);
      glVertex3d(xFront, yBottom, zLeft);
      glVertex3d(xFront, yTop, zLeft);
      glVertex3d(xFront, yTop, zRight);
      glEnd();

      // Back
      glBegin(GL_LINE_LOOP);
      glVertex3d(xBack, yBottom, zLeft);
      glVertex3d(xBack, yBottom, zRight);
      glVertex3d(xBack, yTop, zRight);
      glVertex3d(xBack, yTop, zLeft);
      glEnd();

      // Top
      glBegin(GL_LINE_LOOP);
      glVertex3d(xFront, yTop, zLeft);
      glVertex3d(xBack, yTop, zLeft);
      glVertex3d(xBack, yTop, zRight);
      glVertex3d(xFront, yTop, zRight);
      glEnd();

      // Bottom
      glBegin(GL_LINE_LOOP);
      glVertex3d(xFront, yBottom, zLeft);
      glVertex3d(xFront, yBottom, zRight);
      glVertex3d(xBack, yBottom, zRight);
      glVertex3d(xBack, yBottom, zLeft);
      glEnd();

      glLineWidth(1.0);
   }

   glPopMatrix();

   glDisable(GL_LIGHTING);
   glDisable(GL_LIGHT0);
   glDisable(GL_NORMALIZE);
}

void GeometryGLWidget::DrawFuelTank(double aPosX,
                                    double aPosY,
                                    double aPosZ,
                                    double aDiameter_ft,
                                    bool   aWireframe)
{
   // Check data
   if (aDiameter_ft <= 0.0) { return; }

   if (!aWireframe)
   {
      glEnable(GL_LIGHTING);
      glEnable(GL_LIGHT0);
      glEnable(GL_NORMALIZE);
   }

   glPushMatrix();
   glTranslated(aPosX, -aPosZ, aPosY);

   double xFront = aDiameter_ft *  0.5;
   double xBack = aDiameter_ft * -0.5;
   double yTop = aDiameter_ft *  0.5;
   double yBottom = aDiameter_ft * -0.5;
   double zRight = aDiameter_ft  *  0.5;
   double zLeft = aDiameter_ft  * -0.5;

   if (!aWireframe)
   {
      // Right
      glBegin(GL_POLYGON);
      glNormal3d(0.0, 0.0, 1.0);
      glVertex3d(xBack, yBottom, zRight);
      glVertex3d(xFront, yBottom, zRight);
      glVertex3d(xFront, yTop, zRight);
      glVertex3d(xBack, yTop, zRight);
      glEnd();

      // Left
      glBegin(GL_POLYGON);
      glNormal3d(0.0, 0.0, -1.0);
      glVertex3d(xFront, yBottom, zLeft);
      glVertex3d(xBack, yBottom, zLeft);
      glVertex3d(xBack, yTop, zLeft);
      glVertex3d(xFront, yTop, zLeft);
      glEnd();

      // Front
      glBegin(GL_POLYGON);
      glNormal3d(1.0, 0.0, 0.0);
      glVertex3d(xFront, yBottom, zRight);
      glVertex3d(xFront, yBottom, zLeft);
      glVertex3d(xFront, yTop, zLeft);
      glVertex3d(xFront, yTop, zRight);
      glEnd();

      // Back
      glBegin(GL_POLYGON);
      glNormal3d(-1.0, 0.0, 0.0);
      glVertex3d(xBack, yBottom, zLeft);
      glVertex3d(xBack, yBottom, zRight);
      glVertex3d(xBack, yTop, zRight);
      glVertex3d(xBack, yTop, zLeft);
      glEnd();

      // Top
      glBegin(GL_POLYGON);
      glNormal3d(0.0, 1.0, 0.0);
      glVertex3d(xFront, yTop, zLeft);
      glVertex3d(xBack, yTop, zLeft);
      glVertex3d(xBack, yTop, zRight);
      glVertex3d(xFront, yTop, zRight);
      glEnd();

      // Bottom
      glBegin(GL_POLYGON);
      glNormal3d(0.0, -1.0, 0.0);
      glVertex3d(xFront, yBottom, zLeft);
      glVertex3d(xFront, yBottom, zRight);
      glVertex3d(xBack, yBottom, zRight);
      glVertex3d(xBack, yBottom, zLeft);
      glEnd();
   }
   else
   {
      glLineWidth(2.0);

      // Right
      glBegin(GL_LINE_LOOP);
      glVertex3d(xBack, yBottom, zRight);
      glVertex3d(xFront, yBottom, zRight);
      glVertex3d(xFront, yTop, zRight);
      glVertex3d(xBack, yTop, zRight);
      glEnd();

      // Left
      glBegin(GL_LINE_LOOP);
      glVertex3d(xFront, yBottom, zLeft);
      glVertex3d(xBack, yBottom, zLeft);
      glVertex3d(xBack, yTop, zLeft);
      glVertex3d(xFront, yTop, zLeft);
      glEnd();

      // Front
      glBegin(GL_LINE_LOOP);
      glVertex3d(xFront, yBottom, zRight);
      glVertex3d(xFront, yBottom, zLeft);
      glVertex3d(xFront, yTop, zLeft);
      glVertex3d(xFront, yTop, zRight);
      glEnd();

      // Back
      glBegin(GL_LINE_LOOP);
      glVertex3d(xBack, yBottom, zLeft);
      glVertex3d(xBack, yBottom, zRight);
      glVertex3d(xBack, yTop, zRight);
      glVertex3d(xBack, yTop, zLeft);
      glEnd();

      // Top
      glBegin(GL_LINE_LOOP);
      glVertex3d(xFront, yTop, zLeft);
      glVertex3d(xBack, yTop, zLeft);
      glVertex3d(xBack, yTop, zRight);
      glVertex3d(xFront, yTop, zRight);
      glEnd();

      // Bottom
      glBegin(GL_LINE_LOOP);
      glVertex3d(xFront, yBottom, zLeft);
      glVertex3d(xFront, yBottom, zRight);
      glVertex3d(xBack, yBottom, zRight);
      glVertex3d(xBack, yBottom, zLeft);
      glEnd();

      glLineWidth(1.0);
   }

   glPopMatrix();

   glDisable(GL_LIGHTING);
   glDisable(GL_LIGHT0);
   glDisable(GL_NORMALIZE);
}

void GeometryGLWidget::DrawDish(double aPosX,
                                double aPosY,
                                double aPosZ,
                                double aDiameter_ft,
                                double aThickness_ft,
                                bool   aWireframe)
{
   // Check data
   if ((aDiameter_ft <= 0.0) || (aThickness_ft <= 0.0)) { return; }

   // Data points
   double r[19];
   double h[19];
   double normal_r[19];
   double normal_h[19];

   double radius = 0.5 * aDiameter_ft;
   double halfThickness = 0.5 * aThickness_ft;

   h[0] = -halfThickness;
   r[0] = 0.0;
   h[18] = halfThickness;
   r[18] = 0.0;

   normal_r[0] = 0;
   normal_h[0] = -1.0;
   normal_r[18] = 0.0;
   normal_h[18] = 1.0;

   int count = 1;
   for (int iang_deg = -80; iang_deg <= 80; iang_deg += 10)
   {
      double ang_r = (double)iang_deg * UtMath::cRAD_PER_DEG;
      r[count] = radius * cos(ang_r);
      h[count] = halfThickness * sin(ang_r);
      ++count;
   }

   for (int i = 1; i < 18; ++i)
   {
      UtVec3dX lastPt(r[i - 1], h[i - 1], 0.0);
      UtVec3dX currentPt(r[i], h[i], 0.0);
      UtVec3dX nextPt(r[i + 1], h[i + 1], 0.0);
      UtVec3dX lastVec = lastPt - currentPt;
      UtVec3dX nextVec = nextPt - currentPt;
      UtVec3dX posZVec(0.0, 0.0, 1.0);
      UtVec3dX negZVec(0.0, 0.0, -1.0);

      UtVec3dX lastNormVec = lastVec.Cross(negZVec);
      UtVec3dX nextNormVec = nextVec.Cross(posZVec);
      UtVec3dX normVec = lastNormVec + nextNormVec;
      normVec.Normalize();
      normal_r[i] = normVec.X();
      normal_h[i] = normVec.Y();
   }

   if (!aWireframe)
   {
      glEnable(GL_LIGHTING);
      glEnable(GL_LIGHT0);
      glEnable(GL_NORMALIZE);
   }

   glPushMatrix();
   glTranslated(aPosX, -aPosZ, aPosY);

   int delta_iang = 10;
   for (int iang = delta_iang; iang <= 360; iang += delta_iang)
   {
      double ang0_rad = (double)(iang - delta_iang) * UtMath::cRAD_PER_DEG;
      double ang1_rad = (double)iang * UtMath::cRAD_PER_DEG;


      {
         double pt0x = r[0];
         double pt0y = h[0];
         double pt0z = r[0];

         double norm0x = normal_r[0];
         double norm0y = normal_h[0];
         double norm0z = normal_r[0];

         double pt1x = r[1] * sin(ang0_rad);
         double pt1y = h[1];
         double pt1z = r[1] * cos(ang0_rad);

         double norm1x = normal_r[1] * sin(ang0_rad);
         double norm1y = normal_h[1];
         double norm1z = normal_r[1] * cos(ang0_rad);

         double pt2x = r[1] * sin(ang1_rad);
         double pt2y = h[1];
         double pt2z = r[1] * cos(ang1_rad);

         double norm2x = normal_r[1] * sin(ang1_rad);
         double norm2y = normal_h[1];
         double norm2z = normal_r[1] * cos(ang1_rad);

         if (!aWireframe)
         {
            glBegin(GL_POLYGON);
            glNormal3d(norm2x, norm2y, norm2z);
            glVertex3d(pt2x, pt2y, pt2z);
            glNormal3d(norm1x, norm1y, norm1z);
            glVertex3d(pt1x, pt1y, pt1z);
            glNormal3d(norm0x, norm0y, norm0z);
            glVertex3d(pt0x, pt0y, pt0z);
            glEnd();
         }
         else
         {
            glBegin(GL_LINE_LOOP);
            glVertex3d(pt2x, pt2y, pt2z);
            glVertex3d(pt1x, pt1y, pt1z);
            glVertex3d(pt0x, pt0y, pt0z);
            glEnd();
         }
      }

      for (int j = 1; j < 17; ++j)
      {
         double pt0x = r[j] * sin(ang0_rad);
         double pt0y = h[j];
         double pt0z = r[j] * cos(ang0_rad);

         double norm0x = normal_r[j] * sin(ang0_rad);
         double norm0y = normal_h[j];
         double norm0z = normal_r[j] * cos(ang0_rad);

         double pt1x = r[j + 1] * sin(ang0_rad);
         double pt1y = h[j + 1];
         double pt1z = r[j + 1] * cos(ang0_rad);

         double norm1x = normal_r[j + 1] * sin(ang0_rad);
         double norm1y = normal_h[j + 1];
         double norm1z = normal_r[j + 1] * cos(ang0_rad);

         double pt2x = r[j + 1] * sin(ang1_rad);
         double pt2y = h[j + 1];
         double pt2z = r[j + 1] * cos(ang1_rad);

         double norm2x = normal_r[j + 1] * sin(ang1_rad);
         double norm2y = normal_h[j + 1];
         double norm2z = normal_r[j + 1] * cos(ang1_rad);

         double pt3x = r[j] * sin(ang1_rad);
         double pt3y = h[j];
         double pt3z = r[j] * cos(ang1_rad);

         double norm3x = normal_r[j] * sin(ang1_rad);
         double norm3y = normal_h[j];
         double norm3z = normal_r[j] * cos(ang1_rad);

         if (!aWireframe)
         {
            glBegin(GL_POLYGON);
            glNormal3d(norm3x, norm3y, norm3z);
            glVertex3d(pt3x, pt3y, pt3z);
            glNormal3d(norm2x, norm2y, norm2z);
            glVertex3d(pt2x, pt2y, pt2z);
            glNormal3d(norm1x, norm1y, norm1z);
            glVertex3d(pt1x, pt1y, pt1z);
            glNormal3d(norm0x, norm0y, norm0z);
            glVertex3d(pt0x, pt0y, pt0z);
            glEnd();
         }
         else
         {
            glBegin(GL_LINE_LOOP);
            glVertex3d(pt3x, pt3y, pt3z);
            glVertex3d(pt2x, pt2y, pt2z);
            glVertex3d(pt1x, pt1y, pt1z);
            glVertex3d(pt0x, pt0y, pt0z);
            glEnd();
         }
      }

      {
         double pt0x = r[18];
         double pt0y = h[18];
         double pt0z = r[18];

         double norm0x = normal_r[18];
         double norm0y = normal_h[18];
         double norm0z = normal_r[18];

         double pt1x = r[17] * sin(ang0_rad);
         double pt1y = h[17];
         double pt1z = r[17] * cos(ang0_rad);

         double norm1x = normal_r[17] * sin(ang0_rad);
         double norm1y = normal_h[17];
         double norm1z = normal_r[17] * cos(ang0_rad);

         double pt2x = r[17] * sin(ang1_rad);
         double pt2y = h[17];
         double pt2z = r[17] * cos(ang1_rad);

         double norm2x = normal_r[17] * sin(ang1_rad);
         double norm2y = normal_h[17];
         double norm2z = normal_r[17] * cos(ang1_rad);

         if (!aWireframe)
         {
            glBegin(GL_POLYGON);
            glNormal3d(norm0x, norm0y, norm0z);
            glVertex3d(pt0x, pt0y, pt0z);
            glNormal3d(norm1x, norm1y, norm1z);
            glVertex3d(pt1x, pt1y, pt1z);
            glNormal3d(norm2x, norm2y, norm2z);
            glVertex3d(pt2x, pt2y, pt2z);
            glEnd();
         }
         else
         {
            glBegin(GL_LINE_LOOP);
            glVertex3d(pt0x, pt0y, pt0z);
            glVertex3d(pt1x, pt1y, pt1z);
            glVertex3d(pt2x, pt2y, pt2z);
            glEnd();
         }
      }
   }

   glPopMatrix();

   // Turn off lighting
   glDisable(GL_LIGHTING);
   glDisable(GL_LIGHT0);
   glDisable(GL_NORMALIZE);
}

double GeometryGLWidget::EllipticalY_GivenX(double x, double aHalfWidth, double aHalfHeight)
{
   return sqrt((1 - ((x * x) / (aHalfWidth * aHalfWidth))) * (aHalfHeight * aHalfHeight));
}

double GeometryGLWidget::EllipticalX_GivenY(double y, double aHalfWidth, double aHalfHeight)
{
   return sqrt((1 - ((y * y) / (aHalfHeight * aHalfHeight))) * (aHalfWidth * aHalfWidth));
}

void GeometryGLWidget::DrawBodyCylinder(double aPosX,
                                        double aPosY,
                                        double aPosZ,
                                        double aLength,
                                        double aHeight,
                                        double aWidth,
                                        bool   aWireframe,
                                        bool   aAft)
{
   GraphicalPointCollection pointCollection(true);

   double halfHeight = 0.5 * aHeight;
   double halfWidth = 0.5 * aWidth;

   int numSteps = 10;
   if (aWireframe) { numSteps = 4; }

   double stepFactor = 1.0 / static_cast<double>(numSteps);

   // Add main/base ring
   pointCollection.AddRing();

   for (int hgt = 0; hgt < numSteps; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight;
      double z = EllipticalX_GivenY(y, halfWidth, halfHeight);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = numSteps; hgt > 0; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight;
      double z = -EllipticalX_GivenY(y, halfWidth, halfHeight);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -numSteps; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight;
      double z = -EllipticalX_GivenY(y, halfWidth, halfHeight);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -numSteps; hgt < 0; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight;
      double z = EllipticalX_GivenY(y, halfWidth, halfHeight);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX;
      pointCollection.AddPoint(x, y, z);
   }

   // Add far end ring
   pointCollection.AddRing();
   for (int hgt = 0; hgt < numSteps; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight;
      double z = EllipticalX_GivenY(y, halfWidth, halfHeight);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = numSteps; hgt > 0; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight;
      double z = -EllipticalX_GivenY(y, halfWidth, halfHeight);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -numSteps; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight;
      double z = -EllipticalX_GivenY(y, halfWidth, halfHeight);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -numSteps; hgt < 0; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight;
      double z = EllipticalX_GivenY(y, halfWidth, halfHeight);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength;
      pointCollection.AddPoint(x, y, z);
   }

   // Calculate normals
   pointCollection.CalculatePointCollection();

   // Draw
   if (aAft)
   {
      glPushMatrix();
      glRotated(180.0, 0.0, -1.0, 0.0);
      pointCollection.Draw(aWireframe);
      glPopMatrix();
   }
   else
   {
      pointCollection.Draw(aWireframe);
   }
}

void GeometryGLWidget::DrawBodyOgive(double aPosX,
                                     double aPosY,
                                     double aPosZ,
                                     double aLength,
                                     double aHeight,
                                     double aWidth,
                                     bool   aWireframe,
                                     bool   aAft)
{
   GraphicalPointCollection pointCollection(true);

   double halfHeight = 0.5 * aHeight;
   double halfWidth = 0.5 * aWidth;
   double lengthFactor = 0.0;
   double radiusFactor = 1.0;

   int numSteps = 10;
   if (aWireframe) { numSteps = 4; }

   double stepFactor = 1.0 / static_cast<double>(numSteps);

   // Add main/base ring
   pointCollection.AddRing();
   for (int hgt = 0; hgt < numSteps; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = numSteps; hgt > 0; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -numSteps; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -numSteps; hgt < 0; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }

   // Add second ring
   lengthFactor = 0.3;
   radiusFactor = 0.9;
   pointCollection.AddRing();
   for (int hgt = 0; hgt < numSteps; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = numSteps; hgt > 0; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -numSteps; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -numSteps; hgt < 0; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }

   // Add third ring
   lengthFactor = 0.55;
   radiusFactor = 0.7;
   pointCollection.AddRing();
   for (int hgt = 0; hgt < numSteps; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = numSteps; hgt > 0; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -numSteps; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -numSteps; hgt < 0; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }

   // Add fourth ring
   lengthFactor = 0.8;
   radiusFactor = 0.4;
   pointCollection.AddRing();
   for (int hgt = 0; hgt < numSteps; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = numSteps; hgt > 0; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -numSteps; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -numSteps; hgt < 0; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }

   // Add fifth (last) ring
   lengthFactor = 1.0;
   radiusFactor = 0.001;
   pointCollection.AddRing();
   for (int hgt = 0; hgt < numSteps; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = numSteps; hgt > 0; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -numSteps; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -numSteps; hgt < 0; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }

   // Calculate normals
   pointCollection.CalculatePointCollection();

   // Draw
   if (aAft)
   {
      glPushMatrix();
      glRotated(180.0, 0.0, -1.0, 0.0);
      pointCollection.Draw(aWireframe);
      glPopMatrix();
   }
   else
   {
      pointCollection.Draw(aWireframe);
   }
}

void GeometryGLWidget::DrawBodyCone(double aPosX,
                                    double aPosY,
                                    double aPosZ,
                                    double aLength,
                                    double aHeight,
                                    double aWidth,
                                    bool   aWireframe,
                                    bool   aAft)
{
   GraphicalPointCollection pointCollection(true);

   double halfHeight = 0.5 * aHeight;
   double halfWidth = 0.5 * aWidth;

   int numSteps = 10;
   if (aWireframe) { numSteps = 4; }

   double stepFactor = 1.0 / static_cast<double>(numSteps);

   // Add main/base ring
   pointCollection.AddRing();
   for (int hgt = 0; hgt < numSteps; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight;
      double z = EllipticalX_GivenY(y, halfWidth, halfHeight);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = numSteps; hgt > 0; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight;
      double z = -EllipticalX_GivenY(y, halfWidth, halfHeight);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -numSteps; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight;
      double z = -EllipticalX_GivenY(y, halfWidth, halfHeight);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -numSteps; hgt < 0; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight;
      double z = EllipticalX_GivenY(y, halfWidth, halfHeight);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX;
      pointCollection.AddPoint(x, y, z);
   }

   // Add nose/tip ring
   pointCollection.AddRing();
   for (int hgt = 0; hgt < numSteps; ++hgt)
   {
      double y = -aPosZ;
      double z = aPosY;
      double x = aPosX + aLength;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = numSteps; hgt > 0; --hgt)
   {
      double y = -aPosZ;
      double z = aPosY;
      double x = aPosX + aLength;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -numSteps; --hgt)
   {
      double y = -aPosZ;
      double z = aPosY;
      double x = aPosX + aLength;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -numSteps; hgt < 0; ++hgt)
   {
      double y = -aPosZ;
      double z = aPosY;
      double x = aPosX + aLength;
      pointCollection.AddPoint(x, y, z);
   }

   // Calculate normals
   pointCollection.CalculatePointCollection();

   // Draw
   if (aAft)
   {
      glPushMatrix();
      glRotated(180.0, 0.0, -1.0, 0.0);
      pointCollection.Draw(aWireframe);
      glPopMatrix();
   }
   else
   {
      pointCollection.Draw(aWireframe);
   }
}

void GeometryGLWidget::DrawBodySphere(double aPosX,
                                      double aPosY,
                                      double aPosZ,
                                      double aLength,
                                      double aHeight,
                                      double aWidth,
                                      bool   aWireframe,
                                      bool   aAft)
{
   GraphicalPointCollection pointCollection(true);

   double halfHeight = 0.5 * aHeight;
   double halfWidth = 0.5 * aWidth;
   double lengthFactor = 0.0;
   double radiusFactor = 1.0;

   int numSteps = 10;
   if (aWireframe) { numSteps = 4; }

   double stepFactor = 1.0 / static_cast<double>(numSteps);

   // Add main/base ring
   pointCollection.AddRing();
   for (int hgt = 0; hgt < numSteps; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = numSteps; hgt > 0; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -numSteps; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -numSteps; hgt < 0; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }

   // Add second ring
   lengthFactor = 0.2;
   radiusFactor = sqrt(1.0 - lengthFactor * lengthFactor);
   pointCollection.AddRing();
   for (int hgt = 0; hgt < numSteps; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = numSteps; hgt > 0; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -numSteps; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -numSteps; hgt < 0; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }

   // Add third ring
   lengthFactor = 0.4;
   radiusFactor = sqrt(1.0 - lengthFactor * lengthFactor);
   pointCollection.AddRing();
   for (int hgt = 0; hgt < numSteps; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = numSteps; hgt > 0; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -numSteps; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -numSteps; hgt < 0; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }

   // Add fourth ring
   lengthFactor = 0.6;
   radiusFactor = sqrt(1.0 - lengthFactor * lengthFactor);
   pointCollection.AddRing();
   for (int hgt = 0; hgt < numSteps; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = numSteps; hgt > 0; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -numSteps; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -numSteps; hgt < 0; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }

   // Add fifth ring
   lengthFactor = 0.8;
   radiusFactor = sqrt(1.0 - lengthFactor * lengthFactor);
   pointCollection.AddRing();
   for (int hgt = 0; hgt < numSteps; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = numSteps; hgt > 0; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -numSteps; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -numSteps; hgt < 0; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }

   // Add sixth ring
   lengthFactor = 0.9;
   radiusFactor = sqrt(1.0 - lengthFactor * lengthFactor);
   pointCollection.AddRing();
   for (int hgt = 0; hgt < numSteps; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = numSteps; hgt > 0; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -numSteps; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -numSteps; hgt < 0; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }

   // Add seventh (and last) ring
   lengthFactor = 0.999;
   radiusFactor = sqrt(1.0 - lengthFactor * lengthFactor);
   pointCollection.AddRing();
   for (int hgt = 0; hgt < numSteps; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = numSteps; hgt > 0; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -numSteps; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -numSteps; hgt < 0; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }

   // Calculate normals
   pointCollection.CalculatePointCollection();

   // Draw
   if (aAft)
   {
      glPushMatrix();
      glRotated(180.0, 0.0, -1.0, 0.0);
      pointCollection.Draw(aWireframe);
      glPopMatrix();
   }
   else
   {
      pointCollection.Draw(aWireframe);
   }
}

void GeometryGLWidget::DrawBodyBoatTail(double aPosX,
                                        double aPosY,
                                        double aPosZ,
                                        double aLength,
                                        double aHeight,
                                        double aWidth,
                                        double aDiam,
                                        bool   aWireframe,
                                        bool   aAft)
{
   GraphicalPointCollection pointCollection(true);

   double halfHeight = 0.5 * aHeight;
   double halfWidth = 0.5 * aWidth;
   double radius = 0.5 * aDiam;

   int numSteps = 10;
   if (aWireframe) { numSteps = 4; }

   double stepFactor = 1.0 / static_cast<double>(numSteps);

   // Add main/base ring
   pointCollection.AddRing();
   for (int hgt = 0; hgt < numSteps; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight;
      double z = EllipticalX_GivenY(y, halfWidth, halfHeight);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = numSteps; hgt > 0; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight;
      double z = -EllipticalX_GivenY(y, halfWidth, halfHeight);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -numSteps; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight;
      double z = -EllipticalX_GivenY(y, halfWidth, halfHeight);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -numSteps; hgt < 0; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight;
      double z = EllipticalX_GivenY(y, halfWidth, halfHeight);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX;
      pointCollection.AddPoint(x, y, z);
   }

   // Add boat tail ring
   pointCollection.AddRing();
   for (int hgt = 0; hgt < numSteps; ++hgt)
   {
      double y = stepFactor * (double)hgt * radius;
      double z = EllipticalX_GivenY(y, radius, radius);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = numSteps; hgt > 0; --hgt)
   {
      double y = stepFactor * (double)hgt * radius;
      double z = -EllipticalX_GivenY(y, radius, radius);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -numSteps; --hgt)
   {
      double y = stepFactor * (double)hgt * radius;
      double z = -EllipticalX_GivenY(y, radius, radius);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -numSteps; hgt < 0; ++hgt)
   {
      double y = stepFactor * (double)hgt * radius;
      double z = EllipticalX_GivenY(y, radius, radius);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength;
      pointCollection.AddPoint(x, y, z);
   }

   // Calculate normals
   pointCollection.CalculatePointCollection();

   // Draw
   if (aAft)
   {
      glPushMatrix();
      glRotated(180.0, 0.0, -1.0, 0.0);
      pointCollection.Draw(aWireframe);
      glPopMatrix();
   }
   else
   {
      pointCollection.Draw(aWireframe);
   }

   // Now, handle the end disk

   std::vector<UtVec3dX> vertices;

   for (int hgt = 0; hgt < numSteps; ++hgt)
   {
      double y = stepFactor * (double)hgt * radius;
      double z = EllipticalX_GivenY(y, radius, radius);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength;
      UtVec3dX vert(x, y, z);
      vertices.push_back(vert);
   }
   for (int hgt = numSteps; hgt > 0; --hgt)
   {
      double y = stepFactor * (double)hgt * radius;
      double z = -EllipticalX_GivenY(y, radius, radius);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength;
      UtVec3dX vert(x, y, z);
      vertices.push_back(vert);
   }
   for (int hgt = 0; hgt > -numSteps; --hgt)
   {
      double y = stepFactor * (double)hgt * radius;
      double z = -EllipticalX_GivenY(y, radius, radius);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength;
      UtVec3dX vert(x, y, z);
      vertices.push_back(vert);
   }
   for (int hgt = -numSteps; hgt < 0; ++hgt)
   {
      double y = stepFactor * (double)hgt * radius;
      double z = EllipticalX_GivenY(y, radius, radius);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength;
      UtVec3dX vert(x, y, z);
      vertices.push_back(vert);
   }

   // Draw disk
   if (aAft)
   {
      glPushMatrix();
      glRotated(180.0, 0.0, -1.0, 0.0);
   }

   if (aWireframe)
   {
      glBegin(GL_LINE_LOOP);

      // Need to run through points in reverse to get proper winding order
      for (auto revIter = vertices.rbegin(); revIter != vertices.rend(); ++revIter)
      {
         UtVec3dX& vec = *revIter;
         glVertex3d(vec.X(), vec.Y(), vec.Z());
      }

      glEnd();
   }
   else
   {
      glBegin(GL_POLYGON);
      glNormal3d(1.0, 0.0, 0.0);

      // Need to run through points in reverse to get proper winding order
      for (auto revIter = vertices.rbegin(); revIter != vertices.rend(); ++revIter)
      {
         UtVec3dX& vec = *revIter;
         glVertex3d(vec.X(), vec.Y(), vec.Z());
      }

      glEnd();
   }

   if (aAft)
   {
      glPopMatrix();
   }
}

void GeometryGLWidget::DrawBodyBlunt(double aPosX,
                                     double aPosY,
                                     double aPosZ,
                                     double aLength,
                                     double aHeight,
                                     double aWidth,
                                     bool   aWireframe,
                                     bool   aAft)
{
   double halfHeight = 0.5 * aHeight;
   double halfWidth = 0.5 * aWidth;

   int numSteps = 10;
   if (aWireframe) { numSteps = 4; }

   double stepFactor = 1.0 / static_cast<double>(numSteps);

   std::vector<UtVec3dX> vertices;

   for (int hgt = 0; hgt < numSteps; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight;
      double z = EllipticalX_GivenY(y, halfWidth, halfHeight);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength;
      UtVec3dX vert(x, y, z);
      vertices.push_back(vert);
   }
   for (int hgt = numSteps; hgt > 0; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight;
      double z = -EllipticalX_GivenY(y, halfWidth, halfHeight);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength;
      UtVec3dX vert(x, y, z);
      vertices.push_back(vert);
   }
   for (int hgt = 0; hgt > -numSteps; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight;
      double z = -EllipticalX_GivenY(y, halfWidth, halfHeight);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength;
      UtVec3dX vert(x, y, z);
      vertices.push_back(vert);
   }
   for (int hgt = -numSteps; hgt < 0; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight;
      double z = EllipticalX_GivenY(y, halfWidth, halfHeight);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength;
      UtVec3dX vert(x, y, z);
      vertices.push_back(vert);
   }

   // Draw disk
   if (aAft)
   {
      glPushMatrix();
      glRotated(180.0, 0.0, -1.0, 0.0);
   }

   if (aWireframe)
   {
      glBegin(GL_LINE_LOOP);

      // Need to run through points in reverse to get proper winding order
      for (std::vector<UtVec3dX>::reverse_iterator revIter = vertices.rbegin();
           revIter != vertices.rend(); ++revIter)
      {
         UtVec3dX& vec = *revIter;
         glVertex3d(vec.X(), vec.Y(), vec.Z());
      }

      glEnd();
   }
   else
   {
      glBegin(GL_POLYGON);
      glNormal3d(1.0, 0.0, 0.0);

      // Need to run through points in reverse to get proper winding order
      for (std::vector<UtVec3dX>::reverse_iterator revIter = vertices.rbegin();
           revIter != vertices.rend(); ++revIter)
      {
         UtVec3dX& vec = *revIter;
         glVertex3d(vec.X(), vec.Y(), vec.Z());
      }

      glEnd();
   }

   if (aAft)
   {
      glPopMatrix();
   }
}

void GeometryGLWidget::DrawBody(double                         aPosX,
                                double                         aPosY,
                                double                         aPosZ,
                                double                         aLength,
                                double                         aHeight,
                                double                         aWidth,
                                GeometryBody::ForwardShapeType aFrontShape,
                                double                         aFrontLength,
                                GeometryBody::AftShapeType     aAftShape,
                                double                         aRearLength,
                                double                         aBoatTailDiam,
                                double                         aYaw_deg,
                                double                         aPitch_deg,
                                double                         aRoll_deg,
                                bool                           aWireframe)
{
   // Verify correct values
   if ((aFrontLength + aRearLength) >= aLength) { return; }

   if (!aWireframe)
   {
      glEnable(GL_LIGHTING);
      glEnable(GL_LIGHT0);
      glEnable(GL_NORMALIZE);
   }

   glPushMatrix();
   glTranslated(aPosX, -aPosZ, aPosY);
   glRotated(aYaw_deg, 0.0, -1.0, 0.0);
   glRotated(aPitch_deg, 0.0, 0.0, 1.0);
   glRotated(aRoll_deg, 1.0, 0.0, 0.0);

   // Draw forward shape
   switch (aFrontShape)
   {
      case Designer::GeometryBody::ForwardShapeType::cCONE:
         DrawBodyCone((aLength * 0.5 - aFrontLength), 0.0, 0.0, aFrontLength, aHeight, aWidth, aWireframe);
         break;
      case Designer::GeometryBody::ForwardShapeType::cOGIVE:
         DrawBodyOgive((aLength * 0.5 - aFrontLength), 0.0, 0.0, aFrontLength, aHeight, aWidth, aWireframe);
         break;
      case Designer::GeometryBody::ForwardShapeType::cROUND:
         DrawBodySphere((aLength * 0.5 - aFrontLength), 0.0, 0.0, aFrontLength, aHeight, aWidth, aWireframe);
         break;
      case Designer::GeometryBody::ForwardShapeType::cBLUNT:
         aFrontLength = 0.0; // Ensure front length is zero if blunt
         DrawBodyBlunt((aLength * 0.5 - aFrontLength), 0.0, 0.0, aFrontLength, aHeight, aWidth, aWireframe);
         break;
      default:
         break;
   }

   // Draw main/center shape (cylinder)
   DrawBodyCylinder((-aLength * 0.5 + aRearLength), 0.0, 0.0, aLength - aFrontLength - aRearLength, aHeight, aWidth, aWireframe);


   switch (aAftShape)
   {
      case Designer::GeometryBody::AftShapeType::cCONE:
         DrawBodyCone((aLength * 0.5 - aRearLength), 0.0, 0.0, aRearLength, aHeight, aWidth, aWireframe, true);
         break;
      case Designer::GeometryBody::AftShapeType::cOGIVE:
         DrawBodyOgive((aLength * 0.5 - aRearLength), 0.0, 0.0, aRearLength, aHeight, aWidth, aWireframe, true);
         break;
      case Designer::GeometryBody::AftShapeType::cROUND:
         DrawBodySphere((aLength * 0.5 - aRearLength), 0.0, 0.0, aRearLength, aHeight, aWidth, aWireframe, true);
         break;
      case Designer::GeometryBody::AftShapeType::cBOATTAIL:
         DrawBodyBoatTail((aLength * 0.5 - aRearLength), 0.0, 0.0, aRearLength, aHeight, aWidth, aBoatTailDiam, aWireframe, true);
         break;
      case Designer::GeometryBody::AftShapeType::cBLUNT:
         aRearLength = 0.0; // Ensure aft length is zero if blunt
         DrawBodyBlunt((aLength * 0.5 - aRearLength), 0.0, 0.0, aRearLength, aHeight, aWidth, aWireframe, true);
         break;
      default:
         break;
   }

   glPopMatrix();

   // Turn off lighting
   glDisable(GL_LIGHTING);
   glDisable(GL_LIGHT0);
   glDisable(GL_NORMALIZE);
}

void GeometryGLWidget::DrawBox(double aLength, double aHeight, double aWidth, bool aWireframe)
{
   if (!aWireframe)
   {
      glEnable(GL_LIGHTING);
      glEnable(GL_LIGHT0);
   }

   double xFront  = aLength *  0.5;
   double xBack   = aLength * -0.5;
   double yTop    = aHeight *  0.5;
   double yBottom = aHeight * -0.5;
   double zRight  = aWidth  *  0.5;
   double zLeft   = aWidth  * -0.5;

   if (!aWireframe)
   {
      // Right
      glBegin(GL_POLYGON);
      glNormal3d(0.0, 0.0, 1.0);
      glVertex3d(xBack, yBottom, zRight);
      glVertex3d(xFront, yBottom, zRight);
      glVertex3d(xFront, yTop, zRight);
      glVertex3d(xBack, yTop, zRight);
      glEnd();

      // Left
      glBegin(GL_POLYGON);
      glNormal3d(0.0, 0.0, -1.0);
      glVertex3d(xFront, yBottom, zLeft);
      glVertex3d(xBack, yBottom, zLeft);
      glVertex3d(xBack, yTop, zLeft);
      glVertex3d(xFront, yTop, zLeft);
      glEnd();

      // Front
      glBegin(GL_POLYGON);
      glNormal3d(1.0, 0.0, 0.0);
      glVertex3d(xFront, yBottom, zRight);
      glVertex3d(xFront, yBottom, zLeft);
      glVertex3d(xFront, yTop, zLeft);
      glVertex3d(xFront, yTop, zRight);
      glEnd();

      // Back
      glBegin(GL_POLYGON);
      glNormal3d(-1.0, 0.0, 0.0);
      glVertex3d(xBack, yBottom, zLeft);
      glVertex3d(xBack, yBottom, zRight);
      glVertex3d(xBack, yTop, zRight);
      glVertex3d(xBack, yTop, zLeft);
      glEnd();

      // Top
      glBegin(GL_POLYGON);
      glNormal3d(0.0, 1.0, 0.0);
      glVertex3d(xFront, yTop, zLeft);
      glVertex3d(xBack, yTop, zLeft);
      glVertex3d(xBack, yTop, zRight);
      glVertex3d(xFront, yTop, zRight);
      glEnd();

      // Bottom
      glBegin(GL_POLYGON);
      glNormal3d(0.0, -1.0, 0.0);
      glVertex3d(xFront, yBottom, zLeft);
      glVertex3d(xFront, yBottom, zRight);
      glVertex3d(xBack, yBottom, zRight);
      glVertex3d(xBack, yBottom, zLeft);
      glEnd();
   }
   else
   {
      // Right
      glBegin(GL_LINE_LOOP);
      glVertex3d(xBack, yBottom, zRight);
      glVertex3d(xFront, yBottom, zRight);
      glVertex3d(xFront, yTop, zRight);
      glVertex3d(xBack, yTop, zRight);
      glEnd();

      // Left
      glBegin(GL_LINE_LOOP);
      glVertex3d(xFront, yBottom, zLeft);
      glVertex3d(xBack, yBottom, zLeft);
      glVertex3d(xBack, yTop, zLeft);
      glVertex3d(xFront, yTop, zLeft);
      glEnd();

      // Front
      glBegin(GL_LINE_LOOP);
      glVertex3d(xFront, yBottom, zRight);
      glVertex3d(xFront, yBottom, zLeft);
      glVertex3d(xFront, yTop, zLeft);
      glVertex3d(xFront, yTop, zRight);
      glEnd();

      // Back
      glBegin(GL_LINE_LOOP);
      glVertex3d(xBack, yBottom, zLeft);
      glVertex3d(xBack, yBottom, zRight);
      glVertex3d(xBack, yTop, zRight);
      glVertex3d(xBack, yTop, zLeft);
      glEnd();

      // Top
      glBegin(GL_LINE_LOOP);
      glVertex3d(xFront, yTop, zLeft);
      glVertex3d(xBack, yTop, zLeft);
      glVertex3d(xBack, yTop, zRight);
      glVertex3d(xFront, yTop, zRight);
      glEnd();

      // Bottom
      glBegin(GL_LINE_LOOP);
      glVertex3d(xFront, yBottom, zLeft);
      glVertex3d(xFront, yBottom, zRight);
      glVertex3d(xBack, yBottom, zRight);
      glVertex3d(xBack, yBottom, zLeft);
      glEnd();
   }

   // Turn off lighting
   glDisable(GL_LIGHTING);
   glDisable(GL_LIGHT0);
}

void GeometryGLWidget::DrawBoxWithHole(double aLength, double aHeight, double aWidth, double aThickness, bool aWireframe)
{
   if (!aWireframe)
   {
      glEnable(GL_LIGHTING);
      glEnable(GL_LIGHT0);
   }

   double xFront = aLength *  0.5;
   double xBack = aLength * -0.5;
   double yTop = aHeight *  0.5;
   double yBottom = aHeight * -0.5;
   double zRight = aWidth  *  0.5;
   double zLeft = aWidth  * -0.5;

   if (!aWireframe)
   {
      // Outside Right
      glBegin(GL_POLYGON);
      glNormal3d(0.0, 0.0, 1.0);
      glVertex3d(xBack, yBottom, zRight);
      glVertex3d(xFront, yBottom, zRight);
      glVertex3d(xFront, yTop, zRight);
      glVertex3d(xBack, yTop, zRight);
      glEnd();

      // Outside Left
      glBegin(GL_POLYGON);
      glNormal3d(0.0, 0.0, -1.0);
      glVertex3d(xFront, yBottom, zLeft);
      glVertex3d(xBack, yBottom, zLeft);
      glVertex3d(xBack, yTop, zLeft);
      glVertex3d(xFront, yTop, zLeft);
      glEnd();

      // Outside Top
      glBegin(GL_POLYGON);
      glNormal3d(0.0, 1.0, 0.0);
      glVertex3d(xFront, yTop, zLeft);
      glVertex3d(xBack, yTop, zLeft);
      glVertex3d(xBack, yTop, zRight);
      glVertex3d(xFront, yTop, zRight);
      glEnd();

      // Outside Bottom
      glBegin(GL_POLYGON);
      glNormal3d(0.0, -1.0, 0.0);
      glVertex3d(xFront, yBottom, zLeft);
      glVertex3d(xFront, yBottom, zRight);
      glVertex3d(xBack, yBottom, zRight);
      glVertex3d(xBack, yBottom, zLeft);
      glEnd();

      // Inside Right
      glBegin(GL_POLYGON);
      glNormal3d(0.0, 0.0, -1.0);
      glVertex3d(xBack, yTop - aThickness, zRight - aThickness);
      glVertex3d(xFront, yTop - aThickness, zRight - aThickness);
      glVertex3d(xFront, yBottom + aThickness, zRight - aThickness);
      glVertex3d(xBack, yBottom + aThickness, zRight - aThickness);
      glEnd();

      // Inside Left
      glBegin(GL_POLYGON);
      glNormal3d(0.0, 0.0, 1.0);
      glVertex3d(xFront, yTop - aThickness, zLeft + aThickness);
      glVertex3d(xBack, yTop - aThickness, zLeft + aThickness);
      glVertex3d(xBack, yBottom + aThickness, zLeft + aThickness);
      glVertex3d(xFront, yBottom + aThickness, zLeft + aThickness);
      glEnd();

      // Inside Top
      glBegin(GL_POLYGON);
      glNormal3d(0.0, -1.0, 0.0);
      glVertex3d(xFront, yTop - aThickness, zRight - aThickness);
      glVertex3d(xBack, yTop - aThickness, zRight - aThickness);
      glVertex3d(xBack, yTop - aThickness, zLeft + aThickness);
      glVertex3d(xFront, yTop - aThickness, zLeft + aThickness);
      glEnd();

      // Inside Bottom
      glBegin(GL_POLYGON);
      glNormal3d(0.0, 1.0, 0.0);
      glVertex3d(xBack, yBottom + aThickness, zLeft + aThickness);
      glVertex3d(xBack, yBottom + aThickness, zRight - aThickness);
      glVertex3d(xFront, yBottom + aThickness, zRight - aThickness);
      glVertex3d(xFront, yBottom + aThickness, zLeft + aThickness);
      glEnd();

      // Front
      glBegin(GL_POLYGON);
      glNormal3d(1.0, 0.0, 0.0);
      glVertex3d(xFront, yBottom, zRight);
      glVertex3d(xFront, yBottom, zLeft);
      glVertex3d(xFront, yBottom + aThickness, zLeft + aThickness);
      glVertex3d(xFront, yBottom + aThickness, zRight - aThickness);
      glEnd();

      glBegin(GL_POLYGON);
      glNormal3d(1.0, 0.0, 0.0);
      glVertex3d(xFront, yBottom, zLeft);
      glVertex3d(xFront, yTop, zLeft);
      glVertex3d(xFront, yTop - aThickness, zLeft + aThickness);
      glVertex3d(xFront, yBottom + aThickness, zLeft + aThickness);
      glEnd();

      glBegin(GL_POLYGON);
      glNormal3d(1.0, 0.0, 0.0);
      glVertex3d(xFront, yTop, zRight);
      glVertex3d(xFront, yBottom, zRight);
      glVertex3d(xFront, yBottom + aThickness, zRight - aThickness);
      glVertex3d(xFront, yTop - aThickness, zRight - aThickness);
      glEnd();

      glBegin(GL_POLYGON);
      glNormal3d(1.0, 0.0, 0.0);
      glVertex3d(xFront, yTop, zLeft);
      glVertex3d(xFront, yTop, zRight);
      glVertex3d(xFront, yTop - aThickness, zRight - aThickness);
      glVertex3d(xFront, yTop - aThickness, zLeft + aThickness);
      glEnd();

      // Back
      glBegin(GL_POLYGON);
      glNormal3d(-1.0, 0.0, 0.0);
      glVertex3d(xBack, yBottom + aThickness, zRight - aThickness);
      glVertex3d(xBack, yBottom + aThickness, zLeft + aThickness);
      glVertex3d(xBack, yBottom, zLeft);
      glVertex3d(xBack, yBottom, zRight);
      glEnd();

      glBegin(GL_POLYGON);
      glNormal3d(-1.0, 0.0, 0.0);
      glVertex3d(xBack, yBottom + aThickness, zLeft + aThickness);
      glVertex3d(xBack, yTop - aThickness, zLeft + aThickness);
      glVertex3d(xBack, yTop, zLeft);
      glVertex3d(xBack, yBottom, zLeft);
      glEnd();

      glBegin(GL_POLYGON);
      glNormal3d(-1.0, 0.0, 0.0);
      glVertex3d(xBack, yTop - aThickness, zRight - aThickness);
      glVertex3d(xBack, yBottom + aThickness, zRight - aThickness);
      glVertex3d(xBack, yBottom, zRight);
      glVertex3d(xBack, yTop, zRight);
      glEnd();

      glBegin(GL_POLYGON);
      glNormal3d(-1.0, 0.0, 0.0);
      glVertex3d(xBack, yTop - aThickness, zLeft + aThickness);
      glVertex3d(xBack, yTop - aThickness, zRight - aThickness);
      glVertex3d(xBack, yTop, zRight);
      glVertex3d(xBack, yTop, zLeft);
      glEnd();
   }
   else
   {
      // Outside Right
      glBegin(GL_LINE_LOOP);
      glVertex3d(xBack, yBottom, zRight);
      glVertex3d(xFront, yBottom, zRight);
      glVertex3d(xFront, yTop, zRight);
      glVertex3d(xBack, yTop, zRight);
      glEnd();

      // Outside Left
      glBegin(GL_LINE_LOOP);
      glVertex3d(xFront, yBottom, zLeft);
      glVertex3d(xBack, yBottom, zLeft);
      glVertex3d(xBack, yTop, zLeft);
      glVertex3d(xFront, yTop, zLeft);
      glEnd();

      // Outside Top
      glBegin(GL_LINE_LOOP);
      glVertex3d(xFront, yTop, zLeft);
      glVertex3d(xBack, yTop, zLeft);
      glVertex3d(xBack, yTop, zRight);
      glVertex3d(xFront, yTop, zRight);
      glEnd();

      // Outside Bottom
      glBegin(GL_LINE_LOOP);
      glVertex3d(xFront, yBottom, zLeft);
      glVertex3d(xFront, yBottom, zRight);
      glVertex3d(xBack, yBottom, zRight);
      glVertex3d(xBack, yBottom, zLeft);
      glEnd();

      // Inside Right
      glBegin(GL_LINE_LOOP);
      glVertex3d(xBack, yTop - aThickness, zRight - aThickness);
      glVertex3d(xFront, yTop - aThickness, zRight - aThickness);
      glVertex3d(xFront, yBottom + aThickness, zRight - aThickness);
      glVertex3d(xBack, yBottom + aThickness, zRight - aThickness);
      glEnd();

      // Inside Left
      glBegin(GL_LINE_LOOP);
      glVertex3d(xFront, yTop - aThickness, zLeft + aThickness);
      glVertex3d(xBack, yTop - aThickness, zLeft + aThickness);
      glVertex3d(xBack, yBottom + aThickness, zLeft + aThickness);
      glVertex3d(xFront, yBottom + aThickness, zLeft + aThickness);
      glEnd();

      // Inside Top
      glBegin(GL_LINE_LOOP);
      glVertex3d(xFront, yTop - aThickness, zRight - aThickness);
      glVertex3d(xBack, yTop - aThickness, zRight - aThickness);
      glVertex3d(xBack, yTop - aThickness, zLeft + aThickness);
      glVertex3d(xFront, yTop - aThickness, zLeft + aThickness);
      glEnd();

      // Inside Bottom
      glBegin(GL_LINE_LOOP);
      glVertex3d(xBack, yBottom + aThickness, zLeft + aThickness);
      glVertex3d(xBack, yBottom + aThickness, zRight - aThickness);
      glVertex3d(xFront, yBottom + aThickness, zRight - aThickness);
      glVertex3d(xFront, yBottom + aThickness, zLeft + aThickness);
      glEnd();

      // Front
      glBegin(GL_LINE_LOOP);
      glVertex3d(xFront, yBottom, zRight);
      glVertex3d(xFront, yBottom, zLeft);
      glVertex3d(xFront, yBottom + aThickness, zLeft + aThickness);
      glVertex3d(xFront, yBottom + aThickness, zRight - aThickness);
      glEnd();

      glBegin(GL_LINE_LOOP);
      glVertex3d(xFront, yBottom, zLeft);
      glVertex3d(xFront, yTop, zLeft);
      glVertex3d(xFront, yTop - aThickness, zLeft + aThickness);
      glVertex3d(xFront, yBottom + aThickness, zLeft + aThickness);
      glEnd();

      glBegin(GL_LINE_LOOP);
      glVertex3d(xFront, yTop, zRight);
      glVertex3d(xFront, yBottom, zRight);
      glVertex3d(xFront, yBottom + aThickness, zRight - aThickness);
      glVertex3d(xFront, yTop - aThickness, zRight - aThickness);
      glEnd();

      glBegin(GL_LINE_LOOP);
      glVertex3d(xFront, yTop, zLeft);
      glVertex3d(xFront, yTop, zRight);
      glVertex3d(xFront, yTop - aThickness, zRight - aThickness);
      glVertex3d(xFront, yTop - aThickness, zLeft + aThickness);
      glEnd();

      // Back
      glBegin(GL_LINE_LOOP);
      glVertex3d(xBack, yBottom + aThickness, zRight - aThickness);
      glVertex3d(xBack, yBottom + aThickness, zLeft + aThickness);
      glVertex3d(xBack, yBottom, zLeft);
      glVertex3d(xBack, yBottom, zRight);
      glEnd();

      glBegin(GL_LINE_LOOP);
      glVertex3d(xBack, yBottom + aThickness, zLeft + aThickness);
      glVertex3d(xBack, yTop - aThickness, zLeft + aThickness);
      glVertex3d(xBack, yTop, zLeft);
      glVertex3d(xBack, yBottom, zLeft);
      glEnd();

      glBegin(GL_LINE_LOOP);
      glVertex3d(xBack, yTop - aThickness, zRight - aThickness);
      glVertex3d(xBack, yBottom + aThickness, zRight - aThickness);
      glVertex3d(xBack, yBottom, zRight);
      glVertex3d(xBack, yTop, zRight);
      glEnd();

      glBegin(GL_LINE_LOOP);
      glVertex3d(xBack, yTop - aThickness, zLeft + aThickness);
      glVertex3d(xBack, yTop - aThickness, zRight - aThickness);
      glVertex3d(xBack, yTop, zRight);
      glVertex3d(xBack, yTop, zLeft);
      glEnd();
   }

   // Turn off lighting
   glDisable(GL_LIGHTING);
   glDisable(GL_LIGHT0);
}

void GeometryGLWidget::DrawCone(UtVec3dX& aPtVec, double aLength, double aDiameter)
{
   double r  = aDiameter * 0.5;

   double ang_rad[20];
   for (int i = 0; i < 20; ++i)
   {
      ang_rad[i] = (double)i * 18.0 * UtMath::cRAD_PER_DEG;
   }
   UtVec3dX ptVec[20];
   for (int i = 0; i < 20; ++i)
   {
      double x = 0.0;
      double y = r * sin(ang_rad[i]);
      double z = r * cos(ang_rad[i]);
      ptVec[i].Set(x, y, z);
   }

   UtVec3dX normVec[20];
   for (int i = 0; i < 20; ++i)
   {
      double x = r;
      double y = aLength * sin(ang_rad[i]);
      double z = aLength * cos(ang_rad[i]);
      normVec[i].Set(x, y, z);
      normVec[i].Normalize();
   }

   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_NORMALIZE);

   for (int i = 0; i < 20; ++i)
   {
      glPushMatrix();
      glTranslated(aPtVec.X(), aPtVec.Y(), aPtVec.Z());

      glBegin(GL_POLYGON);

      int pt1 = i;
      int pt2 = i + 1;
      if (pt2 > 19) { pt2 = 0; }
      glNormal3d(normVec[pt2].X(), normVec[pt2].Y(), normVec[pt2].Z());
      glVertex3d(ptVec[pt2].X(), ptVec[pt2].Y(), ptVec[pt2].Z());

      glNormal3d(normVec[pt1].X(), normVec[pt1].Y(), normVec[pt1].Z());
      glVertex3d(ptVec[pt1].X(), ptVec[pt1].Y(), ptVec[pt1].Z());

      glNormal3d(0.5 * (normVec[pt1].X() + normVec[pt2].X()), 0.5 * (normVec[pt1].Y() + normVec[pt2].Y()), 0.5 * (normVec[pt1].Z() + normVec[pt2].Z()));
      glVertex3d(aLength, 0.0, 0.0);

      glEnd();

      glPopMatrix();
   }

   glPushMatrix();
   glTranslated(aPtVec.X(), aPtVec.Y(), aPtVec.Z());
   glBegin(GL_POLYGON);
   glNormal3d(-1.0, 0.0, 0.0);
   for (int i = 0; i < 20; ++i)
   {
      glVertex3d(ptVec[i].X(), ptVec[i].Y(), ptVec[i].Z());
   }
   glEnd();
   glPopMatrix();

   // Turn off lighting
   glDisable(GL_LIGHTING);
   glDisable(GL_LIGHT0);
   glDisable(GL_NORMALIZE);
}

void GeometryGLWidget::DrawDisk(UtVec3dX& aPtVec, double aRadius, bool aForward, double aInnerRadius, bool aUseInnerRadius)
{
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);

   glPushMatrix();

   if (aUseInnerRadius)
   {
      if (aForward)
      {
         double delAng = 2.0 * UtMath::cPI / 20.0;
         for (int i = 0; i < 20; ++i)
         {
            double angle_rad1 = (double)i * delAng;
            double angle_rad2 = (double)(i + 1) * delAng;

            glBegin(GL_POLYGON);

            glNormal3d(1.0, 0.0, 0.0);

            double x1 = aPtVec.X();
            double y1 = aPtVec.Y() + aRadius * sin(angle_rad1);
            double z1 = aPtVec.Z() + aRadius * cos(angle_rad1);
            glVertex3d(x1, y1, z1);

            double x2 = aPtVec.X();
            double y2 = aPtVec.Y() + aInnerRadius * sin(angle_rad1);
            double z2 = aPtVec.Z() + aInnerRadius * cos(angle_rad1);
            glVertex3d(x2, y2, z2);

            double x3 = aPtVec.X();
            double y3 = aPtVec.Y() + aInnerRadius * sin(angle_rad2);
            double z3 = aPtVec.Z() + aInnerRadius * cos(angle_rad2);
            glVertex3d(x3, y3, z3);

            double x4 = aPtVec.X();
            double y4 = aPtVec.Y() + aRadius * sin(angle_rad2);
            double z4 = aPtVec.Z() + aRadius * cos(angle_rad2);
            glVertex3d(x4, y4, z4);

            glEnd();
         }
      }
      else
      {
         double delAng = 2.0 * UtMath::cPI / 20.0;
         for (int i = 0; i < 20; ++i)
         {
            double angle_rad1 = (double)i * delAng;
            double angle_rad2 = (double)(i + 1) * delAng;

            glBegin(GL_POLYGON);

            glNormal3d(-1.0, 0.0, 0.0);

            double x1 = aPtVec.X();
            double y1 = aPtVec.Y() + aInnerRadius * sin(angle_rad1);
            double z1 = aPtVec.Z() + aInnerRadius * cos(angle_rad1);
            glVertex3d(x1, y1, z1);

            double x2 = aPtVec.X();
            double y2 = aPtVec.Y() + aRadius * sin(angle_rad1);
            double z2 = aPtVec.Z() + aRadius * cos(angle_rad1);
            glVertex3d(x2, y2, z2);

            double x3 = aPtVec.X();
            double y3 = aPtVec.Y() + aRadius * sin(angle_rad2);
            double z3 = aPtVec.Z() + aRadius * cos(angle_rad2);
            glVertex3d(x3, y3, z3);

            double x4 = aPtVec.X();
            double y4 = aPtVec.Y() + aInnerRadius * sin(angle_rad2);
            double z4 = aPtVec.Z() + aInnerRadius * cos(angle_rad2);
            glVertex3d(x4, y4, z4);

            glEnd();
         }
      }
   }
   else
   {
      glBegin(GL_POLYGON);
      if (aForward)
      {
         glNormal3d(1.0, 0.0, 0.0);
         double delAng = -2.0 * UtMath::cPI / 20.0;
         for (int i = 0; i < 20; ++i)
         {
            double angle_rad = (double)i * delAng;
            double x = aPtVec.X();
            double y = aPtVec.Y() + aRadius * sin(angle_rad);
            double z = aPtVec.Z() + aRadius * cos(angle_rad);
            glVertex3d(x, y, z);
         }
      }
      else
      {
         glNormal3d(-1.0, 0.0, 0.0);
         double delAng = 2.0 * UtMath::cPI / 20.0;
         for (int i = 0; i < 20; ++i)
         {
            double angle_rad = (double)i * delAng;
            double x = aPtVec.X();
            double y = aPtVec.Y() + aRadius * sin(angle_rad);
            double z = aPtVec.Z() + aRadius * cos(angle_rad);
            glVertex3d(x, y, z);
         }
      }
      glEnd();
   }

   glPopMatrix();

   // Turn off lighting
   glDisable(GL_LIGHTING);
   glDisable(GL_LIGHT0);
}

void GeometryGLWidget::DrawCylinder(UtVec3dX& aPtVec, double aLength, double aDiameter, bool aDrawEnds, bool aInside, bool aWireframe)
{
   double r = aDiameter * 0.5;

   double ang_rad[20];
   for (int i = 0; i < 20; ++i)
   {
      ang_rad[i] = (double)i * 18.0 * UtMath::cRAD_PER_DEG;
   }
   UtVec3dX ptVec[20];
   for (int i = 0; i < 20; ++i)
   {
      double x = 0.0;
      double y = r * sin(ang_rad[i]);
      double z = r * cos(ang_rad[i]);
      ptVec[i].Set(x, y, z);
   }

   UtVec3dX normVec[20];
   for (int i = 0; i < 20; ++i)
   {
      double x = 0.0;
      double y = sin(ang_rad[i]);
      double z = cos(ang_rad[i]);

      if (aInside)
      {
         y *= -1.0;
         z *= -1.0;
      }

      normVec[i].Set(x, y, z);
      normVec[i].Normalize();
   }

   if (!aWireframe)
   {
      glEnable(GL_LIGHTING);
      glEnable(GL_LIGHT0);
      glEnable(GL_NORMALIZE);

      for (int i = 0; i < 20; ++i)
      {
         glPushMatrix();
         glTranslated(aPtVec.X(), aPtVec.Y(), aPtVec.Z());

         glBegin(GL_POLYGON);

         int pt1 = i;
         int pt2 = i + 1;
         if (pt2 > 19) { pt2 = 0; }

         if (aInside)
         {
            glNormal3d(normVec[pt2].X(), normVec[pt2].Y(), normVec[pt2].Z());
            glVertex3d(aLength, ptVec[pt2].Y(), ptVec[pt2].Z());

            glNormal3d(normVec[pt1].X(), normVec[pt1].Y(), normVec[pt1].Z());
            glVertex3d(aLength, ptVec[pt1].Y(), ptVec[pt1].Z());

            glNormal3d(normVec[pt1].X(), normVec[pt1].Y(), normVec[pt1].Z());
            glVertex3d(0.0, ptVec[pt1].Y(), ptVec[pt1].Z());

            glNormal3d(normVec[pt2].X(), normVec[pt2].Y(), normVec[pt2].Z());
            glVertex3d(0.0, ptVec[pt2].Y(), ptVec[pt2].Z());
         }
         else
         {
            glNormal3d(normVec[pt2].X(), normVec[pt2].Y(), normVec[pt2].Z());
            glVertex3d(0.0, ptVec[pt2].Y(), ptVec[pt2].Z());

            glNormal3d(normVec[pt1].X(), normVec[pt1].Y(), normVec[pt1].Z());
            glVertex3d(0.0, ptVec[pt1].Y(), ptVec[pt1].Z());

            glNormal3d(normVec[pt1].X(), normVec[pt1].Y(), normVec[pt1].Z());
            glVertex3d(aLength, ptVec[pt1].Y(), ptVec[pt1].Z());

            glNormal3d(normVec[pt2].X(), normVec[pt2].Y(), normVec[pt2].Z());
            glVertex3d(aLength, ptVec[pt2].Y(), ptVec[pt2].Z());
         }

         glEnd();

         glPopMatrix();
      }

      if (aDrawEnds && !aInside)
      {
         glPushMatrix();
         glTranslated(aPtVec.X(), aPtVec.Y(), aPtVec.Z());
         glBegin(GL_POLYGON);
         glNormal3d(-1.0, 0.0, 0.0);
         for (int i = 0; i < 20; ++i)
         {
            glVertex3d(0.0, ptVec[i].Y(), ptVec[i].Z());
         }
         glEnd();
         glBegin(GL_POLYGON);
         glNormal3d(1.0, 0.0, 0.0);
         for (int i = 19; i >= 0; --i)
         {
            glVertex3d(aLength, ptVec[i].Y(), ptVec[i].Z());
         }
         glEnd();
         glPopMatrix();
      }
   }
   else
   {
      for (int i = 0; i < 20; ++i)
      {
         glPushMatrix();
         glTranslated(aPtVec.X(), aPtVec.Y(), aPtVec.Z());
         glBegin(GL_LINE_LOOP);

         int pt1 = i;
         int pt2 = i + 1;
         if (pt2 > 19) { pt2 = 0; }

         if (aInside)
         {
            glVertex3d(aLength, ptVec[pt2].Y(), ptVec[pt2].Z());
            glVertex3d(aLength, ptVec[pt1].Y(), ptVec[pt1].Z());
            glVertex3d(0.0, ptVec[pt1].Y(), ptVec[pt1].Z());
            glVertex3d(0.0, ptVec[pt2].Y(), ptVec[pt2].Z());
         }
         else
         {
            glVertex3d(0.0, ptVec[pt2].Y(), ptVec[pt2].Z());
            glVertex3d(0.0, ptVec[pt1].Y(), ptVec[pt1].Z());
            glVertex3d(aLength, ptVec[pt1].Y(), ptVec[pt1].Z());
            glVertex3d(aLength, ptVec[pt2].Y(), ptVec[pt2].Z());
         }

         glEnd();
         glPopMatrix();
      }

      if (aDrawEnds && !aInside)
      {
         glPushMatrix();
         glTranslated(aPtVec.X(), aPtVec.Y(), aPtVec.Z());
         glBegin(GL_LINE_LOOP);
         for (int i = 0; i < 20; ++i)
         {
            glVertex3d(0.0, ptVec[i].Y(), ptVec[i].Z());
         }
         glEnd();
         glBegin(GL_LINE_LOOP);
         for (int i = 19; i >= 0; --i)
         {
            glVertex3d(aLength, ptVec[i].Y(), ptVec[i].Z());
         }
         glEnd();
         glPopMatrix();
      }
   }

   // Turn off lighting
   glDisable(GL_LIGHTING);
   glDisable(GL_LIGHT0);
   glDisable(GL_NORMALIZE);
}

void GeometryGLWidget::DrawArrow(UtVec3dX& aPtVec, double aHeadLength, double aTotalLength, double aDiameter)
{
   double shaftLength = aTotalLength - aHeadLength;
   UtVec3dX tempShaftVec(shaftLength, 0.0, 0.0);
   UtVec3dX tempVec = aPtVec + tempShaftVec;
   DrawCone(tempVec, aHeadLength, aDiameter);
   DrawCylinder(aPtVec, shaftLength, aDiameter * 0.5);
}

void GeometryGLWidget::DrawAxisLabel(UtVec3dX& aPtVec, double aTotalLength, double aDiameter, const char* aLabel)
{
   mTextFont->SetScale(aDiameter / 20.0); // Fonts are nominally 1.0, which is 32x16

   glPushMatrix();
   glTranslated(aPtVec.X(), aPtVec.Y(), aPtVec.Z());
   glRotated(mViewAz_deg, 0.0, 1.0, 0.0);
   glRotated(mViewEl_deg, -1.0, 0.0, 0.0);

   mTextFont->RenderText(0.0, 0.0, aLabel, TextureFont::eCenterHorizTextJustification, TextureFont::eCenterVertTextJustification, 0, true);

   glPopMatrix();
}

void GeometryGLWidget::DrawSurface(double aPosX,
                                   double aPosY,
                                   double aPosZ,
                                   double aSpan,
                                   double aSweep_deg,
                                   double aRootChord,
                                   double aTipChord,
                                   double aThicknessRatio,
                                   double aDihedral_deg,
                                   double aIncidence_deg,
                                   bool   aWireframe,
                                   int    aNumSpanElements,
                                   std::vector<GeometrySurface::GeometryPanel> aControlSurfaces)
{
   glPushMatrix();

   glTranslated(aPosX, -aPosZ, aPosY);
   glRotated(aDihedral_deg, -1.0, 0.0, 0.0);
   glRotated(aIncidence_deg, 0.0, 0.0, 1.0);

   if (!aWireframe)
   {
      glEnable(GL_LIGHTING);
      glEnable(GL_LIGHT0);
      glEnable(GL_NORMALIZE);
   }

   // Root Airfoil loop
   double rootLeadingEdgeX = aRootChord * 0.25;  // LE is 25% root chord forward of ref pt

   // Tip Airfoil loop
   double tipLeadingEdgeX = rootLeadingEdgeX - aSpan * tan(aSweep_deg * UtMath::cRAD_PER_DEG);

   if (!aWireframe)
   {
      // Root top surface
      glBegin(GL_POLYGON);
      glNormal3d(0.0, 0.0, -1.0);
      for (int i = 6; i >= 0; --i)
      {
         glVertex3d(rootLeadingEdgeX - aRootChord * mAirfoilPtsX[i], 0.5 * aThicknessRatio * aRootChord * mAirfoilPtsY[i], 0.0);
      }
      glEnd();

      // Root bottom surface
      glBegin(GL_POLYGON);
      glNormal3d(0.0, 0.0, -1.0);
      for (int i = 0; i < 7; ++i)
      {
         // Negative height
         glVertex3d(rootLeadingEdgeX - aRootChord * mAirfoilPtsX[i], -0.5 * aThicknessRatio * aRootChord * mAirfoilPtsY[i], 0.0);
      }
      glEnd();

      //      // Tip Airfoil loop
      //      double midChord = -aSpan * tan(aSweep_deg * UtMath::cRAD_PER_DEG);
      //      double tipLeadingEdgeX = midChord + aTipChord * 0.25;

      // Tip top surface
      glBegin(GL_POLYGON);
      glNormal3d(0.0, 0.0, 1.0);
      for (int i = 0; i < 7; ++i)
      {
         glVertex3d(tipLeadingEdgeX - aTipChord * mAirfoilPtsX[i], 0.5 * aThicknessRatio * aTipChord * mAirfoilPtsY[i], aSpan);
      }
      glEnd();

      // Tip bottom surface
      glBegin(GL_POLYGON);
      glNormal3d(0.0, 0.0, 1.0);
      for (int i = 6; i >= 0; --i)
      {
         // Negative height
         glVertex3d(tipLeadingEdgeX - aTipChord * mAirfoilPtsX[i], -0.5 * aThicknessRatio * aTipChord * mAirfoilPtsY[i], aSpan);
      }
      glEnd();
   }
   else
   {
      // Root top surface
      glBegin(GL_LINE_LOOP);
      for (int i = 6; i >= 0; --i)
      {
         glVertex3d(rootLeadingEdgeX - aRootChord * mAirfoilPtsX[i], 0.5 * aThicknessRatio * aRootChord * mAirfoilPtsY[i], 0.0);
      }
      glEnd();

      // Root bottom surface
      glBegin(GL_LINE_LOOP);
      for (int i = 0; i < 7; ++i)
      {
         // Negative height
         glVertex3d(rootLeadingEdgeX - aRootChord * mAirfoilPtsX[i], -0.5 * aThicknessRatio * aRootChord * mAirfoilPtsY[i], 0.0);
      }
      glEnd();

      //      // Tip Airfoil loop
      //      double midChord = -aSpan * tan(aSweep_deg * UtMath::cRAD_PER_DEG);
      //      double tipLeadingEdgeX = midChord + aTipChord * 0.25;

      // Tip top surface
      glBegin(GL_LINE_LOOP);
      for (int i = 0; i < 7; ++i)
      {
         glVertex3d(tipLeadingEdgeX - aTipChord * mAirfoilPtsX[i], 0.5 * aThicknessRatio * aTipChord * mAirfoilPtsY[i], aSpan);
      }
      glEnd();

      // Tip bottom surface
      glBegin(GL_LINE_LOOP);
      for (int i = 6; i >= 0; --i)
      {
         // Negative height
         glVertex3d(tipLeadingEdgeX - aTipChord * mAirfoilPtsX[i], -0.5 * aThicknessRatio * aTipChord * mAirfoilPtsY[i], aSpan);
      }
      glEnd();
   }

   // Next, we calculate the normals for the root/tip top surfaces. Keep in mind that lower values are near the LE.
   UtVec3dX TopRootNormals[7];
   UtVec3dX TopTipNormals[7];
   UtVec3dX spanVec(0.0, 0.0, 0.0);
   UtVec3dX chordVec(0.0, 0.0, 0.0);
   for (int i = 0; i < 7; ++i)
   {
      if (i == 0)
      {
         // Special case - no fore
         UtVec3dX tipCenter(tipLeadingEdgeX - aTipChord * mAirfoilPtsX[i], 0.5 * aThicknessRatio * aTipChord * mAirfoilPtsY[i], aSpan);
         UtVec3dX tipAft(tipLeadingEdgeX - aTipChord * mAirfoilPtsX[i + 1], 0.5 * aThicknessRatio * aTipChord * mAirfoilPtsY[i + 1], aSpan);
         UtVec3dX rootCenter(rootLeadingEdgeX - aRootChord * mAirfoilPtsX[i], 0.5 * aThicknessRatio * aRootChord * mAirfoilPtsY[i], 0.0);
         UtVec3dX rootAft(rootLeadingEdgeX - aRootChord * mAirfoilPtsX[i + 1], 0.5 * aThicknessRatio * aRootChord * mAirfoilPtsY[i + 1], 0.0);

         spanVec = tipCenter - rootCenter;
         chordVec = rootAft - rootCenter;
         TopRootNormals[i] = chordVec.Cross(spanVec);
         TopRootNormals[i].Normalize();

         spanVec = rootCenter - tipCenter;
         chordVec = tipAft - tipCenter;
         TopTipNormals[i] = spanVec.Cross(chordVec);
         TopTipNormals[i].Normalize();
      }
      else if (i == 6)
      {
         // Special case - no aft
         UtVec3dX tipFore(tipLeadingEdgeX - aTipChord * mAirfoilPtsX[i - 1], 0.5 * aThicknessRatio * aTipChord * mAirfoilPtsY[i - 1], aSpan);
         UtVec3dX tipCenter(tipLeadingEdgeX - aTipChord * mAirfoilPtsX[i], 0.5 * aThicknessRatio * aTipChord * mAirfoilPtsY[i], aSpan);
         UtVec3dX rootFore(rootLeadingEdgeX - aRootChord * mAirfoilPtsX[i - 1], 0.5 * aThicknessRatio * aRootChord * mAirfoilPtsY[i - 1], 0.0);
         UtVec3dX rootCenter(rootLeadingEdgeX - aRootChord * mAirfoilPtsX[i], 0.5 * aThicknessRatio * aRootChord * mAirfoilPtsY[i], 0.0);

         spanVec = tipCenter - rootCenter;
         chordVec = rootFore - rootCenter;
         TopRootNormals[i] = spanVec.Cross(chordVec);
         TopRootNormals[i].Normalize();

         spanVec = rootCenter - tipCenter;
         chordVec = tipFore - tipCenter;
         TopTipNormals[i] = chordVec.Cross(spanVec);
         TopTipNormals[i].Normalize();
      }
      else
      {
         // Nominal case
         UtVec3dX tipFore(tipLeadingEdgeX - aTipChord * mAirfoilPtsX[i - 1], 0.5 * aThicknessRatio * aTipChord * mAirfoilPtsY[i - 1], aSpan);
         UtVec3dX tipCenter(tipLeadingEdgeX - aTipChord * mAirfoilPtsX[i], 0.5 * aThicknessRatio * aTipChord * mAirfoilPtsY[i], aSpan);
         UtVec3dX tipAft(tipLeadingEdgeX - aTipChord * mAirfoilPtsX[i + 1], 0.5 * aThicknessRatio * aTipChord * mAirfoilPtsY[i + 1], aSpan);
         UtVec3dX rootFore(rootLeadingEdgeX - aRootChord * mAirfoilPtsX[i - 1], 0.5 * aThicknessRatio * aRootChord * mAirfoilPtsY[i - 1], 0.0);
         UtVec3dX rootCenter(rootLeadingEdgeX - aRootChord * mAirfoilPtsX[i], 0.5 * aThicknessRatio * aRootChord * mAirfoilPtsY[i], 0.0);
         UtVec3dX rootAft(rootLeadingEdgeX - aRootChord * mAirfoilPtsX[i + 1], 0.5 * aThicknessRatio * aRootChord * mAirfoilPtsY[i + 1], 0.0);

         spanVec = tipCenter - rootCenter;
         chordVec = rootAft - rootCenter;
         UtVec3dX rootAftTempNormal = chordVec.Cross(spanVec);
         chordVec = rootFore - rootCenter;
         UtVec3dX rootForeTempNormal = spanVec.Cross(chordVec);
         TopRootNormals[i] = rootAftTempNormal + rootForeTempNormal;
         TopRootNormals[i].Normalize();

         spanVec = rootCenter - tipCenter;
         chordVec = tipAft - tipCenter;
         UtVec3dX tipAftTempNormal = spanVec.Cross(chordVec);
         chordVec = tipFore - tipCenter;
         UtVec3dX tipForeTempNormal = chordVec.Cross(spanVec);
         TopTipNormals[i] = tipAftTempNormal + tipForeTempNormal;
         TopTipNormals[i].Normalize();
      }
   }

   // Next, we calculate the normals for the root/tip bottom surfaces. Keep in mind that lower values are near the LE.
   UtVec3dX BottomRootNormals[7];
   UtVec3dX BottomTipNormals[7];
   for (int i = 0; i < 7; ++i)
   {
      if (i == 0)
      {
         // Special case - no fore (be sure to negate the Y term, since we're doing the bottom)
         UtVec3dX tipCenter(tipLeadingEdgeX - aTipChord * mAirfoilPtsX[i], -0.5 * aThicknessRatio * aTipChord * mAirfoilPtsY[i], aSpan);
         UtVec3dX tipAft(tipLeadingEdgeX - aTipChord * mAirfoilPtsX[i + 1], -0.5 * aThicknessRatio * aTipChord * mAirfoilPtsY[i + 1], aSpan);
         UtVec3dX rootCenter(rootLeadingEdgeX - aRootChord * mAirfoilPtsX[i], -0.5 * aThicknessRatio * aRootChord * mAirfoilPtsY[i], 0.0);
         UtVec3dX rootAft(rootLeadingEdgeX - aRootChord * mAirfoilPtsX[i + 1], -0.5 * aThicknessRatio * aRootChord * mAirfoilPtsY[i + 1], 0.0);

         spanVec = tipCenter - rootCenter;
         chordVec = rootAft - rootCenter;
         BottomRootNormals[i] = spanVec.Cross(chordVec);
         BottomRootNormals[i].Normalize();

         spanVec = rootCenter - tipCenter;
         chordVec = tipAft - tipCenter;
         BottomTipNormals[i] = chordVec.Cross(spanVec);
         BottomTipNormals[i].Normalize();
      }
      else if (i == 6)
      {
         // Special case - no aft (be sure to negate the Y term, since we're doing the bottom)
         UtVec3dX tipFore(tipLeadingEdgeX - aTipChord * mAirfoilPtsX[i - 1], -0.5 * aThicknessRatio * aTipChord * mAirfoilPtsY[i - 1], aSpan);
         UtVec3dX tipCenter(tipLeadingEdgeX - aTipChord * mAirfoilPtsX[i], -0.5 * aThicknessRatio * aTipChord * mAirfoilPtsY[i], aSpan);
         UtVec3dX rootFore(rootLeadingEdgeX - aRootChord * mAirfoilPtsX[i - 1], -0.5 * aThicknessRatio * aRootChord * mAirfoilPtsY[i - 1], 0.0);
         UtVec3dX rootCenter(rootLeadingEdgeX - aRootChord * mAirfoilPtsX[i], -0.5 * aThicknessRatio * aRootChord * mAirfoilPtsY[i], 0.0);

         spanVec = tipCenter - rootCenter;
         chordVec = rootFore - rootCenter;
         BottomRootNormals[i] = chordVec.Cross(spanVec);
         BottomRootNormals[i].Normalize();

         spanVec = rootCenter - tipCenter;
         chordVec = tipFore - tipCenter;
         BottomTipNormals[i] = spanVec.Cross(chordVec);
         BottomTipNormals[i].Normalize();
      }
      else
      {
         // Nominal case (be sure to negate the Y term, since we're doing the bottom)
         UtVec3dX tipFore(tipLeadingEdgeX - aTipChord * mAirfoilPtsX[i - 1], -0.5 * aThicknessRatio * aTipChord * mAirfoilPtsY[i - 1], aSpan);
         UtVec3dX tipCenter(tipLeadingEdgeX - aTipChord * mAirfoilPtsX[i], -0.5 * aThicknessRatio * aTipChord * mAirfoilPtsY[i], aSpan);
         UtVec3dX tipAft(tipLeadingEdgeX - aTipChord * mAirfoilPtsX[i + 1], -0.5 * aThicknessRatio * aTipChord * mAirfoilPtsY[i + 1], aSpan);
         UtVec3dX rootFore(rootLeadingEdgeX - aRootChord * mAirfoilPtsX[i - 1], -0.5 * aThicknessRatio * aRootChord * mAirfoilPtsY[i - 1], 0.0);
         UtVec3dX rootCenter(rootLeadingEdgeX - aRootChord * mAirfoilPtsX[i], -0.5 * aThicknessRatio * aRootChord * mAirfoilPtsY[i], 0.0);
         UtVec3dX rootAft(rootLeadingEdgeX - aRootChord * mAirfoilPtsX[i + 1], -0.5 * aThicknessRatio * aRootChord * mAirfoilPtsY[i + 1], 0.0);

         spanVec = tipCenter - rootCenter;
         chordVec = rootAft - rootCenter;
         UtVec3dX rootAftTempNormal = spanVec.Cross(chordVec);
         chordVec = rootFore - rootCenter;
         UtVec3dX rootForeTempNormal = chordVec.Cross(spanVec);
         BottomRootNormals[i] = rootAftTempNormal + rootForeTempNormal;
         BottomRootNormals[i].Normalize();

         spanVec = rootCenter - tipCenter;
         chordVec = tipAft - tipCenter;
         UtVec3dX tipAftTempNormal = chordVec.Cross(spanVec);
         chordVec = tipFore - tipCenter;
         UtVec3dX tipForeTempNormal = spanVec.Cross(chordVec);
         BottomTipNormals[i] = tipAftTempNormal + tipForeTempNormal;
         BottomTipNormals[i].Normalize();
      }
   }

   if (!aWireframe)
   {
      // Top surface
      for (int i = 0; i < 6; ++i)
      {
         UtVec3dX tipFore(tipLeadingEdgeX - aTipChord * mAirfoilPtsX[i], 0.5 * aThicknessRatio * aTipChord * mAirfoilPtsY[i], aSpan);
         UtVec3dX rootFore(rootLeadingEdgeX - aRootChord * mAirfoilPtsX[i], 0.5 * aThicknessRatio * aRootChord * mAirfoilPtsY[i], 0.0);
         UtVec3dX rootAft(rootLeadingEdgeX - aRootChord * mAirfoilPtsX[i + 1], 0.5 * aThicknessRatio * aRootChord * mAirfoilPtsY[i + 1], 0.0);
         UtVec3dX tipAft(tipLeadingEdgeX - aTipChord * mAirfoilPtsX[i + 1], 0.5 * aThicknessRatio * aTipChord * mAirfoilPtsY[i + 1], aSpan);

         glBegin(GL_POLYGON);
         glNormal3d(TopTipNormals[i].X(), TopTipNormals[i].Y(), TopTipNormals[i].Z());
         glVertex3d(tipFore.X(), tipFore.Y(), tipFore.Z());

         glNormal3d(TopRootNormals[i].X(), TopRootNormals[i].Y(), TopRootNormals[i].Z());
         glVertex3d(rootFore.X(), rootFore.Y(), rootFore.Z());

         glNormal3d(TopRootNormals[i + 1].X(), TopRootNormals[i + 1].Y(), TopRootNormals[i + 1].Z());
         glVertex3d(rootAft.X(), rootAft.Y(), rootAft.Z());

         glNormal3d(TopTipNormals[i + 1].X(), TopTipNormals[i + 1].Y(), TopTipNormals[i + 1].Z());
         glVertex3d(tipAft.X(), tipAft.Y(), tipAft.Z());
         glEnd();
      }

      // Bottom surface
      for (int i = 0; i < 6; ++i)
      {
         UtVec3dX tipFore(tipLeadingEdgeX - aTipChord * mAirfoilPtsX[i], -0.5 * aThicknessRatio * aTipChord * mAirfoilPtsY[i], aSpan);
         UtVec3dX rootFore(rootLeadingEdgeX - aRootChord * mAirfoilPtsX[i], -0.5 * aThicknessRatio * aRootChord * mAirfoilPtsY[i], 0.0);
         UtVec3dX rootAft(rootLeadingEdgeX - aRootChord * mAirfoilPtsX[i + 1], -0.5 * aThicknessRatio * aRootChord * mAirfoilPtsY[i + 1], 0.0);
         UtVec3dX tipAft(tipLeadingEdgeX - aTipChord * mAirfoilPtsX[i + 1], -0.5 * aThicknessRatio * aTipChord * mAirfoilPtsY[i + 1], aSpan);

         glBegin(GL_POLYGON);
         glNormal3d(BottomRootNormals[i].X(), BottomRootNormals[i].Y(), BottomRootNormals[i].Z());
         glVertex3d(rootFore.X(), rootFore.Y(), rootFore.Z());

         glNormal3d(BottomTipNormals[i].X(), BottomTipNormals[i].Y(), BottomTipNormals[i].Z());
         glVertex3d(tipFore.X(), tipFore.Y(), tipFore.Z());

         glNormal3d(BottomTipNormals[i + 1].X(), BottomTipNormals[i + 1].Y(), BottomTipNormals[i + 1].Z());
         glVertex3d(tipAft.X(), tipAft.Y(), tipAft.Z());

         glNormal3d(BottomRootNormals[i + 1].X(), BottomRootNormals[i + 1].Y(), BottomRootNormals[i + 1].Z());
         glVertex3d(rootAft.X(), rootAft.Y(), rootAft.Z());
         glEnd();
      }
   }
   else
   {
      double fraction      = 1.0 / static_cast<double>(aNumSpanElements);
      double rootQuarterX  = rootLeadingEdgeX - 0.25 * aRootChord;
      double rootMidX      = rootLeadingEdgeX - 0.50 * aRootChord;
      double rootTrailingX = rootLeadingEdgeX - aRootChord;
      double tipQuarterX   = tipLeadingEdgeX - 0.25 * aTipChord;
      double tipMidX       = tipLeadingEdgeX - 0.50 * aTipChord;
      double tipTrailingX  = tipLeadingEdgeX - aTipChord;
      double deltaFore     = (tipLeadingEdgeX - rootLeadingEdgeX);
      double deltaQuarter  = (tipQuarterX - rootQuarterX);
      double deltaMid      = (tipMidX - rootMidX);
      double deltaTrailing = (tipTrailingX - rootTrailingX);

      glDisable(GL_DEPTH);

      // Wing Border
      glLineWidth(3.0);
      UtVec3dX tipFore(tipLeadingEdgeX, 0.0, aSpan);
      UtVec3dX rootFore(rootLeadingEdgeX, 0.0, 0.0);
      UtVec3dX rootAft(rootTrailingX, 0.0, 0.0);
      UtVec3dX tipAft(tipTrailingX, 0.0, aSpan);

      glBegin(GL_LINES);
      glVertex3d(tipFore.X(), tipFore.Y(), tipFore.Z());
      glVertex3d(rootFore.X(), rootFore.Y(), rootFore.Z());
      glVertex3d(rootFore.X(), rootFore.Y(), rootFore.Z());
      glVertex3d(rootAft.X(), rootAft.Y(), rootAft.Z());
      glVertex3d(rootAft.X(), rootAft.Y(), rootAft.Z());
      glVertex3d(tipAft.X(), tipAft.Y(), tipAft.Z());
      glVertex3d(tipAft.X(), tipAft.Y(), tipAft.Z());
      glVertex3d(tipFore.X(), tipFore.Y(), tipFore.Z());
      glEnd();

      // Control Surface Borders
      for (auto& controlSurface : aControlSurfaces)
      {
         glLineWidth(2.0);
         UtVec3dX chordFractionStartRoot = rootFore + controlSurface.mChordFractionStart * (rootAft - rootFore);
         UtVec3dX chordFractionStartTip = tipFore + controlSurface.mChordFractionStart * (tipAft - tipFore);
         UtVec3dX chordFractionEndRoot = rootFore + controlSurface.mChordFractionEnd * (rootAft-rootFore);
         UtVec3dX chordFractionEndTip = tipFore + controlSurface.mChordFractionEnd * (tipAft - tipFore);

         UtVec3dX chordFractionStartVector = chordFractionStartTip - chordFractionStartRoot;
         UtVec3dX chordFractionEndVector = chordFractionEndTip - chordFractionEndRoot;

         UtVec3dX controlSurfaceTipFore = chordFractionStartRoot + controlSurface.mSpanFractionEnd * chordFractionStartVector;
         UtVec3dX controlSurfaceRootFore = chordFractionStartRoot + controlSurface.mSpanFractionStart * chordFractionStartVector;
         UtVec3dX controlSurfaceTipAft = chordFractionEndRoot + controlSurface.mSpanFractionEnd * chordFractionEndVector;
         UtVec3dX controlSurfaceRootAft = chordFractionEndRoot + controlSurface.mSpanFractionStart * chordFractionEndVector;

         glBegin(GL_LINES);
         glVertex3d(controlSurfaceTipFore.X(), controlSurfaceTipFore.Y(), controlSurfaceTipFore.Z());
         glVertex3d(controlSurfaceRootFore.X(), controlSurfaceRootFore.Y(), controlSurfaceRootFore.Z());
         glVertex3d(controlSurfaceRootFore.X(), controlSurfaceRootFore.Y(), controlSurfaceRootFore.Z());
         glVertex3d(controlSurfaceRootAft.X(), controlSurfaceRootAft.Y(), controlSurfaceRootAft.Z());
         glVertex3d(controlSurfaceRootAft.X(), controlSurfaceRootAft.Y(), controlSurfaceRootAft.Z());
         glVertex3d(controlSurfaceTipAft.X(), controlSurfaceTipAft.Y(), controlSurfaceTipAft.Z());
         glVertex3d(controlSurfaceTipAft.X(), controlSurfaceTipAft.Y(), controlSurfaceTipAft.Z());
         glVertex3d(controlSurfaceTipFore.X(), controlSurfaceTipFore.Y(), controlSurfaceTipFore.Z());
         glEnd();
      }
      glLineWidth(1.0);

      for (int i = 0; i < aNumSpanElements; ++i)
      {
         auto n1 = fraction * static_cast<double>(i);
         auto n2 = fraction * static_cast<double>(i + 1);

         // Leading edge to quarter
         {
            UtVec3dX csTipFore(rootLeadingEdgeX + deltaFore * n2, 0.0, aSpan * n2);
            UtVec3dX csRootFore(rootLeadingEdgeX + deltaFore * n1, 0.0, aSpan * n1);
            UtVec3dX csRootAft(rootQuarterX + deltaQuarter * n1, 0.0, aSpan * n1);
            UtVec3dX csTipAft(rootQuarterX + deltaQuarter * n2, 0.0, aSpan * n2);
            glBegin(GL_LINE_LOOP);
            glVertex3d(csTipFore.X(), csTipFore.Y(), csTipFore.Z());
            glVertex3d(csRootFore.X(), csRootFore.Y(), csRootFore.Z());
            glVertex3d(csRootAft.X(), csRootAft.Y(), csRootAft.Z());
            glVertex3d(csTipAft.X(), csTipAft.Y(), csTipAft.Z());
            glEnd();
         }
         // Quarter to mid
         {
            UtVec3dX csTipFore(rootQuarterX + deltaQuarter * n2, 0.0, aSpan * n2);
            UtVec3dX csRootFore(rootQuarterX + deltaQuarter * n1, 0.0, aSpan * n1);
            UtVec3dX csRootAft(rootMidX + deltaMid * n1, 0.0, aSpan * n1);
            UtVec3dX csTipAft(rootMidX + deltaMid * n2, 0.0, aSpan * n2);
            glBegin(GL_LINE_LOOP);
            glVertex3d(csTipFore.X(), csTipFore.Y(), csTipFore.Z());
            glVertex3d(csRootFore.X(), csRootFore.Y(), csRootFore.Z());
            glVertex3d(csRootAft.X(), csRootAft.Y(), csRootAft.Z());
            glVertex3d(csTipAft.X(), csTipAft.Y(), csTipAft.Z());
            glEnd();
         }
         // Mid to trailing edge
         {
            UtVec3dX csTipFore(rootMidX + deltaMid * n2, 0.0, aSpan * n2);
            UtVec3dX csRootFore(rootMidX + deltaMid * n1, 0.0, aSpan * n1);
            UtVec3dX csRootAft(rootTrailingX + deltaTrailing * n1, 0.0, aSpan * n1);
            UtVec3dX csTipAft(rootTrailingX + deltaTrailing * n2, 0.0, aSpan * n2);
            glBegin(GL_LINE_LOOP);
            glVertex3d(csTipFore.X(), csTipFore.Y(), csTipFore.Z());
            glVertex3d(csRootFore.X(), csRootFore.Y(), csRootFore.Z());
            glVertex3d(csRootAft.X(), csRootAft.Y(), csRootAft.Z());
            glVertex3d(csTipAft.X(), csTipAft.Y(), csTipAft.Z());
            glEnd();
         }
      }

      glEnable(GL_DEPTH);
   }

   glPopMatrix();

   // Turn off lighting
   glDisable(GL_LIGHTING);
   glDisable(GL_LIGHT0);
   glDisable(GL_NORMALIZE);
}

void GeometryGLWidget::DrawEngine(double aPosX,
                                  double aPosY,
                                  double aPosZ,
                                  double aDiameter_ft,
                                  double aLength_ft,
                                  double aOffset_ft,
                                  double aYaw_deg,
                                  double aPitch_deg,
                                  double aRoll_deg,
                                  bool   aDrawThrustVector,
                                  bool   aHighlighted,
                                  bool   aWireframe)
{
   UtVec3dX zeroPt(0.0, 0.0, 0.0);

   if (!aWireframe)
   {
      if (aHighlighted)
      {
         SetHighlightedMaterialProperties();
      }
      else
      {
         SetEngineMaterialProperties();
      }
   }

   glPushMatrix();
   glTranslated(aPosX, -aPosZ, aPosY);
   glRotated(aYaw_deg,   0.0, -1.0, 0.0);
   glRotated(aPitch_deg, 0.0, 0.0, 1.0);
   glRotated(aRoll_deg,  1.0, 0.0, 0.0);
   glTranslated(-aLength_ft * 0.5, 0.0, 0.0);
   DrawCylinder(zeroPt, aLength_ft, aDiameter_ft, true, false, aWireframe);
   glPopMatrix();

   if ((aLength_ft * 0.5 + aOffset_ft) < 0.0)
   {
      if (aHighlighted)
      {
         SetHighlightedMaterialProperties(0.6);
      }
      else
      {
         SetEngineMaterialProperties(0.6);
      }

      double burnerLength = -aLength_ft * 0.5 - aOffset_ft;

      glPushMatrix();
      glTranslated(aPosX, -aPosZ, aPosY);
      glRotated(aYaw_deg, 0.0, -1.0, 0.0);
      glRotated(aPitch_deg, 0.0, 0.0, 1.0);
      glRotated(aRoll_deg, 1.0, 0.0, 0.0);
      glTranslated(-aLength_ft * 0.5 - burnerLength, 0.0, 0.0);
      DrawCylinder(zeroPt, burnerLength, aDiameter_ft, true, false, aWireframe);
      glPopMatrix();
   }

   if (aDrawThrustVector)
   {
      glPushMatrix();
      glTranslated(aPosX, -aPosZ, aPosY);
      glRotated(aYaw_deg, 0.0, -1.0, 0.0);
      glRotated(aPitch_deg, 0.0, 0.0, 1.0);
      glRotated(aRoll_deg, 1.0, 0.0, 0.0);
      SetVioletMaterialProperties();
      double arrowDiameter = aDiameter_ft * 0.25;
      double arrowHeadLength = arrowDiameter * 3.0;
      double arrowLength = arrowHeadLength * 2.5;
      UtVec3dX ptVec(-arrowLength + aOffset_ft, 0.0, 0.0);
      DrawArrow(ptVec, arrowHeadLength, arrowLength, arrowDiameter);
      glPopMatrix();

      if (aHighlighted)
      {
         SetHighlightedMaterialProperties();
      }
      else
      {
         SetEngineMaterialProperties();
      }
   }
}

void GeometryGLWidget::DrawRoundedNacelle(double             aLength_ft,
                                          double             aHeight_ft,
                                          double             aWidth_ft,
                                          double             aThickness_ft,
                                          const std::string& aOverallShapeString,
                                          bool               aAftSectionIsTapered,
                                          double             aAftSectionLength_ft,
                                          bool               aWireframe)
{
   double halfHeight = 0.5 * aHeight_ft;
   double halfWidth = 0.5 * aWidth_ft;
   double offsetPosX = 0.0;
   double offsetPosY = 0.0;
   double offsetPosZ = 0.0;

   // Ensure zero aft length, if appropriate
   if (!aAftSectionIsTapered)
   {
      aAftSectionLength_ft = 0.0;
   }

   // Inlet ........................................................

   GraphicalPointCollection inletPointCollection(true);

   // Draw rings in increasing x

   double inletX0 = offsetPosX + aLength_ft * 0.5;
   double inletHalfHeight0 = halfHeight - aThickness_ft;
   double inletHalfWidth0 = halfWidth - aThickness_ft;

   double inletX1 = offsetPosX + aLength_ft * 0.5 + 0.001 * aLength_ft;
   double inletHalfHeight1 = halfHeight * 0.001;
   double inletHalfWidth1 = halfWidth * 0.001;

   DarkenMaterialProperties(0.4f);

   // Add center ring
   inletPointCollection.AddRing();
   for (int hgt = 0; hgt < 10; ++hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight0;
      double z = EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX0;
      inletPointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 10; hgt > 0; --hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight0;
      double z = -EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX0;
      inletPointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -10; --hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight0;
      double z = -EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX0;
      inletPointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -10; hgt < 0; ++hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight0;
      double z = EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX0;
      inletPointCollection.AddPoint(x, y, z);
   }

   // Add inlet outer radius ring
   inletPointCollection.AddRing();
   for (int hgt = 0; hgt < 10; ++hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight1;
      double z = EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX1;
      inletPointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 10; hgt > 0; --hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight1;
      double z = -EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX1;
      inletPointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -10; --hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight1;
      double z = -EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX1;
      inletPointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -10; hgt < 0; ++hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight1;
      double z = EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX1;
      inletPointCollection.AddPoint(x, y, z);
   }

   // Calculate normals and draw
   inletPointCollection.CalculatePointCollection();
   inletPointCollection.Draw(aWireframe);

   // Main portion of nacelle ......................................

   double exitLipLength = aLength_ft * 0.15;

   double inletLipLength = aLength_ft * 0.075;
   if (aAftSectionIsTapered)
   {
      double maxLipLength = aLength_ft - aAftSectionLength_ft;
      if (inletLipLength > maxLipLength)
      {
         inletLipLength = maxLipLength - aLength_ft * 0.01;
      }

      // Modify values if necessary
      if (inletLipLength < aLength_ft * 0.01)
      {
         inletLipLength = aLength_ft * 0.01;
         aAftSectionLength_ft = aLength_ft * 0.98;
      }
   }

   RestoreMaterialProperties();

   GraphicalPointCollection pointCollection(true);

   double x3 = offsetPosX + aLength_ft * 0.5;
   double halfHeight3 = halfHeight - aThickness_ft;
   double halfWidth3 = halfWidth - aThickness_ft;

   double x2 = offsetPosX + aLength_ft * 0.5 - inletLipLength;
   double halfHeight2 = halfHeight;
   double halfWidth2 = halfWidth;

   double x1 = 0.0;
   double halfHeight1 = 0.0;
   double halfWidth1 = 0.0;
   double x0 = 0.0;
   double halfHeight0 = 0.0;
   double halfWidth0 = 0.0;

   if (aAftSectionIsTapered)
   {
      x1 = offsetPosX - aLength_ft * 0.5 + aAftSectionLength_ft;
      halfHeight1 = halfHeight;
      halfWidth1 = halfWidth;

      x0 = offsetPosX - aLength_ft * 0.5;
      halfHeight0 = halfHeight * 0.001;
      halfWidth0 = halfWidth * 0.001;
   }
   else
   {
      x1 = offsetPosX - aLength_ft * 0.5 + exitLipLength;
      halfHeight1 = halfHeight;
      halfWidth1 = halfWidth;

      x0 = offsetPosX - aLength_ft * 0.5;
      halfHeight0 = halfHeight - aThickness_ft;
      halfWidth0 = halfWidth - aThickness_ft;
   }

   pointCollection.AddRing();
   for (int hgt = 0; hgt < 10; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight0;
      double z = EllipticalX_GivenY(y, halfWidth0, halfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x0;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 10; hgt > 0; --hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight0;
      double z = -EllipticalX_GivenY(y, halfWidth0, halfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x0;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -10; --hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight0;
      double z = -EllipticalX_GivenY(y, halfWidth0, halfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x0;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -10; hgt < 0; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight0;
      double z = EllipticalX_GivenY(y, halfWidth0, halfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x0;
      pointCollection.AddPoint(x, y, z);
   }

   pointCollection.AddRing();
   for (int hgt = 0; hgt < 10; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight1;
      double z = EllipticalX_GivenY(y, halfWidth1, halfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x1;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 10; hgt > 0; --hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight1;
      double z = -EllipticalX_GivenY(y, halfWidth1, halfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x1;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -10; --hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight1;
      double z = -EllipticalX_GivenY(y, halfWidth1, halfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x1;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -10; hgt < 0; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight1;
      double z = EllipticalX_GivenY(y, halfWidth1, halfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x1;
      pointCollection.AddPoint(x, y, z);
   }

   pointCollection.AddRing();
   for (int hgt = 0; hgt < 10; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight2;
      double z = EllipticalX_GivenY(y, halfWidth2, halfHeight2);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x2;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 10; hgt > 0; --hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight2;
      double z = -EllipticalX_GivenY(y, halfWidth2, halfHeight2);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x2;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -10; --hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight2;
      double z = -EllipticalX_GivenY(y, halfWidth2, halfHeight2);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x2;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -10; hgt < 0; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight2;
      double z = EllipticalX_GivenY(y, halfWidth2, halfHeight2);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x2;
      pointCollection.AddPoint(x, y, z);
   }

   pointCollection.AddRing();
   for (int hgt = 0; hgt < 10; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight3;
      double z = EllipticalX_GivenY(y, halfWidth3, halfHeight3);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x3;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 10; hgt > 0; --hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight3;
      double z = -EllipticalX_GivenY(y, halfWidth3, halfHeight3);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x3;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -10; --hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight3;
      double z = -EllipticalX_GivenY(y, halfWidth3, halfHeight3);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x3;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -10; hgt < 0; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight3;
      double z = EllipticalX_GivenY(y, halfWidth3, halfHeight3);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x3;
      pointCollection.AddPoint(x, y, z);
   }

   // Calculate normals and draw
   pointCollection.CalculatePointCollection();
   pointCollection.Draw(aWireframe);

   // Exit .........................................................

   if (!aAftSectionIsTapered)
   {
      GraphicalPointCollection exitPointCollection(true);

      // Draw rings in increasing x

      inletX0 = offsetPosX - aLength_ft * 0.5 - 0.001 * aLength_ft;
      inletHalfHeight0 = halfHeight * 0.001;
      inletHalfWidth0 = halfWidth * 0.001;

      inletX1 = offsetPosX - aLength_ft * 0.5;
      inletHalfHeight1 = halfHeight - aThickness_ft;
      inletHalfWidth1 = halfWidth - aThickness_ft;

      DarkenMaterialProperties(0.4f);

      // Add center ring
      exitPointCollection.AddRing();
      for (int hgt = 0; hgt < 10; ++hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight0;
         double z = EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX0;
         exitPointCollection.AddPoint(x, y, z);
      }
      for (int hgt = 10; hgt > 0; --hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight0;
         double z = -EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX0;
         exitPointCollection.AddPoint(x, y, z);
      }
      for (int hgt = 0; hgt > -10; --hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight0;
         double z = -EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX0;
         exitPointCollection.AddPoint(x, y, z);
      }
      for (int hgt = -10; hgt < 0; ++hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight0;
         double z = EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX0;
         exitPointCollection.AddPoint(x, y, z);
      }

      // Add inlet outer radius ring
      exitPointCollection.AddRing();
      for (int hgt = 0; hgt < 10; ++hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight1;
         double z = EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX1;
         exitPointCollection.AddPoint(x, y, z);
      }
      for (int hgt = 10; hgt > 0; --hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight1;
         double z = -EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX1;
         exitPointCollection.AddPoint(x, y, z);
      }
      for (int hgt = 0; hgt > -10; --hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight1;
         double z = -EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX1;
         exitPointCollection.AddPoint(x, y, z);
      }
      for (int hgt = -10; hgt < 0; ++hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight1;
         double z = EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX1;
         exitPointCollection.AddPoint(x, y, z);
      }

      // Calculate normals and draw
      exitPointCollection.CalculatePointCollection();
      exitPointCollection.Draw(aWireframe);
   }
}

void GeometryGLWidget::DrawRoundedRightNacelle(double             aLength_ft,
                                               double             aHeight_ft,
                                               double             aWidth_ft,
                                               double             aThickness_ft,
                                               const std::string& aOverallShapeString,
                                               bool               aAftSectionIsTapered,
                                               double             aAftSectionLength_ft,
                                               bool               aWireframe)
{
   double halfHeight = 0.5 * aHeight_ft;
   double halfWidth = 0.5 * aWidth_ft;
   double offsetPosX = 0.0;
   double offsetPosY = 0.0;
   double offsetPosZ = 0.0;

   // Ensure zero aft length, if appropriate
   if (!aAftSectionIsTapered)
   {
      aAftSectionLength_ft = 0.0;
   }

   // Inlet ........................................................

   GraphicalPointCollection inletPointCollection(true);

   // Draw rings in increasing x

   double inletX0 = offsetPosX + aLength_ft * 0.5;
   double inletHalfHeight0 = halfHeight - aThickness_ft;
   double inletHalfWidth0 = halfWidth - aThickness_ft;

   double inletX1 = offsetPosX + aLength_ft * 0.5 + 0.001 * aLength_ft;
   double inletHalfHeight1 = halfHeight * 0.001;
   double inletHalfWidth1 = halfWidth * 0.001;

   DarkenMaterialProperties(0.4f);

   // Add center ring
   inletPointCollection.AddRing();
   for (int hgt = -10; hgt < 0; ++hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight0;
      double z = EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX0;
      inletPointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt <= 10; ++hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight0;
      double z = EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX0;
      inletPointCollection.AddPoint(x, y, z);
   }

   // Add inlet outer radius ring
   inletPointCollection.AddRing();
   for (int hgt = -10; hgt < 0; ++hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight1;
      double z = EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX1;
      inletPointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt <= 10; ++hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight1;
      double z = EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX1;
      inletPointCollection.AddPoint(x, y, z);
   }

   // Calculate normals and draw
   inletPointCollection.CalculatePointCollection();
   inletPointCollection.Draw(aWireframe);

   // Main portion of nacelle ......................................

   double exitLipLength = aLength_ft * 0.15;

   double inletLipLength = aLength_ft * 0.075;
   if (aAftSectionIsTapered)
   {
      double maxLipLength = aLength_ft - aAftSectionLength_ft;
      if (inletLipLength > maxLipLength)
      {
         inletLipLength = maxLipLength - aLength_ft * 0.01;
      }

      // Modify values if necessary
      if (inletLipLength < aLength_ft * 0.01)
      {
         inletLipLength = aLength_ft * 0.01;
         aAftSectionLength_ft = aLength_ft * 0.98;
      }
   }

   RestoreMaterialProperties();

   GraphicalPointCollection pointCollection(true);

   double x3 = offsetPosX + aLength_ft * 0.5;
   double halfHeight3 = halfHeight - aThickness_ft;
   double halfWidth3 = halfWidth - aThickness_ft;

   double x2 = offsetPosX + aLength_ft * 0.5 - inletLipLength;
   double halfHeight2 = halfHeight;
   double halfWidth2 = halfWidth;

   double x1 = 0.0;
   double halfHeight1 = 0.0;
   double halfWidth1 = 0.0;
   double x0 = 0.0;
   double halfHeight0 = 0.0;
   double halfWidth0 = 0.0;

   if (aAftSectionIsTapered)
   {
      x1 = offsetPosX - aLength_ft * 0.5 + aAftSectionLength_ft;
      halfHeight1 = halfHeight;
      halfWidth1 = halfWidth;

      x0 = offsetPosX - aLength_ft * 0.5;
      halfHeight0 = halfHeight * 0.001;
      halfWidth0 = halfWidth * 0.001;
   }
   else
   {
      x1 = offsetPosX - aLength_ft * 0.5 + exitLipLength;
      halfHeight1 = halfHeight;
      halfWidth1 = halfWidth;

      x0 = offsetPosX - aLength_ft * 0.5;
      halfHeight0 = halfHeight - aThickness_ft;
      halfWidth0 = halfWidth - aThickness_ft;
   }

   pointCollection.AddRing();
   for (int hgt = -10; hgt < 0; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight0;
      double z = EllipticalX_GivenY(y, halfWidth0, halfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x0;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt <= 10; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight0;
      double z = EllipticalX_GivenY(y, halfWidth0, halfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x0;
      pointCollection.AddPoint(x, y, z);
   }

   pointCollection.AddRing();
   for (int hgt = -10; hgt < 0; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight1;
      double z = EllipticalX_GivenY(y, halfWidth1, halfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x1;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt <= 10; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight1;
      double z = EllipticalX_GivenY(y, halfWidth1, halfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x1;
      pointCollection.AddPoint(x, y, z);
   }

   pointCollection.AddRing();
   for (int hgt = -10; hgt < 0; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight2;
      double z = EllipticalX_GivenY(y, halfWidth2, halfHeight2);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x2;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt <= 10; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight2;
      double z = EllipticalX_GivenY(y, halfWidth2, halfHeight2);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x2;
      pointCollection.AddPoint(x, y, z);
   }

   pointCollection.AddRing();
   for (int hgt = -10; hgt < 0; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight3;
      double z = EllipticalX_GivenY(y, halfWidth3, halfHeight3);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x3;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt <= 10; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight3;
      double z = EllipticalX_GivenY(y, halfWidth3, halfHeight3);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x3;
      pointCollection.AddPoint(x, y, z);
   }

   // Calculate normals and draw
   pointCollection.CalculatePointCollection();
   pointCollection.Draw(aWireframe);

   // Exit .........................................................

   if (!aAftSectionIsTapered)
   {
      GraphicalPointCollection exitPointCollection(true);

      // Draw rings in increasing x

      inletX0 = offsetPosX - aLength_ft * 0.5 - 0.001 * aLength_ft;
      inletHalfHeight0 = halfHeight * 0.001;
      inletHalfWidth0 = halfWidth * 0.001;

      inletX1 = offsetPosX - aLength_ft * 0.5;
      inletHalfHeight1 = halfHeight - aThickness_ft;
      inletHalfWidth1 = halfWidth - aThickness_ft;

      DarkenMaterialProperties(0.4f);

      // Add center ring
      exitPointCollection.AddRing();
      for (int hgt = -10; hgt < 0; ++hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight0;
         double z = EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX0;
         exitPointCollection.AddPoint(x, y, z);
      }
      for (int hgt = 0; hgt <= 10; ++hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight0;
         double z = EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX0;
         exitPointCollection.AddPoint(x, y, z);
      }

      // Add inlet outer radius ring
      exitPointCollection.AddRing();
      for (int hgt = -10; hgt < 0; ++hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight1;
         double z = EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX1;
         exitPointCollection.AddPoint(x, y, z);
      }
      for (int hgt = 0; hgt <= 10; ++hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight1;
         double z = EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX1;
         exitPointCollection.AddPoint(x, y, z);
      }

      // Calculate normals and draw
      exitPointCollection.CalculatePointCollection();
      exitPointCollection.Draw(aWireframe);
   }
}

void GeometryGLWidget::DrawRoundedLeftNacelle(double             aLength_ft,
                                              double             aHeight_ft,
                                              double             aWidth_ft,
                                              double             aThickness_ft,
                                              const std::string& aOverallShapeString,
                                              bool               aAftSectionIsTapered,
                                              double             aAftSectionLength_ft,
                                              bool               aWireframe)
{
   double halfHeight = 0.5 * aHeight_ft;
   double halfWidth = 0.5 * aWidth_ft;
   double offsetPosX = 0.0;
   double offsetPosY = 0.0;
   double offsetPosZ = 0.0;

   // Ensure zero aft length, if appropriate
   if (!aAftSectionIsTapered)
   {
      aAftSectionLength_ft = 0.0;
   }

   // Inlet ........................................................

   GraphicalPointCollection inletPointCollection(true);

   // Draw rings in increasing x

   double inletX0 = offsetPosX + aLength_ft * 0.5;
   double inletHalfHeight0 = halfHeight - aThickness_ft;
   double inletHalfWidth0 = halfWidth - aThickness_ft;

   double inletX1 = offsetPosX + aLength_ft * 0.5 + 0.001 * aLength_ft;
   double inletHalfHeight1 = halfHeight * 0.001;
   double inletHalfWidth1 = halfWidth * 0.001;

   DarkenMaterialProperties(0.4f);

   // Add center ring
   inletPointCollection.AddRing();
   for (int hgt = 10; hgt >= -10; --hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight0;
      double z = -EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX0;
      inletPointCollection.AddPoint(x, y, z);
   }

   // Add inlet outer radius ring
   inletPointCollection.AddRing();
   for (int hgt = 10; hgt >= -10; --hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight1;
      double z = -EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX1;
      inletPointCollection.AddPoint(x, y, z);
   }

   // Calculate normals and draw
   inletPointCollection.CalculatePointCollection();
   inletPointCollection.Draw(aWireframe);

   // Main portion of nacelle ......................................

   double exitLipLength = aLength_ft * 0.15;

   double inletLipLength = aLength_ft * 0.075;
   if (aAftSectionIsTapered)
   {
      double maxLipLength = aLength_ft - aAftSectionLength_ft;
      if (inletLipLength > maxLipLength)
      {
         inletLipLength = maxLipLength - aLength_ft * 0.01;
      }

      // Modify values if necessary
      if (inletLipLength < aLength_ft * 0.01)
      {
         inletLipLength = aLength_ft * 0.01;
         aAftSectionLength_ft = aLength_ft * 0.98;
      }
   }

   RestoreMaterialProperties();

   GraphicalPointCollection pointCollection(true);

   double x3 = offsetPosX + aLength_ft * 0.5;
   double halfHeight3 = halfHeight - aThickness_ft;
   double halfWidth3 = halfWidth - aThickness_ft;

   double x2 = offsetPosX + aLength_ft * 0.5 - inletLipLength;
   double halfHeight2 = halfHeight;
   double halfWidth2 = halfWidth;

   double x1 = 0.0;
   double halfHeight1 = 0.0;
   double halfWidth1 = 0.0;
   double x0 = 0.0;
   double halfHeight0 = 0.0;
   double halfWidth0 = 0.0;

   if (aAftSectionIsTapered)
   {
      x1 = offsetPosX - aLength_ft * 0.5 + aAftSectionLength_ft;
      halfHeight1 = halfHeight;
      halfWidth1 = halfWidth;

      x0 = offsetPosX - aLength_ft * 0.5;
      halfHeight0 = halfHeight * 0.001;
      halfWidth0 = halfWidth * 0.001;
   }
   else
   {
      x1 = offsetPosX - aLength_ft * 0.5 + exitLipLength;
      halfHeight1 = halfHeight;
      halfWidth1 = halfWidth;

      x0 = offsetPosX - aLength_ft * 0.5;
      halfHeight0 = halfHeight - aThickness_ft;
      halfWidth0 = halfWidth - aThickness_ft;
   }

   pointCollection.AddRing();
   for (int hgt = 10; hgt >= -10; --hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight0;
      double z = -EllipticalX_GivenY(y, halfWidth0, halfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x0;
      pointCollection.AddPoint(x, y, z);
   }

   pointCollection.AddRing();
   for (int hgt = 10; hgt >= -10; --hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight1;
      double z = -EllipticalX_GivenY(y, halfWidth1, halfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x1;
      pointCollection.AddPoint(x, y, z);
   }

   pointCollection.AddRing();
   for (int hgt = 10; hgt >= -10; --hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight2;
      double z = -EllipticalX_GivenY(y, halfWidth2, halfHeight2);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x2;
      pointCollection.AddPoint(x, y, z);
   }

   pointCollection.AddRing();
   for (int hgt = 10; hgt >= -10; --hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight3;
      double z = -EllipticalX_GivenY(y, halfWidth3, halfHeight3);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x3;
      pointCollection.AddPoint(x, y, z);
   }

   // Calculate normals and draw
   pointCollection.CalculatePointCollection();
   pointCollection.Draw(aWireframe);

   // Exit .........................................................

   if (!aAftSectionIsTapered)
   {
      GraphicalPointCollection exitPointCollection(true);

      // Draw rings in increasing x

      inletX0 = offsetPosX - aLength_ft * 0.5 - 0.001 * aLength_ft;
      inletHalfHeight0 = halfHeight * 0.001;
      inletHalfWidth0 = halfWidth * 0.001;

      inletX1 = offsetPosX - aLength_ft * 0.5;
      inletHalfHeight1 = halfHeight - aThickness_ft;
      inletHalfWidth1 = halfWidth - aThickness_ft;

      DarkenMaterialProperties(0.4f);

      // Add center ring
      exitPointCollection.AddRing();
      for (int hgt = 10; hgt >= -10; --hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight0;
         double z = -EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX0;
         exitPointCollection.AddPoint(x, y, z);
      }

      // Add inlet outer radius ring
      exitPointCollection.AddRing();
      for (int hgt = 10; hgt >= -10; --hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight1;
         double z = -EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX1;
         exitPointCollection.AddPoint(x, y, z);
      }

      // Calculate normals and draw
      exitPointCollection.CalculatePointCollection();
      exitPointCollection.Draw(aWireframe);
   }
}

void GeometryGLWidget::DrawRoundedTopNacelle(double             aLength_ft,
                                             double             aHeight_ft,
                                             double             aWidth_ft,
                                             double             aThickness_ft,
                                             const std::string& aOverallShapeString,
                                             bool               aAftSectionIsTapered,
                                             double             aAftSectionLength_ft,
                                             bool               aWireframe)
{
   double halfHeight = 0.5 * aHeight_ft;
   double halfWidth = 0.5 * aWidth_ft;
   double offsetPosX = 0.0;
   double offsetPosY = 0.0;
   double offsetPosZ = 0.0;

   // Ensure zero aft length, if appropriate
   if (!aAftSectionIsTapered)
   {
      aAftSectionLength_ft = 0.0;
   }

   // Inlet ........................................................

   GraphicalPointCollection inletPointCollection(true);

   // Draw rings in increasing x

   double inletX0 = offsetPosX + aLength_ft * 0.5;
   double inletHalfHeight0 = halfHeight - aThickness_ft;
   double inletHalfWidth0 = halfWidth - aThickness_ft;

   double inletX1 = offsetPosX + aLength_ft * 0.5 + 0.001 * aLength_ft;
   double inletHalfHeight1 = halfHeight * 0.001;
   double inletHalfWidth1 = halfWidth * 0.001;

   DarkenMaterialProperties(0.4f);

   // Add center ring
   inletPointCollection.AddRing();
   for (int hgt = 0; hgt < 10; ++hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight0;
      double z = EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX0;
      inletPointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 10; hgt >= 0; --hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight0;
      double z = -EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX0;
      inletPointCollection.AddPoint(x, y, z);
   }

   // Add inlet outer radius ring
   inletPointCollection.AddRing();
   for (int hgt = 0; hgt < 10; ++hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight1;
      double z = EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX1;
      inletPointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 10; hgt >= 0; --hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight1;
      double z = -EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX1;
      inletPointCollection.AddPoint(x, y, z);
   }

   // Calculate normals and draw
   inletPointCollection.CalculatePointCollection();
   inletPointCollection.Draw(aWireframe);

   // Main portion of nacelle ......................................

   double exitLipLength = aLength_ft * 0.15;

   double inletLipLength = aLength_ft * 0.075;
   if (aAftSectionIsTapered)
   {
      double maxLipLength = aLength_ft - aAftSectionLength_ft;
      if (inletLipLength > maxLipLength)
      {
         inletLipLength = maxLipLength - aLength_ft * 0.01;
      }

      // Modify values if necessary
      if (inletLipLength < aLength_ft * 0.01)
      {
         inletLipLength = aLength_ft * 0.01;
         aAftSectionLength_ft = aLength_ft * 0.98;
      }
   }

   RestoreMaterialProperties();

   GraphicalPointCollection pointCollection(true);

   double x3 = offsetPosX + aLength_ft * 0.5;
   double halfHeight3 = halfHeight - aThickness_ft;
   double halfWidth3 = halfWidth - aThickness_ft;

   double x2 = offsetPosX + aLength_ft * 0.5 - inletLipLength;
   double halfHeight2 = halfHeight;
   double halfWidth2 = halfWidth;

   double x1 = 0.0;
   double halfHeight1 = 0.0;
   double halfWidth1 = 0.0;
   double x0 = 0.0;
   double halfHeight0 = 0.0;
   double halfWidth0 = 0.0;

   if (aAftSectionIsTapered)
   {
      x1 = offsetPosX - aLength_ft * 0.5 + aAftSectionLength_ft;
      halfHeight1 = halfHeight;
      halfWidth1 = halfWidth;

      x0 = offsetPosX - aLength_ft * 0.5;
      halfHeight0 = halfHeight * 0.001;
      halfWidth0 = halfWidth * 0.001;
   }
   else
   {
      x1 = offsetPosX - aLength_ft * 0.5 + exitLipLength;
      halfHeight1 = halfHeight;
      halfWidth1 = halfWidth;

      x0 = offsetPosX - aLength_ft * 0.5;
      halfHeight0 = halfHeight - aThickness_ft;
      halfWidth0 = halfWidth - aThickness_ft;
   }

   pointCollection.AddRing();
   for (int hgt = 0; hgt < 10; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight0;
      double z = EllipticalX_GivenY(y, halfWidth0, halfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x0;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 10; hgt >= 0; --hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight0;
      double z = -EllipticalX_GivenY(y, halfWidth0, halfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x0;
      pointCollection.AddPoint(x, y, z);
   }

   pointCollection.AddRing();
   for (int hgt = 0; hgt < 10; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight1;
      double z = EllipticalX_GivenY(y, halfWidth1, halfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x1;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 10; hgt >= 0; --hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight1;
      double z = -EllipticalX_GivenY(y, halfWidth1, halfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x1;
      pointCollection.AddPoint(x, y, z);
   }

   pointCollection.AddRing();
   for (int hgt = 0; hgt < 10; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight2;
      double z = EllipticalX_GivenY(y, halfWidth2, halfHeight2);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x2;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 10; hgt >= 0; --hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight2;
      double z = -EllipticalX_GivenY(y, halfWidth2, halfHeight2);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x2;
      pointCollection.AddPoint(x, y, z);
   }

   pointCollection.AddRing();
   for (int hgt = 0; hgt < 10; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight3;
      double z = EllipticalX_GivenY(y, halfWidth3, halfHeight3);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x3;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 10; hgt >= 0; --hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight3;
      double z = -EllipticalX_GivenY(y, halfWidth3, halfHeight3);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x3;
      pointCollection.AddPoint(x, y, z);
   }

   // Calculate normals and draw
   pointCollection.CalculatePointCollection();
   pointCollection.Draw(aWireframe);

   // Exit .........................................................

   if (!aAftSectionIsTapered)
   {
      GraphicalPointCollection exitPointCollection(true);

      // Draw rings in increasing x

      inletX0 = offsetPosX - aLength_ft * 0.5 - 0.001 * aLength_ft;
      inletHalfHeight0 = halfHeight * 0.001;
      inletHalfWidth0 = halfWidth * 0.001;

      inletX1 = offsetPosX - aLength_ft * 0.5;
      inletHalfHeight1 = halfHeight - aThickness_ft;
      inletHalfWidth1 = halfWidth - aThickness_ft;

      DarkenMaterialProperties(0.4f);

      // Add center ring
      exitPointCollection.AddRing();
      for (int hgt = 0; hgt < 10; ++hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight0;
         double z = EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX0;
         exitPointCollection.AddPoint(x, y, z);
      }
      for (int hgt = 10; hgt >= 0; --hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight0;
         double z = -EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX0;
         exitPointCollection.AddPoint(x, y, z);
      }

      // Add inlet outer radius ring
      exitPointCollection.AddRing();
      for (int hgt = 0; hgt < 10; ++hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight1;
         double z = EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX1;
         exitPointCollection.AddPoint(x, y, z);
      }
      for (int hgt = 10; hgt >= 0; --hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight1;
         double z = -EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX1;
         exitPointCollection.AddPoint(x, y, z);
      }

      // Calculate normals and draw
      exitPointCollection.CalculatePointCollection();
      exitPointCollection.Draw(aWireframe);
   }

}

void GeometryGLWidget::DrawRoundedBottomNacelle(double             aLength_ft,
                                                double             aHeight_ft,
                                                double             aWidth_ft,
                                                double             aThickness_ft,
                                                const std::string& aOverallShapeString,
                                                bool               aAftSectionIsTapered,
                                                double             aAftSectionLength_ft,
                                                bool               aWireframe)
{
   double halfHeight = 0.5 * aHeight_ft;
   double halfWidth = 0.5 * aWidth_ft;
   double offsetPosX = 0.0;
   double offsetPosY = 0.0;
   double offsetPosZ = 0.0;

   // Ensure zero aft length, if appropriate
   if (!aAftSectionIsTapered)
   {
      aAftSectionLength_ft = 0.0;
   }

   // Inlet ........................................................

   GraphicalPointCollection inletPointCollection(true);

   // Draw rings in increasing x

   double inletX0 = offsetPosX + aLength_ft * 0.5;
   double inletHalfHeight0 = halfHeight - aThickness_ft;
   double inletHalfWidth0 = halfWidth - aThickness_ft;

   double inletX1 = offsetPosX + aLength_ft * 0.5 + 0.001 * aLength_ft;
   double inletHalfHeight1 = halfHeight * 0.001;
   double inletHalfWidth1 = halfWidth * 0.001;

   DarkenMaterialProperties(0.4f);

   // Add center ring
   inletPointCollection.AddRing();
   for (int hgt = 0; hgt > -10; --hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight0;
      double z = -EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX0;
      inletPointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -10; hgt <= 0; ++hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight0;
      double z = EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX0;
      inletPointCollection.AddPoint(x, y, z);
   }

   // Add inlet outer radius ring
   inletPointCollection.AddRing();
   for (int hgt = 0; hgt > -10; --hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight1;
      double z = -EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX1;
      inletPointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -10; hgt <= 0; ++hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight1;
      double z = EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX1;
      inletPointCollection.AddPoint(x, y, z);
   }

   // Calculate normals and draw
   inletPointCollection.CalculatePointCollection();
   inletPointCollection.Draw(aWireframe);

   // Main portion of nacelle ......................................

   double exitLipLength = aLength_ft * 0.15;

   double inletLipLength = aLength_ft * 0.075;
   if (aAftSectionIsTapered)
   {
      double maxLipLength = aLength_ft - aAftSectionLength_ft;
      if (inletLipLength > maxLipLength)
      {
         inletLipLength = maxLipLength - aLength_ft * 0.01;
      }

      // Modify values if necessary
      if (inletLipLength < aLength_ft * 0.01)
      {
         inletLipLength = aLength_ft * 0.01;
         aAftSectionLength_ft = aLength_ft * 0.98;
      }
   }

   RestoreMaterialProperties();

   GraphicalPointCollection pointCollection(true);

   double x3 = offsetPosX + aLength_ft * 0.5;
   double halfHeight3 = halfHeight - aThickness_ft;
   double halfWidth3 = halfWidth - aThickness_ft;

   double x2 = offsetPosX + aLength_ft * 0.5 - inletLipLength;
   double halfHeight2 = halfHeight;
   double halfWidth2 = halfWidth;

   double x1 = 0.0;
   double halfHeight1 = 0.0;
   double halfWidth1 = 0.0;
   double x0 = 0.0;
   double halfHeight0 = 0.0;
   double halfWidth0 = 0.0;

   if (aAftSectionIsTapered)
   {
      x1 = offsetPosX - aLength_ft * 0.5 + aAftSectionLength_ft;
      halfHeight1 = halfHeight;
      halfWidth1 = halfWidth;

      x0 = offsetPosX - aLength_ft * 0.5;
      halfHeight0 = halfHeight * 0.001;
      halfWidth0 = halfWidth * 0.001;
   }
   else
   {
      x1 = offsetPosX - aLength_ft * 0.5 + exitLipLength;
      halfHeight1 = halfHeight;
      halfWidth1 = halfWidth;

      x0 = offsetPosX - aLength_ft * 0.5;
      halfHeight0 = halfHeight - aThickness_ft;
      halfWidth0 = halfWidth - aThickness_ft;
   }

   pointCollection.AddRing();
   for (int hgt = 0; hgt > -10; --hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight0;
      double z = -EllipticalX_GivenY(y, halfWidth0, halfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x0;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -10; hgt <= 0; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight0;
      double z = EllipticalX_GivenY(y, halfWidth0, halfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x0;
      pointCollection.AddPoint(x, y, z);
   }

   pointCollection.AddRing();
   for (int hgt = 0; hgt > -10; --hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight1;
      double z = -EllipticalX_GivenY(y, halfWidth1, halfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x1;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -10; hgt <= 0; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight1;
      double z = EllipticalX_GivenY(y, halfWidth1, halfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x1;
      pointCollection.AddPoint(x, y, z);
   }

   pointCollection.AddRing();
   for (int hgt = 0; hgt > -10; --hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight2;
      double z = -EllipticalX_GivenY(y, halfWidth2, halfHeight2);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x2;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -10; hgt <= 0; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight2;
      double z = EllipticalX_GivenY(y, halfWidth2, halfHeight2);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x2;
      pointCollection.AddPoint(x, y, z);
   }

   pointCollection.AddRing();
   for (int hgt = 0; hgt > -10; --hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight3;
      double z = -EllipticalX_GivenY(y, halfWidth3, halfHeight3);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x3;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -10; hgt <= 0; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight3;
      double z = EllipticalX_GivenY(y, halfWidth3, halfHeight3);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x3;
      pointCollection.AddPoint(x, y, z);
   }

   // Calculate normals and draw
   pointCollection.CalculatePointCollection();
   pointCollection.Draw(aWireframe);

   // Exit .........................................................

   if (!aAftSectionIsTapered)
   {
      GraphicalPointCollection exitPointCollection(true);

      // Draw rings in increasing x

      inletX0 = offsetPosX - aLength_ft * 0.5 - 0.001 * aLength_ft;
      inletHalfHeight0 = halfHeight * 0.001;
      inletHalfWidth0 = halfWidth * 0.001;

      inletX1 = offsetPosX - aLength_ft * 0.5;
      inletHalfHeight1 = halfHeight - aThickness_ft;
      inletHalfWidth1 = halfWidth - aThickness_ft;

      DarkenMaterialProperties(0.4f);

      // Add center ring
      exitPointCollection.AddRing();
      for (int hgt = 0; hgt > -10; --hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight0;
         double z = -EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX0;
         exitPointCollection.AddPoint(x, y, z);
      }
      for (int hgt = -10; hgt <= 0; ++hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight0;
         double z = EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX0;
         exitPointCollection.AddPoint(x, y, z);
      }

      // Add inlet outer radius ring
      exitPointCollection.AddRing();
      for (int hgt = 0; hgt > -10; --hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight1;
         double z = -EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX1;
         exitPointCollection.AddPoint(x, y, z);
      }
      for (int hgt = -10; hgt <= 0; ++hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight1;
         double z = EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX1;
         exitPointCollection.AddPoint(x, y, z);
      }

      // Calculate normals and draw
      exitPointCollection.CalculatePointCollection();
      exitPointCollection.Draw(aWireframe);
   }
}

void GeometryGLWidget::DrawFlatSidedNacelle(double             aLength_ft,
                                            double             aHeight_ft,
                                            double             aWidth_ft,
                                            double             aThickness_ft,
                                            double             aForwardSweepLength_ft,
                                            const std::string& aOverallShapeString,
                                            bool               aAftSectionIsTapered,
                                            double             aAftSectionLength_ft,
                                            bool               aWireframe)
{
   // F = front, A = aft, T = top, B = bottom, R = right, L = right

   UtVec3dX ptFTR;
   UtVec3dX ptFTL;
   UtVec3dX ptFBR;
   UtVec3dX ptFBL;

   UtVec3dX ptATR;
   UtVec3dX ptATL;
   UtVec3dX ptABR;
   UtVec3dX ptABL;

   DrawBoxWithHole(aLength_ft, aHeight_ft, aWidth_ft, aThickness_ft, aWireframe);
}

void GeometryGLWidget::DrawFlatSweptRightNacelle(double             aLength_ft,
                                                 double             aHeightInner_ft,
                                                 double             aHeightOuter_ft,
                                                 double             aWidth_ft,
                                                 double             aThickness_ft,
                                                 double             aForwardSweepLength_ft,
                                                 const std::string& aOverallShapeString,
                                                 bool               aAftSectionIsTapered,
                                                 double             aAftSectionLength_ft,
                                                 bool               aWireframe)
{
   // Check value for aAftSectionLength_ft
   if (!aAftSectionIsTapered) { aAftSectionLength_ft = 0.0; }

   // F = front, A = aft, T = top, B = bottom, R = right, L = right

   // Incremental length for offset for inlets
   double del = aLength_ft * 0.0001;
   double dx = aForwardSweepLength_ft;
   double dy = aHeightInner_ft * 0.5 - aHeightOuter_ft * 0.5;
   double dz = aWidth_ft;

   double dxOverDz = dx / dz;
   double xOffset = aThickness_ft * dxOverDz;
   double dyOverDz = dy / dz;
   double yOffset = aThickness_ft * dyOverDz;

   UtVec3dX inletFTR(xOffset + del + aLength_ft * 0.5 - aForwardSweepLength_ft,  aHeightOuter_ft * 0.5 - aThickness_ft + yOffset, aWidth_ft - aThickness_ft);
   UtVec3dX inletFTL(-xOffset + del + aLength_ft * 0.5,                           aHeightInner_ft * 0.5 - aThickness_ft - yOffset, aThickness_ft);
   UtVec3dX inletFBR(xOffset + del + aLength_ft * 0.5 - aForwardSweepLength_ft, -aHeightOuter_ft * 0.5 + aThickness_ft - yOffset, aWidth_ft - aThickness_ft);
   UtVec3dX inletFBL(-xOffset + del + aLength_ft * 0.5,                          -aHeightInner_ft * 0.5 + aThickness_ft + yOffset, aThickness_ft);

   UtVec3dX ptFTR(aLength_ft * 0.5 - aForwardSweepLength_ft,  aHeightOuter_ft * 0.5, aWidth_ft);
   UtVec3dX ptFTL(aLength_ft * 0.5,                           aHeightInner_ft * 0.5, 0.0);
   UtVec3dX ptFBR(aLength_ft * 0.5 - aForwardSweepLength_ft, -aHeightOuter_ft * 0.5, aWidth_ft);
   UtVec3dX ptFBL(aLength_ft * 0.5,                          -aHeightInner_ft * 0.5, 0.0);

   UtVec3dX ptATR(-aLength_ft * 0.5 + aAftSectionLength_ft, aHeightOuter_ft * 0.5, aWidth_ft);
   UtVec3dX ptATL(-aLength_ft * 0.5, aHeightInner_ft * 0.5, 0.0);
   UtVec3dX ptABR(-aLength_ft * 0.5 + aAftSectionLength_ft, -aHeightOuter_ft * 0.5, aWidth_ft);
   UtVec3dX ptABL(-aLength_ft * 0.5, -aHeightInner_ft * 0.5, 0.0);

   UtVec3dX vec1;
   UtVec3dX vec2;
   UtVec3dX norm;

   // Inlet
   DarkenMaterialProperties(0.4f);

   glBegin(GL_POLYGON);
   vec1 = inletFTL - inletFBL;
   vec2 = inletFBR - inletFBL;
   norm = vec1.Cross(vec2);
   norm.Normalize();
   glNormal3d(norm.X(), norm.Y(), norm.Z());
   glVertex3d(inletFBR.X(), inletFBR.Y(), inletFBR.Z());
   glVertex3d(inletFBL.X(), inletFBL.Y(), inletFBL.Z());
   glVertex3d(inletFTL.X(), inletFTL.Y(), inletFTL.Z());
   glVertex3d(inletFTR.X(), inletFTR.Y(), inletFTR.Z());
   glEnd();

   RestoreMaterialProperties();

   // Front
   glBegin(GL_POLYGON);
   vec1 = ptFTL - ptFBL;
   vec2 = ptFBR - ptFBL;
   norm = vec1.Cross(vec2);
   norm.Normalize();
   glNormal3d(norm.X(), norm.Y(), norm.Z());
   glVertex3d(ptFBR.X(), ptFBR.Y(), ptFBR.Z());
   glVertex3d(ptFBL.X(), ptFBL.Y(), ptFBL.Z());
   glVertex3d(ptFTL.X(), ptFTL.Y(), ptFTL.Z());
   glVertex3d(ptFTR.X(), ptFTR.Y(), ptFTR.Z());
   glEnd();

   // Aft
   glBegin(GL_POLYGON);
   vec1 = ptABR - ptABL;
   vec2 = ptATL - ptABL;
   norm = vec1.Cross(vec2);
   norm.Normalize();
   glNormal3d(norm.X(), norm.Y(), norm.Z());
   glVertex3d(ptATR.X(), ptATR.Y(), ptATR.Z());
   glVertex3d(ptATL.X(), ptATL.Y(), ptATL.Z());
   glVertex3d(ptABL.X(), ptABL.Y(), ptABL.Z());
   glVertex3d(ptABR.X(), ptABR.Y(), ptABR.Z());
   glEnd();

   // Top
   glBegin(GL_POLYGON);
   vec1 = ptATL - ptFTL;
   vec2 = ptATR - ptFTL;
   norm = vec1.Cross(vec2);
   norm.Normalize();
   glNormal3d(norm.X(), norm.Y(), norm.Z());
   glVertex3d(ptFTR.X(), ptFTR.Y(), ptFTR.Z());
   glVertex3d(ptFTL.X(), ptFTL.Y(), ptFTL.Z());
   glVertex3d(ptATL.X(), ptATL.Y(), ptATL.Z());
   glVertex3d(ptATR.X(), ptATR.Y(), ptATR.Z());
   glEnd();

   // Bottom
   glBegin(GL_POLYGON);
   vec1 = ptFBR - ptFBL;
   vec2 = ptABL - ptFBL;
   norm = vec1.Cross(vec2);
   norm.Normalize();
   glNormal3d(norm.X(), norm.Y(), norm.Z());
   glNormal3d(0.0, -1.0, 0.0);
   glVertex3d(ptABR.X(), ptABR.Y(), ptABR.Z());
   glVertex3d(ptABL.X(), ptABL.Y(), ptABL.Z());
   glVertex3d(ptFBL.X(), ptFBL.Y(), ptFBL.Z());
   glVertex3d(ptFBR.X(), ptFBR.Y(), ptFBR.Z());
   glEnd();

   // Right
   glBegin(GL_POLYGON);
   glNormal3d(0.0, 0.0, 1.0);
   glVertex3d(ptABR.X(), ptABR.Y(), ptABR.Z());
   glVertex3d(ptFBR.X(), ptFBR.Y(), ptFBR.Z());
   glVertex3d(ptFTR.X(), ptFTR.Y(), ptFTR.Z());
   glVertex3d(ptATR.X(), ptATR.Y(), ptATR.Z());
   glEnd();

   // Left
   glBegin(GL_POLYGON);
   glNormal3d(-1.0, 0.0, -1.0);
   glVertex3d(ptFTL.X(), ptFTL.Y(), ptFTL.Z());
   glVertex3d(ptFBL.X(), ptFBL.Y(), ptFBL.Z());
   glVertex3d(ptABL.X(), ptABL.Y(), ptABL.Z());
   glVertex3d(ptATL.X(), ptATL.Y(), ptATL.Z());
   glEnd();
}

void GeometryGLWidget::DrawFlatSweptLeftNacelle(double             aLength_ft,
                                                double             aHeightInner_ft,
                                                double             aHeightOuter_ft,
                                                double             aWidth_ft,
                                                double             aThickness_ft,
                                                double             aForwardSweepLength_ft,
                                                const std::string& aOverallShapeString,
                                                bool               aAftSectionIsTapered,
                                                double             aAftSectionLength_ft,
                                                bool               aWireframe)
{
   // Check value for aAftSectionLength_ft
   if (!aAftSectionIsTapered) { aAftSectionLength_ft = 0.0; }

   // F = front, A = aft, T = top, B = bottom, R = right, L = right

   // Incremental length for offset for inlets
   double del = aLength_ft * 0.0001;
   double dx = aForwardSweepLength_ft;
   double dy = aHeightInner_ft * 0.5 - aHeightOuter_ft * 0.5;
   double dz = aWidth_ft;

   double dxOverDz = dx / dz;
   double xOffset = aThickness_ft * dxOverDz;
   double dyOverDz = dy / dz;
   double yOffset = aThickness_ft * dyOverDz;

   UtVec3dX inletFTR(xOffset + del + aLength_ft * 0.5,                            aHeightOuter_ft * 0.5 - aThickness_ft + yOffset, aWidth_ft - aThickness_ft);
   UtVec3dX inletFTL(-xOffset + del + aLength_ft * 0.5 - aForwardSweepLength_ft,  aHeightInner_ft * 0.5 - aThickness_ft - yOffset, aThickness_ft);
   UtVec3dX inletFBR(xOffset + del + aLength_ft * 0.5,                           -aHeightOuter_ft * 0.5 + aThickness_ft - yOffset, aWidth_ft - aThickness_ft);
   UtVec3dX inletFBL(-xOffset + del + aLength_ft * 0.5 - aForwardSweepLength_ft, -aHeightInner_ft * 0.5 + aThickness_ft + yOffset, aThickness_ft);

   UtVec3dX ptFTR(aLength_ft * 0.5,                           aHeightOuter_ft * 0.5, aWidth_ft);
   UtVec3dX ptFTL(aLength_ft * 0.5 - aForwardSweepLength_ft,  aHeightInner_ft * 0.5, 0.0);
   UtVec3dX ptFBR(aLength_ft * 0.5,                          -aHeightOuter_ft * 0.5, aWidth_ft);
   UtVec3dX ptFBL(aLength_ft * 0.5 - aForwardSweepLength_ft, -aHeightInner_ft * 0.5, 0.0);

   UtVec3dX ptATR(-aLength_ft * 0.5 + aAftSectionLength_ft,  aHeightOuter_ft * 0.5, aWidth_ft);
   UtVec3dX ptATL(-aLength_ft * 0.5,                         aHeightInner_ft * 0.5, 0.0);
   UtVec3dX ptABR(-aLength_ft * 0.5 + aAftSectionLength_ft, -aHeightOuter_ft * 0.5, aWidth_ft);
   UtVec3dX ptABL(-aLength_ft * 0.5,                        -aHeightInner_ft * 0.5, 0.0);

   UtVec3dX vec1;
   UtVec3dX vec2;
   UtVec3dX norm;

   // Inlet
   DarkenMaterialProperties(0.4f);

   glBegin(GL_POLYGON);
   vec1 = inletFTL - inletFBL;
   vec2 = inletFBR - inletFBL;
   norm = vec1.Cross(vec2);
   norm.Normalize();
   glNormal3d(norm.X(), norm.Y(), norm.Z());
   glVertex3d(inletFBR.X(), inletFBR.Y(), inletFBR.Z());
   glVertex3d(inletFBL.X(), inletFBL.Y(), inletFBL.Z());
   glVertex3d(inletFTL.X(), inletFTL.Y(), inletFTL.Z());
   glVertex3d(inletFTR.X(), inletFTR.Y(), inletFTR.Z());
   glEnd();

   RestoreMaterialProperties();

   // Front
   glBegin(GL_POLYGON);
   vec1 = ptFTL - ptFBL;
   vec2 = ptFBR - ptFBL;
   norm = vec1.Cross(vec2);
   norm.Normalize();
   glNormal3d(norm.X(), norm.Y(), norm.Z());
   glVertex3d(ptFBR.X(), ptFBR.Y(), ptFBR.Z());
   glVertex3d(ptFBL.X(), ptFBL.Y(), ptFBL.Z());
   glVertex3d(ptFTL.X(), ptFTL.Y(), ptFTL.Z());
   glVertex3d(ptFTR.X(), ptFTR.Y(), ptFTR.Z());
   glEnd();

   // Aft
   glBegin(GL_POLYGON);
   vec1 = ptABR - ptABL;
   vec2 = ptATL - ptABL;
   norm = vec1.Cross(vec2);
   norm.Normalize();
   glNormal3d(norm.X(), norm.Y(), norm.Z());
   glVertex3d(ptATR.X(), ptATR.Y(), ptATR.Z());
   glVertex3d(ptATL.X(), ptATL.Y(), ptATL.Z());
   glVertex3d(ptABL.X(), ptABL.Y(), ptABL.Z());
   glVertex3d(ptABR.X(), ptABR.Y(), ptABR.Z());
   glEnd();

   // Top
   glBegin(GL_POLYGON);
   vec1 = ptATL - ptFTL;
   vec2 = ptATR - ptFTL;
   norm = vec1.Cross(vec2);
   norm.Normalize();
   glNormal3d(norm.X(), norm.Y(), norm.Z());
   glVertex3d(ptFTR.X(), ptFTR.Y(), ptFTR.Z());
   glVertex3d(ptFTL.X(), ptFTL.Y(), ptFTL.Z());
   glVertex3d(ptATL.X(), ptATL.Y(), ptATL.Z());
   glVertex3d(ptATR.X(), ptATR.Y(), ptATR.Z());
   glEnd();

   // Bottom
   glBegin(GL_POLYGON);
   vec1 = ptFBR - ptFBL;
   vec2 = ptABL - ptFBL;
   norm = vec1.Cross(vec2);
   norm.Normalize();
   glNormal3d(norm.X(), norm.Y(), norm.Z());
   glNormal3d(0.0, -1.0, 0.0);
   glVertex3d(ptABR.X(), ptABR.Y(), ptABR.Z());
   glVertex3d(ptABL.X(), ptABL.Y(), ptABL.Z());
   glVertex3d(ptFBL.X(), ptFBL.Y(), ptFBL.Z());
   glVertex3d(ptFBR.X(), ptFBR.Y(), ptFBR.Z());
   glEnd();

   // Right
   glBegin(GL_POLYGON);
   glNormal3d(0.0, 0.0, 1.0);
   glVertex3d(ptABR.X(), ptABR.Y(), ptABR.Z());
   glVertex3d(ptFBR.X(), ptFBR.Y(), ptFBR.Z());
   glVertex3d(ptFTR.X(), ptFTR.Y(), ptFTR.Z());
   glVertex3d(ptATR.X(), ptATR.Y(), ptATR.Z());
   glEnd();

   // Left
   glBegin(GL_POLYGON);
   glNormal3d(-1.0, 0.0, -1.0);
   glVertex3d(ptFTL.X(), ptFTL.Y(), ptFTL.Z());
   glVertex3d(ptFBL.X(), ptFBL.Y(), ptFBL.Z());
   glVertex3d(ptABL.X(), ptABL.Y(), ptABL.Z());
   glVertex3d(ptATL.X(), ptATL.Y(), ptATL.Z());
   glEnd();
}

void GeometryGLWidget::DrawNacelle(double             aPosX,
                                   double             aPosY,
                                   double             aPosZ,
                                   double             aLength_ft,
                                   double             aHeight_ft,
                                   double             aHeightInner_ft,
                                   double             aHeightOuter_ft,
                                   double             aWidth_ft,
                                   double             aThickness_ft,
                                   double             aForwardSweepLength_ft,
                                   const std::string& aOverallShapeString,
                                   const std::string& aAftSectionString,
                                   double             aAftSectionLength_ft,
                                   double             aYaw_deg,
                                   double             aPitch_deg,
                                   double             aRoll_deg,
                                   bool               aWireframe)
{
   if (!aWireframe)
   {
      glEnable(GL_LIGHTING);
      glEnable(GL_LIGHT0);
      glEnable(GL_NORMALIZE);
   }

   glPushMatrix();
   glTranslated(aPosX, -aPosZ, aPosY);
   glRotated(aYaw_deg, 0.0, -1.0, 0.0);
   glRotated(aPitch_deg, 0.0, 0.0, 1.0);
   glRotated(aRoll_deg, 1.0, 0.0, 0.0);

   // Tapered flag
   bool aftNacelleIsTapered = false;
   if (aAftSectionString == "Blunt") { aftNacelleIsTapered = false; }
   else if (aAftSectionString == "Tapered") { aftNacelleIsTapered = true; }

   if (aftNacelleIsTapered)
   {
      // Verify length values - return if aft length is too long
      if (aAftSectionLength_ft > aLength_ft) { return; }
   }

   if (aOverallShapeString == "Rounded")
   {
      DrawRoundedNacelle(aLength_ft, aHeight_ft, aWidth_ft, aThickness_ft, aOverallShapeString, aftNacelleIsTapered, aAftSectionLength_ft, aWireframe);
   }
   else if (aOverallShapeString == "Half-Round-Right")
   {
      DrawRoundedRightNacelle(aLength_ft, aHeight_ft, aWidth_ft, aThickness_ft, aOverallShapeString, aftNacelleIsTapered, aAftSectionLength_ft, aWireframe);
   }
   else if (aOverallShapeString == "Half-Round-Left")
   {
      DrawRoundedLeftNacelle(aLength_ft, aHeight_ft, aWidth_ft, aThickness_ft, aOverallShapeString, aftNacelleIsTapered, aAftSectionLength_ft, aWireframe);
   }
   else if (aOverallShapeString == "Half-Round-Top")
   {
      DrawRoundedTopNacelle(aLength_ft, aHeight_ft, aWidth_ft, aThickness_ft, aOverallShapeString, aftNacelleIsTapered, aAftSectionLength_ft, aWireframe);
   }
   else if (aOverallShapeString == "Half-Round-Bottom")
   {
      DrawRoundedBottomNacelle(aLength_ft, aHeight_ft, aWidth_ft, aThickness_ft, aOverallShapeString, aftNacelleIsTapered, aAftSectionLength_ft, aWireframe);
   }
   else if (aOverallShapeString == "Flat-Sided")
   {
      DrawFlatSidedNacelle(aLength_ft, aHeight_ft, aWidth_ft, aThickness_ft, aForwardSweepLength_ft, aOverallShapeString, aftNacelleIsTapered, aAftSectionLength_ft, aWireframe);
   }
   else if (aOverallShapeString == "Flat-Swept-Right")
   {
      DrawFlatSweptRightNacelle(aLength_ft, aHeightInner_ft, aHeightOuter_ft, aWidth_ft, aThickness_ft, aForwardSweepLength_ft, aOverallShapeString, aftNacelleIsTapered, aAftSectionLength_ft, aWireframe);
   }
   else if (aOverallShapeString == "Flat-Swept-Left")
   {
      DrawFlatSweptLeftNacelle(aLength_ft, aHeightInner_ft, aHeightOuter_ft, aWidth_ft, aThickness_ft, aForwardSweepLength_ft, aOverallShapeString, aftNacelleIsTapered, aAftSectionLength_ft, aWireframe);
   }

   glPopMatrix();

   // Turn off lighting
   glDisable(GL_LIGHTING);
   glDisable(GL_LIGHT0);
   glDisable(GL_NORMALIZE);
}

void GeometryGLWidget::DrawSpeedBrake(double aPosX,
                                      double aPosY,
                                      double aPosZ,
                                      double aLength_ft,
                                      double aWidth_ft,
                                      double aRoll_deg,
                                      double aMaxAngle_deg,
                                      bool   aWireframe)
{
   glPushMatrix();
   glTranslated(aPosX, -aPosZ, aPosY);
   glRotated(aRoll_deg, 1.0, 0.0, 0.0);
   glRotated(aMaxAngle_deg, 0.0, 0.0, -1.0);
   glTranslated(-aLength_ft * 0.5, 0.0, 0.0);
   double thickness_ft = aLength_ft * 0.04;
   if (aWidth_ft < aLength_ft)
   {
      thickness_ft = aWidth_ft * 0.04;
   }
   DrawBox(aLength_ft, thickness_ft, aWidth_ft, aWireframe);
   glPopMatrix();
}

void GeometryGLWidget::DrawLandingGear(double aPosX,
                                       double aPosY,
                                       double aPosZ,
                                       double aLength_ft,
                                       double aStrutDiam_ft,
                                       double aTireDiam_ft,
                                       double aTireWidth_ft,
                                       double aExtendedAngle_deg,
                                       bool   aWireframe)
{
   UtVec3dX zeroPtVec(0.0, 0.0, 0.0);

   // Draw strut
   glPushMatrix();
   glTranslated(aPosX, -aPosZ, aPosY);
   glRotated(180.0 - aExtendedAngle_deg, 0.0, .0, -1.0);
   DrawCylinder(zeroPtVec, aLength_ft - aTireDiam_ft, aStrutDiam_ft, true, false, aWireframe);
   glPopMatrix();

   // Draw tire
   glPushMatrix();
   double x = aPosX - (aLength_ft - aTireDiam_ft * 0.5) * cos((aExtendedAngle_deg) * UtMath::cRAD_PER_DEG);
   double y = -aPosZ - (aLength_ft - aTireDiam_ft * 0.5) * sin((aExtendedAngle_deg) * UtMath::cRAD_PER_DEG);
   double z = aPosY + aTireWidth_ft * 0.5;
   glTranslated(x, y, z);
   glRotated(90.0, 0.0, 1.0, 0.0);
   DrawCylinder(zeroPtVec, aTireWidth_ft, aTireDiam_ft, true, false, aWireframe);
   glPopMatrix();
}

void GeometryGLWidget::ToggleAxis()
{
   mShowAxis = !mShowAxis;
   Update();
}

void GeometryGLWidget::ToggleSelectedObjectCG()
{
   mShowSelectedObjectCG = !mShowSelectedObjectCG;
   Update();
}

void GeometryGLWidget::ToggleShowEngines()
{
   mShowEngines = !mShowEngines;
   Update();
}

void GeometryGLWidget::ToggleThrustVectors()
{
   mShowThrustVectors = !mShowThrustVectors;
   Update();
}

void GeometryGLWidget::TogglePointMasses()
{
   mShowPointMasses = !mShowPointMasses;
   Update();
}

void GeometryGLWidget::ToggleFuelTanks()
{
   mShowFuelTanks = !mShowFuelTanks;
   Update();
}

void GeometryGLWidget::ToggleVehicleCG()
{
   mShowVehicleCG = !mShowVehicleCG;
   Update();
}

void GeometryGLWidget::ToggleLandingGear()
{
   mShowLandingGear = !mShowLandingGear;
   Update();
}

void GeometryGLWidget::ToggleSpeedBrakes()
{
   mShowSpeedBrakes = !mShowSpeedBrakes;
   Update();
}

void GeometryGLWidget::ShowAxis(bool aShow)
{
   mShowAxis = aShow;
   Update();
}

void GeometryGLWidget::ShowSelectedObjectCG(bool aShow)
{
   mShowSelectedObjectCG = aShow;
   Update();
}

void GeometryGLWidget::ShowEngines(bool aShow)
{
   mShowEngines = aShow;
   Update();
}

void GeometryGLWidget::ShowThrustVectors(bool aShow)
{
   mShowThrustVectors = aShow;
   Update();
}

void GeometryGLWidget::ShowPointMasses(bool aShow)
{
   mShowPointMasses = aShow;
   Update();
}

void GeometryGLWidget::ShowFuelTanks(bool aShow)
{
   mShowFuelTanks = aShow;
   Update();
}

void GeometryGLWidget::ShowVehicleCG(bool aShow)
{
   mShowVehicleCG = aShow;
   Update();
}

void GeometryGLWidget::ShowLandingGear(bool aShow)
{
   mShowLandingGear = aShow;
   Update();
}

void GeometryGLWidget::ShowSpeedBrakes(bool aShow)
{
   mShowSpeedBrakes = aShow;
   Update();
}

void GeometryGLWidget::ShowWireframe(bool aShow)
{
   mShowWireframe = aShow;
   Update();
}

void GeometryGLWidget::HideAeroVectors()
{
   mShowAllAeroVectors = false;
   mShowSelectedAeroVectors = false;
   Update();
}

void GeometryGLWidget::ShowAllAeroVectors()
{
   mShowSelectedAeroVectors = false;
   mShowAllAeroVectors = true;
   Update();
}

void GeometryGLWidget::ShowSelectedAeroVectors()
{
   mShowAllAeroVectors = false;
   mShowSelectedAeroVectors = true;
   Update();
}

void GeometryGLWidget::ToggleShowWireframe()
{
   mShowWireframe = !mShowWireframe;
   Update();
}

void GeometryGLWidget::SetViewingFocusPointX(double aValue)
{
   mViewingFocusPoint.Set(aValue, mViewingFocusPoint.Y(), mViewingFocusPoint.Z());
}

void GeometryGLWidget::SetViewingFocusPointY(double aValue)
{
   mViewingFocusPoint.Set(mViewingFocusPoint.X(), aValue, mViewingFocusPoint.Z());
}

void GeometryGLWidget::SetViewingFocusPointZ(double aValue)
{
   mViewingFocusPoint.Set(mViewingFocusPoint.X(), mViewingFocusPoint.Y(), aValue);
}

void GeometryGLWidget::SetViewingFocusPoint(UtVec3dX aFocusPoint)
{
   mViewingFocusPoint = aFocusPoint;
}

double GeometryGLWidget::GetViewingFocusPointX()
{
   return mViewingFocusPoint.X();
}

double GeometryGLWidget::GetViewingFocusPointY()
{
   return mViewingFocusPoint.Y();
}

double GeometryGLWidget::GetViewingFocusPointZ()
{
   return mViewingFocusPoint.Z();
}

UtVec3dX GeometryGLWidget::GetViewingFocusPoint()
{
   return mViewingFocusPoint;
}

void GeometryGLWidget::SaveCurrentFocusPoint()
{
   mFocusPoints.emplace_back(GetViewingFocusPointX(), GetViewingFocusPointY(), GetViewingFocusPointZ());
}

void GeometryGLWidget::MoveToFocusPoint(unsigned int aIdx)
{
   if (aIdx >= mFocusPoints.size())
   {
      return;
   }
   else
   {
      GeometryGLFocusPoint curPt = mFocusPoints[aIdx];
      UtVec3dX fp(curPt.GetX(), curPt.GetY(), curPt.GetZ());
      SetViewingFocusPoint(fp);
   }
}

void GeometryGLWidget::SaveToggleSettings()
{
   MoverCreatorMainWindow& mainWindow = MoverCreatorMainWindow::GetInstance();
   QSettings* settings = mainWindow.GetSettings();
   settings->setValue("ShowAxis", mShowAxis);
   settings->setValue("ShowSelectedObjectCG", mShowSelectedObjectCG);
   settings->setValue("ShowEngines", mShowEngines);
   settings->setValue("ShowThrustVectors", mShowThrustVectors);
   settings->setValue("ShowPointMasses", mShowPointMasses);
   settings->setValue("ShowFuelTanks", mShowFuelTanks);
   settings->setValue("ShowVehicleCG", mShowVehicleCG);
   settings->setValue("ShowLandingGear", mShowLandingGear);
   settings->setValue("ShowSpeedBrakes", mShowSpeedBrakes);

   if (mShowAllAeroVectors)
   {
      settings->setValue("AeroVectors", "ShowAll");
   }
   else if (mShowSelectedAeroVectors)
   {
      settings->setValue("AeroVectors", "ShowSelected");
   }
   else
   {
      settings->setValue("AeroVectors", "Hide");
   }

   settings->setValue("ShowWireframe", mShowWireframe);

   Vehicle* vehicle = mainWindow.GetMoverCreatorWidget()->GetVehicle();
   settings->setValue("ShowDebug", vehicle->DebugMode());
}

GeometryGLFocusPoint::GeometryGLFocusPoint()
{
   // Do nothing for now
}

GeometryGLFocusPoint::GeometryGLFocusPoint(double aXPos, double aYPos, double aZPos)
   : mXPos(aXPos)
   , mYPos(aYPos)
   , mZPos(aZPos)
{
}

GeometryGLFocusPoint::~GeometryGLFocusPoint()
{
   // Do nothing for now
}

void GeometryGLFocusPoint::Draw()
{
   glPushMatrix();

   glDisable(GL_LIGHTING);
   glDisable(GL_CULL_FACE);

   glColor4d(0.6, 0.6, 0.6, 1.0);

   glTranslated(mXPos, mYPos, mZPos);

   constexpr int horizontalStripes = 10;
   constexpr int verticalStripes = 10;
   double radius = 1; //radius

   for (double j = 0; j < UtMath::cPI; j += UtMath::cPI / (horizontalStripes + 1))
   {
      glBegin(GL_LINE_LOOP);
      double y = (radius * cos(j));
      for (double i = 0; i < 2 * UtMath::cPI; i += UtMath::cPI / 60)
      {
         double x = (radius * cos(i) * sin(j));
         double z = (radius * sin(i) * sin(j));
         glVertex3f(x, y, z);
      }
      glEnd();
   }

   for (double j = 0; j < UtMath::cPI; j += UtMath::cPI / verticalStripes)
   {
      glBegin(GL_LINE_LOOP);
      for (double i = 0; i < 2 * UtMath::cPI; i += UtMath::cPI / 60)
      {
         double x = (radius * sin(i) * cos(j));
         double y = (radius * cos(i));
         double z = (radius * sin(j) * sin(i));
         glVertex3f(x, y, z);
      }
      glEnd();
   }
   glPopMatrix();

   glDisable(GL_LIGHTING);
   glDisable(GL_LIGHT0);
   glDisable(GL_NORMALIZE);
}

} // namespace Designer
