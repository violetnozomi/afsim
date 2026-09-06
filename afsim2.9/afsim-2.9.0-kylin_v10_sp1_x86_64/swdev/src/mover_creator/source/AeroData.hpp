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

#ifndef AERO_DATA_HPP
#define AERO_DATA_HPP

#include <limits>
#include <map>
#include <memory>
#include <QJsonDocument>
#include <QPointF>
#include <string>
#include <vector>

namespace Designer
{
   constexpr double cEpsilon            = std::numeric_limits<double>::epsilon();
   constexpr double cSpeedOfSound       = 1025.0; // ft/sec
   constexpr double cKinematicViscosity = 0.0003; // ft^2/sec

   // The AeroData class is used to represent the lift (CL), drag (Cd), and pitching moment (Cm) characteristics for
   // a 2D airfoil. It also allows modification of the base, subsonic data to reflect Mach and aspect ratio effects
   // for a 3D wing/surface. All alpha values are in degrees.

   class AeroData
   {
      public:

         enum class DataType
         {
            cLIFT,
            cDRAG,
            cMOMENT
         };

         AeroData();
         ~AeroData() = default;

         // Loads the airfoil from a .foil file.  The file is expected in JSON format
         // using degrees as the unit for angles.  Return true if it succeeds.
         bool LoadFromFile(QString aFileName,
                           QString aSurfaceName = QString());

         std::string GetCurrentAirfoilName() { return mCurrentAirfoilFilename; }

         // Returns CL based on specified alpha and control surface configuration
         // in degrees using the modified lift spline
         double GetLiftCoeff(double aAlpha_deg,
                             double aSweepAngle_deg,
                             double aCordFraction,
                             double aDeflectionAngle_deg) const;

         // Returns CL based on specified alpha in degrees
         double GetLiftCoeff(double aAlpha_deg, double aSweepAngle_deg) const;

         // Returns Cd based on specified alpha in degrees
         double GetDragCoeff(double aAlpha_deg, double aSweepAngle_deg) const;

         // Returns Cm based on specified alpha in degrees
         double GetPitchMomentCoeff(double aAlpha_deg, double aSweepAngle_deg) const;

         std::pair<double, double> GetPosLiftPeak(double aSweepAngle_deg) const;
         std::pair<double, double> GetNegLiftPeak(double aSweepAngle_deg) const;
         std::pair<double, double> GetPosLiftValley(double aSweepAngle_deg) const;
         std::pair<double, double> GetNegLiftValley(double aSweepAngle_deg) const;
         double                    GetPreStallSlope(double aSweepAngle_deg) const;

         double GetAeroData(DataType aDataType,
                            double   aAlpha_deg,
                            double   aSweepAngle_deg) const;

         double GetAirfoilFactor() const { return mAirfoilFactor; }
         bool ProcessAeroData();
         void CalculateNominalData(double aAspectRatio, double aSweepAngle_deg);

         std::pair<std::map<double, double>, std::map<double, double>> GetRawLift() const { return GetRawData(DataType::cLIFT); }
         std::pair<std::map<double, double>, std::map<double, double>> GetRawDrag() const { return GetRawData(DataType::cDRAG); }
         std::pair<std::map<double, double>, std::map<double, double>> GetRawMoment() const { return GetRawData(DataType::cMOMENT); }

         using AeroDataMap = std::map<DataType, std::pair<std::map<double, double>, std::map<double, double>>>;

         QStringList Audit();
         QStringList AuditLift();
         QStringList AuditLift(double& aAlphaMin, double& aAlphaMax);
         QStringList AuditDrag();
         QStringList AuditMoment();

      private:

         std::pair<std::map<double, double>, std::map<double, double>> GetRawData(DataType aDataType) const { return mRawAirfoil.at(aDataType); }

         bool ProcessLiftData(QJsonArray aAlpha, QJsonArray aLift);
         bool ProcessDragData(QJsonArray aAlpha, QJsonArray aDrag);
         bool ProcessMomentData(QJsonArray aAlpha, QJsonArray aMoment);


         void CalculateNominalLift(double aAspectRatio,
                                   double aSweepAngle_deg);

         void CalculateNominalDrag(double aSweepAngle_deg,
                                   double aDeltaAlphaLeft,
                                   double aDeltaAlphaRight,
                                   double aAlphaStallLeft,
                                   double aAlphaStallRight);

         void CalculateNominalMoment(double aSweepAngle_deg,
                                     double aDeltaAlphaLeft,
                                     double aDeltaAlphaRight,
                                     double aAlphaStallLeft,
                                     double aAlphaStallRight);

         void CalculateAlphaChange(double  aSweepAngle_deg,
                                   double& aDeltaAlphaLeft,
                                   double& aDeltaAlphaRight,
                                   double& aAlphaStallLeft,
                                   double& aAlphaStallRight);

         void CalcLiftDataCharacteristics(double aSweepAngle_deg);

         // 2-dimensional interpolation {X1, {Y1,  Z1}, {Y2,  Z2} }
         //                             {X2, {Y3,  Z3}, {Y4,  Z4} }
         // given {X_val, Y_val}
         double InterpolateBilinear(std::pair<std::pair<double, std::pair<std::pair<double, double>, std::pair<double, double>>>, 
                                              std::pair<double, std::pair<std::pair<double, double>, std::pair<double, double>>>> aMinMaxValues, std::pair<double, double> aIndependentVars) const;

         struct LiftDataCharacteristics
         {
            std::map<double, std::pair<double, double>> peakPos;
            std::map<double, std::pair<double, double>> peakNeg;
            std::map<double, std::pair<double, double>> valleyPos;
            std::map<double, std::pair<double, double>> valleyNeg;
            std::map<double, double>                    preStallSlope;
         };

         // AeroInfo: each of the maps is alpha -> data
         using AeroInfo = std::pair<std::map<double, double>, std::map<double, double>>;

         // AirfoilDataMap: DataType (lift/drag/moment) -> AeroInfo
         using AirfoilDataMap = std::map<DataType, AeroInfo>;

         // NominalDataMap: DataType (lift/drag/moment) -> Map of sweep -> AeroInfo
         using NominalDataMap = std::map<DataType, std::map<double, AeroInfo>>;

         LiftDataCharacteristics                   mLiftDataCharacteristics;
         double                                    mAirfoilCLMaxRight  = 0.0;
         double                                    mAirfoilCLMaxLeft   = 0.0;

         std::shared_ptr<QJsonDocument>            mCurrentAirfoilFile = nullptr; // raw data from file.

         // AirfoilDataMap: alpha_l -> Cl, alpha_d -> Cd, alpha_m -> Cm
         AirfoilDataMap                            mRawAirfoil;                   // Raw airfoil
         AirfoilDataMap                            mAirfoilData;                  // Raw data plus default data to cover [-180,180]
         NominalDataMap                            mNominalData;                  // mAirfoilData including 3D effects (AR, sweep)
         std::string                               mCurrentAirfoilFilename;
         std::string                               mSurfaceName;

         // This is the 'airfoil technology factor' (ATF). The ATF ranges from a value of 0.87 for a NACA 6-series airfoil to 0.95 for a supercritical airfoil.
         double                                    mAirfoilFactor = 0.87;
   };
}

#endif // AERO_DATA_HPP
