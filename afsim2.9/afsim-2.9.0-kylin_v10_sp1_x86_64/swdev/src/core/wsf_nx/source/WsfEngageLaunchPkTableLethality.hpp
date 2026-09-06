// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFENGAGELAUNCHPKTABLELETHALITY_HPP
#define WSFENGAGELAUNCHPKTABLELETHALITY_HPP

#include <memory>

#include "UtCloneablePtr.hpp"
#include "UtTable.hpp"
#include "WsfWeaponEffects.hpp"

class WsfEngageLaunchPkTableLethality : public WsfWeaponEffects
{
public:
   WsfEngageLaunchPkTableLethality(WsfScenario& aScenario);
   WsfEngageLaunchPkTableLethality& operator=(const WsfEngageLaunchPkTableLethality&) = delete;
   ~WsfEngageLaunchPkTableLethality() override                                        = default;
   WsfWeaponEffects* Clone() const override;
   bool              PreInitialize() override;
   bool              Initialize(double aSimTime, const WsfWeaponEngagement* aEngagementPtr) override;

   bool ProcessInput(UtInput& aInput) override;

protected:
   WsfEngageLaunchPkTableLethality(const WsfEngageLaunchPkTableLethality& aSrc);

   void CalcLaunchPk(double aSimTime) override;

private:
   bool LoadPkTable(const std::string& aFileName, std::string& aErrorString);

   template<typename T>
   bool ReadHeaderLine(std::ifstream& aFile, const std::string& aExpectedHeaderField, T& aHeaderValue, std::string& aErrorString);

   class PkTableInput
   {
   public:
      bool ReadPkTableInput(std::ifstream&     aFile,
                            double             aAltitude,    // meters
                            double             aTargetSpeed, // meters/sec
                            const std::string& aLengthUnits);

      // Use default constant value if table inputs are malformed
      bool CreateTableFromInput(std::unique_ptr<UtTable::Table>& aPkTablePtr,
                                double                           aDefaultConstant,
                                std::string&                     aErrorString);

   private:
      struct CrDrPkTable
      {
         std::vector<double> mCrossRanges; // meters
         std::vector<double> mDownRanges;  // meters
         std::vector<double> mPkValues;
      };

      // File input: Altitude (m) -> Target speed (m/s) -> CR/DR/Pk Table
      // These inputs will be processed into a UtInput object that is loaded into UtTable using ProcessInput
      std::map<double, std::map<double, std::unique_ptr<CrDrPkTable>>> mCrDrPkTables;
   };

   std::string mPkTablesPath;
   double      mDefaultPk;         // Use default Pk if <Launcher type, Target type> pair is not found in tables
   std::string mPkTableFileFilter; // Use to filter table files that are loaded

   // Key is <Launcher type, Target type>
   using KeyType = std::pair<WsfStringId, WsfStringId>;

   std::map<KeyType, std::unique_ptr<PkTableInput>>  mPkTableInputs;
   std::map<KeyType, UtCloneablePtr<UtTable::Table>> mPkTables; // The lookup tables : Pk = f(Altitude, Target speed,
                                                                // Cross Range, Down Range)
};

#endif // WSFENGAGELAUNCHPKTABLELETHALITY_HPP
