// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
 *  @file   weaponTable.cpp
 *  @author Kenneth R. Sewell III \n
 *          Kenneth.Sewell@wpafb.af.mil \n
 *          AFRL/RYWF \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3004
 *
 *  Classification: UNCLASSIFIED
 *
 *  HELIOS_1.2.3.6/dev/custom/src/model/equip/subsy/gtiqbaewspt/gtiqbaewspt.prv
 */

#include <array>
#include <iostream>
#include <numeric>

#include <logger.hpp>
#include <iadsLib/enum.hpp>
#include <iadsLib/weaponTable.hpp>
#include <iadsLib/util.hpp>

namespace il
{

weaponTableRow::weaponTableRow(
    const unitTypeRecord &type,
    const genericAttribute &speed_attribute,
    const genericAttribute &alt_attribute,
    const int &aiPref,
    const int &samPref,
    const int &otherPref
    ):
    m_TargetType (type),
    m_accept_any_type (false),
    m_accept_any_subtype (false),
    m_speed_attribute (speed_attribute),
    m_alt_attribute (alt_attribute),
    m_min_pk (0),   // not all models use this, optional
    m_target_priority (0), // not all models use this, optional
    m_AIPreference( aiPref ),
    m_SAMPreference( samPref ),
    m_OtherPreference( otherPref )
{
}

//****************************************************************************

weaponTable::weaponTable(std::weak_ptr<logger::GlobalLogger> aGlobalLogger)
   : mGlobalLogger(aGlobalLogger)
{

}

weaponTable::weaponTable (const weaponTable &from)
   : mGlobalLogger(from.mGlobalLogger)
{
   for (const auto& row : from.m_Row)
   {
      m_Row.push_back(std::make_shared<weaponTableRow>(*row));
   }
}

weaponTable & weaponTable::operator=(const weaponTable &rhs)
{
   if (this != &rhs)
   {
      m_Row.clear();

      for (const auto& row : rhs.m_Row)
      {
         m_Row.push_back(std::make_shared<weaponTableRow>(*row));
      }
   }

   mGlobalLogger = rhs.mGlobalLogger;

   return *this;
}

bool weaponTable::getPriorities(const std::shared_ptr< weaponRecord > &weapon,
                                const std::shared_ptr<trackRecord> &track,
                                int &WeaponTypePriority,
                                int &WeaponSubtypePriority) const
{
   auto TrackType = track->getType();
   auto WeaponType = weapon->GetType();

   // Find this threat in weapon table...
   const std::shared_ptr< weaponTableRow > CurrentPreference = getWeaponTableRow(track, false);

   // Threat not found in weapons table.
   if( !CurrentPreference )
   {
      WeaponTypePriority = 0;
      WeaponSubtypePriority = 0;
      return false;
   }

   WeaponSubtypePriority = -1;

   // AI Weapon System?
   if( WEAPON_AI == WeaponType.getType() )
   {
      WeaponTypePriority = CurrentPreference->getAIPreference();
   }
   // SAM Weapon System?
   else if( WEAPON_SAM == WeaponType.getType() )
   {
      WeaponTypePriority = CurrentPreference->getSAMPreference();
   }
   // Other Weapon System?
   else if( WEAPON_OTHER == WeaponType.getType() )
   {
      WeaponTypePriority = CurrentPreference->getOtherPreference();
   }
   // Preference record does not allow for this Weapon type?
   else
   {
      WeaponTypePriority = 0;
   }

   return WeaponTypePriority != 0;
}

namespace
{
   template<class Iterable, class T, class BinaryOp>
   T accumulate(const Iterable& iterable, T init, BinaryOp op)
   {
      return std::accumulate(std::begin(iterable), std::end(iterable), init, op);
   }
}

const std::shared_ptr< weaponTableRow > weaponTable::getWeaponTableRow(const std::shared_ptr<trackRecord> & track, const bool match_types_only, const bool log_not_found) const
{
   std::shared_ptr< weaponTableRow > BestPreference;
   std::shared_ptr< weaponTableRow > BetterPreference;
   std::shared_ptr< weaponTableRow > GoodPreference;

   std::multimap <int, std::shared_ptr<weaponTableRow>, std::greater<int>> matchMap;

   auto &TrackType = track->getType();
   for(const auto& row : m_Row)
   {
      bool matchType = false;
      bool matchSubType = false;
      bool matchAnyType = false;
      bool matchAnySubType = false;

      std::string alt_attrib_name = "Default Any", speed_attrib_name = "Default Any";
      bool matchAlt = row->checkAltitudeAttribute(track->getPosition().getAltM(), alt_attrib_name);
      bool matchVel = row->checkSpeedAttribute(track->getVelocity(), speed_attrib_name);

      auto row_target_type = row->getTargetType();
      if (row_target_type.getType() == TrackType.getType())
      {
        matchType = true;
         // If type and subtype match
         if(row_target_type.getSubType() == TrackType.getSubType())
         {
            if (match_types_only || checkPreferencePassed (row, track, log_not_found))
               // Best preference
               BestPreference = row;
         }
         else if (row->isAcceptAnySubtype())
         {
            if( !BetterPreference )
            {
               // If type matches and subtype matches 'ANY'
               if (match_types_only || checkPreferencePassed (row, track, log_not_found))
                  BetterPreference = row;
            }
         }
      }
      // If type matches 'ANY'
      else if (row->isAcceptAnyType())
      {
         if( !GoodPreference )
         {
            if (match_types_only || checkPreferencePassed (row, track, log_not_found))
               GoodPreference = row;
         }
      }

     if (row_target_type.getSubType() == TrackType.getSubType())
        matchSubType = true;
     if (row->isAcceptAnySubtype())
        matchAnySubType = true;
     if (row->isAcceptAnyType())
        matchAnyType = true;

     if (!BestPreference && !BetterPreference && !GoodPreference){//if no match
        auto closeMatchPriority = accumulate(std::array<bool, 6>({matchType, matchSubType, matchAnyType, matchAnySubType, matchAlt, matchVel}), 0, [](int total, bool elem) {
           if(elem)
           {
              return total + 1;
           }
           return total;
        });

        if (closeMatchPriority){//if close match
           matchMap.insert(std::make_pair(closeMatchPriority, row));
        }
     }

   }

   // Best (exact) match not found?
   if( !BestPreference )
   {
      // Better (type) match found ?
      if( BetterPreference )
      {
         BestPreference = BetterPreference;
      }
      else
      {
         BestPreference = GoodPreference;
      }
   }

   // No match found?
   if(log_not_found && !BestPreference)
   {

      if (!matchMap.empty()){
         HCL_WARN_LOGGER(mGlobalLogger) << "WARNING: Could not retrieve preference for Track "
            << track->getID()
            << " of type '"
            << TrackType.getTypeStr()
            << "' and subtype '"
            << TrackType.getSubTypeStr()
            << "'. "
            << "No matches, but " << matchMap.count(matchMap.begin()->first) << " close matches found.";

         HCL_WARN_LOGGER(mGlobalLogger) << "Track-> Type: " << track->getType().getTypeStr() << " \tSubType: " << track->getType().getSubTypeStr()
            << " \tVelocity: " << track->getVelocity() << " \tAltitude: " << track->getPosition().getAltM();

         for (auto& matchPair : matchMap)
         {
            if (matchMap.begin()->first == matchPair.first){

               std::string alt_attrib_name = "Default Any", speed_attrib_name = "Default Any";
               bool matchAlt = matchPair.second->checkAltitudeAttribute(track->getPosition().getAltM(), alt_attrib_name);
               bool matchVel = matchPair.second->checkSpeedAttribute(track->getVelocity(), speed_attrib_name);

               HCL_WARN_LOGGER(mGlobalLogger) << "WeaponRow-> Target Priority: " << matchPair.second->getTargetPriority()
                  << " \tType: " << matchPair.second->getTargetType().getTypeStr() << " [match(" << BoolToString(((matchPair.second->getTargetType().getTypeStr()) == track->getType().getTypeStr()) || matchPair.second->isAcceptAnyType()) << ")] "
                  << " \tSubType: " << matchPair.second->getTargetType().getSubTypeStr() << " [match(" << BoolToString(((matchPair.second->getTargetType().getSubTypeStr()) == track->getType().getSubTypeStr()) || matchPair.second->isAcceptAnySubtype()) << ")] "
                  << " \tSpeed Attr: " << speed_attrib_name << " [match(" << BoolToString(matchVel) << ")] "
                  << " \tAltitude Attr: " << alt_attrib_name << " [match(" << BoolToString(matchAlt) << ")] "
                  << " \tTotal matches: " << matchPair.first;
            }
         }
      }
      else{
         HCL_WARN_LOGGER(mGlobalLogger) << "WARNING: Could not retrieve preference for Track "
            << track->getID()
            << " of type '"
            << TrackType.getTypeStr()
            << "' and subtype '"
            << TrackType.getSubTypeStr()
            << "'. "
            << "No close matches were found.";
      }

   }

   return( BestPreference );
}

void weaponTable::addTableRow( std::shared_ptr< weaponTableRow > NewRow )
{
  m_Row.push_back( NewRow );
}

bool weaponTable::checkPreferencePassed (const std::shared_ptr< weaponTableRow > &row, const std::shared_ptr<trackRecord> &track, const bool log_results) const
{
   // if no speed or altitude attributes were set, assume we're not filtering based on speed or altitude and don't fail
   std::string alt_attrib_name = "Default Any", speed_attrib_name = "Default Any";

   bool alt_passed = row->isAltitudeAttributeSet() ?  row->checkAltitudeAttribute (track->getPosition().getAltM(), alt_attrib_name) : true;
   bool speed_passed = row->isSpeedAttributeSet() ?   row->checkSpeedAttribute (track->getVelocity(), speed_attrib_name) : true;

   if (log_results)
      HCL_TRACE_LOGGER(mGlobalLogger) << "weaponTable::checkPreferencePassed(): alt_passed (" << BoolToString(alt_passed) << "), with attribute :" << alt_attrib_name << " Alt: " << track->getPosition().getAltM()
      << ", speed_passed (" << BoolToString(speed_passed) << "), with attribute :" << speed_attrib_name << " Vel: " << track->getVelocity();

   return alt_passed && speed_passed;
}

} // namespace il
