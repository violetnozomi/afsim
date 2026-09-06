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

#ifndef VaAttachmentVectors_hpp
#define VaAttachmentVectors_hpp

#include "vespatk_export.h"

#include <functional>
#include <string>
#include <vector>

#include <osg/Vec4>

class UtoMapProjection;
#include "UtoViewer.hpp"
class UtoVectorShape;

#include "UtColor.hpp"
#include "UtLength.hpp"
#include "UtVec3.hpp"
#include "VaAttachment.hpp"


namespace vespa
{
//! Class for vector sets that can be attached to entities
class VESPATK_EXPORT VaAttachmentVectors : public VaAttachment
{
public:
   VaAttachmentVectors(VaEntity& aParent, VaViewer* aViewerPtr);
   ~VaAttachmentVectors() override;

   unsigned int GetPickMode() const override { return cPICK_NOTHING; }

   void Update(double aTime) override;

   void UpdateVisibility() override;

   enum eVectorType
   {
      eVECTOR_BODY_XYZ     = 0,
      eVECTOR_VELOCITY     = 1,
      eVECTOR_EARTH        = 2,
      eVECTOR_SUN          = 3,
      eVECTOR_MOON         = 4,
      eVECTOR_STATIC_COUNT = 5
   };

   unsigned int       GetCategoryCount();
   std::string        GetCategoryText(int aValue);
   static std::string GetCategoryTextStatic(int aValue);

   void ShowVector(int aVectorId, bool aState);
   bool IsShown(int aVectorId);

   int AddVector(const std::string& aTitle, float aRed, float aGreen, float aBlue, float aAlpha);

   void ShowBoresight(const std::string& aLabel, double aAzimuth, double aElevation);

   void UpdateBoresight(const std::string& aLabel, double aAzimuth, double aElevation);

   void HideBoresight(const std::string& aLabel);

   int GetBoresightId(const std::string& aTitle);

   void RemoveVector(int aIndex);
   void AddVectorToEntity(int aIndex, VaEntity* aEntity);
   void RemoveVectorToEntity(int aIndex, VaEntity* aEntity);

   UtVec3f GetVectorPosition(int aIndex);
   float   GetVectorScale(int aIndex) const;
   // so that items can keep track of vector positions when updated.
   void SubscribeToPositionUpdate(const std::function<void(int, UtVec3f)>& aCallBack);

private:
   void MapProjectionChangedCB(unsigned int aViewerId, const UtoMapProjection* aProjection);
   void ModelScaleChangedCB(float aScalar);

   void                Load(int aVectorId);
   void                CallPositionCallback(int aVectorId, const UtVec3f& aPosition);
   static unsigned int sId;

   struct Vector
   {
      UtoVectorShape* mShape;
      bool            mVisible;
      UtVec3f         mPosition;
   };

   struct CustomVector
   {
      std::string mLabel;
      osg::Vec4   mColor;
   };

   bool                              mIsCallbackPositionSet = false;
   std::function<void(int, UtVec3f)> mPositionUpdatedCallback;
   std::map<int, Vector>             mVectorShapeMap;
   std::map<int, CustomVector>       mCustomVectorMap;
   int                               mVelocityVector;
   int                               mEarthVector;
   int                               mSunVector;
   int                               mMoonVector;
   int                               mNextCustomSetIndex;
   std::vector<std::pair<int, int>>  mCustomVector;
   std::vector<unsigned int>         mCustomVectorTarget;
   std::map<std::string, int>        mBoresightIdTitles;
   float                             mModelScalar;
};
} // end namespace vespa.

VA_DECLARE_OBJECT_TYPE(vespa::VaAttachmentVectors)

#endif
