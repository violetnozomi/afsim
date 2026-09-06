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

#include "WkfAttachmentRoute.hpp"

#include <fstream>
#include <ostream>

#include <osg/Depth>
#include <osg/MatrixTransform>
#include <osg/Node>
#include <osg/Program>
#include <osg/Switch>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include "UtCast.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtoOverlay.hpp"
#include "UtoPolyLineShape.hpp"
#include "UtoRawShape.hpp"
#include "UtoShaders.hpp"
#include "UtoViewer.hpp"
#include "VaDecoratorNode.hpp"
#include "VaDefs.hpp"
#include "VaEnvironment.hpp"
#include "VaModelDatabase.hpp"
#include "VaObserver.hpp"
#include "VaSelectionBox.hpp"
#include "VaViewer.hpp"
#include "WkfEnvironment.hpp"
#include "WkfObserver.hpp"
#include "WkfPlatform.hpp"
#include "WkfPlugin.hpp"
#include "WkfPluginManager.hpp"
#include "WkfScenario.hpp"
#include "WkfTeamVisibilityPrefObject.hpp"

wkf::AttachmentRoute::AttachmentRoute(vespa::VaEntity& aParent, vespa::VaViewer* aViewerPtr)
   : vespa::VaAttachmentQt(aParent, aViewerPtr, "route")
   , mSelectionEnabled(true)
   , mShowLabels(false)
   , mContextMenuEnabled(true)
   , mShapePtr(nullptr)
   , mShapeLabelPtr(nullptr)
   , mAllowedVertError(5) // determines the level of curvature applied to routes
{
   SetType<AttachmentRoute>();
   mCallbacks.Add(vespa::VaObserver::ViewMatrixChanged.Connect(&AttachmentRoute::ViewMatrixChanged, this));
   mCallbacks.Add(vespa::VaObserver::MapProjectionChanged.Connect(&AttachmentRoute::MapProjectionChanged, this));

   auto entityPtr = dynamic_cast<Platform*>(&aParent);
   if (entityPtr != nullptr)
   {
      UtColor teamColor =
         wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>()->GetTeamUtColor(entityPtr->GetSide());
      float rgba[4];
      teamColor.Get(rgba);
      mTeamColor.SetRed(rgba[0] * 255);
      mTeamColor.SetGreen(rgba[1] * 255);
      mTeamColor.SetBlue(rgba[2] * 255);
      mTeamColor.SetAlpha(rgba[3] * 255);
   }
}

//! Method: ~WkfAttachmentRoute
//! Description: WkfAttachmentRoute destructor.
//!

// public
wkf::AttachmentRoute::~AttachmentRoute()
{
   for (auto&& it : mWaypoints)
   {
      Waypoint* wp = it;
      delete wp;
   }
   mCallbacks.Clear();
}

bool wkf::AttachmentRoute::IsGlobalRoute() const noexcept
{
   return GetParent().GetName() == GetName() + "_anchor";
}

void wkf::AttachmentRoute::AddWaypoint(double             aLat,
                                       double             aLon,
                                       double             aAlt,
                                       int                aGoToIndex,
                                       const std::string& aLabel,
                                       int                aType, /* = 0 */
                                       bool               aRebuild /* = true */)
{
   // add waypoint to route
   mWaypoints.push_back(
      new Waypoint(aLat, aLon, aAlt, aGoToIndex, aType, aLabel, mTeamColor, GetParent().GetUniqueId(), GetUniqueId(), GetViewer()));

   if (aRebuild)
   {
      RebuildSegmentShape();
   }

   AddLabel(aLabel, mWaypoints.size() - 1, mShowIndices);
   if (aRebuild)
   {
      UpdateVisibility();
   }
}

void wkf::AttachmentRoute::InsertWaypoint(double             aLat,
                                          double             aLon,
                                          double             aAlt,
                                          int                aGoToIndex,
                                          const std::string& aLabel,
                                          size_t             aInsertIndex,
                                          int                aType /* = 0 */)
{
   // add waypoint to route
   if (aInsertIndex == mWaypoints.size())
   {
      // AddWaypoint is better suited for appending waypoints to the end.
      AddWaypoint(aLat, aLon, aAlt, aGoToIndex, aLabel, aType, true);
      return;
   }
   mWaypoints.insert(
      mWaypoints.begin() + aInsertIndex,
      new Waypoint(aLat, aLon, aAlt, aGoToIndex, aType, aLabel, mTeamColor, GetParent().GetUniqueId(), GetUniqueId(), GetViewer()));

   size_t i = 0;
   // update the goto indices
   for (auto& w : mWaypoints)
   {
      if ((w->GetGoToIndex() != -1) && (aInsertIndex <= ut::cast_to_size_t(w->GetGoToIndex())) &&
          (ut::cast_to_size_t(w->GetGoToIndex()) < mWaypoints.size() - 1))
      {
         w->SetGoToIndex(w->GetGoToIndex() + 1);
      }
      bool select = (i == aInsertIndex);
      if (w->IsSelected() != select)
      {
         SetSelected(mViewerPtr, select, w->GetId());
      }
      ++i;
   }

   RebuildSegmentShape();

   AddLabel(aLabel, aInsertIndex, mShowIndices);
   UpdateVisibility();
}

void wkf::AttachmentRoute::DeleteWaypoint(int aIndex)
{
   if (aIndex < static_cast<int>(mWaypoints.size()) && aIndex >= 0)
   {
      if (mShapeLabelPtr)
      {
         mShapeLabelPtr->GetRoot()->removeChild(aIndex, 1);
      }
      mWaypoints.at(aIndex)->Hide();
      mWaypoints.erase(mWaypoints.begin() + aIndex);

      // update the goto indices
      for (auto& w : mWaypoints)
      {
         if ((w->GetGoToIndex() != -1) && (aIndex < w->GetGoToIndex()))
         {
            w->SetGoToIndex(w->GetGoToIndex() - 1);
         }
      }
   }

   UpdateLabels();
   RebuildSegmentShape();
   UpdateVisibility();
}

void wkf::AttachmentRoute::Clear()
{
   DeleteWaypoints();
   RebuildSegmentShape();
}

void wkf::AttachmentRoute::DeleteWaypoints()
{
   if (mShapeLabelPtr)
   {
      mShapeLabelPtr->GetRoot()->removeChildren(0, mShapeLabelPtr->GetRoot()->getNumChildren());
      mShapeLabelPtr = nullptr;
   }

   for (auto w : mWaypoints)
   {
      delete w;
   }
   mWaypoints.clear();
}

void wkf::AttachmentRoute::UpdateWaypoint(size_t             aIndex,
                                          double             aLat,
                                          double             aLon,
                                          double             aAlt,
                                          int                aGoToIndex,
                                          const std::string& aLabel)
{
   // Set waypoint attributes
   wkf::Waypoint* waypoint = mWaypoints.at(aIndex);
   waypoint->SetLat(aLat);
   waypoint->SetLon(aLon);
   waypoint->SetAlt(aAlt);
   waypoint->SetLabel(aLabel);
   waypoint->SetGoToIndex(aGoToIndex);

   // Update the waypoint/route visually on the map
   waypoint->UpdateModelMatrix();
   UpdateLabels();
   RebuildSegmentShape();
   UpdateVisibility();
}

void wkf::AttachmentRoute::ShowLabels(bool aShow)
{
   mShowLabels = aShow;
   if (mShapeLabelPtr && mViewerPtr)
   {
      mShapeLabelPtr->Show(aShow, *(mViewerPtr->GetViewer()));
   }
}

void wkf::AttachmentRoute::AddLabel(const std::string& aLabel, size_t aInsertIndex, bool aShowIndex)
{
   if (aShowIndex || !aLabel.empty())
   {
      // add Shape pointer to route for the waypoint labels
      if (mShapeLabelPtr == nullptr)
      {
         UtoRawShape labels;
         labels.SetReferenceFrame(UtoShape::ReferenceFrame::cFRAME_WORLD);
         mShapeLabelPtr = dynamic_cast<UtoRawShape*>(AddShapeToParent("waypointlabels", labels));
      }

      // create the text of the waypoint label
      if (mShapeLabelPtr != nullptr)
      {
         std::string wyptLabel = std::to_string(aInsertIndex);

         if (!aLabel.empty()) // if the waypoint has a label (alias)
         {
            wyptLabel += " (" + aLabel + ")";
         }

         // add the waypoint label to the shape pointer
         mWaypointLabels[aInsertIndex] = mWaypoints.at(aInsertIndex)->AddLabel(wyptLabel, mTeamColor);

         mShapeLabelPtr->GetRoot()->insertChild(aInsertIndex, mWaypointLabels[aInsertIndex].get());
         UpdateLabels();
      }
   }
}

wkf::Waypoint* wkf::AttachmentRoute::GetWaypoint(unsigned int aIndex) const
{
   if (aIndex < mWaypoints.size())
   {
      return mWaypoints.at(aIndex);
   }
   return nullptr;
}

int wkf::AttachmentRoute::FindWaypointIndex(unsigned int aSubId) const
{
   if (aSubId == 0)
   {
      return -1;
   }

   unsigned int index = 0;
   for (auto& w : mWaypoints)
   {
      if (w->GetId() == aSubId)
      {
         return index;
      }
      ++index;
   }
   return -1;
}

void wkf::AttachmentRoute::RebuildSegmentShape()
{
   if (mViewerPtr)
   {
      if (mShapePtr == nullptr)
      {
         UtoPolyLineShape* lineShapePtr = new UtoPolyLineShape();
         lineShapePtr->SetReferenceFrame(UtoShape::cFRAME_WORLD);
         mShapePtr = dynamic_cast<UtoPolyLineShape*>(AddShapeToParent(GetName() + "_routevector", *lineShapePtr));
         if (mShapePtr != nullptr)
         {
            mShapePtr->EnableAntiAliasLines(true);
            mShapePtr->SetDepthBin(10);
            mShapePtr->SetColor(mTeamColor);
         }

         delete lineShapePtr;

         // build it up
      }
      if (mShapePtr != nullptr)
      {
         mShapePtr->Clear();
         mShapePtr->SetAggregation(1); // LINES: [ 0-1 - 1-2 - 2-3 ] rather than [1-2-3]

         int index         = 0;
         int waypointCount = mWaypoints.size();
         mUsedWaypoints.clear();
         int goToIndex{0};
         while (goToIndex < waypointCount)
         {
            if (std::find(mUsedWaypoints.begin(), mUsedWaypoints.end(), mWaypoints[goToIndex]) != mUsedWaypoints.end())
            {
               mUsedWaypoints.push_back(mWaypoints[goToIndex]);
               break;
            }
            mUsedWaypoints.push_back(mWaypoints[goToIndex]);
            int nextIndex = mUsedWaypoints.back()->GetGoToIndex();
            goToIndex     = (nextIndex < 0) ? goToIndex + 1 : nextIndex;
         }

         if (mUsedWaypoints.empty())
         {
            // If mUsedWaypoints is somehow empty, then the expression `mUsedWaypoints.size() - 1` will underflow and cause errors.
            return;
         }
         for (size_t i = 0; i < mUsedWaypoints.size() - 1; ++i)
         {
            wkf::Waypoint* fromWaypoint = mUsedWaypoints[i];
            wkf::Waypoint* toWaypoint   = mUsedWaypoints[i + 1];
            if (fromWaypoint && toWaypoint)
            {
               double fromEcef[3];
               float  fromAlt = fromWaypoint->GetAlt();
               fromWaypoint->GetECEF(fromEcef);

               double toEcef[3];
               float  toAlt = toWaypoint->GetAlt();
               toWaypoint->GetECEF(toEcef);

               // compute spacing
               float fromMagsq = fromEcef[0] * fromEcef[0] + fromEcef[1] * fromEcef[1] + fromEcef[2] * fromEcef[2];
               float toMagsq   = toEcef[0] * toEcef[0] + toEcef[1] * toEcef[1] + toEcef[2] * toEcef[2];
               float r         = sqrt(toMagsq > fromMagsq ? toMagsq : fromMagsq);
               float spacing   = sqrt(r * r - (r - mAllowedVertError) * (r - mAllowedVertError));
               float dist      = sqrt((fromEcef[0] - toEcef[0]) * (fromEcef[0] - toEcef[0]) +
                                 (fromEcef[1] - toEcef[1]) * (fromEcef[1] - toEcef[1]) +
                                 (fromEcef[2] - toEcef[2]) * (fromEcef[2] - toEcef[2]));

               // Handle the case where spacing == 0 due to large numbers and number precision
               if (spacing < mAllowedVertError)
               {
                  spacing = mAllowedVertError;
               }

               // limit the max number of segments drawn in the line
               int numberOfSegments = dist / spacing;
               if (numberOfSegments > 2000) // Cap number of segments at 2000
               {
                  spacing = dist / 2000.0;
               }

               // compute first point
               double prevXYZ[3];
               double prevLLA[3];
               UtEllipsoidalEarth::ConvertECEFToLLA(fromEcef, prevLLA[0], prevLLA[1], prevLLA[2]);
               prevLLA[2] = fromAlt;
               vespa::VaPosition prevP(prevLLA[0], prevLLA[1], prevLLA[2]);
               prevP.GetSceneXYZ(prevXYZ, GetViewer()->GetUniqueId());

               for (float d = spacing; d < dist; d += spacing)
               {
                  // compute second point
                  double nextEcef[3] = {fromEcef[0] + d / dist * (toEcef[0] - fromEcef[0]),
                                        fromEcef[1] + d / dist * (toEcef[1] - fromEcef[1]),
                                        fromEcef[2] + d / dist * (toEcef[2] - fromEcef[2])};
                  double nextLLA[3];
                  UtEllipsoidalEarth::ConvertECEFToLLA(nextEcef, nextLLA[0], nextLLA[1], nextLLA[2]);
                  nextLLA[2] = fromAlt + d / dist * (toAlt - fromAlt);
                  vespa::VaPosition nextP(nextLLA[0], nextLLA[1], nextLLA[2]);
                  double            nextXYZ[3];
                  nextP.GetSceneXYZ(nextXYZ, GetViewer()->GetUniqueId());

                  // insert first and second points
                  mShapePtr->Insert(index, UtoPosition(prevXYZ[0], prevXYZ[1], prevXYZ[2]));
                  mShapePtr->Insert(++index, UtoPosition(nextXYZ[0], nextXYZ[1], nextXYZ[2]));
                  ++index;

                  prevXYZ[0] = nextXYZ[0];
                  prevXYZ[1] = nextXYZ[1];
                  prevXYZ[2] = nextXYZ[2];
               }
               double nextXYZ[3];
               toWaypoint->GetSceneXYZ(nextXYZ, GetViewer()->GetUniqueId());

               // insert first and second points
               mShapePtr->Insert(index, UtoPosition(prevXYZ[0], prevXYZ[1], prevXYZ[2]));
               mShapePtr->Insert(++index, UtoPosition(nextXYZ[0], nextXYZ[1], nextXYZ[2]));
               ++index;
            }
         }
      }
   }
}

void wkf::AttachmentRoute::AddWaypoints(const WaypointDataList& aWaypoints)
{
   for (auto& wpd : aWaypoints)
   {
      // add waypoint to route
      mWaypoints.push_back(new Waypoint(wpd.mLat,
                                        wpd.mLon,
                                        wpd.mAlt,
                                        wpd.mGoToIndex,
                                        wpd.mType,
                                        wpd.mLabel,
                                        mTeamColor,
                                        GetParent().GetUniqueId(),
                                        GetUniqueId(),
                                        GetViewer()));
      AddLabel(wpd.mLabel, mWaypoints.size() - 1, mShowIndices);
   }

   RebuildSegmentShape();
   UpdateVisibility();
}

void wkf::AttachmentRoute::MakeWaypointsMovable(bool aState)
{
   for (auto& wp : mWaypoints)
   {
      wp->SetMovable(aState);
   }
}

void wkf::AttachmentRoute::UpdateLabels()
{
   if (mViewerPtr && mShapeLabelPtr)
   {
      unsigned int index = 0;
      for (auto& w : mWaypoints)
      {
         auto        foundWaypoint = std::find(mUsedWaypoints.begin(), mUsedWaypoints.end(), w);
         std::string fullLabel     = w->GetLabel();
         bool        hasLabel{fullLabel.find_first_of('(') != ut::npos && fullLabel.find_last_of(')') != ut::npos};
         std::string label;
         if (hasLabel)
         {
            size_t indexLoc = fullLabel.find_first_of(' ');
            label           = fullLabel.substr(indexLoc);
         }
         // Do not create transform nodes if there is no label text to show (performance consideration).
         if (!label.empty() && foundWaypoint != mUsedWaypoints.begin())
         {
            osg::ref_ptr<osg::MatrixTransform> node = w->GetTransformNode();
            if (node.valid() && node->getNumChildren() > 0)
            {
               osg::Geode* geode = dynamic_cast<osg::Geode*>(node->getChild(0));
               if (geode != nullptr)
               {
                  vespa::VaDecoratorNode* dec = dynamic_cast<vespa::VaDecoratorNode*>(geode);
                  if (dec != nullptr)
                  {
                     dec->SetText(QString::number(index) + " (" + QString::fromStdString(label) + ")",
                                  QColor(255, 255, 255, 0),
                                  QColor(mTeamColor.Red(), mTeamColor.Green(), mTeamColor.Blue(), mTeamColor.Alpha()),
                                  vespa::VaDecoratorNode::cOutline);
                  }
               }
            }
         }
         ++index;
      }
   }
}

void wkf::AttachmentRoute::ViewMatrixChanged(vespa::VaViewer* aViewerPtr)
{
   if (IsMyViewer(aViewerPtr))
   {
      const osg::Matrixd& vm = aViewerPtr->GetViewMatrix();
      for (auto&& it : mWaypoints)
      {
         it->ViewMatrixChanged(vm);
      }
   }
}

void wkf::AttachmentRoute::MapProjectionChanged(unsigned int aViewerId, const UtoMapProjection* aProjection)
{
   if (GetViewer() && (aViewerId == GetViewer()->GetUniqueId()))
   {
      RebuildSegmentShape();
      for (auto&& it : mWaypoints)
      {
         if (it->IsSelected())
         {
            SetSelected(GetViewer(), false, it->GetId());
         }
         it->UpdateModelMatrix();
      }
   }
}

void wkf::AttachmentRoute::ClearSelection(vespa::VaViewer* aViewerPtr)
{
   for (auto&& it : mWaypoints)
   {
      if (it->IsSelected())
      {
         SetSelected(mViewerPtr, false, it->GetId());
      }
   }
}

bool wkf::AttachmentRoute::SelectWaypoint(int aIndex, bool aSelect)
{
   if (mSelectionEnabled)
   {
      if (aIndex < static_cast<int>(mWaypoints.size()) && aIndex >= 0)
      {
         SetSelected(GetViewer(), aSelect, mWaypoints.at(aIndex)->GetId());
         return true;
      }
   }
   return false;
}

// virtual
void wkf::AttachmentRoute::SetSelected(vespa::VaViewer*                aViewerPtr,
                                       bool                            aSelected,
                                       const vespa::VaAttachmentSubId& aSubId,
                                       bool                            aForce)
{
   auto it = std::find_if(mWaypoints.begin(),
                          mWaypoints.end(),
                          [aSubId](const wkf::Waypoint* w) { return ut::safe_cast<int>(w->GetId()) == aSubId.mSubId; });
   if (GetParent().GetScenario() && it != mWaypoints.end())
   {
      (*it)->SetSelected(aSelected);
      AttachmentSelect(aViewerPtr, (aSelected ? 1 : 0), aSubId);
   }
}

// virtual
bool wkf::AttachmentRoute::GetSelected(vespa::VaViewer* aViewerPtr) const
{
   for (auto& w : mWaypoints)
   {
      if (w->IsSelected())
      {
         return true;
      }
   }
   return false;
}

bool wkf::AttachmentRoute::Pick(vespa::VaViewer*                aViewerPtr,
                                int                             aMouseX,
                                int                             aMouseY,
                                bool                            aToggle,
                                bool                            aDoubleClick,
                                const vespa::VaAttachmentSubId& aAttachmentSubPartId)
{
   if (mSelectionEnabled)
   {
      wkf::Waypoint* wp = nullptr;
      for (auto&& it : mWaypoints)
      {
         if (ut::safe_cast<int>(it->GetId()) == aAttachmentSubPartId.mSubId)
         {
            wp = it;
            break;
         }
      }
      if (nullptr != wp)
      {
         bool selected = aToggle ? !wp->IsSelected() : true;
         SetSelected(aViewerPtr, selected, aAttachmentSubPartId);

         return true;
      }
   }
   return false;
}

bool wkf::AttachmentRoute::BoxPick(vespa::VaViewer* aViewerPtr, const vespa::VaSelectionBox& aBox, bool aInside)
{
   bool retval = false;
   if (mSelectionEnabled)
   {
      for (auto&& it : mWaypoints)
      {
         if (aBox.Contains(it->GetLat(), it->GetLon(), it->GetAlt()))
         {
            SetSelected(aViewerPtr, aInside, it->GetId());
            if (aInside)
            {
               retval = true;
            }
         }
         else
         {
            SetSelected(aViewerPtr, !aInside, it->GetId());
            if (!aInside)
            {
               retval = true;
            }
         }
      }
   }
   return retval;
}


void wkf::AttachmentRoute::MoveAllRelative(UtEntity& aRefBefore, UtEntity& aRefAfter, int aFeedbackFormat, double aFeedbackPrecision)
{
   for (auto&& it : mWaypoints)
   {
      it->MoveRelative(aRefBefore, aRefAfter, aFeedbackFormat, aFeedbackPrecision);
      it->UpdateModelMatrix();
   }
   RebuildSegmentShape();
}

void wkf::AttachmentRoute::MoveRelative(UtEntity&               aRefBefore,
                                        UtEntity&               aRefAfter,
                                        int                     aFeedbackFormat,
                                        double                  aFeedbackPrecision,
                                        vespa::MoveRelativeMask aMoveRelativeMask,
                                        vespa::VaViewer*        aViewerPtr)
{
   // moveRelativePoints is used to identify when to update the position of a relative waypoint (Not absolute)
   //  due to the position of the previous absolute waypoint's position being modified.
   bool         moveRelativePoints = false;
   unsigned int index              = 0;
   for (auto&& it : mWaypoints)
   {
      if (it->IsMovable())
      {
         if (it->IsAbsolute())
         {
            if (it->IsSelected())
            {
               it->MoveRelative(aRefBefore, aRefAfter, aFeedbackFormat, aFeedbackPrecision);
               it->UpdateModelMatrix();
               Observer::WaypointMoved(this, index);
               moveRelativePoints = true;
            }
            else
            {
               moveRelativePoints = false;
            }
         }
         else if (moveRelativePoints)
         {
            it->MoveRelative(aRefBefore, aRefAfter, aFeedbackFormat, aFeedbackPrecision);
            it->UpdateModelMatrix();
            Observer::WaypointMoved(this, index);
         }
      }
      index++;
   }
   RebuildSegmentShape();
}

void wkf::AttachmentRoute::SetTeamColor(const UtColor& aColor)
{
   float rgba[4];
   aColor.Get(rgba);
   mTeamColor.SetRed(rgba[0] * 255);
   mTeamColor.SetGreen(rgba[1] * 255);
   mTeamColor.SetBlue(rgba[2] * 255);
   mTeamColor.SetAlpha(rgba[3] * 255);
}

void wkf::AttachmentRoute::UpdateTeamColor()
{
   for (auto w : mWaypoints)
   {
      w->SetColor(mTeamColor);
   }

   if (mShapePtr != nullptr)
   {
      mShapePtr->SetColor(mTeamColor);
   }

   if (mShapeLabelPtr != nullptr)
   {
      for (unsigned int i = 0; i < mShapeLabelPtr->GetRoot()->getNumChildren(); ++i)
      {
         osg::MatrixTransform* node  = dynamic_cast<osg::MatrixTransform*>(mShapeLabelPtr->GetRoot()->getChild(i));
         osg::Geode*           geode = dynamic_cast<osg::Geode*>(node->getChild(0));
         if (geode)
         {
            vespa::VaDecoratorNode* dec = dynamic_cast<vespa::VaDecoratorNode*>(geode);
            if (dec != nullptr)
            {
               dec->SetText(dec->GetText(),
                            QColor(255, 255, 255, 0),
                            QColor(mTeamColor.Red(), mTeamColor.Green(), mTeamColor.Blue(), mTeamColor.Alpha()),
                            vespa::VaDecoratorNode::cOutline);
            }
         }
      }
   }
}

void wkf::AttachmentRoute::UpdateVisibility()
{
   VaAttachment::UpdateVisibility();

   bool              visible    = IsVisible();
   bool              showLabels = false;
   std::vector<bool> visiblePoints;
   visiblePoints.assign(mWaypoints.size(), false);
   size_t goToIndex{0};
   while (goToIndex < mWaypoints.size())
   {
      if (visiblePoints[goToIndex] == true)
      {
         break;
      }
      visiblePoints[goToIndex] = true;
      int nextIndex            = mWaypoints[goToIndex]->GetGoToIndex();
      goToIndex                = (nextIndex < 0) ? goToIndex + 1 : nextIndex;
   }
   for (size_t i = 0; i < mWaypoints.size(); ++i)
   {
      auto waypoint = mWaypoints[i];
      if (mShapeLabelPtr)
      {
         if (visible && (visiblePoints[i] == true))
         {
            waypoint->Show();
            if (!mShapeLabelPtr->GetRoot()->containsNode(mWaypointLabels[i].get()))
            {
               mShapeLabelPtr->GetRoot()->addChild(mWaypointLabels[i].get());
            }
            if (waypoint->IsSelected())
            {
               showLabels = true;
            }
         }
         else
         {
            waypoint->Hide();
            SetSelected(GetViewer(), false, waypoint->GetId(), false);
            if (mShapeLabelPtr->GetRoot()->containsNode(mWaypointLabels[i].get()))
            {
               mShapeLabelPtr->GetRoot()->removeChild(mWaypointLabels[i].get());
            }
         }
      }
   }
   ShowLabels(showLabels);
}

std::string wkf::AttachmentRoute::GetDescription(unsigned int aSubId)
{
   std::string returnStr = GetParent().GetName() + "'s route";

   int index = 0;
   for (auto& w : mWaypoints)
   {
      if (w->GetId() == aSubId)
      {
         returnStr += "\nWaypoint " + QString::number(index).toStdString();
      }
      index++;
   }
   return returnStr;
}

void wkf::AttachmentRoute::SetDraggable(bool aDraggable)
{
   for (auto& w : mWaypoints)
   {
      w->SetMovable(aDraggable);
   }
}

void wkf::AttachmentRoute::RemoveWaypointHighlights()
{
   for (auto& w : mWaypoints)
   {
      if (!w->IsSelected())
      {
         w->RemoveHighlight();
      }
   }
}

std::string wkf::AttachmentRoute::GetMenuSelectionString(const vespa::VaAttachmentSubId& aSubId) const
{
   if (mSelectionEnabled)
   {
      int waypointCount = 0;
      for (auto& w : mWaypoints)
      {
         if (w->GetId() == static_cast<unsigned int>(aSubId.mSubId))
         {
            QString parentName = QString::fromStdString(GetParent().GetName());
            QString routeName  = QString::fromStdString(GetName());

            // determine if this is a global route
            if (parentName.startsWith(routeName) && parentName.endsWith("_anchor"))
            {
               parentName = "Global Route ";
            }
            else
            {
               parentName += "'s ";
            }
            QString returnStr = parentName + routeName + " waypoint " + QString::number(waypointCount);
            return returnStr.toStdString();
         }
         ++waypointCount;
      }
   }

   // if no waypoint was found or selection is disabled return an empty string to prevent the attachment from being
   // added to the menu created from the hitlist.
   return "";
}

const vespa::VaPosition& wkf::AttachmentRoute::GetPosition(unsigned int aSubId) const
{
   Waypoint* wp = GetWaypoint(FindWaypointIndex(aSubId));
   if (wp)
   {
      return *wp;
   }
   else
   {
      return vespa::VaAttachment::GetPosition(aSubId);
   }
}

void wkf::AttachmentRoute::BuildContextMenu(QMenu& aMenu, unsigned int aSubPartId)
{
   for (auto&& p : wkfEnv.GetPluginManager()->GetLoadedPlugins())
   {
      p.second->BuildAttachmentContextMenu(&aMenu, this, aSubPartId);
   }
}

wkf::Waypoint::Waypoint(double             aLat,
                        double             aLon,
                        double             aAlt,
                        int                aGoToIndex,
                        int                aType,
                        const std::string& aLabel,
                        UtoColor&          aColor,
                        unsigned int       aEntityId,
                        unsigned int       aAttachmentId,
                        vespa::VaViewer*   aViewerPtr)
   : VaPosition(aLat, aLon, aAlt)
   , mMovable(false)
   , mUniqueId()
   , mViewerPtr(aViewerPtr)
   , mSelected(false)
   , mEntityId(aEntityId)
   , mAttachmentId(aAttachmentId)
   , mIconName("waypoint")
   , mColor(osg::Vec4(aColor.RedF(), aColor.GreenF(), aColor.BlueF(), aColor.AlphaF()))
   , mType(aType)
   , mGoToIndex(aGoToIndex)
   , mLabel(aLabel)

{
   if ((aType & 0xFF) == 1)
   {
      mIconName = "waypoint_offset";
   }
   else if ((aType & 0xFF) == 2)
   {
      int turntype = aType >> 8;
      if (turntype == 0)
      {
         mIconName = "waypoint_turn";
      }
      else if (turntype == 1)
      {
         mIconName = "waypoint_left";
      }
      else if (turntype == 2)
      {
         mIconName = "waypoint_right";
      }
   }
   CreateModel();
}

wkf::Waypoint::~Waypoint()
{
   DeleteModel();
}

void wkf::Waypoint::ViewMatrixChanged(const osg::Matrixd& aViewMatrix)
{
   mViewMatrix = aViewMatrix;

   if (0 != mModelId)
   {
      vespa::VaEnvironment::Instance().GetModelDatabase()->UpdateModel(mModelId, mModelMatrix, mViewMatrix, mColor);
   }
}

void wkf::Waypoint::SetSelected(bool aState)
{
   if (aState != mSelected)
   {
      if (aState)
      {
         SetHighlight(UtColor(1.0, 1.0, 1.0, 1.0));
      }
      else
      {
         RemoveHighlight();
      }
   }
   mSelected = aState;
}

void wkf::Waypoint::UpdateModelMatrix()
{
   if (mModelId)
   {
      double xyz[3];
      GetSceneXYZ(xyz, mViewerPtr->GetUniqueId());
      mModelMatrix.makeIdentity();
      mModelMatrix.preMultTranslate(osg::Vec3f(xyz[0], xyz[1], xyz[2]));
      mTransformNode->setMatrix(mModelMatrix);
      mModelMatrix.preMultScale(osg::Vec3f(0.3f, 0.3f, 0.3f));
      mTransformHighlightNode->setMatrix(mModelMatrix);
      vespa::VaEnvironment::Instance().GetModelDatabase()->UpdateModel(mModelId,
                                                                       mModelMatrix,
                                                                       mViewerPtr->GetViewMatrix(),
                                                                       mColor);
   }
}

void wkf::Waypoint::SetHighlight(const UtColor& aColor)
{
   if (mTransformHighlightNode != nullptr)
   {
      mHighlightColor = aColor;
      mHighlightNode =
         dynamic_cast<osg::Node*>(vespa::VaEnvironment::Instance()
                                     .GetModelDatabase()
                                     ->RequestSharedModel(mIconName, mViewerPtr)
                                     ->clone(osg::CopyOp::DEEP_COPY_DRAWABLES | osg::CopyOp::DEEP_COPY_NODES));
      mTransformHighlightNode->addChild(mHighlightNode);

      if ((mHighlightNode->getNumDescriptions() > 1) &&
          (mHighlightNode->getDescription(1) == "VA_Billboarded")) // if the node is billboarded
      {
         osg::Program* prog = UtoShaders::UseProgram("billboard.vert", "highlightObject.frag");
         mHighlightNode->getOrCreateStateSet()->setAttributeAndModes(prog, osg::StateAttribute::PROTECTED);
      }

      mViewerPtr->HighlightNode(mTransformHighlightNode.get(), true, aColor);
   }
}

void wkf::Waypoint::RemoveHighlight()
{
   if (mHighlightNode.valid())
   {
      if (mTransformHighlightNode != nullptr)
      {
         mTransformHighlightNode->removeChild(0, 1);
         if (mViewerPtr != nullptr)
         {
            mViewerPtr->HighlightNode(mTransformHighlightNode.get(), false, mHighlightColor);
         }
      }
      vespa::VaEnvironment::Instance().GetModelDatabase()->ReleaseModel(mIconName, mHighlightNode, mViewerPtr);
   }
}

void wkf::Waypoint::SetColor(const UtoColor& aColor)
{
   mColor = osg::Vec4(aColor.RedF(), aColor.GreenF(), aColor.BlueF(), aColor.AlphaF());
}

osg::ref_ptr<osg::MatrixTransform> wkf::Waypoint::AddLabel(const std::string& aLabel, const UtoColor& aColor)
{
   if (mTransformNode)
   {
      osg::ref_ptr<vespa::VaDecoratorNode> node = new vespa::VaDecoratorNode();
      node->SetText(QString::fromStdString(aLabel),
                    QColor(255, 255, 255, 0),
                    QColor(aColor.Red(), aColor.Green(), aColor.Blue(), aColor.Alpha()),
                    vespa::VaDecoratorNode::cOutline);
      mTransformNode->addChild(node);
   }
   return mTransformNode;
}

void wkf::Waypoint::Show()
{
   // If model is not already create, create the model so it is shown.
   if (mModelId == 0)
   {
      CreateModel();
   }
}

void wkf::Waypoint::Hide()
{
   DeleteModel();
}

void wkf::Waypoint::CreateModel()
{
   vespa::VaHitEntry hit = vespa::VaHitEntry::FromAttachment(mEntityId, mAttachmentId, mUniqueId.GetUniqueId());
   mModelId = vespa::VaEnvironment::Instance().GetModelDatabase()->RequestModel(mIconName, mViewerPtr, hit, hit);
   mModelMatrix.makeIdentity();

   double xyz[3];
   GetSceneXYZ(xyz, mViewerPtr->GetUniqueId());
   mModelMatrix.preMultTranslate(osg::Vec3f(xyz[0], xyz[1], xyz[2]));
   mTransformNode = new osg::MatrixTransform;
   mTransformNode->setMatrix(mModelMatrix);
   mModelMatrix.preMultScale(osg::Vec3f(0.3f, 0.3f, 0.3f));
   mTransformHighlightNode = new osg::MatrixTransform;
   mTransformHighlightNode->setMatrix(mModelMatrix);
   vespa::VaEnvironment::Instance().GetModelDatabase()->UpdateModel(mModelId, mModelMatrix, mViewMatrix, mColor);
}

void wkf::Waypoint::DeleteModel()
{
   RemoveHighlight();
   mTransformHighlightNode = nullptr;
   mTransformNode          = nullptr;
   if (mModelId != 0)
   {
      vespa::VaEnvironment::Instance().GetModelDatabase()->ReleaseModel(mModelId);
      mModelId = 0;
   }
}
