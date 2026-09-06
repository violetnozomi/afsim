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
#include "WkfAttachmentGreatCircleRuler.hpp"

#include <climits>
#include <sstream>

#include <QColor>
#include <QFont>

#include "UtAngleR.hpp"
#include "UtMath.hpp"
#include "UtSpeed.hpp"
#include "UtSphericalEarth.hpp"
#include "UtTime.hpp"
#include "UtoPolyLineShape.hpp"
#include "UtoRawShape.hpp"
#include "VaCamera.hpp"
#include "VaEntity.hpp"
#include "VaEnvironment.hpp"
#include "VaLocationReference.hpp"
#include "VaObserver.hpp"
#include "VaScenario.hpp"
#include "VaViewer.hpp"
#include "VaViewerManager.hpp"

wkf::AttachmentGreatCircleRuler::AttachmentGreatCircleRuler(vespa::VaEntity& aParent, vespa::VaViewer* aViewerPtr)
   : VaAttachment(aParent, aViewerPtr, "great_circle_ruler")
   , mCallbacks()
   , mMeasureMode(cGREAT_CIRCLE)
   , mMeasureLastMode(cGREAT_CIRCLE)
   , mLabelMode(cDEFAULT_LABEL)
   , mLabelLastMode(cDEFAULT_LABEL)
   , mLabelSize(10)
   , mMeasureActive(false)
   , mMeasureSrcDstSet(-1)
   , mMeasureLengthUnits(-1)
   , mLengthUnitOverride(false)
   , mMeasureSpeedUnits(-1)
   , mMeasureTimeUnits(-1)
   , mMeasureAngleUnits(-1)
   , mMeasureFlag(false)
   , mMeasureSrcCallbackId(-1)
   , mMeasureDstCallbackId(-1)
   , mMeasureSrc()
   , mMeasureDst()
   , mMeasureLastSrc()
   , mMeasureLastDst()
   , mMeasureLineShapeName()
   , mMeasureLinePtr(nullptr)
   , mMeasureVerticalLineShapeName()
   , mMeasureVerticalLinePtr(nullptr)
   , mSrcMeasureLabelShapeName()
   , mDstMeasureLabelShapeName()
   , mColor(87, 166, 74)
{
   SetType<AttachmentGreatCircleRuler>();
}

wkf::AttachmentGreatCircleRuler::~AttachmentGreatCircleRuler()
{
   for (auto&& it : mOwnershipList)
   {
      vespa::VaEntity* ent = it;
      if (ent->GetScenario())
      {
         ent->GetScenario()->RemoveEntity(ent);
      }
      delete ent;
   }
}

void wkf::AttachmentGreatCircleRuler::PrivateInitializeCallbacks()
{
   if (!IsInitialized())
   {
      mCallbacks.Add(vespa::VaObserver::EntityMoved.Connect(&AttachmentGreatCircleRuler::EntityMovedCB,
                                                            this,
                                                            GetParent().GetUniqueId()));
      mCallbacks.Add(
         vespa::VaObserver::UpdateCursorPosition.Connect(&AttachmentGreatCircleRuler::UpdateCursorPositionCB, this));
      mCallbacks.Add(vespa::VaObserver::ChangedUnitFormat.Connect(&AttachmentGreatCircleRuler::UnitsChangedCB, this));
   }
}

void wkf::AttachmentGreatCircleRuler::PrivateLoad()
{
   if (mMeasureLineShapeName.empty())
   {
      std::ostringstream  oss;
      static unsigned int uid = 0;
      oss << "WkfAttachmentGreatCircleRuler_Line_" << uid++;
      mMeasureLineShapeName = oss.str().c_str();
   }

   if (mMeasureVerticalLineShapeName.empty())
   {
      mMeasureVerticalLineShapeName = mMeasureLineShapeName + "_Vertical";
   }

   if (mSrcMeasureLabelShapeName.empty())
   {
      std::ostringstream  oss;
      static unsigned int uid = 0;
      oss << "WkfAttachmentGreatCircleRuler_Src_Label_" << uid++;
      mSrcMeasureLabelShapeName = oss.str().c_str();
   }

   if (mDstMeasureLabelShapeName.empty())
   {
      std::ostringstream  oss;
      static unsigned int uid = 0;
      oss << "WkfAttachmentGreatCircleRuler_Dst_Label_" << uid++;
      mDstMeasureLabelShapeName = oss.str().c_str();
   }

   // Remove previous models
   RemoveShapeFromParent(mMeasureLineShapeName);
   mMeasureLinePtr = nullptr;
   RemoveShapeFromParent(mSrcMeasureLabelShapeName);
   mSrcMeasureLabelPtr = nullptr;
   RemoveShapeFromParent(mDstMeasureLabelShapeName);
   mDstMeasureLabelPtr = nullptr;

   UtoPolyLineShape tempLine;
   tempLine.SetReferenceFrame(UtoShape::cFRAME_WORLD_AT_ENTITY);
   tempLine.EnableAntiAliasLines(true);
   tempLine.OverrideRenderBin(1000, false);
   tempLine.SetWidth(2.0);
   mMeasureLinePtr = dynamic_cast<UtoPolyLineShape*>(AddShapeToParent(mMeasureLineShapeName, tempLine));

   UtoPolyLineShape tempVerticalLine;
   tempVerticalLine.SetReferenceFrame(UtoShape::cFRAME_WORLD_AT_ENTITY);
   tempVerticalLine.SetWidth(2.0);
   tempVerticalLine.SetAggregation(UtoPolyLineShape::LINES);
   mMeasureVerticalLinePtr =
      dynamic_cast<UtoPolyLineShape*>(AddShapeToParent(mMeasureVerticalLineShapeName, tempVerticalLine));

   UtoRawShape tempMeasureLabel;
   tempMeasureLabel.SetReferenceFrame(UtoShape::cFRAME_WORLD_AT_ENTITY);
   UtoRawShape* srcShape = dynamic_cast<UtoRawShape*>(AddShapeToParent(mSrcMeasureLabelShapeName, tempMeasureLabel));
   mSrcMeasureLabelPtr   = new vespa::VaDecoratorNode(vespa::VaDecoratorNode::cHideBehindEarth);
   mSrcTransform         = new osg::MatrixTransform;
   srcShape->GetRoot()->addChild(mSrcTransform);
   mSrcTransform->addChild(mSrcMeasureLabelPtr);
   UtoRawShape* dstShape = dynamic_cast<UtoRawShape*>(AddShapeToParent(mDstMeasureLabelShapeName, tempMeasureLabel));
   mDstMeasureLabelPtr   = new vespa::VaDecoratorNode(vespa::VaDecoratorNode::cHideBehindEarth);
   mDstTransform         = new osg::MatrixTransform;
   dstShape->GetRoot()->addChild(mDstTransform);
   mDstTransform->addChild(mDstMeasureLabelPtr);

   UpdateColor();
   Build();
}


void wkf::AttachmentGreatCircleRuler::SetMode(RulerMode aMode)
{
   if (mMeasureMode != aMode)
   {
      mMeasureMode = aMode;
      Build();
      ToggleColor();
   }
}

wkf::AttachmentGreatCircleRuler::RulerMode wkf::AttachmentGreatCircleRuler::ToggleMode()
{
   switch (mMeasureMode)
   {
   case cGREAT_CIRCLE:
      SetMode(cSTRAIGHT_LINE);
      SetName("straight_line_ruler");
      break;
   case cSTRAIGHT_LINE:
      SetMode(cGREAT_CIRCLE);
      SetName("great_circle_ruler");
      break;
   default:
      SetMode(cGREAT_CIRCLE);
      SetName("great_circle_ruler");
      break;
   }
   ToggleColor();
   return mMeasureMode;
}

void wkf::AttachmentGreatCircleRuler::ToggleColor()
{
   switch (mMeasureMode)
   {
   case cGREAT_CIRCLE:
      SetColor(UtoColor(87, 166, 74));
      break;
   case cSTRAIGHT_LINE:
      SetColor(UtoColor(189, 99, 197));
      break;
   default:
      SetColor(UtoColor(0, 255, 0));
      break;
   }
   UpdateColor();
}

void wkf::AttachmentGreatCircleRuler::SetLabelMode(int aMode)
{
   if (mLabelMode != aMode)
   {
      mLabelMode = (LabelMode)aMode;
      Build();
   }
}

void wkf::AttachmentGreatCircleRuler::SetLabelSize(int aSize)
{
   if (mLabelSize != aSize)
   {
      mLabelSize = aSize;

      QFont f;
      f.setPointSize(mLabelSize);
      if (!mSrcString.isEmpty() && mSrcMeasureLabelPtr)
      {
         mSrcMeasureLabelPtr->SetText(mSrcString,
                                      QColor(mColor.Red(), mColor.Green(), mColor.Blue()),
                                      f,
                                      vespa::VaDecoratorNode::cOutline);
      }
      if (!mDstString.isEmpty() && mDstMeasureLabelPtr)
      {
         mDstMeasureLabelPtr->SetText(mDstString,
                                      QColor(mColor.Red(), mColor.Green(), mColor.Blue()),
                                      f,
                                      vespa::VaDecoratorNode::cOutline);
      }
   }
}

// virtual
void wkf::AttachmentGreatCircleRuler::Update(double aTime)
{
   Build();
}

// virtual
void wkf::AttachmentGreatCircleRuler::UpdateFrame(double aTime)
{
   static double mpp = 0.0;
   if (mpp != vespa::VaEnvironment::Instance().GetViewerManager()->CurrentViewer()->GetCamera()->GetMetersPerPixel())
   {
      mpp = vespa::VaEnvironment::Instance().GetViewerManager()->CurrentViewer()->GetCamera()->GetMetersPerPixel();
      Build();
   }
}

void wkf::AttachmentGreatCircleRuler::Build(bool aForce)
{
   if ((mMeasureLinePtr != nullptr) && mMeasureActive && (mMeasureSrcDstSet != -1))
   {
      vespa::VaCamera* cameraPtr = vespa::VaEnvironment::Instance().GetViewerManager()->CurrentViewer()->GetCamera();

      const vespa::VaPosition& cursorPosition(vespa::VaEnvironment::Instance().GetCurrentCursorPosition());
      const vespa::VaPosition& origSrcLLA(mMeasureSrc.GetPosition());
      const vespa::VaPosition& origDstLLA(mMeasureDst.GetPosition());
      vespa::VaPosition        srcLLA(mMeasureSrc.GetPosition());
      vespa::VaPosition        dstLLA = (mMeasureSrcDstSet == 1) ? mMeasureDst.GetPosition() : cursorPosition;

      if (mMeasureMode == cGREAT_CIRCLE)
      {
         srcLLA.SetAlt(0.0);
         dstLLA.SetAlt(0.0);
      }

      double LocalToECEFSrc[3][3];
      UtSphericalEarth::ComputeNEDTransform(srcLLA.GetLat(), srcLLA.GetLon(), srcLLA.GetAlt(), LocalToECEFSrc);

      double LocalToECEFDst[3][3];
      UtSphericalEarth::ComputeNEDTransform(dstLLA.GetLat(), dstLLA.GetLon(), dstLLA.GetAlt(), LocalToECEFDst);

      double srcVelXYZ[3];
      double srcVel[3];
      mMeasureSrc.GetVelocity(srcVelXYZ);
      UtSphericalEarth::ConvertLocalToECEF(LocalToECEFSrc, srcVelXYZ, srcVel);

      double dstVelXYZ[3];
      double dstVel[3];
      mMeasureDst.GetVelocity(dstVelXYZ);
      UtSphericalEarth::ConvertLocalToECEF(LocalToECEFDst, dstVelXYZ, dstVel);

      bool buildMeasureLine =
         (mMeasureLastMode != mMeasureMode) || (mLabelLastMode != mLabelMode) || (mMeasureLastSrc != srcLLA) ||
         (mMeasureLastDst != dstLLA) ||
         (!mLengthUnitOverride && (mMeasureLengthUnits != vespa::VaEnvironment::Instance().GetUnitFormat().mDistFormat)) ||
         (mMeasureSpeedUnits != vespa::VaEnvironment::Instance().GetUnitFormat().mSpeedFormat) ||
         (mMeasureTimeUnits != vespa::VaEnvironment::Instance().GetUnitFormat().mTimeFormat) ||
         (mMeasureAngleUnits != vespa::VaEnvironment::Instance().GetUnitFormat().mAngleFormat) ||
         !UtVec3d::Equals(srcVelXYZ, mMeasureLastSrcVel) || !UtVec3d::Equals(dstVelXYZ, mMeasureLastDstVel) || aForce;
      if (buildMeasureLine)
      {
         double tempXYZ[3];

         mMeasureLastMode = mMeasureMode;
         mLabelLastMode   = mLabelMode;
         mMeasureLastSrc  = srcLLA;
         mMeasureLastDst  = dstLLA;
         UtVec3d::Set(mMeasureLastSrcVel, srcVelXYZ);
         UtVec3d::Set(mMeasureLastDstVel, dstVelXYZ);

         mMeasureLinePtr->Clear();
         mMeasureVerticalLinePtr->Clear();

         double heading  = 0.0;
         double distance = 0.0;
         if (mMeasureMode == cGREAT_CIRCLE)
         {
            srcLLA.CalculateGreatCircleHeadingAndDistance(dstLLA, heading, distance);
         }
         else
         {
            srcLLA.CalculateHeadingAndDistance(dstLLA, heading, distance);
         }

         int pointIndex = 0;

         // Calculate and insert the first point.
         double srcXYZ[3];
         cameraPtr->ConvertLLAToXYZ(srcXYZ, srcLLA.GetLat(), srcLLA.GetLon(), srcLLA.GetAlt());
         GetRelativePosition(srcLLA, tempXYZ);

         if (mSrcMeasureLabelPtr != nullptr)
         {
            mSrcTransform->setMatrix(osg::Matrix::translate(tempXYZ[0], tempXYZ[1], tempXYZ[2]));
         }
         mMeasureLinePtr->Insert(pointIndex++, UtoPositionFromArray(tempXYZ));

         // Calculate the destination point.
         double dstXYZ[3];
         cameraPtr->ConvertLLAToXYZ(dstXYZ, dstLLA.GetLat(), dstLLA.GetLon(), dstLLA.GetAlt());

         double srcVec[3] = {dstXYZ[0] - srcXYZ[0], dstXYZ[1] - srcXYZ[1], dstXYZ[2] - srcXYZ[2]};
         double dstVec[3] = {srcXYZ[0] - dstXYZ[0], srcXYZ[1] - dstXYZ[1], srcXYZ[2] - dstXYZ[2]};

         if (mMeasureMode == cGREAT_CIRCLE)
         {
            // Create the two vertical lines that run from the earth's surface to the source and destination points.
            int vPointIndex = 0;
            if (origSrcLLA.GetAlt() > 0.0)
            {
               GetRelativePosition(srcLLA, tempXYZ);
               mMeasureVerticalLinePtr->Insert(vPointIndex++, UtoPositionFromArray(tempXYZ));

               double origSrcXYZ[3];
               cameraPtr->ConvertLLAToXYZ(origSrcXYZ, origSrcLLA.GetLat(), origSrcLLA.GetLon(), origSrcLLA.GetAlt());
               GetRelativePosition(origSrcLLA, tempXYZ);
               mMeasureVerticalLinePtr->Insert(vPointIndex++, UtoPositionFromArray(tempXYZ));
            }

            if ((mMeasureSrcDstSet == 1) && (origDstLLA.GetAlt() > 0.0))
            {
               GetRelativePosition(dstLLA, tempXYZ);
               mMeasureVerticalLinePtr->Insert(vPointIndex++, UtoPositionFromArray(tempXYZ));

               double origDstXYZ[3];
               cameraPtr->ConvertLLAToXYZ(origDstXYZ, origDstLLA.GetLat(), origDstLLA.GetLon(), origDstLLA.GetAlt());
               GetRelativePosition(origDstLLA, tempXYZ);
               mMeasureVerticalLinePtr->Insert(vPointIndex++, UtoPositionFromArray(tempXYZ));
            }

            // Create points in between the source and destination so that we have a line that follows the curvature of the earth.
            double srcLat = srcLLA.GetLat();
            double srcLon = srcLLA.GetLon();
            double dstLat, dstLon;
            double tempHeading = heading;
            double xyz[3];
            double dummy;
            double altB            = srcLLA.GetAlt();                   // y-intercept
            double altM            = dstLLA.GetAlt() - srcLLA.GetAlt(); // slope of line.
            double segmentDistance = 1000.0;
            int    numSegments     = (int)(distance / segmentDistance - 1.0);
            for (int ii = 0; ii < numSegments; ++ii)
            {
               // Extrapolate from the source point by the segment distance and the heading to the final point.
               UtSphericalEarth::ExtrapolateGreatCirclePosition(srcLat, srcLon, tempHeading, segmentDistance, dstLat, dstLon);

               // Interpolate the altitude between the source and destination altitudes..
               double alt = ((ii + 1) / (double)(numSegments + 2)) * altM + altB;

               // Create a VaPosition object so that we can convert to the current projection.
               vespa::VaPosition lla(dstLat, dstLon, alt);

               // Convert to ECEF and add a point to the line shape.
               cameraPtr->ConvertLLAToXYZ(xyz, dstLat, dstLon, alt);
               GetRelativePosition(lla, tempXYZ);
               mMeasureLinePtr->Insert(pointIndex++, UtoPositionFromArray(tempXYZ));

               // If this is the first point, calculate the vector from the source point to it.
               if (ii == 0)
               {
                  UtVec3d::Subtract(srcVec, xyz, srcXYZ);
               }

               // If this is the last point, calculate the vector from the destination point to it.
               if (ii == (numSegments - 1))
               {
                  UtVec3d::Subtract(dstVec, xyz, dstXYZ);
               }

               // Calculate the new heading to the final point.
               UtSphericalEarth::GreatCircleHeadingAndDistance(dstLat,
                                                               dstLon,
                                                               dstLLA.GetLat(),
                                                               dstLLA.GetLon(),
                                                               tempHeading,
                                                               dummy);

               srcLat = dstLat;
               srcLon = dstLon;
            }

            if (numSegments == 1)
            {
               UtVec3d::Subtract(srcVec, dstXYZ, srcXYZ);
               UtVec3d::Subtract(dstVec, srcXYZ, dstXYZ);
            }
         }

         // Calculate and insert the final point.
         GetRelativePosition(dstLLA, tempXYZ);
         if (mDstMeasureLabelPtr != nullptr)
         {
            mDstTransform->setMatrix(osg::Matrix::translate(tempXYZ[0], tempXYZ[1], tempXYZ[2]));
         }
         mMeasureLinePtr->Insert(pointIndex++, UtoPositionFromArray(tempXYZ));

         // Normalize our source and destination vectors.
         UtVec3d::Normalize(srcVec);
         UtVec3d::Normalize(dstVec);

         if ((mSrcMeasureLabelPtr != nullptr) && (mDstMeasureLabelPtr != nullptr))
         {
            if (!mLengthUnitOverride)
            {
               mMeasureLengthUnits = vespa::VaEnvironment::Instance().GetUnitFormat().mDistFormat;
            }
            mMeasureSpeedUnits = vespa::VaEnvironment::Instance().GetUnitFormat().mSpeedFormat;
            mMeasureTimeUnits  = vespa::VaEnvironment::Instance().GetUnitFormat().mTimeFormat;
            mMeasureAngleUnits = vespa::VaEnvironment::Instance().GetUnitFormat().mAngleFormat;

            // Calculate the relative velocity along the great circle arc.
            double velAlongGreatCircle = (UtVec3d::DotProduct(srcVelXYZ, srcVec) + UtVec3d::DotProduct(dstVelXYZ, dstVec));
            bool velocityValid         = (fabs(velAlongGreatCircle) > 0.01);

            UtLength range(distance, mMeasureLengthUnits);

            UtSpeed speed((double)velAlongGreatCircle, mMeasureSpeedUnits, srcLLA.GetAlt());
            UtTime  eta(0.0, mMeasureTimeUnits);
            if (velocityValid)
            {
               eta = distance / velAlongGreatCircle;
            }

            if (mLabelMode & cAT_SOURCE)
            {
               bool               started = false;
               UtAngleR           hdg1(UtMath::NormalizeAngle0_360(heading - 180.0), mMeasureAngleUnits);
               std::ostringstream oss1;
               if (mLabelMode & cHEADING)
               {
                  oss1 << hdg1;
                  started = true;
               }
               if (mLabelMode & cRANGE)
               {
                  if (started)
                  {
                     oss1 << ", ";
                  }
                  oss1 << range;
                  started = true;
               }
               if (velocityValid)
               {
                  if (mLabelMode & cSPEED)
                  {
                     if (started)
                     {
                        oss1 << ", ";
                     }
                     oss1 << speed;
                     started = true;
                  }
                  if (eta > 0.0)
                  {
                     if (mLabelMode & cETA)
                     {
                        if (started)
                        {
                           oss1 << ", ";
                        }
                        oss1 << eta;
                        started = true;
                     }
                  }
               }
               if (mLabelMode & cORTHO_RANGES)
               {
                  double ori[3];
                  mMeasureSrc.GetOrientation(ori);
                  UtLength crossrange(sin(-(hdg1 - ori[0]) * UtMath::cRAD_PER_DEG) * range, mMeasureLengthUnits);
                  UtLength downrange(cos(-(hdg1 - ori[0]) * UtMath::cRAD_PER_DEG) * range, mMeasureLengthUnits);
                  if (started)
                  {
                     oss1 << ", ";
                  }
                  oss1 << "XR: " << crossrange << " DR: " << downrange;
                  started = true;
               }
               QFont f;
               f.setPointSize(mLabelSize);
               mSrcString = QString::fromStdString(oss1.str());
               mSrcMeasureLabelPtr->SetText(mSrcString,
                                            QColor(mColor.Red(), mColor.Green(), mColor.Blue()),
                                            f,
                                            vespa::VaDecoratorNode::cOutline);
            }
            else
            {
               mSrcString.clear();
               mSrcMeasureLabelPtr->SetText(mSrcString,
                                            QColor(mColor.RedF(), mColor.GreenF(), mColor.BlueF()),
                                            QFont(),
                                            vespa::VaDecoratorNode::cOutline);
            }

            if (mLabelMode & cAT_DESTINATION)
            {
               bool               started = false;
               std::ostringstream oss2;
               UtAngleR           hdg2(heading, mMeasureAngleUnits);
               if (mLabelMode & cHEADING)
               {
                  oss2 << hdg2;
                  started = true;
               }
               if (mLabelMode & cRANGE)
               {
                  if (started)
                  {
                     oss2 << ", ";
                  }
                  oss2 << range;
                  started = true;
               }
               if (velocityValid)
               {
                  if (mLabelMode & cSPEED)
                  {
                     if (started)
                     {
                        oss2 << ", ";
                     }
                     oss2 << speed;
                     started = true;
                  }
                  if (eta > 0.0)
                  {
                     if (mLabelMode & cETA)
                     {
                        if (started)
                        {
                           oss2 << ", ";
                        }
                        oss2 << eta;
                        started = true;
                     }
                  }
               }
               QFont f;
               f.setPointSize(mLabelSize);
               mDstString = QString::fromStdString(oss2.str());
               mDstMeasureLabelPtr->SetText(mDstString,
                                            QColor(mColor.Red(), mColor.Green(), mColor.Blue()),
                                            f,
                                            vespa::VaDecoratorNode::cOutline);
            }
            else
            {
               mDstString.clear();
               mDstMeasureLabelPtr->SetText(mDstString,
                                            QColor(mColor.RedF(), mColor.GreenF(), mColor.BlueF()),
                                            QFont(),
                                            vespa::VaDecoratorNode::cOutline);
            }
         }
      }
   }
}

void wkf::AttachmentGreatCircleRuler::EntityMovedCB(vespa::VaEntity* aEntityPtr, bool aMoving)
{
   if (aEntityPtr != nullptr)
   {
      unsigned int id = aEntityPtr->GetUniqueId();
      if (id == GetParent().GetUniqueId())
      {
         Build();
      }
      else if (aEntityPtr == mMeasureSrc.GetEntity())
      {
         Build();
      }
      else if (aEntityPtr == mMeasureDst.GetEntity())
      {
         Build();
      }
   }
}

void wkf::AttachmentGreatCircleRuler::UpdateCursorPositionCB(vespa::VaViewer* aViewerPtr, int aMouseX, int aMouseY)
{
   if (mMeasureSrcDstSet == 0)
   {
      Build();
   }
}

void wkf::AttachmentGreatCircleRuler::UnitsChangedCB(int aMeasurementType, int aUnit)
{
   Build();
}

void wkf::AttachmentGreatCircleRuler::GetRelativePosition(const vespa::VaPosition& aLLA, double aRelXYZ[3]) const
{
   double xyz[3];
   aLLA.GetSceneXYZ(xyz, 0);

   double parentXYZ[3];
   GetParent().GetPosition().GetSceneXYZ(parentXYZ, 0);

   UtVec3d::Subtract(aRelXYZ, xyz, parentXYZ);
}

void wkf::AttachmentGreatCircleRuler::SetPosition(const vespa::VaPosition& aPosition)
{
   if (mMeasureSrcDstSet < 0)
   {
      SetSrcPosition(aPosition);
   }
   else if (mMeasureSrcDstSet == 0)
   {
      SetDstPosition(aPosition);
   }
}

void wkf::AttachmentGreatCircleRuler::SetPosition(const vespa::VaEntity& aEntity)
{
   if (mMeasureSrcDstSet < 0)
   {
      SetSrcPosition(aEntity);
   }
   else if (mMeasureSrcDstSet == 0)
   {
      // if (aEntity.IsInitialized())
      SetDstPosition(aEntity);
   }
}

void wkf::AttachmentGreatCircleRuler::SetSrcPosition(const vespa::VaPosition& aPosition)
{
   mMeasureSrcDstSet = 0;
   mMeasureSrc.SetPosition(aPosition);
}

void wkf::AttachmentGreatCircleRuler::SetSrcPosition(const vespa::VaEntity& aEntity)
{
   // Disconnect the old 'EntityMoved' callback if it exists.
   if (mMeasureSrcCallbackId != -1)
   {
      mCallbacks.Disconnect(mMeasureSrcCallbackId);
   }
   // Add the new 'EntityMoved' callback.
   mMeasureSrcCallbackId = mCallbacks.Add(
      vespa::VaObserver::EntityMoved.Connect(&AttachmentGreatCircleRuler::EntityMovedCB, this, aEntity.GetUniqueId()));

   mMeasureSrcDstSet = 0;
   mMeasureSrc.SetEntity(&aEntity);
}

void wkf::AttachmentGreatCircleRuler::SetDstPosition(const vespa::VaPosition& aPosition)
{
   mMeasureSrcDstSet = 1;
   mMeasureDst.SetPosition(aPosition);
   Build();
}

void wkf::AttachmentGreatCircleRuler::SetDstPosition(const vespa::VaEntity& aEntity)
{
   // Disconnect the old 'EntityMoved' callback if it exists.
   if (mMeasureDstCallbackId != -1)
   {
      mCallbacks.Disconnect(mMeasureDstCallbackId);
   }
   // Add the new 'EntityMoved' callback.
   mMeasureDstCallbackId = mCallbacks.Add(
      vespa::VaObserver::EntityMoved.Connect(&AttachmentGreatCircleRuler::EntityMovedCB, this, aEntity.GetUniqueId()));

   mMeasureSrcDstSet = 1;
   mMeasureDst.SetEntity(&aEntity);
   Build();
}

void wkf::AttachmentGreatCircleRuler::Take(vespa::VaEntity* aEntityPtr)
{
   // don't give us nulls
   assert(aEntityPtr);
   // don't try to take ownership of the parent entity
   assert(aEntityPtr->GetUniqueId() != GetParent().GetUniqueId());

   mOwnershipList.push_back(aEntityPtr);
}

void wkf::AttachmentGreatCircleRuler::Clear()
{
   if (mMeasureSrcCallbackId != -1)
   {
      mCallbacks.Disconnect(mMeasureSrcCallbackId);
      mMeasureSrcCallbackId = -1;
   }
   if (mMeasureDstCallbackId != -1)
   {
      mCallbacks.Disconnect(mMeasureDstCallbackId);
      mMeasureDstCallbackId = -1;
   }
   if (mMeasureLinePtr != nullptr)
   {
      mMeasureLinePtr->Clear();
   }
   if (mMeasureVerticalLinePtr != nullptr)
   {
      mMeasureVerticalLinePtr->Clear();
   }
   if (mSrcMeasureLabelPtr != nullptr)
   {
      mSrcString.clear();
      mSrcMeasureLabelPtr->SetText(mSrcString,
                                   QColor(mColor.RedF(), mColor.GreenF(), mColor.BlueF()),
                                   QFont(),
                                   vespa::VaDecoratorNode::cOutline);
   }
   if (mDstMeasureLabelPtr != nullptr)
   {
      mDstString.clear();
      mDstMeasureLabelPtr->SetText(mDstString,
                                   QColor(mColor.RedF(), mColor.GreenF(), mColor.BlueF()),
                                   QFont(),
                                   vespa::VaDecoratorNode::cOutline);
   }
   mMeasureSrcDstSet = -1;
   mMeasureLastSrc.SetLat(91); // force a redraw if cleared and then uncleared
}

bool wkf::AttachmentGreatCircleRuler::Pick(vespa::VaViewer*                aViewerPtr,
                                           int                             aMouseX,
                                           int                             aMouseY,
                                           bool                            aToggle,
                                           bool                            aDoubleClick,
                                           const vespa::VaAttachmentSubId& aAttachmentSubPartId)
{
   if (!IsMyViewer(aViewerPtr))
   {
      return false;
   }
   bool allowSelection = false;
   if (mMeasureSrcDstSet == 1)
   {
      bool selected = aToggle ? !GetSelected(aViewerPtr) : true;
      SetSelected(aViewerPtr, selected, aAttachmentSubPartId);
      allowSelection = true;
   }
   return allowSelection;
}

void wkf::AttachmentGreatCircleRuler::UpdateColor()
{
   if (!mMeasureLinePtr)
   {
      return;
   }
   UtoColor color(mColor);
   if (GetSelected(mViewerPtr))
   {
      color.SetRed(std::min(1.5f * mColor.RedF(), 1.0f) * UCHAR_MAX);
      color.SetGreen(std::min(1.5f * mColor.GreenF(), 1.0f) * UCHAR_MAX);
      color.SetBlue(std::min(1.5f * mColor.BlueF(), 1.0f) * UCHAR_MAX);
   }

   UtoColor dimColor(color);
   dimColor.SetRed((unsigned char)(0.5f * color.Red()));
   dimColor.SetGreen((unsigned char)(0.5f * color.Green()));
   dimColor.SetBlue((unsigned char)(0.5f * color.Blue()));
   mMeasureVerticalLinePtr->SetColor(dimColor);
   mMeasureLinePtr->SetColor(color);
}

void wkf::AttachmentGreatCircleRuler::SetColor(const UtoColor& aNewColor)
{
   mColor = aNewColor;
}

void wkf::AttachmentGreatCircleRuler::SetSelected(vespa::VaViewer*                aViewerPtr,
                                                  bool                            aSelected,
                                                  const vespa::VaAttachmentSubId& aSubId,
                                                  bool                            aForce)
{
   if (!IsMyViewer(aViewerPtr))
   {
      return;
   }
   VaAttachment::SetSelected(aViewerPtr, aSelected, aSubId, aForce);
   UpdateColor();
}

void wkf::AttachmentGreatCircleRuler::SetMeasureLengthUnits(int aUnits)
{
   mMeasureLengthUnits = aUnits;
   mLengthUnitOverride = true;
   Build(true);
}

void wkf::AttachmentGreatCircleRuler::ResetMeasureLengthUnits()
{
   mLengthUnitOverride = false;
   Build(true);
}
