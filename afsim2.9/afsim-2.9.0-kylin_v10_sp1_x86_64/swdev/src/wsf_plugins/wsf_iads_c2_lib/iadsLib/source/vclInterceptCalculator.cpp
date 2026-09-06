// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
 /*********************************
 *
 *  Classification: UNCLASSIFIED
 *
 **********************************/

#include <logger.hpp>
#include <iadsLib/vclInterceptCalculator.hpp>
#include <vclMath/InterceptPrediction.hpp>
#include <iadsLib/vclWeaponRecord.hpp>

namespace il
{

bool VclInterceptCalculator::CanIntercept (const il::trackRecord &track, const il::weaponRecord *weapon, double &interceptTime_s, double pred_pt_ecef_m[3], double &est_pk, double &weap_max_range, std::weak_ptr<logger::GlobalLogger> aGlobalLogger)
{

   vm::Vector3<double> intercept_position;
   double epsilon = 0.00001;

   auto vcl_weapon = dynamic_cast<const il::vclWeaponRecord *> (weapon);
   if (!vcl_weapon)
   {
      HCL_FATAL_LOGGER(aGlobalLogger) << "VclInterceptCalculator::CanIntercept(): Attempted to call on a non-vclWeaponRecord...throwing";
      throw ("VclInterceptCalculator::CanIntercept(): Attempted to call on a non-vclWeaponRecord...throwing");
   }

   bool can_intercept =  vm::predictIntercept( track.getPosition().getXYZ(),
                                               track.getVelocityVector(),
                                               vcl_weapon->GetPosition().getXYZ(),
                                               vcl_weapon->GetInterceptSpeed(),
                                               interceptTime_s,
                                               intercept_position,
                                               epsilon) ;

   pred_pt_ecef_m[0] = intercept_position.getX();
   pred_pt_ecef_m[1] = intercept_position.getY();
   pred_pt_ecef_m[2] = intercept_position.getZ();

   // vcl doesn't provide this at the moment, assume perfection
   est_pk = 1.0;

   // vcl may not have max intercept range for the weapon either - take as the intercept distance - max range
   // that way if they use the unclass bm, all the results will normalize out on distance
   if (vcl_weapon->HasMaxRangeAttribute())
      weap_max_range = vcl_weapon->GetMaxRange();
   else
      weap_max_range = (vcl_weapon->GetPosition().getXYZ() - intercept_position).norm();

   return can_intercept;
}

} // namespace il
