// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/*******************************************************************************
*
*   File: vclInterceptCalculator.hpp
*
*   Classification: UNCLASSIFIED
*
*   Details:
*
*   Contract Number:
*   Contractor Name:	Radiance Technologies
*   Address:			350 Wynn Drive
*   					   Huntsville, AL 35805
*
*   Abstract:
*
*******************************************************************************/

#ifndef VCL_INTERCEPT_CALCULATOR_HPP
#define VCL_INTERCEPT_CALCULATOR_HPP

#include <iadsLib/interceptCalculatorInterface.hpp>

namespace il
{

class VclInterceptCalculator : public InterceptCalculatorIface
{
public:
   virtual ~VclInterceptCalculator() = default;
   bool CanIntercept (const il::trackRecord &track, const il::weaponRecord *weapon, double &interceptTime_s, double pred_pt_ecef_m[3], double &est_pk, double &weap_max_range, std::weak_ptr<logger::GlobalLogger> aGlobalLogger) override;
};


}


#endif

