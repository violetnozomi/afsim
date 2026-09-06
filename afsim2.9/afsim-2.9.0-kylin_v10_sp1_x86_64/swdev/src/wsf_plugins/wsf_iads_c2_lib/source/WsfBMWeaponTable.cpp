// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// Classification: UNCLASSIFIED

#include <sstream>
#include "WsfBMWeaponTable.hpp"
#include "WsfBMUtils.hpp"
#include <UtSpeed.hpp>
#include <UtLength.hpp>
#include <iadsLib/util.hpp>

namespace
{
   bool comp_less(double lower, double upper, double value)
   {
      return value > lower && value < upper;
   }

   bool comp_less_equal(double lower, double upper, double value)
   {
      return value >= lower && value <= upper;
   }
}

WsfBMWeaponTable::WsfBMWeaponTable()
   : WsfBMUnmanagedTypeWrapper("WsfBMWeaponTable")
{
}

WsfBMWeaponTable::WsfBMWeaponTable(il::weaponTable* table)
   : WsfBMUnmanagedTypeWrapper("WsfBMWeaponTable", table)
{
}

WsfBMWeaponTable* WsfBMWeaponTable::Clone() const
{
   return new WsfBMWeaponTable(*this);
}

bool WsfBMWeaponTable::ProcessInput(UtInput& aInput, bool has_priority, bool has_pk_percent)
{
   bool processed = true;

   std::string command = aInput.GetCommand();

   if (command == "weapon_table")
   {
      UtInputBlock block(aInput, "end_" + command);

      while (block.ReadCommand(command) && processed)
      {
         if (command == "speed_attributes")
         {
            processed = ParseSpeedAttributes(block.GetInput(), "end_" + command);
         }
         else if (command == "altitude_attributes")
         {
            processed = ParseAltitudeAttributes(block.GetInput(), "end_" + command);
         }
         else if (command == "weapon_rows")
         {
            processed = ParseWeaponRowsBlock(block.GetInput(), "end_" + command, has_priority, has_pk_percent);
         }
         else
         {
            processed = false;
         }
      }
   }
   else // unknown block
   {
      processed = false;
   }

   return processed;
}

bool WsfBMWeaponTable::ParseSpeedAttributes(UtInput& aInput, const std::string& end_block_str)
{
   bool processed = true;
   UtInputBlock block(aInput, end_block_str);

   std::string command;
   while (block.ReadCommand(command) && processed)
   {
      if (command == "attrib")
      {
         std::string attrib_name, operator_str;
         double lower_speed = 0, upper_speed = 0;

         block.GetInput().ReadValue(attrib_name);
         block.GetInput().ReadValueOfType(lower_speed, UtInput::cSPEED);
         block.GetInput().ReadValue(operator_str);
         block.GetInput().ReadValueOfType(upper_speed, UtInput::cSPEED);

         std::function<bool(double, double, double)> functor;
         if (operator_str == "<")
         {
            functor = comp_less;
         }
         else if (operator_str == "<=")
         {
            functor = comp_less_equal;
         }
         else
         {
            throw UtInput::BadValue(block.GetInput(), "Speed attributes must conform to [name] [min speed] [<,<=] [max speed] format currently");
         }

         // construct a speed attribute object and cache it for future reference
         m_speed_attributes[attrib_name] = il::genericAttribute(attrib_name, lower_speed, upper_speed, functor);
      }
      else
      {
         processed = false;
      }
   }

   return processed;
}

bool WsfBMWeaponTable::ParseAltitudeAttributes(UtInput& aInput, const std::string& end_block_str)
{
   bool processed = true;
   UtInputBlock block(aInput, end_block_str);

   std::string command;
   while (block.ReadCommand(command) && processed)
   {
      if (command == "attrib")
      {
         std::string attrib_name, operator_str;
         double lower_alt = 0, upper_alt = 0;

         block.GetInput().ReadValue(attrib_name);
         block.GetInput().ReadValueOfType(lower_alt, UtInput::cLENGTH);
         block.GetInput().ReadValue(operator_str);
         block.GetInput().ReadValueOfType(upper_alt, UtInput::cLENGTH);

         std::function<bool(double, double, double)> functor;
         if (operator_str == "<")
         {
            functor = comp_less;
         }
         else if (operator_str == "<=")
         {
            functor = comp_less_equal;
         }
         else
         {
            throw UtInput::BadValue(block.GetInput(), "Altitude attributes must conform to [name] [min altitude] [<,<=] [max altitude] format currently");
         }

         // construct a speed attribute object and cache it for future reference
         m_altitude_attributes[attrib_name] = il::genericAttribute(attrib_name, lower_alt, upper_alt, functor);
      }
      else
      {
         processed = false;
      }
   }

   return processed;
}

bool WsfBMWeaponTable::ParseWeaponRowsBlock(UtInput& aInput, const std::string& end_block_str, bool has_priority, bool has_pk_percent)
{
   bool processed = true;
   UtInputBlock block(aInput, end_block_str);

   std::string command;
   while (block.ReadCommand(command) && processed)
   {
      if (command == "target_pref")
      {
         //////////////
         // Priority //
         //////////////
         // if it is valid for this type of parse
         int priority = -1;
         if (has_priority)
         {
            std::string priority_str;
            block.GetInput().ReadValue(priority_str);
            if (priority_str != "priority")
            {
               std::stringstream strm_error;
               strm_error << "Expected 'priority' but got: " << priority_str;
               throw UtInput::BadValue(block.GetInput(), strm_error.str());
            }

            block.GetInput().ReadValue(priority);
         }

         /////////////////////////
         // threat type/subtype //
         /////////////////////////
         std::string strType, strSubtype;
         block.GetInput().ReadValueQuoted(strType);
         block.GetInput().ReadValueQuoted(strSubtype);

         /////////////////////
         // speed attribute //
         /////////////////////
         std::string speed_attrib_name;
         block.GetInput().ReadValue(speed_attrib_name);
         // check the speed attribute to make sure we've got a cached entry for it, otherwise throw
         auto speed_attrib_entry = m_speed_attributes.find(speed_attrib_name);
         if (speed_attrib_entry == m_speed_attributes.end())
         {
            std::stringstream strm_error;
            strm_error << "Unknown speed attribute referenced in weapon row: " << speed_attrib_name;
            throw UtInput::BadValue(block.GetInput(), strm_error.str());
         }

         ////////////////////////
         // altitude attribute //
         ////////////////////////
         std::string alt_attrib_name;
         block.GetInput().ReadValue(alt_attrib_name);
         // check the altitude attribute to make sure we've got a cached entry for it, otherwise throw
         auto altitude_attrib_entry = m_altitude_attributes.find(alt_attrib_name);
         if (altitude_attrib_entry == m_altitude_attributes.end())
         {
            std::stringstream strm_error;
            strm_error << "Unknown altitude attribute referenced in weapon row: " << alt_attrib_name;
            throw UtInput::BadValue(block.GetInput(), strm_error.str());
         }

         //////////
         // PK % //
         //////////
         // if it is valid for this type of parse
         double min_pk = -1;
         if (has_pk_percent)
         {
            block.GetInput().ReadValue(min_pk);
         }

         //////////////////////
         // type preferences //
         //////////////////////
         int ai_priority = 0, sam_priority = 0, other_priority = 0;
         block.GetInput().ReadValue(ai_priority);
         block.GetInput().ReadValue(sam_priority);
         block.GetInput().ReadValue(other_priority);

         il::unitTypeRecord target_type(WsfBMUtils::GetType(strType), strType, WsfBMUtils::GetSubType(strSubtype), strSubtype);
         std::shared_ptr<il::weaponTableRow> wtr(new il::weaponTableRow(target_type, speed_attrib_entry->second, altitude_attrib_entry->second, ai_priority, sam_priority, other_priority));
         // check if any of the target type/subtype row settings are "Any"
         wtr->setAcceptAnyType(WsfBMUtils::IsTypeAny(strType));
         wtr->setAcceptAnySubtype(WsfBMUtils::IsSubtypeAny(strSubtype));

         // set the optional arguments dependent on the types of formats we're parsing
         if (has_priority)
         {
            wtr->setTargetPriority(priority);
         }

         if (has_pk_percent)
         {
            wtr->setMinPK(min_pk);
         }

         /////////////////////////
         // subtype preferences //
         /////////////////////////
         std::string subtype_block_str;
         block.GetInput().ReadValue(subtype_block_str);
         if (subtype_block_str == "ai_subtype_prefs")
         {
            ParseAISubtypePrefs(block.GetInput(), "end_" + subtype_block_str, wtr);
         }
         else if (subtype_block_str == "sam_subtype_prefs")
         {
            ParseSAMSubtypePrefs(block.GetInput(), "end_" + subtype_block_str, wtr);
         }
         else if (subtype_block_str == "other_subtype_prefs")
         {
            ParseOtherSubtypePrefs(block.GetInput(), "end_" + subtype_block_str, wtr);
         }
         else
            // was not what we expected, may there isn't a subtype block on this row...
         {
            block.GetInput().PushBack(subtype_block_str);
         }

         GetRawPtr()->addTableRow(wtr);
      }
      else
      {
         processed = false;
      }
   }

   return processed;
}

bool WsfBMWeaponTable::ParseAISubtypePrefs(UtInput& aInput, const std::string& end_block_str, std::shared_ptr<il::weaponTableRow>& wtr)
{
   bool processed = true;
   UtInputBlock block(aInput, end_block_str);

   std::string command;
   while (block.ReadCommand(command) && processed)
   {
      if (command == "subtype_pref")
      {
         std::string wsys_subtype;
         block.GetInput().ReadValueQuoted(wsys_subtype);

         int priority = 0;
         block.GetInput().ReadValue(priority);

         auto type_id = il::WEAPON_AI;
         auto type_str = il::AssetTypeEnumToString(type_id);
         auto subtype_id = WsfBMUtils::GetSubType(wsys_subtype);
         auto subtype_str = wsys_subtype;

         wtr->addAIWeaponTableEntry(il::weaponTableEntry(il::unitTypeRecord(type_id, type_str, subtype_id, subtype_str), priority));
      }
      else
      {
         processed = false;
      }
   }

   return processed;
}

bool WsfBMWeaponTable::ParseSAMSubtypePrefs(UtInput& aInput, const std::string& end_block_str, std::shared_ptr<il::weaponTableRow>& wtr)
{
   bool processed = true;
   UtInputBlock block(aInput, end_block_str);

   std::string command;
   while (block.ReadCommand(command) && processed)
   {
      if (command == "subtype_pref")
      {
         std::string wsys_subtype;
         block.GetInput().ReadValueQuoted(wsys_subtype);

         int priority = 0;
         block.GetInput().ReadValue(priority);

         auto type_id = il::WEAPON_SAM;
         auto type_str = il::AssetTypeEnumToString(type_id);
         auto subtype_id = WsfBMUtils::GetSubType(wsys_subtype);
         auto subtype_str = wsys_subtype;

         wtr->addSAMWeaponTableEntry(il::weaponTableEntry(il::unitTypeRecord(type_id, type_str, subtype_id, subtype_str), priority));
      }
      else
      {
         processed = false;
      }
   }

   return processed;
}

bool WsfBMWeaponTable::ParseOtherSubtypePrefs(UtInput& aInput, const std::string& end_block_str, std::shared_ptr<il::weaponTableRow>& wtr)
{
   bool processed = true;
   UtInputBlock block(aInput, end_block_str);

   std::string command;
   while (block.ReadCommand(command) && processed)
   {
      if (command == "subtype_pref")
      {
         std::string wsys_subtype;
         block.GetInput().ReadValueQuoted(wsys_subtype);

         int priority = 0;
         block.GetInput().ReadValue(priority);

         auto type_id = il::WEAPON_OTHER;
         auto type_str = il::AssetTypeEnumToString(type_id);
         auto subtype_id = WsfBMUtils::GetSubType(wsys_subtype);
         auto subtype_str = wsys_subtype;

         wtr->addOtherWeaponTableEntry(il::weaponTableEntry(il::unitTypeRecord(type_id, type_str, subtype_id, subtype_str), priority));
      }
      else
      {
         processed = false;
      }
   }

   return processed;
}
