// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017-2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef GEOMETRY_MASS_PROPERTIES_HPP
#define GEOMETRY_MASS_PROPERTIES_HPP

#include <string>

#include "GeometryObject.hpp"

// ****************************************************************************
// GeometryMassProperties is derived from GeometryObject and provides data
// for mass properties.
// ****************************************************************************
namespace Designer
{
   class VehicleGeometry;
   class VehiclePropulsion;
}

namespace Designer
{
   class VehicleGeometry;
   class VehiclePropulsion;

   class GeometryMassProperties : public GeometryObject
   {
      public:
         explicit GeometryMassProperties(Vehicle* aVehicle);
         virtual ~GeometryMassProperties();

         // This is called to load the widget data (GUI functionality)
         QTreeWidgetItem* CreateTreeWidgetItem(QString&     aName,
                                               QTreeWidget* aTreeWidgetPtr,
                                               bool         aNoneditableName) override;

         // This calculates the max/min extent of the object in all three directions (x,y,z)
         // Return true if extents are valid (some object do not support "extent").
         bool CalcExtents(double& xMin,
                          double& xMax,
                          double& yMin,
                          double& yMax,
                          double& zMin,
                          double& zMax) override;

         double GetEmptyMass_lbs() { return mEmptyMass_lbs; }
         double GetTotalMass_lbs() { return mTotalMass_lbs; }
         double GetIxx() { return mIxx; }
         double GetIyy() { return mIyy; }
         double GetIzz() { return mIzz; }
         double GetCgX() { return mCG_Location_ft.X(); }
         double GetCgY() { return mCG_Location_ft.Y(); }
         double GetCgZ() { return mCG_Location_ft.Z(); }
         UtVec3dX GetCg() { return mCG_Location_ft; }

         double GetFueledIxx() { return mFueledIxx; }
         double GetFueledIyy() { return mFueledIyy; }
         double GetFueledIzz() { return mFueledIzz; }
         double GetFueledCgX() { return mFueledCG_Location_ft.X(); }
         double GetFueledCgY() { return mFueledCG_Location_ft.Y(); }
         double GetFueledCgZ() { return mFueledCG_Location_ft.Z(); }
         UtVec3dX GetFueledCg() { return mFueledCG_Location_ft; }

         UtVec3dX GetOriginalCg() { return mOriginalCG_Location_ft; }
         void SetOriginalCg(const UtVec3dX& aCg_ft) { mOriginalCG_Location_ft = aCg_ft; }

         // This is called to calculate the mass properties for the entire vehicle. It loops through
         // all geometry objects
         void CalcMassAndInertiaProperties();

         // This is called to calculate a full set of mass properties, given the specified total mass.
         // The calculations include any symmetry effects and represent the combined mass properties for
         // the entire set of subobjects, if appropriate. This function sets mCalculatedMass_lbs and
         // then calculates mCalculatedCG_ft, mCalculatedIxx, mCalculatedIyy, and mCalculatedIzz.
         void CalcMassProperties(double aTotalMass_lbs) override;

         // This provides a full set of mass properties. It requires that CalcMassProperties() was called
         // prior to calling this function. The results are corrected for any symmetry effects and
         // represent the combined mass properties for the entire set of subobjects, if appropriate.
         // Returns true if the data is valid. Data should not be used if invalid.
         bool GetCalculatedMassProperties(UtVec3dX& aCgLocation_ft,
                                          double&   aMass_lbs,
                                          UtVec3dX& aRotationalInertia) override;

         // This performs an "audit" to check that a geometry object is fully
         // defined, identifying any issues that may exist. The function returns a
         // list of QStrings that detail any problems. If no problems are found,
         // the list will be empty.
         QStringList Audit() override;

         // This returns the fuselage/body mass (or zero)
         double GetFuselageBodyMass_lbs();

         // This returns the mass of non-fuselage/body pods (or zero)
         double GetPodsMass_lbs();

         // This returns the wing mass (or zero)
         double GetWingMass_lbs();

         // This returns the sum of all (non-wing) surfaces and fins (or zero)
         double GetSurfacesMass_lbs();

         // This returns the sum of all landing gear (or zero)
         double GetLandingGearMasses_lbs();

         // This returns the sum of all nacelles (or zero)
         double GetNacelleMasses_lbs();

         // This returns the sum of all dish objects (or zero)
         double GetDishMasses_lbs();

         // This returns the sum of all point masses (or zero)
         double GetPointMasses_lbs();

         // This returns the sum of all engines (or zero)
         double GetEngineMasses_lbs();

      private slots:
         void HandleMayNotBeDeletedCheckBoxChanged(int aValue);
         void HandleTreeItemClicked(QTreeWidgetItem* item, int column);
         void HandleEmptyMassLineEditChanged(QString aValue);
         void HandleTotalMassLineEditChanged(QString aValue);
         void HandleAutoCalcInertiaCheckBoxChanged(int aValue);
         void HandleIxxLineEditChanged(QString aValue);
         void HandleIyyLineEditChanged(QString aValue);
         void HandleIzzLineEditChanged(QString aValue);
         void HandleFuselageBodyMassMultiplierLineEditChanged(QString aValue);
         void HandlePodsMassMultiplierLineEditChanged(QString aValue);
         void HandleNacellesMassMultiplierLineEditChanged(QString aValue);
         void HandleDishesMassMultiplierLineEditChanged(QString aValue);
         void HandleSurfacesMassMultiplierLineEditChanged(QString aValue);
         void HandleWingMassMultiplierLineEditChanged(QString aValue);
         void HandleLandingGearMassMultiplierLineEditChanged(QString aValue);
         void HandleFuselageBodyMassLineEditChanged(QString aValue);
         void HandlePodsMassLineEditChanged(QString aValue);
         void HandleNacellesMassLineEditChanged(QString aValue);
         void HandleDishesMassLineEditChanged(QString aValue);
         void HandleSurfacesMassLineEditChanged(QString aValue);
         void HandleWingMassLineEditChanged(QString aValue);
         void HandleLandingGearMassLineEditChanged(QString aValue);
         void HandlePointMassesLineEditChanged(QString aValue);

      protected:
         double CalcObjectRelativeMass(double aMassArea_ft2,
                                       double aObjectectMassFactor,
                                       double aObjectCategoryMassMultiplier);
         double CalcFuselageBodyRelativeMass();
         double CalcPodsRelativeMass();
         double CalcNacellesRelativeMass();
         double CalcDishesRelativeMass();
         double CalcSurfacesRelativeMass();
         double CalcWingRelativeMass();
         double CalcLandingGearRelativeMass();
         double CalcFuelMass_lbs();
         double CalcPointMasses_lbs();

         double CalcEnginesMass_lbs();

         void CalcFuselageBodyMass(double aRealtiveMassMultiplier_lbs);
         void CalcPodsMass(double aRealtiveMassMultiplier_lbs);
         void CalcNacellesMass(double aRealtiveMassMultiplier_lbs);
         void CalcDishesMass(double aRealtiveMassMultiplier_lbs);
         void CalcSurfacesMass(double aRealtiveMassMultiplier_lbs);
         void CalcWingMass(double aRealtiveMassMultiplier_lbs);
         void CalcLandingGearMass(double aRealtiveMassMultiplier_lbs);
         void CalcPointMassesMass(double aRealtiveMassMultiplier_lbs);
         void CalcPropulsionMass(double aRealtiveMassMultiplier_lbs);

         void CalculateTotalMassProperties();

         double      mEmptyMass_lbs                  = 0.0;
         double      mTotalMass_lbs                  = 0.0;
         bool        mAutoCalcInertia                = true;

         // Empty conditions
         double      mIxx                            = 0.0;
         double      mIyy                            = 0.0;
         double      mIzz                            = 0.0;
         UtVec3dX    mCG_Location_ft;                // Reference position of CG

         // Fully fueled conditions
         double      mFueledIxx                      = 0.0;
         double      mFueledIyy                      = 0.0;
         double      mFueledIzz                      = 0.0;
         UtVec3dX    mFueledCG_Location_ft;          // Reference position of CG

         double      mFuselageBodyMassMultiplier     = 0.0;
         double      mPodsMassMultiplier             = 0.0;
         double      mNacellesMassMultiplier         = 0.0;
         double      mDishesMassMultiplier           = 0.0;
         double      mSurfacesMassMultiplier         = 0.0;
         double      mWingMassMultiplier             = 0.0;
         double      mLandingGearMassMultiplier      = 0.0;

         double      mFuselageBodyMass_lbs           = 0.0;
         double      mPodsMass_lbs                   = 0.0;
         double      mNacellesMass_lbs               = 0.0;
         double      mDishesMass_lbs                 = 0.0;
         double      mSurfacesMass_lbs               = 0.0;
         double      mWingMass_lbs                   = 0.0;
         double      mLandingGearMass_lbs            = 0.0;
         double      mPointMassesMass_lbs            = 0.0;

         double      mEnginesMass_lbs                = 0.0;
         UtVec3dX    mEnginesCG_Location_ft;
         double      mEnginesIxx                     = 0.0;
         double      mEnginesIyy                     = 0.0;
         double      mEnginesIzz                     = 0.0;

         UtVec3dX    mOriginalCG_Location_ft;

         QLineEdit*  mTotalMassLineEdit              = nullptr;
         QLineEdit*  mMassFuselageBodyMassLineEdit   = nullptr;
         QLineEdit*  mMassPodsMassLineEdit           = nullptr;
         QLineEdit*  mMassNacellesMassLineEdit       = nullptr;
         QLineEdit*  mMassDishesMassLineEdit         = nullptr;
         QLineEdit*  mMassSurfacesMassLineEdit       = nullptr;
         QLineEdit*  mMassWingMassLineEdit           = nullptr;
         QLineEdit*  mMassLandingGearMassLineEdit    = nullptr;
         QLineEdit*  mPointMassesLineEdit            = nullptr;
         QLineEdit*  mCGPointX                       = nullptr;
         QLineEdit*  mCGPointY                       = nullptr;
         QLineEdit*  mCGPointZ                       = nullptr;
         QLineEdit*  mInertiaIxxLineEdit             = nullptr;
         QLineEdit*  mInertiaIyyLineEdit             = nullptr;
         QLineEdit*  mInertiaIzzLineEdit             = nullptr;

         VehicleGeometry&   mVehicleGeometry;
         VehiclePropulsion* mVehiclePropulsionPtr    = nullptr;
   };
}
#endif // ! GEOMETRY_MASS_PROPERTIES_HPP
