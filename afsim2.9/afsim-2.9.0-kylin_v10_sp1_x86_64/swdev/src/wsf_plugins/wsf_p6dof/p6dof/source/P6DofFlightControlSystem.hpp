// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef P6DOFFLIGHTCONTROLSYSTEM_H
#define P6DOFFLIGHTCONTROLSYSTEM_H

#include "p6dof_export.h"

#include <cinttypes>
#include <string>
#include <vector>

#include "P6DofControlActuator.hpp"
#include "P6DofKinematicState.hpp"
#include "P6DofObject.hpp"
#include "P6DofVehicleData.hpp"
#include "UtCloneablePtr.hpp"
#include "UtTable.hpp"

class P6DofPilotManager;
class P6DofPilotObject;
class P6DofVehicle;
class UtInput;

// The P6DofFlightControlSystem class provides a means to transform control inputs into
// control surface movement. P6DofFlightControlSystem connects to a P6DofPilotController
// to get control inputs. It offers functions for P6DofObjects to get control surface
// positions. Control surfaces include typical aerodynamic control surfaces but can
// also include unconventional control surfaces as well as other components (such as
// landing gear) that can produce forces and moments that are a function of extended
// position of the component.

class P6DOF_EXPORT P6DofFlightControlSystem : public P6DofObject
{
public:
   explicit P6DofFlightControlSystem(P6DofScenario* aScenario);
   ~P6DofFlightControlSystem() override;

   P6DofFlightControlSystem& operator=(const P6DofFlightControlSystem& other) = delete;

   P6DofFlightControlSystem* Clone() const;

   void ConnectInputsWithOutputs();

   // ProcessInput reads initialization data
   bool ProcessInput(UtInput& aInput);
   bool Initialize(int64_t aSimTime_nanosec);

   void Update(int64_t aSimTime_nanosec, P6DofKinematicState& aState);

   void SetLastUpdateSimTime(int64_t aLastUpdateSimTime_nanosec)
   {
      mLastUpdateSimTime_nanosec = aLastUpdateSimTime_nanosec;
   }

   // This sets the pilot manager so the flight controls can get the active pilot object
   void SetPilotManager(P6DofPilotManager* aP6DofPilotManager);

   // This sets the pilot object, allowing the flight controls to get control inputs
   void SetPilotObject(P6DofPilotObject* aP6DofPilotObject);

   // ......................................................................................

   // This returns a 'handle' to a control surface. If there is no control surface
   // with the specified name, the function returns 0;
   size_t GetControlSurfaceHandle(const std::string& aControlSurfaceName) const;

   // This returns a 'handle' to the first control surface that contains the
   // specified string. If there is no control surface that contains the name,
   // the function returns 0;
   size_t GetControlSurfaceHandleContainingString(const std::string& aString) const;

   // This returns a 'handle' to the first control surface that contains the
   // two specified strings. If there is no control surface that contains the names,
   // the function returns 0;
   size_t GetControlSurfaceHandleContainingTwoStrings(const std::string& aString1, const std::string& aString2) const;

   // This returns the name of the control surface, given its handle
   std::string GetControlSurfaceName(size_t aControlSurfaceHandle) const;

   // This returns the angle of the control surface in degrees, given its handle
   double GetControlSurfaceAngle_deg(size_t aControlSurfaceHandle) const;

   // This returns the max angle of the control surface in degrees, given its handle
   double GetControlSurfaceMinAngle_deg(size_t aControlSurfaceHandle) const;

   // This returns the max angle of the control surface in degrees, given its handle
   double GetControlSurfaceMaxAngle_deg(size_t aControlSurfaceHandle) const;

   // This returns the normalized angle of the control surface, given its handle
   double GetControlSurfaceValueNormalized(size_t aControlSurfaceHandle) const;

   // This sets the angle of the control surface in degrees, given its handle.
   // Note that this should only be used for initialization, to set the position
   // to a desired angle. This will also set the actuator position.
   void SetControlSurfaceAngle_deg(size_t aControlSurfaceHandle, double aAngle_deg);

   // ......................................................................................

   // This returns a 'handle' to a control output value. If there is no control value
   // with the specified name, the function returns 0;
   size_t GetControlValueHandle(const std::string& aControlValueName) const;

   // This returns the name of the control value, given its handle
   std::string GetControlValueName(size_t aControlValueHandle) const;

   // This returns the value of the control value, given its handle
   double GetControlValue(size_t aControlValueHandle) const;

   // ......................................................................................

   // This returns a 'handle' to a boolean control. If there is no boolean control
   // with the specified name, the function returns 0;
   size_t GetBooleanControlHandle(const std::string& aControlBooleanName) const;

   // This returns the name of the boolean control, given its handle
   std::string GetBooleanControlName(size_t aControlBooleanHandle) const;

   // This returns the value of the boolean control, given its handle
   bool GetControlBoolean(size_t aControlBooleanHandle) const;

   // This returns the last value of the boolean control, given its handle
   bool GetControlBooleanLastValue(size_t aControlBooleanHandle) const;

   // ......................................................................................

   // This clears all control data
   void ClearAllControlData();

   // This returns the number of flight control inputs
   size_t NumFlightControlInputs() const;

   // This loads data for the specified control input
   void LoadFlightControlInputData(size_t aIndex, P6DOF::FlightControlInputValue* aData) const;

   // This returns the number of flight control surfaces
   size_t NumFlightControlSurfaces() const;

   // This loads data for the specified control surface
   void LoadFlightControlSurfaceData(size_t aIndex, P6DOF::ControlSurfaceValue* aData) const;

   // .....................................................................................................

   struct ControlInputValue
   {
      std::string             name;
      size_t                  handle;
      double                  normalizedValue;
      P6DOF::DataType::Simple typeData;
      bool                    boolData;
   };

   enum ControlSignalModifierType
   {
      Undefined,
      ScalarGain,
      ClampGain,
      SASChannel,
      MachGainTable,
      KTASGainTable,
      AlphaGainTable,
      BetaGainTable,
      GxLoadGainTable,
      GyLoadGainTable,
      GzLoadGainTable,
      AltGainTable,
      QGainTable,
      MachMappingTable,
      KTASMappingTable,
      AlphaMappingTable,
      BetaMappingTable,
      GxLoadMappingTable,
      GyLoadMappingTable,
      GzLoadMappingTable,
      AltMappingTable,
      QMappingTable,
      SignalMappingTable
   };

   struct ControlSignalModifier
   {
      ControlSignalModifier()
         : type(Undefined)
         , table(nullptr)
         , curve(nullptr)
         , scalarGain(1.0)
         , minClamp(-1.0)
         , maxClamp(1.0)
      {
      }
      std::string                    name;
      ControlSignalModifierType      type;
      UtCloneablePtr<UtTable::Table> table;
      UtCloneablePtr<UtTable::Curve> curve;
      double                         scalarGain;
      double                         minClamp;
      double                         maxClamp;
   };

   struct ControlInputGainStream
   {
      std::string                         controlInputName;
      ControlInputValue*                  controlInput; // This is owned externally
      std::vector<std::string>            modifierNameList;
      std::vector<ControlSignalModifier*> modifierList; // This is owned externally
   };

   // This contains data for a single control surface, including a list
   // of control inputs and modifiers as well as the angle mapping and actuator
   struct ControlSurfaceElement
   {
      std::string                          controlSurfaceName;
      std::vector<ControlInputGainStream*> controlInputList;
      UtTable::Curve*                      angleMappingAuto;
      UtTable::Curve*                      angleMappingManual;
      P6DofControlActuator*                actuator;
      double                               currentAngle_deg;
      double                               minAngle_deg;
      double                               maxAngle_deg;
      ~ControlSurfaceElement()
      {
         std::vector<ControlInputGainStream*>::iterator it;
         for (it = controlInputList.begin(); it != controlInputList.end(); ++it)
         {
            delete *it;
         }
         if (angleMappingAuto != nullptr)
         {
            delete angleMappingAuto;
         }
         if (angleMappingManual != nullptr)
         {
            delete angleMappingManual;
         }
         if (actuator != nullptr)
         {
            delete actuator;
         }
      }
   };

   // This contains data for a single control output, including a list
   // of control inputs and modifiers
   struct ControlValueElement
   {
      std::string                          controlValueName;
      std::vector<ControlInputGainStream*> controlInputList;
      double                               currentValue;
      double                               minValue;
      double                               maxValue;
      ~ControlValueElement()
      {
         std::vector<ControlInputGainStream*>::iterator it;
         for (it = controlInputList.begin(); it != controlInputList.end(); ++it)
         {
            delete *it;
         }
      }
   };

   // This contains data for a single boolean output, including a list
   // of control inputs and modifiers
   struct ControlBooleanElement
   {
      std::string                          controlValueName;
      std::vector<ControlInputGainStream*> controlInputList;
      double                               thresholdValue;
      bool                                 currentValue;
      bool                                 lastValue;
      ~ControlBooleanElement()
      {
         std::vector<ControlInputGainStream*>::iterator it;
         for (it = controlInputList.begin(); it != controlInputList.end(); ++it)
         {
            delete *it;
         }
      }
   };

   // .....................................................................................................

   // This provides a list/vector of control surfaces that use the specified input handle
   // If there are none, it returns false.
   bool GetControlSurfacesUsingInput(std::vector<ControlSurfaceElement*>& list, unsigned int aInputHandle);

   // This provides a list/vector of control surface names. If there are none, it returns an empty vector.
   void GetControlSurfaceNames(std::vector<std::string>& list);

   // This provides a vector of the current control surface angles and actuator angles. It is used with
   // the SetAllControlSurfaceAndActuatorsAngles function to get/save angles and later reset the angles.
   void GetAllControlSurfaceAndActuatorsAngles(std::vector<double>& aControlSurfaceAngles_deg,
                                               std::vector<double>& aActuatorAngles_deg) const;

   // This uses vectors of the current control surface angles and actuator angles obtained
   // from the function GetAllControlSurfaceAndActuatorsAngles function to set the angles.
   void SetAllControlSurfaceAndActuatorsAngles(const std::vector<double>& aControlSurfaceAngles_deg,
                                               const std::vector<double>& aActuatorAngles_deg);

   // This sets the control surface positions based on the control inputs
   void SetControlSurfacesBasedOnControlInputs(int64_t aSimTime_nanosec);

   // This sets the control values based on the control inputs
   void SetControlValuesBasedOnControlInputs();

   // This sets the control booleans based on the control inputs
   void SetControlBooleansBasedOnControlInputs();

   // .....................................................................................................

protected:
   P6DofFlightControlSystem(const P6DofFlightControlSystem& aSrc);

   // This processes file input for signal modifiers
   bool ProcessSignalModifierTable(UtInput& aInput);

   // This processes file input for control surfaces
   bool ProcessControlSurfaceInput(UtInput& aInput);

   // This processes file input for control values
   bool ProcessControlValueInput(UtInput& aInput);

   // This processes file input for control booleans
   bool ProcessControlBooleanInput(UtInput& aInput);

   bool ProcessInputSet(UtInput& aInput, ControlSurfaceElement* aActiveControlSurfaceElement);
   bool ProcessInputSet(UtInput& aInput, ControlValueElement* aActiveControlValueElement);
   bool ProcessInputSet(UtInput& aInput, ControlBooleanElement* aActiveControlBooleanElement);

   bool ProcessControlInputs(UtInput& aInput, ControlSurfaceElement* aActiveControlSurfaceElement);
   bool ProcessControlInputs(UtInput& aInput, ControlValueElement* aActiveControlValueElement);
   bool ProcessControlInputs(UtInput& aInput, ControlBooleanElement* aActiveControlBooleanElement);

   // This adds the control surface to the list if the name is unique.
   // If it cannot add it to the list, it returns false, else it returns true
   bool AddControlSurfaceToList(ControlSurfaceElement* aControlSurface);

   // This adds the control value to the list if the name is unique.
   // If it cannot add it to the list, it returns false, else it returns true
   bool AddControlValueToList(ControlValueElement* aControlValue);

   // This adds the control boolean to the list if the name is unique.
   // If it cannot add it to the list, it returns false, else it returns true
   bool AddControlBooleanToList(ControlBooleanElement* aControlBoolean);

   ControlInputValue*     GetControlInputByName(const std::string& aControlInputName) const;
   ControlSignalModifier* GetControlSignalModifierByName(const std::string& aSignalModifierName) const;

   P6DofPilotObject*  mP6DofPilotObjectPtr;
   P6DofPilotManager* mP6DofPilotManager;

   std::vector<ControlInputValue*>     mControlInputList;
   std::vector<ControlSignalModifier*> mControlSignalModifiersList;
   std::vector<ControlSurfaceElement*> mControlSurfaceList;
   std::vector<ControlValueElement*>   mControlValueList;
   std::vector<ControlBooleanElement*> mControlBooleanList;

   int64_t             mLastUpdateSimTime_nanosec;
   P6DofKinematicState mState;
};

#endif
