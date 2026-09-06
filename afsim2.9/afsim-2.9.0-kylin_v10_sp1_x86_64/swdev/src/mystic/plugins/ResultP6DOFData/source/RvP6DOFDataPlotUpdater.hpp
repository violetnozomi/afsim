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
#ifndef RVP6DOFDATAPLOTUPDATER_HPP
#define RVP6DOFDATAPLOTUPDATER_HPP

#include <vector>

#include <QStringList>

#include "RvPlotUpdater.hpp"

namespace rv
{
class MsgP6dofCoreData;
class MsgP6dofKinematic;
class MsgP6dofEngineFuel;
class MsgP6dofAutopilot;
class MsgP6dofAutopilotLimits;
class MsgP6dofControlInputs;
class MsgP6dofControlSurfaces;
class MsgP6dofForceMoment;
} // namespace rv
namespace wkf
{
class PlatformGrouping;
}

namespace RvP6DOFData
{
/** Enum to help in using data from the right message. */
enum class MsgType
{
   eMsgP6dofCoreData,
   eMsgP6dofKinematic,
   eMsgP6dofEngineFuel,
   eMsgP6dofAutopilot,
   eMsgP6dofAutopilotLimits,
   eMsgP6dofControlInputs,
   eMsgP6dofControlSurfaces,
   eMsgP6dofForceMoment
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

   /** Get P6dof Core Data.
    * @param aDesc description/label for variable
    * @param aState Msg to read data from
    */
   double GetCoreData(const QString& aDesc, const rv::MsgP6dofCoreData* aState) const;

   /** Get P6dof Kinematic Data.
    * @param aDesc description/label for variable
    * @param aState Msg to read data from
    */
   double GetKinematicData(const QString& aDesc, const rv::MsgP6dofKinematic* aState) const;

   /** Get P6dof Engine and Fuel Data.
    * @param aDesc description/label for variable
    * @param aState Msg to read data from
    */
   double GetEngineFuelData(const QString& aDesc, const rv::MsgP6dofEngineFuel* aState) const;

   /** Get P6dof Autopilot Data.
    * @param aDesc description/label for variable
    * @param aState Msg to read data from
    */
   double GetAutopilotData(const QString& aDesc, const rv::MsgP6dofAutopilot* aState) const;

   /** Get P6dof Autopilot Limits Data.
    * @param aDesc description/label for variable
    * @param aState Msg to read data from
    */
   double GetControlInputsData(const QString& aDesc, const rv::MsgP6dofControlInputs* aState) const;

   /** Get P6dof Control Surfaces Data.
    * @param aDesc description/label for variable
    * @param aIndex index of the control surface in the list
    * @param aState Msg to read data from
    */
   double GetControlSurfacesData(const size_t aIndex, const rv::MsgP6dofControlSurfaces* aState) const;

   /** Get P6dof Force Moment Data.
    * @param aDesc description/label for variable
    * @param aState Msg to read data from
    */
   double GetForceMomentData(const QString& aDesc, const rv::MsgP6dofForceMoment* aState) const;

   /** Get the message type of a string, so we know what type of message to load.
    * @param aDesc description/label for variable
    */
   MsgType GetMessageType(const QString& aDesc) const;

   QStringList mInitialSelection;
   QString     mInitialXAxis;
   QString     mInitialYAxis;
   QStringList mControlSurfacesList;
};
} // namespace RvP6DOFData

#endif
