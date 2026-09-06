// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef GEOMETRY_ENGINE_HPP
#define GEOMETRY_ENGINE_HPP

#include <QComboBox>
#include <QLineEdit>
#include <QTreeWidgetItem>
#include <string>

#include "CreateEngineDialog.hpp"
#include "EditEngineDialog.hpp"
#include "GeometryObject.hpp"
#include "UtMath.hpp"

namespace Designer
{
   class GeometryEngine : public GeometryObject
   {
      public:

         enum class EngineType
         {
            cJET,
            cSOLIDROCKET,
            cLIQUIDROCKET,
            cRAMJET
         };

         explicit GeometryEngine(Vehicle* aVehicle);
         virtual ~GeometryEngine();

         // This moves the reference point by the specified amount
         void MoveRefPoint(UtVec3dX aMoveDelta_ft) override;

         // This is overridable, but not typically
         UtVec3dX         GetRefPoint_ft() override { return mRefPoint_ft; }

         bool             GetCG_ft(UtVec3dX& aCg_ft) override;

         // Get the installation yaw angle in degrees
         double           GetYaw_deg() { return UtMath::cDEG_PER_RAD * mYaw_rad; }

         // Get the installation pitch angle in degrees
         double           GetPitch_deg() { return UtMath::cDEG_PER_RAD * mPitch_rad; }

         // Get the installation roll angle in degrees
         double           GetRoll_deg() { return UtMath::cDEG_PER_RAD * mRoll_rad; }

         // Get the engine length in feet
         double           GetLength_ft() { return mLength_ft; }

         // Get the engine diameter in feet
         double           GetDiameter_ft() { return mDiameter_ft; }

         double           GetMass_lbs() { return mMass_lbs; }

         // Get the thrust offset in feet
         double           GetThrustOffset_ft() { return mThrustOffset_ft; }

         // Get the latch fuel injection boolean
         bool             GetLatchFuelInjection() { return mLatchFuelInjection; }
         bool             UseProportionalThrottle() { return mUseProportionalThrottle; }
         double           GetMinProportionalThrottle() { return mMinProportionalThrottle; }

         bool             IsSymmetrical() { return mSymmetrical; }

         double           GetSymmetryHorizontalY() { return mSymmetryHorizontalY; }

         EngineType       GetEngineType() { return mEngineType; }
         std::string      GetEngineTypeString();
         std::string      GetEngineModel() { return mEngineModel; }

         // This returns the propellant mass (lbs) or zero if no propellant is present
         double           GetPropellantMass_lbs() { return mPropellantMass_lbs; }

         // This returns the empty mass (lbs)
         double           GetEmptyMass_lbs() { return mMass_lbs - mPropellantMass_lbs; }

         double           GetIgnitionDelay_sec() { return mIgnitionDelay_sec; }

         void             CalcMassProperties(double) override; // Although a derived function, it ignores aTotalMass_lbs

         // This provides a full set of mass properties for the propellant.
         // It requires that CalcMassProperties() was called prior to calling this function.
         // The results are corrected for any symmetry effects and represent the combined
         // mass properties for the entire set of subobjects, if appropriate.
         // Returns true if the data is valid. Data should not be used if invalid.
         // This function is for use by solid propellant rockets.
         virtual bool GetCalculatedPropellantMassProperties(UtVec3dX& aCgLocation_ft,
                                                            double&   aMass_lbs,
                                                            UtVec3dX& aRotationalInertia);

         // This is called to load the widget data (GUI functionality)
         QTreeWidgetItem* CreateTreeWidgetItem(QString&     aType,
                                               QString&     aName,
                                               QTreeWidget* aTreeWidgetPtr,
                                               bool         aNoneditableName);

      protected:

         // This should be called to adjust the viewing distance after an engine is modified
         void AdjustViewingDistance() override;

      private:

         void PopulateEngineTypes();

         void HandleYawLineEditChanged(QString aValue);
         void HandlePitchLineEditChanged(QString aValue);
         void HandleRollLineEditChanged(QString aValue);
         void HandleEngineTypeChanged(QString aVal);
         void HandleEngineModelChanged(QString aVal);
         void HandleSymmetricalCheckBoxChanged(int aValue);
         void HandleLatchFuelInjectionCheckBoxChanged(int aValue);
         void HandleUseProportionalThrottleCheckBoxChanged(int aValue);
         void HandleMinProportionalThrottleLineEditChanged(double aValue);
         void HandleLengthLineEditChanged(QString aValue);
         void HandleDiameterLineEditChanged(QString aValue);
         void HandleThrustOffsetLineEditChanged(QString aValue);
         void HandleMassLineEditChanged(QString aValue);
         void HandlepropellantMassLineEditChanged(QString aValue);
         void HandleIgnitionDelayLineEditChanged(QString aValue);

         void HandleTreeItemClicked(QTreeWidgetItem* item, int column);

         void EditEngineModelClicked();
         void CreateEngineModelClicked();

         void HandleEngineCreated(QString aModelName);

         bool                                mSymmetrical                  = false;
         bool                                mLatchFuelInjection           = false;
         bool                                mUseProportionalThrottle      = false;
         double                              mMinProportionalThrottle      = 0.0;

         EngineType                          mEngineType                   = EngineType::cJET; // Default is Jet
         std::string                         mEngineModel;

         double                              mLength_ft                    = 0.0;
         double                              mDiameter_ft                  = 0.0;
         double                              mThrustOffset_ft              = 0.0;
         double                              mMass_lbs                     = 0.0;  // Mass of a single engine (regardless of symmetry)
         double                              mPropellantMass_lbs           = 0.0;  // Mass of integral propellant
         double                              mYaw_rad                      = 0.0;
         double                              mPitch_rad                    = 0.0;
         double                              mRoll_rad                     = 0.0;
         double                              mIgnitionDelay_sec            = 0.0;
         UtDCM                               mDCM;

         std::unique_ptr<EditEngineDialog>   mEditEngineDialog             = nullptr;
         std::unique_ptr<CreateEngineDialog> mCreateEngineDialog           = nullptr;

         QTreeWidgetItem*                    mEngineTypeItem               = nullptr;
         QTreeWidgetItem*                    mLatchFuelInjectionItem       = nullptr;
         QTreeWidgetItem*                    mUseProportionalThrottleItem  = nullptr;
         QTreeWidgetItem*                    mMinProportionalThrottleItem  = nullptr;

         QComboBox*                          mEngineModelComboBox          = nullptr;
         QComboBox*                          mEngineTypeComboBox           = nullptr;
         QLineEdit*                          mLengthLineEditPtr            = nullptr;
         QLineEdit*                          mDiameterLineEditPtr          = nullptr;
         QLineEdit*                          mThrustOffsetLineEditPtr      = nullptr;
         QLineEdit*                          mMassLineEditPtr              = nullptr;
         QLineEdit*                          mIgnitionDelayEditPtr         = nullptr;
         QComboBox*                          mEngineTypeComboBoxPtr        = nullptr;
         QComboBox*                          mEngineModelComboBoxPtr       = nullptr;
         double                              mSymmetryHorizontalY          = 0.0;

         UtVec3dX                            mCalculatedPropellantCG_ft;
         double                              mCalculatedPropellantIxx      = 0.0;
         double                              mCalculatedPropellantIyy      = 0.0;
         double                              mCalculatedPropellantIzz      = 0.0;
   };
}

#endif
