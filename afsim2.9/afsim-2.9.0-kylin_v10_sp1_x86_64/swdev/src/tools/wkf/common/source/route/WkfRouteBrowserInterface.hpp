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

#ifndef WKFROUTEBROWSERINTERFACE_HPP
#define WKFROUTEBROWSERINTERFACE_HPP

#include "wkf_common_export.h"

#include <unordered_map>

#include <QMetaType>
#include <QMutex>
#include <QObject>

#include "UtCast.hpp"
#include "UtLLAPos.hpp"
#include "UtUnitTypes.hpp"
#include "VaCallbackHolder.hpp"
#include "VaDefs.hpp"
#include "WkfPlatform.hpp"
#include "WkfVtkEnvironment.hpp"

namespace vespa
{
class VaViewer;
class VaEntity;
} // namespace vespa

namespace wkf
{
class AttachmentRoute;
class Platform;
class RouteBrowserDockWidget;
class RouteBrowserEditDialog;

namespace WaypointValueType
{
constexpr float cUSE_DEFAULT  = 1.0E-12F;
constexpr float cUSE_PREVIOUS = 1.0E-13F;
constexpr float cNOT_SET      = 1.0E-14F;
} // namespace WaypointValueType

const std::string cTEMPORARY_ROUTE = "Temporary_Route";

class WKF_COMMON_EXPORT RouteBrowserInterface : public QObject
{
   Q_OBJECT
public:
   struct WaypointInfo
   {
      WaypointInfo()
         : WaypointInfo(false)
      {
      }
      WaypointInfo(int aIndex)
         : WaypointInfo(aIndex != 0)
      {
      }
      WaypointInfo(bool aUsePreviousValues)
         : mPointType(cABSOLUTE)
         , mLabel("")
         , mLat(0)
         , mXOffset(0)
         , mYOffset(0)
         , mHeading(0)
         , mLon(0)
         , mAltRef(0)
         , mGoTo("")
         , mTurnDir(0)
         , mSwitchType(0)
      {
         if (!aUsePreviousValues)
         {
            mAlt       = WaypointValueType::cNOT_SET;
            mSpeed     = WaypointValueType::cNOT_SET;
            mClimbRate = WaypointValueType::cNOT_SET;
            mLinAccel  = WaypointValueType::cNOT_SET;
            mRadAccel  = WaypointValueType::cNOT_SET;
         }
         else
         {
            mAlt       = WaypointValueType::cUSE_PREVIOUS;
            mSpeed     = WaypointValueType::cUSE_PREVIOUS;
            mClimbRate = WaypointValueType::cUSE_PREVIOUS;
            mLinAccel  = WaypointValueType::cUSE_PREVIOUS;
            mRadAccel  = WaypointValueType::cUSE_PREVIOUS;
         }
      }

      enum WaypointType
      {
         cABSOLUTE,
         cRELATIVE,
         cOTHER
      };

      //! Represents the altitude reference
      //! @par details
      //!   Altitude reference determines whether or not to account for ground level in the altitude measurement.
      //!   Mean sea level (MSL) means that the altitude measurement starts from sea level, not ground level.
      //!   Above ground level (AGL) means that the altitude measurement starts from ground level, not sea level.
      //!   For example, the peak of Mount Everest would be 0 km AGL or 8.84886 km MSL.
      //! @par Default Altitude Reference
      //!   Default altitude reference for platforms is MSL.
      //!   Default altitude reference for WsfAirMover is MSL.
      //!   Default altitude reference for other WsfMovers is AGL.
      enum class AltitudeReference
      {
         UsePrevious = -2, //!< Denotes that the current waypoint uses the previously-defined altitude reference
         Unset,            //!< Denotes that the current waypoint has no altitude reference set
         MoverDefault,     //!< Denotes that the current waypoint uses the mover default altitude reference
         MSL,              //!< Denotes that the current waypoint has MSL altitude reference set explicitly
         AGL               //!< Denotes that the current waypoint has AGL altitude reference set explicitly
      };

      WaypointType        mPointType; // See WsfWaypoint for anonymous enum definition
      QString             mLabel;
      UtLatPos            mLat;
      int                 mXOffset;
      int                 mYOffset;
      double              mHeading;
      UtLonPos            mLon;
      UtLengthValue       mAlt;
      int                 mAltRef; // MOVER_DEFAULT = 0; MSL  = 1; AGL = 2;
      QString             mGoTo;
      UtSpeedValue        mSpeed;
      UtSpeedValue        mClimbRate;
      UtAccelerationValue mLinAccel;
      UtAccelerationValue mRadAccel;
      int                 mTurnDir;    // LEFT = -1 ; SHORTEST = 0; RIGHT = 1;
      int                 mSwitchType; // MOVER_DEFAULT = 0; PASSING = 1 ; APPROACH = 2;
   };

   struct RouteInfo
   {
      RouteInfo()
         : mPlatformName("")
         , mName("")
         , mEndPathType(0)
         , mUserDefined(true)
      {
      }

      QString             mPlatformName; // for global routes, this is an empty string
      QString             mName;         // for local routes, this is an empty string
      QList<WaypointInfo> mWaypoints;
      int                 mEndPathType; // MOVER_DEFAULT = 0 ; EXTRAPOPLATE = 1 ; STOP = 2 ; REMOVE = 3;
      bool                mUserDefined;

      bool operator<(const RouteInfo& aOther) const { return (QString::compare(mName, aOther.mName) < 0); }
      bool IsGlobal() const { return mPlatformName.isEmpty(); }
   };

   RouteBrowserInterface(QWidget* aParent, vespa::VaViewer* aViewer);

   virtual void Clear();
   void         Connect();

   RouteBrowserDockWidget* GetDockWidget() const { return mDockWidget; }
   int                     GetSelectedWaypoint() const { return mSelectedWaypoint; }
   int                     GetCurrentWaypoint() const;

   void PlatformOfInterestAddedHandler(const RouteInfo& aRouteInfo);
   void GlobalRouteCreatedHandler(const RouteInfo& aRouteInfo);
   void RouteEditedHandler(const RouteInfo& aRouteInfo);

signals:
   void DeleteRouteHandler(const QString& aRouteName);

protected:
   virtual void DeleteRoute(const QString& aRouteName);
   void         DrawRoute(const RouteInfo& aRouteInfo, wkf::AttachmentRoute* aRouteAttachmentPtr);
   virtual void GlobalRouteDeselected(wkf::AttachmentRoute* aRoutePtr);
   virtual void DockGlobalRouteSelectedHandler(const QString& aPrevRouteName,
                                               int            aPrevWaypoint,
                                               const QString& aCurrRouteName,
                                               int            aCurrWaypoint);
   virtual void DockRouteSelectionChangedHandler(const QString& aPrevPlatformName,
                                                 const QString& aPrevRouteName,
                                                 int            aPrevWaypoint,
                                                 const QString& aCurrPlatformName,
                                                 const QString& aCurrRouteName,
                                                 int            aCurrWaypoint);
   virtual void ConstructRoute(const RouteInfo& aRouteInfo, const std::string& aEntityName);
   virtual void PlatformDeselectedHandler(wkf::Platform* aPlatform);
   virtual void PlatformSelectionChanged(const PlatformList& aSelected, const PlatformList& aUnselected);

   void CreateDialogGlobalWaypointSelectedHandler(const QString& aPlatformName, int aWaypointIndex);
   void GlobalRouteEditedHandler(const RouteInfo& aRouteInfo);
   void CreateRouteStartedHandler(const QString& aRouteName);
   void CreateRouteTerminatedHandler(const QString& aRouteName);
   void WaypointAddedHandler(const WaypointInfo& aWaypoint,
                             int                 aInsertIndex,
                             const QString&      aPlatformName,
                             const QString&      aRouteName);
   void WaypointRemovedHandler(int aIndex, const QString& aPlatformName, const QString& aRouteName);


   // used with the edit route dialog to create "preview" routes on the map
   void CreateTemporaryRoute(const RouteInfo& aRouteInfo);
   void RemoveTemporaryRoute(const RouteInfo& aRouteInfo);
   void RemoveTemporaryRouteP(const std::string& aEntityName);
   void ShowExistingRoute(const RouteInfo& aRouteInfo);
   void HideExistingRoute(const RouteInfo& aRouteInfo);

   void DialogLocalWaypointSelectedHandler(const QString& aPlatformName, int aWaypointIndex);
   void DialogGlobalWaypointSelectedHandler(const QString& aPlatformName, int aWaypointIndex);

   RouteBrowserDockWidget* mDockWidget;
   vespa::VaViewer*        mViewer;
   vespa::VaCallbackHolder mCallbacks;

private:
   void         AttachmentSelected(vespa::VaViewer*     aViewer,
                                   int                  aSelect,
                                   vespa::VaAttachment* aAttachmentPtr,
                                   const vespa::VaAttachmentSubId&);
   virtual void PlatformsAdded(const wkf::PlatformList& aPlatforms);
   virtual void EntityRemoved(vespa::VaEntity* aEntityPtr);
   virtual void ScenarioAdded(wkf::Scenario* aScenarioPtr){};
   virtual void SelectRoute(const std::string& aPlatformName){};


   void ContextMenuRequestedHandler(QWidget*       aParent,
                                    const QPoint&  aPoint,
                                    const QString& aRoute,
                                    const QString& aPlatform,
                                    bool           aEditable,
                                    bool           aDeleteable);
   void AddPlatformOfInterest(unsigned int aPlatformIndex, int aWaypointIndex);
   void RemovePlatformOfInterest(unsigned int aPlatformIndex);
   void RemovePlatformOfInterestWaypoints(unsigned int aPlatformIndex);
   void RemovePlatformOfInterestWaypoint(unsigned int aPlatformIndex, int aWaypointIndex);

   bool IsSelected(unsigned int aPlatformIndex) const;
   bool IsPlatformOfInterest(unsigned int aPlatformIndex) const;
   int  GetWaypointCount(unsigned int aPlatformIndex) const;
   int  GetPOICount() const { return ut::cast_to_int(mPOIToWaypointsMap.size()); }

   void PlatformOfInterestRemovedHandler(const QString& aPlatformName);

   void LocalWaypointSelected(const QString& aPlatformName, unsigned int aPlatformIndex, int aWaypointIndex);
   void LocalWaypointDeselected(const QString& aPlatformName, unsigned int aPlatformIndex, int aWaypointIndex);
   void LocalRouteDeselected();

   void GlobalWaypointSelected(wkf::AttachmentRoute* aRoutePtr, int aWaypointIndex);
   void GlobalWaypointDeselected(wkf::AttachmentRoute* aRoutePtr, int aWaypointIndex);

   void CreateRouteOnEntityAnchor(vespa::VaEntity* aEntityPtr, const RouteInfo& aRouteInfo);

   void GlobalWaypointAddedHandler(const WaypointInfo& aWaypoint, int aInsertIndex, const QString& aRouteName);
   void LocalWaypointAddedHandler(const WaypointInfo& aWaypoint, int aInsertIndex, const QString& aPlatformName);

   void GlobalWaypointRemovedHandler(int aIndex, const QString& aRouteName);
   void LocalWaypointRemovedHandler(int aIndex, const QString& aPlatformName);

   void EditRoute();

   void TeamColorChanged();


   bool                      mEntityAnchorInitialized;
   int                       mSelectedWaypoint; // selected waypoint == -1 if there is not exactly one waypoint selected
   std::vector<unsigned int> mSelectedPlatforms;                                   // selected platforms
   std::unordered_map<unsigned int, std::vector<unsigned int>> mPOIToWaypointsMap; // Platform of Interest to selected waypoint map
   bool mOriginalVisibilityState; // the visibility state of the original route prior to the edit dialog being opened
};
// Vector of pairs < Index of the waypoint, Waypoint Information >
using WaypointInfoList = std::vector<std::pair<int, wkf::RouteBrowserInterface::WaypointInfo>>;
} // namespace wkf

Q_DECLARE_METATYPE(wkf::RouteBrowserInterface::RouteInfo)
Q_DECLARE_METATYPE(wkf::RouteBrowserInterface::WaypointInfo)

#endif
