// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef WkfAttachmentSensorVolume_HPP
#define WkfAttachmentSensorVolume_HPP

#include "wkf_common_export.h"

#include <map>
#include <string>
#include <vector>

#include <osg/Array>
#include <osg/Geode>
#include <osg/Group>
#include <osg/MatrixTransform>

#include "UtoMapProjection.hpp"
#include "VaAttachment.hpp"

class UtoRawShape;

namespace vespa
{
class VaEntity;
}

class QColor;

namespace wkf
{
class AttachmentSensorVolume;

class WKF_COMMON_EXPORT SensorBeam
{
public:
   SensorBeam(osg::MatrixTransform* aParentNode, AttachmentSensorVolume* aParentAttachment);

   bool IsMarked() const { return mMarked; }
   void SetMarked(bool aState) { mMarked = aState; }
   void SetVisible(bool aState);

   void RebuildRectangular(float aTilt, float aMinR, float aMaxR, float aMinAz, float aMinEl, float aMaxAz, float aMaxEl);
   void RebuildCircular(float aTilt, float aMinR, float aMaxR, float aHalfAngle);
   void RebuildPolygonal(float aTilt, float aMinR, float aMaxR, std::vector<std::pair<float, float>>& aPoints);

   bool SetShapeColor(const osg::Vec4& aColor);
   bool SetLineColor(const osg::Vec4& aColor);
   bool SetProjectionColor(const osg::Vec4& aColor);
   void SetProjectionEnabled(bool aState);
   void SetFacesEnabled(bool aState);
   void SetEdgesEnabled(bool aState);
   void SetShaders(osg::Program* aLineProgram, osg::Program* aTriProgram);
   void UseFlatStateSet(bool aState);

private:
   void BuildDrawables();
   void AddVertex(int                       aAz,
                  int                       aEl,
                  float                     aMinAz,
                  float                     aMinEl,
                  float                     aMaxAz,
                  float                     aMaxEl,
                  float                     aSampling,
                  float                     aRange,
                  osg::Vec3Array::iterator& aItVertex);
   void BuildFullAzShape(float aMinR, float aMaxR, float aMinEl, float aMaxEl);
   void RebuildDrawables();

   osg::ref_ptr<osg::MatrixTransform> mLocalPAT;
   osg::ref_ptr<osg::Geode>           mGeode;
   osg::ref_ptr<osg::Geode>           mLinesGeode;
   osg::ref_ptr<osg::Geode>           mGridGeode;
   osg::ref_ptr<osg::Geode>           mProjectionGeode;
   osg::ref_ptr<osg::Group>           mProjFace;
   osg::ref_ptr<osg::Group>           mProjFill;

   bool mMarked{false};
   int  mLineShapeIdx{0};
   int  mGridShapeIdx{0};

   static float msRangeDivisionRate;
   static float msCapDivisionRate;
   static float msMaxRange;

   enum BeamMode
   {
      cUNDEFINED,
      cCIRCULAR,
      cPOLYGONAL,
      cRECTANGULAR
   };
};

class WKF_COMMON_EXPORT AttachmentSensorVolume : public vespa::VaAttachment
{
public:
   AttachmentSensorVolume(vespa::VaEntity& aEntity, vespa::VaViewer* aViewerPtr, const std::string& aName);
   ~AttachmentSensorVolume() override = default;

   unsigned int NumberBeams() const { return (unsigned int)mBeams.size(); }
   SensorBeam&  GetBeam(int i) { return mBeams[i]; }
   void         AddBeam();

   std::string ModeName() { return mModeName; }
   void        SetModeName(const std::string& aName) { mModeName = aName; }

   // radians
   void Articulate(const double aSlew[3], const double aCue[3], const double aTrans[3]);

   void SetDisplayData(bool          aShowFaces,
                       const QColor& aFaceColor,
                       bool          aShowEdges,
                       const QColor& aEdgeColor,
                       bool          aShowProjections,
                       const QColor& aProjectionColor);

   const UtoMapProjection& GetMapProjection() const { return mProjection; }

private:
   void MapProjectionChangedCB(unsigned int aViewerId, const UtoMapProjection* aProjection);
   void EntityMovedCB(vespa::VaEntity* aEntityPtr, bool aMoving);
   void ConfigureForMapProjection();

   std::string             mModeName;
   std::vector<SensorBeam> mBeams;
   UtoRawShape*            mShapePtr;

   osg::ref_ptr<osg::MatrixTransform> mPAT;

   bool             mShowFaces{true};
   bool             mShowEdges{true};
   bool             mShowProjections{true};
   osg::Vec4        mFaceColor{0.0, 1.0, 0.0, 0.5};
   osg::Vec4        mProjectionColor{0.0, 1.0, 0.0, 0.5};
   osg::Vec4        mEdgeColor{0.0, 1.0, 0.0, 1.0};
   UtoMapProjection mProjection;
};
} // namespace wkf

VA_DECLARE_OBJECT_TYPE(wkf::AttachmentSensorVolume)
#endif
