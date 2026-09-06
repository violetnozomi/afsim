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

#ifndef VaAttachmentZonePolygon_hpp
#define VaAttachmentZonePolygon_hpp

#include "vespatk_export.h"

#include <string>

#include "UtColor.hpp"
#include "UtLength.hpp"
#include "VaAttachmentZone.hpp"

class UtoMapProjection;
class UtoPolyLineShape;
class UtoTessellatedPolygonShape;

namespace vespa
{
class VaPosition;
class VaSelectionBox;
class VaViewer;
class VaZonePoint;

class VESPATK_DEPRECATED_EXPORT VaAttachmentZonePolygon : public VaAttachmentZone
{
public:
   typedef std::list<VaZonePoint*> ZonePoints;


   VaAttachmentZonePolygon(VaEntity& aParent, VaViewer* aViewerPtr);
   ~VaAttachmentZonePolygon() override;

   VaAttachmentZonePolygon*       Ref() override { return this; }
   const VaAttachmentZonePolygon* Ref() const override { return this; }

   VaObject* FindSubPart(const VaAttachmentSubId& aSubId) const override;

   void UpdateVisibility() override;

   std::string GetMenuSelectionString(const VaAttachmentSubId& aAttachmentSubPartId) const override;

   bool Pick(VaViewer*                aViewerPtr,
             int                      aMouseX,
             int                      aMouseY,
             bool                     aToggle,
             bool                     aDoubleClick,
             const VaAttachmentSubId& aZonePointUniqueId) override;
   bool BoxPick(VaViewer* aViewerPtr, const VaSelectionBox& aBox, bool aInside) override;

   DeleteAction DeleteSelection(VaViewer* aViewerPtr) override;
   void         ClearSelection(VaViewer* aViewerPtr) override;
   void SetSelected(VaViewer* aViewerPtr, bool aSelected, const VaAttachmentSubId& aSubId, bool aForce = false) override;

   void         MoveRelative(UtEntity&        aRefBefore,
                             UtEntity&        aRefAfter,
                             int              aFeedbackFormat,
                             double           aFeedbackPrecision,
                             MoveRelativeMask aMoveRelativeMask,
                             VaViewer*        aViewerPtr) override;
   void         SetHeight(const UtLength& aHeight) override;
   void         SetBaseHeight(const UtLength& aBaseHeight) override;
   virtual void SetHeight(const UtLength& aHeight, bool aBuild);
   virtual void SetBaseHeight(const UtLength& aBaseHeight, bool aBuild);
   void         SetColor(const UtColor& aColor) override;
   void         SetFillColor(const UtColor& aColor) override;
   void         SetLineWidth(double aLineWidth) override;
   void         SetLineStyle(int aLineStyle) override;
   void         SetEnableFill(bool aEnableFill) override;
   void         SetOrientation(double aHeading, double aPitch, double aRoll) override;
   void         SetClosed(bool aClosed, bool aBuild = true);
   void         SetHideZonePoints(bool aHideZonePoints);
   void         SetBlendFactor(float aFactor) override;
   void         SetModifier(std::string aKey, std::string aValue);
   void         RemoveModifier(std::string aKey);

   void EnableAntiAliasLines(bool aEnable);

   VaPosition         GetCentroid() const;
   bool               GetClosed() const { return mClosed; }
   bool               GetHideZonePoints() const { return mHideZonePoints; }
   const std::string& GetShapeName() const override { return mPolygonShapeName; }

   virtual void AddZonePoint(VaZonePoint* aZonePointPtr, VaViewer* aViewerPtr, bool aAddToSelected = true, bool aBuild = true);
   virtual bool      RemoveZonePoint(int aZonePointUniqueId);
   ZonePoints&       GetZonePoints() { return mZonePoints; }
   const ZonePoints& GetZonePoints() const { return mZonePoints; }
   size_t            GetModifierCount() const { return mModifierKeys.size(); }
   std::string       GetModifierKey(int aIndex) const { return mModifierKeys[aIndex]; }
   std::string       GetModifierValue(std::string aKey) const { return mModifierList.find(aKey)->second; }

   VaZonePoint* FindPoint(int aUniqueId) const;
   VaZonePoint* GetLastPoint();

   void Rebuild() { Build(); }

   virtual void Translated(double aDeltaLat, double aDeltaLon);

   void SetShared(bool aShared) override
   {
      VaAttachment::SetShared(aShared);
      mRelative = aShared;
   }
   void         SetNeverShowZones(bool aOmit) { mNeverShowZonePoints = aOmit; }
   virtual void SetRelative(bool aRelative) { mRelative = aRelative; }
   bool         IsRelative() const { return mRelative; }

   void SetDepthTesting(bool aDepthTest);


   double GetArea() const;
   double GetLength() const;

protected:
   void         GetShapesForSubpart(const VaAttachmentSubId& aSubpartId, ShapeList& aShapes) const override;
   virtual void LoadZonePoints();
   bool         GetZonePointsLoaded() const { return mPointsLoaded; }
   void         SetSelectionCleared(unsigned int aViewerId, bool aCleared) { mSelectionCleared[aViewerId] = aCleared; }

private:
   bool PrivateInitialize() override;
   void PrivateInitializeCallbacks() override;
   void PrivateLoad() override;

   void LoadState();

   void Build();

   void ComputeModelXYZ(const VaPosition& aPosition, double aModelXYZ[3]) const;


   void EntityMovedCB(VaEntity* aEntityPtr, bool aMoving);
   void MapProjectionChangedCB(unsigned int aViewer, const UtoMapProjection* aProjection);
   void TimeUpdatedCB(double aTime, double aRate, double aMinTime, double aMaxTime);

   void SetEnableFillP(bool aEnableFill, bool aBuild = true);

   UtoPolyLineShape*                  mPolylinePtr;
   std::string                        mPolylineShapeName;
   UtoTessellatedPolygonShape*        mPolygonPtr;
   std::string                        mPolygonShapeName;
   bool                               mClosed;
   bool                               mHideZonePoints;
   ZonePoints                         mZonePoints;
   bool                               mRelative;
   std::map<std::string, std::string> mModifierList;
   std::vector<std::string>           mModifierKeys;
   bool                               mPointsLoaded;
   bool                               mDepthTest;
   bool                               mAntiAliasLines;
   bool                               mNeverShowZonePoints; // used to ignore zonepoint visibility settings

   std::map<int, bool> mSelectionCleared;
};
} // end namespace vespa

VA_DECLARE_OBJECT_TYPE(vespa::VaAttachmentZonePolygon)

#endif
