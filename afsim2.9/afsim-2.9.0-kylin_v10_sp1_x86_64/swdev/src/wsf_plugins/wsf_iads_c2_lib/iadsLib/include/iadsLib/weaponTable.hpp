// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
*  @file weaponTable.hpp
*  @author Kenneth R. Sewell III
*          Kenneth.Sewell@wpafb.af.mil
*          AFRL/RYWF
*          2241 Avionics Circle
*          WPAFB, Ohio 45433-7303
*          937-656-4899 x3004
*
*  Classification: UNCLASSIFIED
*
*  HELIOS_1.2.3.6/dev/custom/src/model/equip/subsy/gtiqbaewspt/gtiqbaewspt.prv
*/

#include <string>
#include <vector>
#include <memory>
#include <functional>

#include <iadsLib/weaponRecordInterface.hpp>
#include <iadsLib/trackRecord.hpp>
#include <iadsLib/unitTypeRecord.hpp>

#ifndef IADSLIB_WEAPONTABLE_HPP
#define IADSLIB_WEAPONTABLE_HPP

namespace il
{

   class IADSLIB_EXPORT weaponTableEntry
   {
   public:
      weaponTableEntry () { }
      weaponTableEntry (const unitTypeRecord &wsys_type, const int priority)
         : m_wsys_type (wsys_type)
         , m_Priority (priority)
      { }

      ~weaponTableEntry() = default;

      inline const unitTypeRecord &getWeaponSystemType() const { return m_wsys_type; }

      inline int getPriority() const { return m_Priority; }

   protected:
      unitTypeRecord m_wsys_type;
      int m_Priority;
   };

   class IADSLIB_EXPORT genericAttribute
   {
   public:
      genericAttribute () { }
      genericAttribute (const std::string &name, const double value1, const double value2, std::function<bool (double, double, double)> functor)
         : m_name (name)
         , m_value1 (value1)
         , m_value2 (value2)
         , m_comparison (functor)
      { }

      inline virtual bool CheckValue (double value)
      {
         return m_comparison (m_value1, m_value2, value);
      }

      inline virtual bool IsSet () const { return m_comparison ? true : false; }

      inline const std::string &GetName () { return m_name; }
   protected:
      std::string m_name;
      double      m_value1;
      double      m_value2;
      std::function<bool (double, double, double)> m_comparison;
   };

   class IADSLIB_EXPORT weaponTableRow
   {
   public:
      typedef std::vector<weaponTableEntry> tWeaponPrefList;

   public:
      // weaponTableRow();
      weaponTableRow(
         const unitTypeRecord &type,
         const genericAttribute &speed_attribute,
         const genericAttribute &alt_attribute,
         const int &aiPref,
         const int &samPref,
         const int &otherPref
         );

      ~weaponTableRow() = default;

      inline void setTargetType (const unitTypeRecord &type) { m_TargetType = type; }
      inline const unitTypeRecord &getTargetType() const { return m_TargetType; }
      inline void setAcceptAnyType (const bool value = true) { m_accept_any_type = value; }
      inline bool isAcceptAnyType () const { return m_accept_any_type; }
      inline void setAcceptAnySubtype (const bool value = true) { m_accept_any_subtype = value; }
      inline bool isAcceptAnySubtype () const { return m_accept_any_subtype; }

      inline void setAIPreference( const int &pref ) { m_AIPreference = pref; }
      inline const int &getAIPreference() const { return m_AIPreference; }

      inline void setSAMPreference( const int &pref ) { m_SAMPreference = pref; }
      inline const int &getSAMPreference() const { return m_SAMPreference; }

      inline void setOtherPreference( const int &pref ) { m_OtherPreference = pref; }
      inline const int &getOtherPreference() const { return m_OtherPreference; }

      inline void setSpeedAttribute (const genericAttribute &attribute) { m_speed_attribute = attribute; }
      inline bool checkSpeedAttribute (const double speed_ms, std::string &name)
      {
         name = m_speed_attribute.GetName();
         return m_speed_attribute.CheckValue (speed_ms);
      }

      inline bool isSpeedAttributeSet () const { return m_speed_attribute.IsSet(); }
      inline void setAltitudeAttribute (const genericAttribute &attribute) { m_alt_attribute = attribute; }
      inline bool checkAltitudeAttribute (const double alt_m, std::string &name)
      {
         name = m_alt_attribute.GetName();
         return m_alt_attribute.CheckValue (alt_m);
      }

      inline bool isAltitudeAttributeSet () const { return m_alt_attribute.IsSet(); }

      inline void setMinPK (const double min_pk) { m_min_pk = min_pk; }
      inline bool checkMinPK (const double pk) { return pk >= m_min_pk; }

      inline void setTargetPriority (const int priority) { m_target_priority = priority; }
      inline int getTargetPriority () const { return m_target_priority; }

      inline void addAIWeaponTableEntry (const weaponTableEntry &entry) { m_AIWeaponPrefList.push_back (entry); }
      inline void addSAMWeaponTableEntry (const weaponTableEntry &entry) { m_SAMWeaponPrefList.push_back (entry); }
      inline void addOtherWeaponTableEntry (const weaponTableEntry &entry) { m_OtherWeaponPrefList.push_back (entry); }
      inline const tWeaponPrefList &getAIWeaponPreferenceList () const { return m_AIWeaponPrefList; }
      inline const tWeaponPrefList &getSAMWeaponPreferenceList () const { return m_SAMWeaponPrefList; }
      inline const tWeaponPrefList &getOtherWeaponPreferenceList () const { return m_OtherWeaponPrefList; }

   protected:
      unitTypeRecord m_TargetType;
      bool m_accept_any_type;
      bool m_accept_any_subtype;

      genericAttribute m_speed_attribute;
      genericAttribute m_alt_attribute;

      double m_min_pk;

      int m_target_priority;

      int m_AIPreference;
      int m_SAMPreference;
      int m_OtherPreference;

      tWeaponPrefList m_AIWeaponPrefList;
      tWeaponPrefList m_SAMWeaponPrefList;
      tWeaponPrefList m_OtherWeaponPrefList;

   private:

   };

   class IADSLIB_EXPORT weaponTable
   {
   public:
      weaponTable(std::weak_ptr<logger::GlobalLogger> aGlobalLogger);
      weaponTable (const weaponTable &from);
      weaponTable & operator=(const weaponTable &rhs);
      ~weaponTable() = default;

      bool getPriorities(const std::shared_ptr< weaponRecord > &weapon,
                         const std::shared_ptr<trackRecord> &track,
                         int &WeaponTypePriority,
                         int &WeaponSubtypePriority) const;

      const std::shared_ptr< weaponTableRow > getWeaponTableRow(const std::shared_ptr<trackRecord> &track, const bool match_types_only = true, const bool log_not_found = true) const;

      void addTableRow( std::shared_ptr< weaponTableRow > NewRow );

   protected:
      // Weapon Rows
      std::vector<std::shared_ptr<weaponTableRow>> m_Row;

      bool checkPreferencePassed (const std::shared_ptr< weaponTableRow > &row, const std::shared_ptr<trackRecord> &track, const bool log_results = true) const;
      std::weak_ptr<logger::GlobalLogger> mGlobalLogger;
   };
}
#endif
