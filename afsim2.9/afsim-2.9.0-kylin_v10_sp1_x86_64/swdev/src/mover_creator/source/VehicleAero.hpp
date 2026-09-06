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

#ifndef VEHICLE_AERO_HPP
#define VEHICLE_AERO_HPP

#include <map>
#include <QProgressDialog>
#include <QString>
#include <string>

#include "AeroObject.hpp"
#include "AeroTable2d.hpp"
#include "AeroTable3d.hpp"
#include "AeroTable4d.hpp"
#include "AeroWing.hpp"
#include "P6DofForceAndMomentsObject.hpp"
#include "P6DofMassProperties.hpp"
#include "UtAtmosphere.hpp"
#include "UtDCM.hpp"
#include "UtMemory.hpp"
#include "UtVec3dX.hpp"
#include "VehicleAeroCore.hpp"
#include "VehicleAeroMovable.hpp"

class QTextStream;

// VehicleAero is used to generate AFSIM P6DOF data files. It contains one or more AeroObjects
// that are used to calculate the vehicle's AFSIM P6DOF aerodynamics data files.

namespace Designer
{
   class Vehicle;

   class VehicleAero
   {
      public:

         enum eTableKey
         {
            cCL_ALPHA_MACH,
            cCD_ALPHA_MACH,
            cCM_ALPHA_MACH,
            cCD_BETA_MACH,
            cCl_BETA_MACH,
            cCN_BETA_MACH,
            cCY_BETA_MACH
         };

         explicit VehicleAero(Vehicle* aParentVehicle, UtAtmosphere aAtmosphere);
         virtual ~VehicleAero() = default;

         // Returns true if the specified name is not currently found
         // in the list of aero objects.
         bool AeroObjectNameIsUnique(const std::string& aName);

         // Returns a name (composed of the base name) that is unique in the list
         // of aero objects. If the base name is unique, it will be returned.
         std::string MakeNameUnique(const std::string& aBaseName);

         // This will enable/disable the appropriate tables corresponding to the mover type
         void EnableDisableTables(bool aGenerateP6DOFMover, bool aGenerateGuidedMover, bool aGeneratePointMassMover);

         // This will calculate the aerodynamics for the full vehicle, including
         // both base aero (non-controls) and controls aero
         void CalculateFullVehicleAerodynamics();

         // This sets the vehicle alpha/beta in degrees. Note that a positive yaw
         // (to the right) generates a negative beta angle
         void SetVehicleAlphaBeta(double aAlpha_deg, double aBeta_deg);

         // This sets the vehicle yaw, pitch, and roll rates in degrees/sec.
         void SetVehicleYawPitchRollRates(double aYawRate_dps,
                                          double aPitchRate_dps,
                                          double aRollRate_dps);

         // This sets the vehicle parameters pertaining to speed
         void SetVehicleSpeedParameters(double aSpeed_fps,
                                        double aMach,
                                        double aKTAS,
                                        double aKCAS,
                                        double aDynamicPressure_psf,
                                        double aAirDensity_slugs_ft3);

         // This sets the vehicle altitude
         void SetVehicleAltitude_ft(double aVehicleAltitude_ft) { mVehicleAltitude_ft = aVehicleAltitude_ft; }

         // This returns the current vehicle altitude
         double GetVehicleAltitude_ft() { return mVehicleAltitude_ft; }

         // This sets the Mach used in testing
         void SetTestingMach(double aMach);

         // This sets the maximum Mach for data files
         void SetAeroDataMaxMach(double aMach);

         // This sets the maximum beta (deg) for data files
         void SetAeroDataMaxBeta(double aBeta_deg);

         // This sets the minimum/maximum alpha (deg) for data files
         void SetAeroDataAlphas(double aAlphaMin_deg, double aAlphaMax_deg);

         // This sets the alpha/beta symmetry boolean. 
         // If this is true, only positive alpha/beta values will be used in aero calculations.
         // The negative half of the data will be inferred from the positive half.
         void SetAeroAlphaBetaSymmetry(bool aAlphaSymmetry, bool aBetaSymmetry);

         // This sets the vehicle wing area
         void SetVehicleWingArea_ft2(double aVehicleWingArea_ft2) { mVehicleWingArea_ft2 = aVehicleWingArea_ft2; }

         // This returns the vehicle wing area
         double GetVehicleWingArea_ft2() { return mVehicleWingArea_ft2; }

         // This sets the vehicle wing span
         void SetVehicleWingSpan_ft(double aVehicleWingSpan_ft) { mVehicleWingSpan_ft = aVehicleWingSpan_ft; }

         // This returns the vehicle wing span
         double GetVehicleWingSpan_ft() { return mVehicleWingSpan_ft; }

         // This sets the vehicle wing chord
         void SetVehicleWingChord_ft(double aVehicleWingChord_ft) { mVehicleWingChord_ft = aVehicleWingChord_ft; }

         // This returns the vehicle wing chord
         double GetVehicleWingChord_ft() { return mVehicleWingChord_ft; }

         // This sets the vehicle reference area (used in place of wing area, wing span, and wing chord)
         void SetVehicleRefArea_ft2(double aVehicleRefArea_ft2) { mVehicleRefArea_ft2 = aVehicleRefArea_ft2; }

         // This returns the vehicle reference area (used in place of wing area, wing span, and wing chord)
         double GetVehicleRefArea_ft2() { return mVehicleRefArea_ft2; }

         bool   UseRefArea() const { return mUseRefAreaInsteadOfWingArea; }

         double GetAspectRatio() const { return mAspectRatio; }

         void   SetCLMax(double aCLMax) { mCLMax = aCLMax; }
         double GetCLMax() const { return mCLMax; }

         // This should be called after calling SetVehicleAlphaBeta, SetVehicleYawPitchRollRates,
         // and SetVehicleSpeedParameters. It updates the main aero calculation vectors.
         void CalculateAeroAttitudeVectors();

         // Add a new aero object to the list
         size_t AddAeroObject(AeroObject* aAeroObject);

         // Returns the map of all aero objects
         const std::map<size_t, AeroObject*>& GetAeroObjectMap() { return mAeroObjMap; }

         // This returns a list/vector of aero objects that are of the type specified
         const std::vector<AeroObject*> GetAeroObjectBasedOnType(const std::string& aAeroTypeString);

         // This returns a list/vector of aero objects that are generated from the geometry object
         // with the specified name
         const std::vector<AeroObject*> GetAeroObjectsGeneratedFrom(const std::string& aGeometryObjectName);

         void ResetAeroObjMap() { mAeroObjMap.clear(); }

         Vehicle* GetParentVehicle() { return mParentVehicle; }

         VehicleAeroCore& GetCoreAero() { return mCoreAero; }

         // Returns true if the specified name is not in the list of movable aero objects
         bool MovableAeroNameIsUnique(std::string& aName);

         // Adds a new movable aero object to the list, using the specified name and object
         void AddMovableAero(AeroObject* aAeroObject);

         // Returns a pointer to the movable aero with the specified name.
         // Returns null if no movable aero object exists.
         VehicleAeroMovable* GetMovableAero(const std::string& aName);

         // Returns a reference to the movable aero list
         //std::vector<std::pair<std::string, VehicleAeroMovable>>& GetMovableAeroList();
         std::vector<VehicleAeroMovable>& GetMovableAeroList();

         // This generates the movable aero list. It should be called after all other aero components
         // have been generated/created.
         void GenerateMovableAeroList();

         // This sets the aero multipliers, based on vehicle type
         void SetAeroMultipliers();
         
         void Populate2D_Table(AeroTable2d&        aTable,
                               double              aReferenceMach,
                               double              aReferenceValue,
                               std::vector<double> aOutputMachs);

         QStringList Audit();

         void CalculateVehicleAspectRatio();

      private:

         struct AeroAnalysisData
         {
            void ZeroOutParameters()
            {
               mAeroCenterVec_ft.Set(0.0, 0.0, 0.0);
               mRefArea_sqft = 0.0;
               mLiftVecBody_lbs.Set(0.0, 0.0, 0.0);
               mDragVecBody_lbs.Set(0.0, 0.0, 0.0);
               mSideVecBody_lbs.Set(0.0, 0.0, 0.0);
               mMomentBody_ftlbs.Set(0.0, 0.0, 0.0);
               mLiftPrimary_lbs = 0.0;
               mDragPrimary_lbs = 0.0;
               mSidePrimary_lbs = 0.0;
               mTotalMoment_ftlbs.Set(0.0, 0.0, 0.0);
               mFractionalLift = 0.0;
               mFractionalDrag = 0.0;
            }

            // Conditions

            double mMach          = 0.0;
            double mSpeed_fps     = 0.0;
            double mKTAS          = 0.0;
            double mKCAS          = 0.0;
            double mAlt_ft        = 0.0;
            double mQ_psf         = 0.0;
            double mRho_slugs_ft3 = 0.0;
            double mAlpha_deg     = 0.0;
            double mBeta_deg      = 0.0;
            double mYawRate_dps   = 0.0;
            double mPitchRate_dps = 0.0;
            double mRollRate_dps  = 0.0;

            // Parameters

            std::string mComponentName;

            double mRefArea_sqft    = 0.0;
            double mLiftPrimary_lbs = 0.0;
            double mDragPrimary_lbs = 0.0;
            double mSidePrimary_lbs = 0.0;
            double mFractionalLift  = 0.0;
            double mFractionalDrag  = 0.0;

            UtVec3dX mAeroCenterVec_ft;
            UtVec3dX mLiftVecBody_lbs;
            UtVec3dX mDragVecBody_lbs;
            UtVec3dX mSideVecBody_lbs;
            UtVec3dX mMomentBody_ftlbs;
            UtVec3dX mTotalMoment_ftlbs;
         };

         void EnableDisableP6DOFTables(bool aEnableP6DOF);
         void EnableDisablePointMassTables(bool aEnablePM6DOF);
         void EnableDisableGMTables(bool aEnableGM);

         // This will calculate the core/base aerodynamics for the full p6dof vehicle
         void CalculateP6DOFVehicleCoreAerodynamics(QProgressDialog& aProgressDialog);

         // This will calculate the core/base aerodynamics for the full pm6dof vehicle
         void CalculatePM6DOFVehicleCoreAerodynamics(QProgressDialog& aProgressDialog);

         // This will calculate the movable aerodynamics for the full p6dof vehicle,
         // including all movable components
         void CalculateP6DOFVehicleAllMovableAerodynamics(QProgressDialog& aProgressDialog);

         // This will calculate the movable aerodynamics for a particular p6dof movable component
         void CalculateP6DOFComponentMovableAerodynamics(VehicleAeroMovable& aAero, QProgressDialog& aProgressDialog);

         // This will calculate the core/base aerodynamics for the full p6dof vehicle
         void CalculateGMVehicleCoreAerodynamics(QProgressDialog& aProgressDialog);


         // This is used to calculate the aero data for a given set of conditions and is
         // used within CalculateFullVehicleAerodynamics() to loop over condition sets

         void CalcAeroData(double    aAltitude_ft,
                           double    aSpeed_fps,
                           double    aMach,
                           double    aKTAS,
                           double    aKCAS,
                           double    aDynamicPressure_psf,
                           double    aDensity_slugs_ft3,
                           double    aVehicleAlpha_deg,
                           double    aVehicleBeta_deg,
                           double    aVehicleYawRate_dps,
                           double    aVehiclePitchRate_dps,
                           double    aVehicleRollRate_dps,
                           double&   aTotalLift_lbs,
                           double&   aTotalDrag_lbs,
                           double&   aTotalSide_lbs,
                           UtVec3dX& aTotalMomentVector_ftlbs);

         void CalcForcesAndMomentsOnSubobjects(double                         aAltitude_ft,
                                               double                         aSpeed_fps,
                                               double                         aMach,
                                               double                         aKTAS,
                                               double                         aKCAS,
                                               double                         aDynamicPressure_psf,
                                               double                         aDensity_slugs_ft3,
                                               double                         aVehicleAlpha_deg,
                                               double                         aVehicleBeta_deg,
                                               double                         aVehicleYawRate_dps,
                                               double                         aVehiclePitchRate_dps,
                                               double                         aVehicleRollRate_dps,
                                               double&                        aTotalLift_lbs,
                                               double&                        aTotalDrag_lbs,
                                               double&                        aTotalSide_lbs,
                                               UtVec3dX&                      aMomentVector_ftlbs);

         void CalcAeroCoefficients(double    aAltitude_ft,
                                   double    aSpeed_fps,
                                   double    aMach,
                                   double    aKTAS,
                                   double    aKCAS,
                                   double    aDynamicPressure_psf,
                                   double    aDensity_slugs_ft3,
                                   double    aVehicleAlpha_deg,
                                   double    aVehicleBeta_deg,
                                   double    aVehicleYawRate_dps,
                                   double    aVehiclePitchRate_dps,
                                   double    aVehicleRollRate_dps,
                                   double&   aTotalLift_lbs,
                                   double&   aTotalDrag_lbs,
                                   double&   aTotalSide_lbs,
                                   UtVec3dX& aTotalMomentVector_ftlbs,
                                   double&   aCL,
                                   double&   aCd,
                                   double&   aCY,
                                   double&   aCl,
                                   double&   aCm,
                                   double&   aCn);

         double ReferenceAreaOfComponent_sqft(AeroObject* aAeroObject);

         void CalcP6DOFCoreAeroDataUsingAlphaAndBetaSweep(QProgressDialog& aProgressDialog);
         void CalcP6DOFCoreAeroDataUsingPitchRateSweep(QProgressDialog& aProgressDialog);
         void CalcP6DOFCoreAeroDataUsingAlphaAndPitchRateSweep(QProgressDialog& aProgressDialog);
         void CalcP6DOFCoreAeroDataUsingYawRateSweep(QProgressDialog& aProgressDialog);
         void CalcP6DOFCoreAeroDataUsingBetaAndYawRateSweep(QProgressDialog& aProgressDialog);
         void CalcP6DOFCoreAeroDataUsingRollRateSweep(QProgressDialog& aProgressDialog);

         void CalcPM6DOFCoreAeroDataUsingAlphaAndBetaSweep(QProgressDialog& aProgressDialog);

         void CalcVehicleAspectRatio(QProgressDialog& aProgressDialog);

         void CalcP6DOFMovableAeroDataUsingAngleAndAlphaSweep(VehicleAeroMovable&  aAero,
                                                              std::vector<double>& aAngleDataPts_deg,
                                                              QProgressDialog&     aProgressDialog);

         void CalcP6DOFMovableAeroDataUsingAngleAndBetaSweep(VehicleAeroMovable&  aAero,
                                                             std::vector<double>& aAngleDataPts_deg,
                                                             QProgressDialog&     aProgressDialog);
          
         void CalcP6DOFMovableAeroDataUsingAngleSweep(VehicleAeroMovable&  aAero,
                                                      std::vector<double>& aAngleDataPts_deg,
                                                      QProgressDialog&     aProgressDialog);

         void CalcP6DOFMovableAeroDataUsingAngleAndAlphaBetaSweep(VehicleAeroMovable&  aAero,
                                                                  std::vector<double>& aAngleDataPts_deg,
                                                                  double               aMach,
                                                                  QProgressDialog&     aProgressDialog);

         void CalcGMCoreAeroData(QProgressDialog& aProgressDialog);

         void ShowProgress(QProgressDialog& progressDialog);

         // This sets the reference area (based on wing or body). It returns true if it succeeds.
         bool SetVehicleAeroReferenceArea();

         int CalcNumberOfAeroTests();
         int CalcBaseNumberOfAeroTests(bool aUseAlphaSweep, bool aUseBetaSweep, bool aUseMachSweep = true);
         int CalcNumberOfMovableAeroTests(bool aUseMachSweep,
                                          bool aUseAlphaSweep,
                                          bool aUseBetaSweep,
                                          int  aNumberOfAngleTests);

         // This calculates the vector of angles to be used for movable parts. Note that wings should
         // use the alternate/overloaded function below. This returns false if the vector is empty.
         bool CalculateVectorOfAngles(VehicleAeroMovable&  aAero,
                                      std::vector<double>& aAngleDataPts_deg);

         // This calculates the vector of angles to be used for movable parts. This variant should be used
         // for wing objects. It returns false if no vectors contain valid data.
         bool CalculateWingVectorOfAngles(VehicleAeroMovable&  aAero,
                                          std::vector<double>& aAileronAngleDataPts_deg,
                                          std::vector<double>& aFlapsAngleDataPts_deg,
                                          std::vector<double>& aLeadingEdgeFlapsAngleDataPts_deg);

         // This calculates the vector of angles to be used for movable parts, based on the max/min angles.
         // This returns false if the vector is empty.
         bool CalculateVectorOfControlSurfaceAngles(double               aMinAngle_deg,
                                                    double               aMaxAngle_deg,
                                                    std::vector<double>& aAngleDataPts_deg);


         void OutputInterpolatedTestDataForAlphaBetaMach(AeroTable4d* aTable,
                                                         FILE*        aFile,
                                                         double       aMach,
                                                         int          aAlphaMin_deg,
                                                         int          aAlphaMax_deg,
                                                         int          aAlphaStep_deg,
                                                         int          aBetaMin_deg,
                                                         int          aBetaMax_deg,
                                                         int          aBetaStep_deg);

         void OutputRawNearestTestDataForAlphaBetaMach(AeroTable4d* aTable,
                                                       FILE*        aFile,
                                                       double       aMach,
                                                       int          aBetaMin_deg,
                                                       int          aBetaMax_deg,
                                                       int          aBetaStep_deg);

         void OutputInterpolatedTestDataFor3dTableMach(AeroTable3d* aTable,
                                                       FILE*        aFile,
                                                       double       aMach,
                                                       int          aPrimaryMin,
                                                       int          aPrimaryMax,
                                                       int          aPrimaryStep);

         void OutputRawNearestTestDataFor3dTableMach(AeroTable3d* aTable,
                                                     FILE*        aFile,
                                                     double       aMach);

         void OutputTestDataForBaseAeroCL_AlphaBetaMach(FILE*  aFile,
                                                        double aMach,
                                                        int    aAlphaMin_deg,
                                                        int    aAlphaMax_deg,
                                                        int    aAlphaStep_deg,
                                                        int    aBetaMin_deg,
                                                        int    aBetaMax_deg,
                                                        int    aBetaStep_deg);

         void OutputTestDataForBaseAeroCLq_AlphaMach(FILE*  aFile,
                                                     double aMach,
                                                     int    alphaMin_deg,
                                                     int    alphaMax_deg,
                                                     int    alphaStep_deg);

         void OutputTestDataForBaseAeroCL_AlphaDotAlphaMach(FILE*  aFile,
                                                            double aMach,
                                                            int    alphaMin_deg,
                                                            int    alphaMax_deg,
                                                            int    alphaStep_deg);

         void OutputTestDataForBaseAeroCd_AlphaBetaMach(FILE*  aFile,
                                                        double aMach,
                                                        int    aAlphaMin_deg,
                                                        int    aAlphaMax_deg,
                                                        int    aAlphaStep_deg,
                                                        int    aBetaMin_deg,
                                                        int    aBetaMax_deg,
                                                        int    aBetaStep_deg);

         void OutputTestDataForBaseAeroCY_AlphaBetaMach(FILE*  aFile,
                                                        double aMach,
                                                        int    aAlphaMin_deg,
                                                        int    aAlphaMax_deg,
                                                        int    aAlphaStep_deg,
                                                        int    aBetaMin_deg,
                                                        int    aBetaMax_deg,
                                                        int    aBetaStep_deg);

         void OutputTestDataForBaseAeroCYr_BetaMach(FILE*  aFile,
                                                    double aMach,
                                                    int    aBetaMin_deg,
                                                    int    aBetaMax_deg,
                                                    int    aBetaStep_deg);

         void OutputTestDataForBaseAeroCY_BetaDotBetaMach(FILE*  aFile,
                                                          double aMach,
                                                          int    aBetaMin_deg,
                                                          int    aBetaMax_deg,
                                                          int    aBetaStep_deg);

         void OutputTestDataForBaseAeroCm_AlphaBetaMach(FILE*  aFile,
                                                        double aMach,
                                                        int    aAlphaMin_deg,
                                                        int    aAlphaMax_deg,
                                                        int    aAlphaStep_deg,
                                                        int    aBetaMin_deg,
                                                        int    aBetaMax_deg,
                                                        int    aBetaStep_deg);

         void OutputTestDataForBaseAeroCmq_Mach(FILE* aFile);

         void OutputTestDataForBaseAeroCmp_Mach(FILE* aFile);

         void OutputTestDataForBaseAeroCm_AlphaDotMach(FILE* aFile);

         void OutputTestDataForBaseAeroCn_AlphaBetaMach(FILE*  aFile,
                                                        double aMach,
                                                        int    aAlphaMin_deg,
                                                        int    aAlphaMax_deg,
                                                        int    aAlphaStep_deg,
                                                        int    aBetaMin_deg,
                                                        int    aBetaMax_deg,
                                                        int    aBetaStep_deg);

         void OutputTestDataForBaseAeroCn_BetaDotMach(FILE* aFile);

         void OutputTestDataForBaseAeroCnr_Mach(FILE* aFile);

         void OutputTestDataForBaseAeroCnp_Mach(FILE* aFile);

         void OutputTestDataForBaseAeroCl_AlphaBetaMach(FILE*  aFile,
                                                        double aMach,
                                                        int    aAlphaMin_deg,
                                                        int    aAlphaMax_deg,
                                                        int    aAlphaStep_deg,
                                                        int    aBetaMin_deg,
                                                        int    aBetaMax_deg,
                                                        int    aBetaStep_deg);

         void OutputTestDataForBaseAeroClp_Mach(FILE* aFile);

         void OutputTestDataForBaseAeroClr_Mach(FILE* aFile);

         void OutputTestDataForBaseAeroClq_Mach(FILE* aFile);

         void OutputTestDataForBaseAeroCl_AlphaDotMach(FILE* aFile);

         void OutputTestDataForBaseAeroCl_BetaDotMach(FILE* aFile);

         FILE* OpenFileForOutput(std::string aFilename);

         void ApplyAllModifiers();
         void ApplyCoreAeroModifiers(const QJsonObject& aModifiers);
         void ApplyMovableAeroModifiers(const QJsonObject& aModifiers);

         Vehicle*                                                mParentVehicle               = nullptr;
         double                                                  mVehicleAltitude_ft          = 15000.0;
         double                                                  mVehicleAlpha_deg            = 0.0;
         double                                                  mVehicleBeta_deg             = 0.0;
         double                                                  mVehicleYawRate_dps          = 0.0;
         double                                                  mVehiclePitchRate_dps        = 0.0;
         double                                                  mVehicleRollRate_dps         = 0.0;
         double                                                  mVehicleSpeed_fps            = 0.0;
         double                                                  mVehicleMach                 = 0.0;
         double                                                  mVehicleKTAS                 = 0.0;
         double                                                  mVehicleKCAS                 = 0.0;
         double                                                  mVehicleDynamicPressure_psf  = 0.0;
         double                                                  mAirDensity_slugs_ft3        = 0.0;
         double                                                  mVehicleTestMach             = 0.8;
         double                                                  mVehicleWingArea_ft2         = 0.0;
         double                                                  mVehicleWingSpan_ft          = 0.0;
         double                                                  mVehicleWingChord_ft         = 0.0;
         double                                                  mVehicleRefArea_ft2          = 0.0;
         double                                                  mVehicleRefLength_ft         = 0.0;
         bool                                                    mUseRefAreaInsteadOfWingArea = false;

         size_t                                                  mMaxMapIndex                 = 0;
         std::map<size_t, AeroObject*>                           mAeroObjMap;
         VehicleAeroCore                                         mCoreAero;
         UtAtmosphere                                            mAtmosphere;
         //std::vector<std::pair<std::string, VehicleAeroMovable>> mMovableAeroList;
         std::vector<VehicleAeroMovable>                         mMovableAeroList;

         // ..............................................................................................................................
         // This section defines the test points to be used for various Mach, alpha, beta, and control surface positions

         // These are Mach test points

         // These Mach test points are used in non-rate-based derivatives and are considered the nominal test points
         std::vector<int> mMachDataPts_HundredthsMach = { 10, 50, 70, 75, 80, 85, 90, 92, 94, 96, 98, 100, 102, 104, 106,
                                                                 108, 110, 120, 130, 140, 150, 175, 200, 250, 300, 350, 400, 500,
                                                                 600, 700, 800, 900, 1000, 1500, 2000
                                                               };


         // These are Beta (sideslip) test points
         std::vector<int> mBetaDataPts_deg = { -88, -85, -80, -75, -70, -65, -60, -55, -50, -45, -40, -35, -30, -28, -26, -24, -22,
                                               -20, -18, -16, -14, -12, -10, -8, -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6,
                                               8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 35, 40, 45, 50, 55, 60, 65, 70,
                                               75, 80, 85, 88
                                             };

         // These are Alpha (angle of attack) test points
         std::vector<int> mAlphaDataPts_deg = { -180, -175, -170, -165, -160, -155, -150, -145, -140, -135, -130, -125, -120, -115,
                                                -110, -105, -100, -95, -92, -88, -85, -80, -75, -70, -65, -60, -55, -50, -45, -40,
                                                -35, -30, -28, -26, -24, -22, -20, -18, -16, -14, -12, -10, -8,
                                                -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 
                                                35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 88, 92, 95, 100, 105, 110, 115,
                                                120, 125, 130, 135, 140, 145, 150, 155, 160, 165, 170, 175, 180
         };

         // These are control surface test points -- These can be used to effect both positive and negative angles
         std::vector<int> mBaseControlSurfaceDataPts_deg = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 14, 16, 18, 20, 22, 24, 26,
                                                             28, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85
                                                           };

         // ..............................................................................................................................

         // Set the max/min data ranges
         int mMaxTableMach_HundredthsMach =  300;
         int mMinTableBetaDataPts_deg     =  -89;
         int mMaxTableBetaDataPts_deg     =   89;
         int mMinTableAlphaDataPts_deg    = -179;
         int mMaxTableAlphaDataPts_deg    =  179;
         int mTotalNumberOfAeroTests      =    0;
         int mCurrentNumberOfAeroTests    =    0;

         // Set alpha/beta symmetry booleans
         bool mAlphaSymmetry = false;
         bool mBetaSymmetry = false;

         const double mEpsilon = std::numeric_limits<double>::epsilon();

         // Derivative multipliers
         bool   mDerivativeMultipliersSet = false;
         double mCnBetadotMult            = 1.0;
         double mCnpMult                  = 1.0;
         double mCnrMult                  = 1.0;
         double mCmAlphadotMult           = 1.0;
         double mCmpMult                  = 1.0;
         double mCmqMult                  = 1.0;
         double mClpMult                  = 1.0;
         double mClrMult                  = 1.0;
         double mClBetadotMult            = 1.0;
         double mClqMult                  = 1.0;
         double mClAlphadotMult           = 1.0;
         double mCnMovableMult            = 1.0;
         double mCmMovableMult            = 1.0;
         double mClMovableMult            = 1.0;

         double mAspectRatio              = 1.0;
         double mCLMax                    = 0.0;
         double mCdAtCLMax                = 0.0;
   };
}
#endif // !VEHICLE_AERO_HPP
