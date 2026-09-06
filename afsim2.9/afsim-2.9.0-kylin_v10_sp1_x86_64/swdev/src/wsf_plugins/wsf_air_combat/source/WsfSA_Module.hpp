// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSA_MODULE_HPP
#define WSFSA_MODULE_HPP

class WsfPlatform;
class WsfSimulation;

#include "WsfSA_Processor.hpp"

// Base class module for all other WsfSA classes that need access to data
class WsfSA_Module
{
public:
   WsfSA_Module(WsfSA_Processor* aSA_ProcPtr, WsfSA_Processor::PerceiveData* aDataPtr);

   ~WsfSA_Module() = default;

   /** Update this module.
    * @param aSimTime sim time of the update.
    */
   virtual void Update(double aSimTime);

   /** Return a pointer to the SA Processor that owns this module. */
   WsfSA_Processor* GetSAP() const { return mSA_ProcessorPtr; }

   /** Sets the SA Processor that owns this module.
    * @param aSA_ProcPtr pointer to SAP.
    * @param aDataPtr pointer to SAP's PerceiveData container.
    */
   void SetSAP(WsfSA_Processor* aSA_ProcPtr, WsfSA_Processor::PerceiveData* aDataPtr);

   /** Return a pointer to the platform */
   WsfPlatform* GetPlatform() const;

   /** Return a reference to the script context */
   WsfScriptContext& GetScriptContext() const;

   /** Return a pointer to the simulation */
   WsfSimulation* GetSimulation() const;

protected:
   WsfSA_Module(const WsfSA_Module& aSrc);

   WsfSA_Processor* mSA_ProcessorPtr{nullptr}; ///< Pointer to the owning processor for this module.

   WsfSA_Processor::PerceiveData* mDataPtr{
      nullptr}; ///< Perceived data container used by multiple SAP classes and lives in the SA Processor
};

#endif
