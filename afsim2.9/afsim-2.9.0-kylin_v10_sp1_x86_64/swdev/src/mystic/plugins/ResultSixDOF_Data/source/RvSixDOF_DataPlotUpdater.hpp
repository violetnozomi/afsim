// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef RVSIXDOFDATAPLOTUPDATER_HPP
#define RVSIXDOFDATAPLOTUPDATER_HPP

#include <vector>

#include <QStringList>

#include "RvPlotUpdater.hpp"

namespace rv
{
class MsgSixDOF_CoreData;
class MsgSixDOF_Kinematic;
class MsgSixDOF_EngineFuel;
class MsgSixDOF_Autopilot;
class MsgSixDOF_AutopilotLimits;
class MsgSixDOF_ControlInputs;
class MsgSixDOF_ControlSurfaces;
class MsgSixDOF_ForceMoment;
class MsgSixDOF_ForceRotation;
} // namespace rv
namespace wkf
{
class PlatformGrouping;
}

namespace RvSixDOF_Data
{
/** Enum to help in using data from the right message. */
enum class MsgType
{
   eMsgSixDOF_CoreData,
   eMsgSixDOF_Kinematic,
   eMsgSixDOF_EngineFuel,
   eMsgSixDOF_Autopilot,
   eMsgSixDOF_AutopilotLimits,
   eMsgSixDOF_ControlInputs,
   eMsgSixDOF_ControlSurfaces,
   eMsgSixDOF_ForceMoment
};

class PlotUpdater : public rv::PlotUpdater
{
public:
   PlotUpdater(const QString& aX, const QString& aY, const wkf::PlatformGrouping& aGroup);
   ~PlotUpdater() override = default;

   /** Get the list of platforms. */
   QStringList GetPlatforms() const override;

   /** Get the axes for X. */
   QStringList GetAxesX() const override { return GetAxes(); }

   /** Get the axes for Y. */
   QStringList GetAxesY() const override { return GetAxes(); }

   /** Get the units for the X axis. */
   QString GetUnitsX(const QString& aAxis) const override { return GetUnits(aAxis); }

   /** Get the units for the X axis. */
   QString GetUnitsY(const QString& aAxis) const override { return GetUnits(aAxis); }

   /** Get the initial selection for the plot. */
   const QStringList& GetInitialSelection() const override;

   /** Get the initial X axis. */
   const QString& GetInitialXAxis() const override;

   /** Get the initial Y axis */
   const QString& GetInitialYAxis() const override;

   /** Get a series(plot) of X vs. Y.
    * @param aItem platform name
    * @param aXAxis name of the x axis to plot
    * @param aYAxis name of the y axis to plot
    */
   std::vector<std::pair<double, double>> GetSeries(const QString& aItem,
                                                    const QString& aXAxis,
                                                    const QString& aYAxis) const override;

private:
   /** Helper function to get the available axes. */
   QStringList GetAxes() const;

   /** Helper function to get the units. */
   QString GetUnits(const QString& aAxis) const;

   /** Get SixDOF Core Data.
    * @param aDesc description/label for variable
    * @param aState Msg to read data from
    */
   double GetCoreData(const QString& aDesc, const rv::MsgSixDOF_CoreData* aState) const;

   /** Get SixDOF Kinematic Data.
    * @param aDesc description/label for variable
    * @param aState Msg to read data from
    */
   double GetKinematicData(const QString& aDesc, const rv::MsgSixDOF_Kinematic* aState) const;

   /** Get SixDOF Engine and Fuel Data.
    * @param aDesc description/label for variable
    * @param aState Msg to read data from
    */
   double GetEngineFuelData(const QString& aDesc, const rv::MsgSixDOF_EngineFuel* aState) const;

   /** Get SixDOF Autopilot Data.
    * @param aDesc description/label for variable
    * @param aState Msg to read data from
    */
   double GetAutopilotData(const QString& aDesc, const rv::MsgSixDOF_Autopilot* aState) const;

   /** Get SixDOF Autopilot Limits Data.
    * @param aDesc description/label for variable
    * @param aState Msg to read data from
    */
   double GetControlInputsData(const QString& aDesc, const rv::MsgSixDOF_ControlInputs* aState) const;

   /** Get SixDOF Control Surfaces Data.
    * @param aDesc description/label for variable
    * @param aIndex index of the control surface in the list
    * @param aState Msg to read data from
    */
   double GetControlSurfacesData(const size_t aIndex, const rv::MsgSixDOF_ControlSurfaces* aState) const;

   /** Get Rigid Body 6-DOF Force & Moment Data.
    * @param aDesc description/label for variable
    * @param aState Msg to read data from
    */
   double GetForceMomentData(const QString& aDesc, const rv::MsgSixDOF_ForceMoment* aState) const;

   /** Get the message type of a string, so we know what type of message to load.
    * @param aDesc description/label for variable
    */
   MsgType GetMessageType(const QString& aDesc) const;

   QStringList mInitialSelection;
   QString     mInitialXAxis;
   QString     mInitialYAxis;
   QStringList mControlSurfacesList;
};
} // namespace RvSixDOF_Data

#endif
