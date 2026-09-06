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

#ifndef SOSM_TABLETARGET_HPP
#define SOSM_TABLETARGET_HPP

#include <list>
#include <string>
#include <vector>

class SOSM_Selector;
#include "SOSM_ScalarTable2D.hpp"
#include "SOSM_SpectralObject.hpp"
#include "SOSM_SpectralTable2D.hpp"
#include "SOSM_Target.hpp"

//! A target defined by a tables.
//!
//! This class models the target signature using tabular data. The signature can be
//! separated into body and plume.
class SOSM_TableTarget : public SOSM_Target
{
public:
   SOSM_TableTarget(SOSM_Manager* aManagerPtr);
   SOSM_TableTarget(const SOSM_TableTarget& aSrc);
   SOSM_TableTarget& operator=(const SOSM_TableTarget&) = delete;

   SOSM_Target* Clone() const override;

   void InputComplete() override;

   void ComputeRadiantIntensity(const SOSM_SpectralObject& aRequestor,
                                SOSM_Interaction&          aInteraction,
                                std::vector<float>&        aBodyIntensity,
                                float&                     aBodyArea,
                                std::vector<float>&        aPlumeIntensity,
                                float&                     aPlumeArea) override;

   void GetSamplePoints(SOSM_Interaction& aInteraction, std::vector<float>& aAzValues, std::vector<float>& aElValues) override;

   void GetSampleTargetState(SOSM_Interaction& aInteraction, float& aAltitude, float& aSpeed, float& aThrottle) override;

protected:
   using AreaTablePtr      = SOSM_ScalarTable2D::TablePtr;
   using IntensityTablePtr = std::shared_ptr<SOSM_SpectralTable2D>;

   StateBase* CreateState(const std::string& aStateName) const override;

   static AreaTablePtr LoadAreaTable(const std::string& aFileName, const std::string& aCachePrefix);

   static IntensityTablePtr LoadIntensityTable(const std::string& aFileName, const std::string& aCachePrefix);

   //! A 'Component' represents something like a cold_part, hot part or plume.
   class Component
   {
   public:
      void CheckConsistency(const std::string& aMessagePrefix, const std::string& aComponentType) const;

      void LoadAreaTable(SOSM_Manager* aManagerPtr, const std::string& aFileName, const std::string& aCachePrefix);

      void LoadIntensityTable(SOSM_Manager* aManagerPtr, const std::string& aFileName, const std::string& aCachePrefix);

      AreaTablePtr      mAreaTablePtr;
      IntensityTablePtr mIntensityTablePtr;
   };

   //! A 'Band' contains the components for a contiguous section of the spectrum.
   //! A target may have one or more non-overlapping bands.
   class Band : public SOSM_SpectralObject
   {
   public:
      Band(const std::string& aBandName);

      virtual void InputComplete(const std::string& aStateName);

      virtual bool ProcessInput(UtInput& aInput, SOSM_Target& aTarget);

      std::string mBandName;
      Component   mBody;
      Component   mPlume;
   };

   //! A 'State' represents the signature for a given operating mode.
   class TT_State : public StateBase
   {
   public:
      TT_State(const std::string& aStateName);

      void InputComplete(SOSM_Target& aTarget) override;
      bool ProcessInput(UtInput& aInput, SOSM_Target& aTarget) override;

      std::list<Band> mBandList;
      bool            mExplicitBandUsed;
      bool            mImplicitBandUsed;
   };

   //! @name Persistent collection of radiant intensity tables.
   //! A persistent collection of tables is maintained. Requests to load the same table
   //! multiple times will result in only one copy of the table being loaded and all
   //! common references will point to the one copy.
   //!
   //! SOSM_ScalarTable2D maintains the persistent list of area tables.
   //@{
   typedef std::map<std::string, IntensityTablePtr> IntensityTableMap;

   //! The persistent collection of radiant intensity tables that have been read, indexed by filename.
   static IntensityTableMap sIntensityTables;
   //@}
};

#endif
