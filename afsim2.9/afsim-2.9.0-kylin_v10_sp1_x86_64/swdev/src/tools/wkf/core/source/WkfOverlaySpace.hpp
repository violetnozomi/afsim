// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WKFOVERLAYSPACE_HPP
#define WKFOVERLAYSPACE_HPP

#include "wkf_export.h"

#include <QString>
#include <osg/Geometry>
#include <osg/PositionAttitudeTransform>

#include "UtCalendar.hpp"
#include "UtoMapProjection.hpp"
#include "UtoRawShape.hpp"
#include "VaCallbackHolder.hpp"
#include "VaDecoratorNode.hpp"
#include "VaOverlay.hpp"

namespace wkf
{
class WKF_EXPORT OverlaySpace : public vespa::VaOverlay
{
public:
   OverlaySpace();
   OverlaySpace(const std::string& aName);
   ~OverlaySpace() override;

   virtual VaOverlay* Clone() const;

   bool Initialize() override;
   void ShowTerminator(bool aState);
   void ShowSolarPath(bool aState);
   void ShowSubSolarPoint(bool aState);
   void ShowSubLunarPoint(bool aState);

   void Update() override;
   void SetTerminatorColor(const float aColor[4]);
   void SetTerminatorWidth(unsigned int aWidth);
   void SetSolarPathColor(const float aColor[4]);
   void SetSolarPathWidth(unsigned int aWidth);
   void SetSubSolarPointColor(const float aColor[4]);
   void SetSubSolarPointScale(const float aScale);
   void SetSubLunarPointColor(const float aColor[4]);
   void SetSubLunarPointScale(const float aScale);
   // empty path indicates that the default icon should be used
   void SetSubSolarPointIcon(const QString& aIconPath);
   void SetSubLunarPointIcon(const QString& aIconPath);

   void SetUpdatePeriod(const double aPeriod);

private:
   struct SubPointInfo
   {
      SubPointInfo(bool aPointType)
         : pointType(aPointType)
      {
      }
      bool                                         pointType; // true for solar, false for lunar
      UtoRawShape*                                 pointShapePtr{nullptr};
      bool                                         showPoint{false};
      UtCalendar                                   pointUpdateEpoch;
      osg::Vec4                                    pointColor;
      float                                        pointScale{1.0f};
      QString                                      pointIconPath;
      osg::ref_ptr<osg::PositionAttitudeTransform> pointTransform;
      osg::ref_ptr<vespa::VaDecoratorNode>         pointDecoratorNode;
   };

   std::string Name(bool aSolar = true) const;

   virtual void MapProjectionChangedCB(unsigned int aViewerId, const UtoMapProjection* aProjectionPtr);

   void RebuildSolarPathGeometry(double aSimTime);
   void CreateZeroSolarPathPoint();
   void AddSolarPathPoints(double aSimTime);

   void SetSubPointColor(SubPointInfo& aInfo, const float aColor[4]);
   void SetSubPointScale(SubPointInfo& aInfo, const float aScale);
   void SetSubPointIcon(SubPointInfo& aInfo, const QString& aIconPath);
   void RebuildPoint(SubPointInfo& aInfo);
   void RemovePoint(SubPointInfo& aInfo);
   void ShowSubPoint(SubPointInfo& aInfo, bool aState);

   void RebuildTerminator();
   void RebuildSolarPath();
   void RemoveTerminator();
   void RemoveSolarPath();

   double mStartEpoch{0};
   double mUpdatePeriod{60.0};

   UtoRawShape* mTerminatorShapePtr{nullptr};

   bool                         mShowTerminator{false};
   UtCalendar                   mTerminatorUpdateEpoch;
   osg::ref_ptr<osg::Geometry>  mTerminatorGeometry;
   osg::ref_ptr<osg::Vec3Array> mTerminatorVArray;
   osg::ref_ptr<osg::Vec4Array> mTerminatorCArray;
   vespa::VaCallbackHolder      mCallbacks;
   osg::Vec4                    mTerminatorColor;
   unsigned int                 mTerminatorWidth{2};

   UtoRawShape* mSolarPathShapePtr{nullptr};

   bool                         mShowSolarPath{false};
   UtCalendar                   mSolarPathUpdateEpoch;
   osg::ref_ptr<osg::Geometry>  mSolarPathGeometry;
   osg::ref_ptr<osg::Vec3Array> mSolarPathVArray;
   osg::ref_ptr<osg::Vec4Array> mSolarPathCArray;
   osg::Vec4                    mSolarPathColor;
   unsigned int                 mSolarPathWidth{2};
   std::vector<size_t>          mSolarPathBreak;
   double                       mLastSolarPathLon;

   SubPointInfo mSubSolarPoint{true};
   SubPointInfo mSubLunarPoint{false};
};
} // namespace wkf

#endif
