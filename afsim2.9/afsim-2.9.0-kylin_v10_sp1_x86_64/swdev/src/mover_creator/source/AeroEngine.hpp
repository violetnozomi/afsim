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

#ifndef AERO_ENGINE_HPP
#define AERO_ENGINE_HPP

#include <map>
#include <string>
#include <vector>

#include "AeroObject.hpp"
#include "AeroTable2d.hpp"
#include "GeometryEngine.hpp"

namespace Designer
{
   class Vehicle;

   class AeroEngine
   {
      public:

         enum class DataKey
         {
            cIDLE_THRUST_ALT,  // Jet
            cMIL_THRUST_ALT,   // Jet
            cAB_THRUST_ALT,    // Jet
            cIDLE_THRUST_MACH, // Jet
            cMIL_THRUST_MACH,  // Jet
            cAB_THRUST_MACH,   // Jet
            cMIL_SPIN_UP,      // Jet
            cAB_SPIN_UP,       // Jet
            cMIL_SPIN_DOWN,    // Jet
            cAB_SPIN_DOWN,     // Jet
            cTHRUST_TIME,      // SolidRocket
            cISP_ALT,          // SolidRocket, Liquid Rocket
            cTHRUST_ALT,       // Ramjet, Liquid Rocket
            cTHRUST_MACH       // Ramjet
         };

         enum class ModifierType
         {
            cNONE,
            cMULTIPLIER_LINE,
            cMULTIPLIER_VALUE,
            cOFFSET_LINE,
            cOFFSET_VALUE
         };

         enum class ExportMoverType
         {
             cP6DOF,
             cRIGID_BODY,
             cPOINT_MASS
         };

         // Used as a simple way to store and pass data about how to label that
         // different plots used by each engine type
         struct DataLabels
         {
            DataLabels()
               : title("title")
               , xLabel("x")
               , yLabel("y") {}

            DataLabels(const std::string& aTitle, const std::string& aX, const std::string& aY)
               : title(aTitle)
               , xLabel(aX)
               , yLabel(aY) {}

            std::string title;
            std::string xLabel;
            std::string yLabel;
         };

         struct PropulsionParameters // Used to output data that GuidedMovers need
         {
            double mFuel_mass = 0.0;
            double mMass_flow_rate = 0.0;
            double mIsp_sea_level = 0.0;
            double mIsp_vacuum = 0.0;

            std::vector<std::pair<double, double>> mThrustVsTime;
            std::vector<std::pair<double, double>> mThrottleVsMach;
         };


         using PlotNameMap = std::map<DataKey, DataLabels>;

         explicit AeroEngine(const std::string& aModelName);
         virtual ~AeroEngine() = default;

         size_t                       GetNumBaseSpecs() const { return mNumBaseSpecs; }
         size_t                       GetNumBaseSpecsOptions() const { return mNumBaseSpecsOptions; }
         virtual std::vector<double>  GetEngineSpecs() const;
         virtual std::vector<QString> GetEngineSpecsOptions() const { return std::vector<QString>(); }
         std::string                  GetName() { return mName; }
         std::string                  GetModelName() { return mModelName; }
         double                       GetLength_ft() { return mLength_ft; }
         double                       GetDiameter_ft() { return mDiameter_ft; }
         double                       GetThrustOffset_ft() { return mThrustOffset_ft; }
         double                       GetMass_lbs() { return mMass_lbs; }
         UtVec3dX                     GetRefPoint_ft() { return mRefPoint_ft; }
         double                       GetYaw_deg() { return mYaw_deg; }
         double                       GetPitch_deg() { return mPitch_deg; }
         double                       GetRoll_deg() { return mRoll_deg; }
         double                       GetIgnitionDelay_sec() { return mIgnitionDelay_sec; }
         GeometryEngine::EngineType   GetEngineType() { return mEngineType; }
         bool                         GetEngineTableData(DataKey aKey, std::vector<std::pair<double, double>>& aDataset) const { return mEngineTables.at(aKey)->GetModifiedData(aDataset); }
         AeroTable2d*                 GetEngineTable(DataKey aKey) const { return mEngineTables.at(aKey); }
         bool                         GetEngineTableData(const QString& aPlotType, std::vector<std::pair<double, double>>& aDataset) const { return mEngineTables.at(mStringToKeyMap.at(aPlotType))->GetModifiedData(aDataset); }
         AeroTable2d*                 GetEngineTable(const QString& aPlotType) const { return mEngineTables.at(mStringToKeyMap.at(aPlotType)); }
         double                       GetDataMultiplier(DataKey aKey) { return mEngineDataMultiplier[aKey]; }
         PlotNameMap                  GetPlotNameMap() const { return mPlotNameMap; }
         DataLabels                   GetPlotName(DataKey aKey) const { return mPlotNameMap.at(aKey); }
         DataLabels                   GetPlotLabels(DataKey aKey) const { return mPlotLabelMap.at(aKey); }
         DataLabels                   GetPlotLabels(QString aPlotType) const { return mPlotLabelMap.at(mStringToKeyMap.at(aPlotType)); }

         virtual void                 SetEngineSpecs(const std::vector<double>& aEngineSpecs);
         virtual void                 SetEngineSpecsOptions(const std::vector<QString>& aEngineSpecsOptions) { }
         void                         SetData(const std::string&         aName,
                                              GeometryEngine::EngineType aEngineType,
                                              UtVec3dX                   aRefPoint,
                                              double                     aYaw_deg,
                                              double                     aPitch_deg,
                                              double                     aRoll_deg,
                                              double                     aLength_ft,
                                              double                     aDiameter_ft,
                                              double                     aMass_lbs,
                                              double                     aThrustOffset_ft,
                                              double                     aIgnitionDelay_sec);


         virtual void                 LoadFromFile() = 0;
         virtual void                 SaveToFile() const = 0;
         virtual void                 Export(const std::string& aFileName, ExportMoverType aExportMoverType) const = 0;
         virtual PropulsionParameters GetPropulsionData(double aMaxMach) const = 0;

         virtual void                 CalcEngineData(); // Recalculate existing data

      protected:

         virtual void                 GenerateEngineData(); // Create new data

         using ModifierTypes  = std::map <DataKey, ModifierType>;
         using ModifierLines  = std::map <DataKey, std::vector<std::pair<double, double>>>;
         using ModifierLine   = std::vector<std::pair<double, double>>;
         using ModifierValues = std::map <DataKey, double>;
         using EngineData     = std::map <DataKey, std::vector<std::pair<double, double>>>;

      protected:

         std::string                     mName                 = "Engine";
         UtVec3dX                        mRefPoint_ft          = UtVec3dX(0.0, 0.0, 0.0);
         double                          mYaw_deg              = 0.0;
         double                          mPitch_deg            = 0.0;
         double                          mRoll_deg             = 0.0;
         double                          mLength_ft            = 0.0;
         double                          mDiameter_ft          = 0.0;
         double                          mMass_lbs             = 0.0;
         double                          mThrustOffset_ft      = 0.0;
         double                          mIgnitionDelay_sec    = 0.0;
         size_t                          mNumBaseSpecs         = 4;
         size_t                          mNumBaseSpecsOptions  = 0;
         GeometryEngine::EngineType      mEngineType           = GeometryEngine::EngineType::cJET;  // Default is Jet

         std::string                     mModelName;

         // These two variables provide a mapping between the data in the amc file and the display data
         // DisplayData = mEngineMultiplier * amcData + mEngineDataOffset
         std::map<DataKey, double>       mEngineDataMultiplier;
         std::map<DataKey, double>       mEngineDataOffset;

         // The value of the independent variable (e.g. altitude, mach) at which the dependent variable (e.g. thrust) goes to zero.
         // A value of 0 indicates that there is no maximum
         std::map<DataKey, double>       mMaxIndepVar;

         std::map<DataKey, AeroTable2d*> mEngineTables;
         EngineData                      mRawData;
         ModifierTypes                   mModifierTypes;
         ModifierLines                   mModifierMultiplierLines;
         ModifierLines                   mModifierOffsetLines;
         ModifierValues                  mModifierMultipliers; // Global multiplier
         ModifierValues                  mModifierOffsets; // Global offset
         PlotNameMap                     mPlotNameMap; // Labels used for the plot data in JSON file
         PlotNameMap                     mPlotLabelMap; // Labels used in plots

         void OutputData(QTextStream&                aOutputStream,
                         double                      aData,
                         int                         aWidth,
                         int                         aPrecision,
                         QTextStream::FieldAlignment aAlignment) const;

         void OutputData(QTextStream&                aOutputStream,
                         QString                     aData,
                         int                         aWidth,
                         int                         aPrependedSpaces,
                         QTextStream::FieldAlignment aAlignment) const;

      private:

         ModifierType                           GetModifierType(DataKey aKey)           const;
         ModifierLine                           GetModifierMultiplierLine(DataKey aKey) const;
         ModifierLine                           GetModifierOffsetLine(DataKey aKey)     const;
         double                                 GetModifierMultiplier(DataKey aKey)     const;
         double                                 GetModifierOffset(DataKey aKey)         const;


         std::map<QString, DataKey> mStringToKeyMap{
            {{ QString("Idle Thrust vs Altitude"),        DataKey::cIDLE_THRUST_ALT },
             { QString("Military Thrust vs Altitude"),    DataKey::cMIL_THRUST_ALT },
             { QString("Afterburner Thrust vs Altitude"), DataKey::cAB_THRUST_ALT },
             { QString("Idle Thrust vs Mach"),            DataKey::cIDLE_THRUST_MACH },
             { QString("Military Thrust vs Mach"),        DataKey::cMIL_THRUST_MACH },
             { QString("Afterburner Thrust vs Mach"),     DataKey::cAB_THRUST_MACH },
             { QString("Military Spin-Up Rate"),          DataKey::cMIL_SPIN_UP },
             { QString("Afterburner Spin-Up Rate"),       DataKey::cAB_SPIN_UP },
             { QString("Military Spin-Down Rate"),        DataKey::cMIL_SPIN_DOWN },
             { QString("Afterburner Spin-Down Rate"),     DataKey::cAB_SPIN_DOWN },
             { QString("Thrust vs Time"),                 DataKey::cTHRUST_TIME },
             { QString("Specific Impulse vs Altitude"),   DataKey::cISP_ALT },
             { QString("Thrust vs Altitude"),             DataKey::cTHRUST_ALT },
             { QString("Thrust vs Mach"),                 DataKey::cTHRUST_MACH }}
         };
   };
}

#endif
