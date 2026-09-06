// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef SOSM_TABLEATMOSPHERE_HPP
#define SOSM_TABLEATMOSPHERE_HPP

#include <memory>
#include <string>
#include <vector>

#include "SOSM_Atmosphere.hpp"
#include "SOSM_SpectralTable2D.hpp"
#include "SOSM_SpectralTable3D.hpp"

//! Atmospheric model using tables.
//!
//! This class provides a model of the atmosphere using tables. The tables are typically
//! generated using a program like MODTRAN.

class SOSM_TableAtmosphere : public SOSM_Atmosphere
{
public:
   SOSM_TableAtmosphere(SOSM_Manager* aManagerPtr);
   // Using the default copy constructor.
   // Using the default destructor.

   SOSM_Atmosphere* Clone() const override;

   void InputComplete() override;

   bool ProcessInput(UtInput& aInput) override;

   void ComputeAtmosphereData(const SOSM_Selector& aSelector,
                              SOSM_Interaction&    aInteraction,
                              std::vector<float>&  aBackgroundRadiance,
                              std::vector<float>&  aForegroundRadiance,
                              std::vector<float>&  aForegroundTransmittance) override;

   void GetSamplePoints(std::vector<float>& aAltValues, std::vector<float>& aElValues, std::vector<float>& aRangeValues) override;

private:
   void CheckIV(const std::string& aVarName, const TblIndVarU<float>& aIV1, const TblIndVarU<float>& aIV2) const;

   class SharedData
   {
   public:
      SharedData(SOSM_Manager* aManagerPtr);
      // NOTE: all tables should have the same independent variable breakpoints.

      //! The background path radiance = f(alt, el)
      SOSM_SpectralTable2D mBackgroundRadiance;

      //! The foreground (target) path radiance = f(alt, el, range)
      SOSM_SpectralTable3D mForegroundRadiance;

      //! The foreground (target) path transmittance = f(alt, el, range)
      SOSM_SpectralTable3D mForegroundTransmittance;
   };

   //! The pointer to the shared tables.
   //! All instances of the same atmosphere type share the same set of tables.
   std::shared_ptr<SharedData> mSharedDataPtr;
};

#endif
