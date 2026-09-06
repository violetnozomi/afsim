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

#ifndef MAPCURSORUPDATER_HPP
#define MAPCURSORUPDATER_HPP

#include <QTimer>

#include "UtCalendar.hpp"
#include "VaCallbackHolder.hpp"
#include "VaPosition.hpp"
#include "VaViewer.hpp"
#include "WkfUnitsObject.hpp"
#include "WkfUpdater.hpp"

namespace Map
{
class CursorUpdater : public wkf::Updater
{
public:
   CursorUpdater(const QString& aName, const vespa::VaViewer& aViewer);
   ~CursorUpdater() override                                                    = default;
   virtual void ReadData(vespa::VaViewer* aViewerPtr, int aMouseX, int aMouseY) = 0;

protected:
   vespa::VaCallbackHolder mCallbacks;
   const vespa::VaViewer&  mViewer;
};

class PositionUpdater : public CursorUpdater
{
public:
   PositionUpdater(const vespa::VaViewer& aViewer)
      : CursorUpdater("Position", aViewer)
   {
   }

   ~PositionUpdater() override = default;
   QString       GetValueString() const override;
   const QString GetDisplayName() const override { return "Position"; }
   void          ReadData(vespa::VaViewer* aViewerPtr, int aMouseX, int aMouseY) override;

private:
   UtLatPos mLat{0.0};
   UtLonPos mLon{0.0};
};

class ElevationUpdater : public CursorUpdater
{
public:
   ElevationUpdater(const vespa::VaViewer& aViewer, size_t aDtedId);
   ~ElevationUpdater() override = default;
   QString       GetValueString() const override;
   const QString GetDisplayName() const override { return "Elevation"; }
   void          ReadData(vespa::VaViewer* aViewerPtr, int aMouseX, int aMouseY) override;

private:
   float  mElevation{0.0};
   bool   mElevationExists{false};
   size_t mDtedId;
   int    mMouseX;
   int    mMouseY;
   QTimer mTimer;
};

class BullseyeUpdater : public CursorUpdater
{
public:
   BullseyeUpdater(const QString& aName, const vespa::VaViewer& aViewer)
      : CursorUpdater(aName, aViewer)
   {
   }

   ~BullseyeUpdater() override = default;
   void ReadData(vespa::VaViewer* aViewerPtr, int aMouseX, int aMouseY) override;

protected:
   double      mBearing{0.0};
   double      mRange{0.0};
   std::string mNearestBullseye;
   bool        mBullseyeExists{false};
};

class BullseyeBearingUpdater : public virtual BullseyeUpdater
{
public:
   BullseyeBearingUpdater(const vespa::VaViewer& aViewer)
      : BullseyeUpdater("Bearing (Bullseye)", aViewer)
   {
   }

   ~BullseyeBearingUpdater() override = default;
   QString       GetValueString() const override;
   const QString GetDisplayName() const override { return "Bearing (Bullseye)"; }
};


class BullseyeRangeUpdater : public virtual BullseyeUpdater
{
public:
   BullseyeRangeUpdater(const vespa::VaViewer& aViewer)
      : BullseyeUpdater("Range (Bullseye)", aViewer)
   {
   }

   ~BullseyeRangeUpdater() override = default;
   QString       GetValueString() const override;
   const QString GetDisplayName() const override { return "Range (Bullseye)"; }
};

class BullseyeNearestUpdater : public virtual BullseyeUpdater
{
public:
   BullseyeNearestUpdater(const vespa::VaViewer& aViewer)
      : BullseyeUpdater("Nearest Bullseye", aViewer)
   {
   }

   ~BullseyeNearestUpdater() override = default;
   QString       GetValueString() const override;
   const QString GetDisplayName() const override { return "Nearest Bullseye"; }
};
} // namespace Map

#endif
