// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef WKFATTACHMENTROUTE_HPP
#define WKFATTACHMENTROUTE_HPP

#include "wkf_common_export.h"

#include <osg/Matrix>
#include <osg/MatrixTransform>

#include "UtoTypes.hpp"
#include "VaAttachmentQt.hpp"
#include "VaPosition.hpp"

// do not pass these classes 0 as a viewer, these are intended to exist in a single viewer
// it certainly isn't impossible to get this to work in multiple viewers
// but there are better uses of time currently

class UtoRawShape;

namespace wkf
{
class WKF_COMMON_EXPORT Waypoint : public vespa::VaPosition
{
public:
   Waypoint(double             aLat,
            double             aLon,
            double             aAlt,
            int                aGoToIndex,
            int                aType,
            const std::string& aLabel,
            UtoColor&          aTeamColor,
            unsigned int       aEntityId,
            unsigned int       aAttachmentId,
            vespa::VaViewer*   aViewerPtr);
   ~Waypoint() override;

   void               ViewMatrixChanged(const osg::Matrixd& aViewMatrix);
   unsigned int       GetId() const { return mUniqueId.GetUniqueId(); }
   int                GetGoToIndex() const { return mGoToIndex; }
   void               SetGoToIndex(int aGoToIndex) { mGoToIndex = aGoToIndex; }
   const std::string& GetLabel() const { return mLabel; }
   void               SetLabel(const std::string& aLabel) { mLabel = aLabel; }

   osg::ref_ptr<osg::MatrixTransform> GetTransformNode() { return mTransformNode; }
   bool                               IsSelected() const { return mSelected; }
   void                               SetSelected(bool aState);
   void                               UpdateModelMatrix();
   bool                               IsAbsolute() const { return (mType == 0); }

   bool IsHighLighted() const { return mHighlightNode.valid(); }
   void SetHighlight(const UtColor& aColor);
   void RemoveHighlight();

   void SetColor(const UtoColor& aColor);

   osg::ref_ptr<osg::MatrixTransform> AddLabel(const std::string& aLabel, const UtoColor& aColor);

   void Show();
   void Hide();

   void SetMovable(bool aMovable) { mMovable = aMovable; }
   bool IsMovable() const { return mMovable; }

private:
   void CreateModel();
   void DeleteModel();

   bool mMovable;

   unsigned int                       mModelId;
   vespa::VaUniqueId                  mUniqueId;
   vespa::VaViewer*                   mViewerPtr;
   osg::Matrixd                       mViewMatrix;
   osg::Matrixd                       mModelMatrix;
   bool                               mSelected;
   int                                mEntityId;
   int                                mAttachmentId;
   std::string                        mIconName;
   UtColor                            mHighlightColor;
   osg::ref_ptr<osg::MatrixTransform> mTransformHighlightNode;
   osg::ref_ptr<osg::MatrixTransform> mTransformNode;
   osg::ref_ptr<osg::Node>            mHighlightNode;
   osg::Vec4                          mColor;
   int                                mType;
   int                                mGoToIndex;
   std::string                        mLabel; // alias assigned to the waypoint by the user
};

class WKF_COMMON_EXPORT AttachmentRoute : public vespa::VaAttachmentQt
{
public:
   AttachmentRoute(vespa::VaEntity& aParent, vespa::VaViewer* aViewerPtr);
   ~AttachmentRoute() override;

   bool IsGlobalRoute() const noexcept;

   void AddWaypoint(double             aLat,
                    double             aLon,
                    double             aAlt,
                    int                aGoToIndex,
                    const std::string& aLabel,
                    int                aType    = 0,
                    bool               aRebuild = true);

   void InsertWaypoint(double             aLat,
                       double             aLon,
                       double             aAlt,
                       int                aGoToIndex,
                       const std::string& aLabel,
                       size_t             aInsertIndex,
                       int                aType = 0);

   void DeleteWaypoint(int aIndex);

   void Clear();
   void DeleteWaypoints();

   void UpdateWaypoint(size_t aIndex, double aLat, double aLon, double aAlt, int aGoToIndex, const std::string& aLabel);

   void ShowLabels(bool aShow);

   void AddLabel(const std::string& aLabel, size_t aInsertIndex, bool aShowIndex = true);

   Waypoint* GetWaypoint(unsigned int aIndex) const;
   int       FindWaypointIndex(unsigned int aSubId) const;

   /**
    * Get the list of all waypoints that are a part of this route.
    * @note This includes waypoints that are never visited due to `goto` statements.
    */
   const std::vector<Waypoint*>& GetWaypoints() const { return mWaypoints; }

   /**
    * Get the list of all waypoints that are visited by a platform
    * that uses this route, in the order in which they are visited.
    * @note If any waypoint is never visited due to `goto` statements, that
    *       waypoint will not appear in this list. If the route loops
    *       due to a `goto` statement, then one waypoint will be listed twice:
    *       once for the first time it's visited, and once at the end,
    *       indicating the loop.
    */
   const std::vector<Waypoint*>& GetUsedWaypoints() const { return mUsedWaypoints; }

   bool Pick(vespa::VaViewer*                aViewerPtr,
             int                             aMouseX,
             int                             aMouseY,
             bool                            aToggle,
             bool                            aDoubleClick,
             const vespa::VaAttachmentSubId& aAttachmentSubPartId) override;
   void ClearSelection(vespa::VaViewer* aViewerPtr) override;

   bool SelectWaypoint(int aIndex, bool aSelect = true);
   void SetSelected(vespa::VaViewer*                aViewerPtr,
                    bool                            aSelected,
                    const vespa::VaAttachmentSubId& aSubId,
                    bool                            aForce = false) override;

   bool GetSelected(vespa::VaViewer* aViewerPtr) const override;
   void SetSelectionEnabled(bool aEnabled) { mSelectionEnabled = aEnabled; }
   bool IsHighlightable() override { return true; }

   void SetContextMenuEnabled(bool aEnabled) { mContextMenuEnabled = aEnabled; }
   bool GetContextMenuEnabled() const { return mContextMenuEnabled; }

   unsigned int GetPickMode() const override { return vespa::VaAttachment::cPICK_SUBPARTS; }

   bool BoxPick(vespa::VaViewer* aViewerPtr, const vespa::VaSelectionBox& aBox, bool aInside) override;

   void MoveAllRelative(UtEntity& aRefBefore, UtEntity& aRefAfter, int aFeedbackFormat, double aFeedbackPrecision);

   void MoveRelative(UtEntity&               aRefBefore,
                     UtEntity&               aRefAfter,
                     int                     aFeedbackFormat,
                     double                  aFeedbackPrecision,
                     vespa::MoveRelativeMask aMoveRelativeMask,
                     vespa::VaViewer*        aViewerPtr) override;

   void SetTeamColor(const UtColor& aColor);
   void UpdateTeamColor();

   void UpdateVisibility() override;

   std::string GetDescription(unsigned int aSubId) override;

   void SetDraggable(bool aDraggable);

   void RemoveWaypointHighlights();

   std::string GetMenuSelectionString(const vespa::VaAttachmentSubId& aSubId) const override;

   void HighlightSubpart(vespa::VaViewer*                aViewerPtr,
                         const vespa::VaAttachmentSubId& aSubpartId,
                         bool                            aHighlight,
                         const UtColor&                  aColor) override
   {
   }

   const vespa::VaPosition& GetPosition(unsigned int aSubId) const override;

   bool SupportsContextMenu() const override { return true; }
   void BuildContextMenu(QMenu& aMenu, unsigned int aSubPartId) override;

   void RebuildSegmentShape();
   void ShowIndices(bool aState) { mShowIndices = aState; }

   struct WaypointData
   {
      double      mLat;
      double      mLon;
      double      mAlt;
      int         mGoToIndex;
      std::string mLabel;
      int         mType;
      WaypointData(double aLat, double aLon, double aAlt, int aGoToIndex, const std::string& aLabel = "", int aType = 0)
         : mLat(aLat)
         , mLon(aLon)
         , mAlt(aAlt)
         , mGoToIndex(aGoToIndex)
         , mLabel(aLabel)
         , mType(aType)
      {
      }
   };
   using WaypointDataList = std::vector<WaypointData>;

   void AddWaypoints(const WaypointDataList& aWaypoints);

   void MakeWaypointsMovable(bool aState);

private:
   void UpdateLabels();
   void ViewMatrixChanged(vespa::VaViewer* aViewerPtr);
   void MapProjectionChanged(unsigned int aViewerId, const UtoMapProjection* aProjection);

   bool                                                 mSelectionEnabled;
   bool                                                 mShowLabels;
   bool                                                 mContextMenuEnabled;
   std::vector<Waypoint*>                               mWaypoints;
   std::vector<Waypoint*>                               mUsedWaypoints;
   std::map<size_t, osg::ref_ptr<osg::MatrixTransform>> mWaypointLabels;
   UtoPolyLineShape*                                    mShapePtr;
   UtoRawShape*                                         mShapeLabelPtr;
   UtoColor                                             mTeamColor;
   float                                                mAllowedVertError;
   bool                                                 mShowIndices{true};
};
} // namespace wkf

VA_DECLARE_OBJECT_TYPE(wkf::AttachmentRoute)

#endif // WKFATTACHMENTROUTE_HPP
