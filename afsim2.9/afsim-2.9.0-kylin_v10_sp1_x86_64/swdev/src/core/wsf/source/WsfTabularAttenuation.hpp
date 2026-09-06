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

#ifndef WSFTABULARATTENUATION_HPP
#define WSFTABULARATTENUATION_HPP

#include "wsf_export.h"

#include <iosfwd>
#include <string>
#include <vector>

#include "UtCloneablePtr.hpp"
#include "UtTable.hpp"
#include "WsfEM_Attenuation.hpp"

//! An attenuation model defined by externally-created tables.
class WSF_EXPORT WsfTabularAttenuation : public WsfEM_Attenuation
{
public:
   WsfTabularAttenuation();
   WsfTabularAttenuation(const WsfTabularAttenuation& aSrc);
   WsfTabularAttenuation& operator=(const WsfTabularAttenuation&) = delete;
   WsfEM_Attenuation*     Clone() const override;

   static WsfEM_Attenuation* ObjectFactory(const std::string& aTypeName);

   bool Initialize(WsfEM_XmtrRcvr* aXmtrRcvrPtr) override;

   bool ProcessInput(UtInput& aInput) override;

   bool AcceptsInlineBlockInput() const override { return true; }

   double ComputeAttenuationFactor(WsfEM_Interaction&          aInteraction,
                                   WsfEnvironment&             aEnvironment,
                                   WsfEM_Interaction::Geometry aGeometry) override;

   //! @name Publicly accessible utility methods for processing MODTRAN spectral data.
   //@{

   static void BuildResponseVector(UtTable::Curve&            aReponseCurve,
                                   const std::vector<double>& aWavenumbers,
                                   std::vector<double>&       aResponseVector);

   static double ComputeAverageContrastTransmittance(const std::vector<double>& aWavenumbers,
                                                     const std::vector<double>& aSensorToTargetTransmittances,
                                                     const std::vector<double>& aTargetToBackgroundRadiances,
                                                     const std::vector<double>& aSensorResponse);

   static double ComputeAverageTransmittance(const std::vector<double>& aWavenumbers,
                                             const std::vector<double>& aTransmittances,
                                             const std::vector<double>& aSensorResponse);

   static bool ReadSpectralData(std::istream&        aFile,
                                double&              aAltitude,
                                double&              aElevation,
                                double&              aRange,
                                std::vector<double>& aWavenumbers,
                                std::vector<double>& aValues);

   static void ReadSpectralHeaders(std::istream& aFile, std::string aHeaders[3]);
   //@}
private:
   void ProcessQueryCommand(UtInput& aInput);

   void SpectralDataConversion(UtInput& aInput);

   //! The attenuation table
   UtCloneablePtr<UtTable::Table> mAttenuationPtr;

   //! An adjustment factor that can be applied to the results.
   double mAdjustmentFactor;

   //! True if the table provides two-way attenuation.
   bool mTwoWayAttenuation;

   //! True if table is a function of frequency
   bool mNeedFrequency;

   //! True if table is a function of 'ground_range'
   bool mNeedGroundRange;

   //! True if table is a function of 'slant_range'
   bool mNeedSlantRange;
};

#endif
