// This file was generated  on 12/5/2018 at 1:1:50 PM by the Boeing OMS CAL generation tools
// @warning  This file was automatically generated, edit at your own risk

/**
* Unclassified               U N C L A S S I F I E D               Unclassified
*
* DEVELOPMENT:
*    This document wholly developed with USG funds.
*    For additional information, contact the AFRCO.
*
* ‒  DISTRIBUTION STATEMENT D.  Distribution authorized to the Department
*    of Defense and U.S. DoD contractors only; Critical Technology; 17 Sep 2015.
*    Other requests shall be referred to Air Force Rapid Capabilities Office,
*    Bolling AFB, Washington DC 20032-6400. 
*
* EXPORT CONTROL:
*    WARNING - ITAR CONTROLLED - US ONLY
*    This distribution contains technical data whose export is restricted by
*    the Arms Export Control Act (Title 22, U.S.C., Sec. 2751 et seq. or the
*    Export Administration Act of 1979 as amended Title 50, U.S.C., App.
*    2401-2420 et seq.), as amended. Violation of these export laws are subject
*    to severe criminal penalties.  Disseminate in accordance with provisions of DoD
*    Directive 5230.25.
*/
#ifndef Uci__Type__TargetTypeEnum_h
#define Uci__Type__TargetTypeEnum_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(StdLibMap)
#  include <map>
//  @cond
#  define StdLibMap 1
//  @endcond
#endif

#if !defined(StdLibSstream)
#  include <sstream>
//  @cond
#  define StdLibSstream 1
//  @endcond
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Indicates the MIL-STD-6016 target type. */
      class TargetTypeEnum : public virtual uci::base::Accessor {
      public:

         //  The following are the enumerated items that comprise this enumeration
         enum EnumerationItem {
            //  Enumerated item that indicates that the enumeration has not been set
            enumNotSet,
            //  UCI_BRIDGE's ordinal value */
            UCI_BRIDGE,

            //  UCI_PETROLEUM_OIL_AND_LUBRICANTS's ordinal value */
            UCI_PETROLEUM_OIL_AND_LUBRICANTS,

            //  UCI_COMMUNICATIONS's ordinal value */
            UCI_COMMUNICATIONS,

            //  UCI_VEHICLES's ordinal value */
            UCI_VEHICLES,

            //  UCI_CONVOY's ordinal value */
            UCI_CONVOY,

            //  UCI_SHIPPING's ordinal value */
            UCI_SHIPPING,

            //  UCI_DOCK_AREA's ordinal value */
            UCI_DOCK_AREA,

            //  UCI_TROOPS's ordinal value */
            UCI_TROOPS,

            //  UCI_ARMORED_VEHICLES's ordinal value */
            UCI_ARMORED_VEHICLES,

            //  UCI_RAIL's ordinal value */
            UCI_RAIL,

            //  UCI_SURFACE_TO_AIR_MISSILE's ordinal value */
            UCI_SURFACE_TO_AIR_MISSILE,

            //  UCI_ANTIAIRCRAFT_ARTILLERY's ordinal value */
            UCI_ANTIAIRCRAFT_ARTILLERY,

            //  UCI_AIR_DEFENSES_AIRCRAFT's ordinal value */
            UCI_AIR_DEFENSES_AIRCRAFT,

            //  UCI_ARTILLERY_MORTARS's ordinal value */
            UCI_ARTILLERY_MORTARS,

            //  UCI_PILL_BOX_BUNKER's ordinal value */
            UCI_PILL_BOX_BUNKER,

            //  UCI_GUN_EMPLACEMENT's ordinal value */
            UCI_GUN_EMPLACEMENT,

            //  UCI_FORTIFICATION's ordinal value */
            UCI_FORTIFICATION,

            //  UCI_POWERPLANT's ordinal value */
            UCI_POWERPLANT,

            //  UCI_INDUSTRIAL_SITE's ordinal value */
            UCI_INDUSTRIAL_SITE,

            //  UCI_SUPPLY_DEPOT's ordinal value */
            UCI_SUPPLY_DEPOT,

            //  UCI_AIRPORT_FACILITIES's ordinal value */
            UCI_AIRPORT_FACILITIES,

            //  UCI_ROAD's ordinal value */
            UCI_ROAD,

            //  UCI_TUNNEL's ordinal value */
            UCI_TUNNEL,

            //  UCI_DAM's ordinal value */
            UCI_DAM,

            //  UCI_WARSHIP_LARGE's ordinal value */
            UCI_WARSHIP_LARGE,

            //  UCI_WARSHIP_MEDIUM's ordinal value */
            UCI_WARSHIP_MEDIUM,

            //  UCI_WARSHIP_SMALL's ordinal value */
            UCI_WARSHIP_SMALL,

            //  UCI_OTHER_SURFACE_VESSELS's ordinal value */
            UCI_OTHER_SURFACE_VESSELS,

            //  UCI_AIRBASE's ordinal value */
            UCI_AIRBASE,

            //  UCI_UNDEFINED's ordinal value */
            UCI_UNDEFINED,

            //  Enumerated item that indicates maximum enumerated item exclusive of this item
            enumMaxExclusive
         };


         /** The destructor */
         virtual ~TargetTypeEnum()
         { }

         /** Returns this accessor's type constant, i.e. targetTypeEnum
           *
           * @return This accessor's type constant, i.e. targetTypeEnum
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::targetTypeEnum;
         }


         /** The assignment operator. Sets the contents of this TargetTypeEnum to the contents of the TargetTypeEnum on the right
           * hand side (rhs) of the assignment operator.TargetTypeEnum
           *
           * @param rhs The TargetTypeEnum on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this TargetTypeEnum
           * @return A constant reference to this TargetTypeEnum.
           */
         const TargetTypeEnum& operator=(const TargetTypeEnum& rhs)
         {
            setValue(rhs.getValue());
            return *this;
         }


         /** The assignment operator. Sets the value of this TargetTypeEnum enumeration to the EnumerationItem on the right hand
           * side of the assignment operator.
           *
           * @param rhs The EnumerationItemwhose value is to be assigned to this TargetTypeEnum enumeration
           * @return A reference to this TargetTypeEnum enumeration
           */
         const TargetTypeEnum& operator=(EnumerationItem rhs)
         {
            setValue(rhs);
            return *this;
         }


         /** Sets the value of this TargetTypeEnum enumeration to the value of the specified enumeration item.
           *
           * @param item The EnumerationItemwhose value is to be used to set the value of this TargetTypeEnum enumeration
           */
         virtual void setValue(EnumerationItem item)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of this TargetTypeEnum enumeration.
           *
           * @param testForValidity=true Specifies whether this TargetTypeEnum enumeration should be validated (true) or not
           *      (false) as part of retrieving its value.
           * @return The value of this TargetTypeEnum enumeration.
           */
         virtual EnumerationItem getValue(bool testForValidity=true) const
            throw(uci::base::UCIException) = 0;


         /** Returns the number of items in this TargetTypeEnum enumeration.
           *
           * @return The number of items in this TargetTypeEnum enumeration.
           */
         int getNumberOfItems() const
         {
            return 30;
         }


         /** Returns whether this TargetTypeEnum enumeration is valid or not. A TargetTypeEnum enumeration is valid if its value
           * is one of the enumeration items other than enumNotSet and enumMaxExclusive.
           *
           * @return A flag indicating whether this TargetTypeEnum enumeration is valid (true) or not (false).
           */
         virtual bool isValid() const
         {
            return isValid(getValue(false));
         }


         /** Returns whether the specified enumeration item is valid or not. A enumeration item is valid if its value is one of
           * the enumeration items other than enumNotSet and enumMaxExclusive.
           *
           * @param item The EnumerationItem to test
           * @return A flag indicating whether the specified EnumerationItem is valid (true) or not (false).
           */
         static bool isValid(EnumerationItem item)
         {
            return (item > enumNotSet) && (item < enumMaxExclusive);
         }


         /** Returns whether the specified name item is valid or not. A name is valid if it is the name of one of the enumeration
           * items other than enumNotSet and enumMaxExclusive.
           *
           * @param name The name to test
           * @return A flag indicating whether the specified name is valid (true) or not (false).
           */
         static bool isValid(const std::string& name)
            throw(uci::base::UCIException)
         {
            initializeTables();
            FromNameTable::const_iterator iter = m_fromNameTable.find(name);
            return !(iter == m_fromNameTable.end());
         }


         /** Tests whether the value of the TargetTypeEnum enumeration on the right hand side of the == operator is equal to the
           * value as this TargetTypeEnum enumeration.
           *
           * @param rhs The TargetTypeEnum enumeration whose value is to be compared against the value of this TargetTypeEnum
           *      enumeration.
           * @return True if the value of the specified TargetTypeEnum enumeration (rhs) is equal to the value of this
           *      TargetTypeEnum enumeration, false otherwise
           */
         bool operator==(const TargetTypeEnum& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() == rhs.getValue();
         }


         /** Tests whether the value of the TargetTypeEnum enumeration on the right hand side of the != operator is not equal to
           * the value as this TargetTypeEnum enumeration.
           *
           * @param rhs The TargetTypeEnum enumeration whose value is to be compared against the value of this TargetTypeEnum
           *      enumeration.
           * @return True if the value of the specified TargetTypeEnum enumeration (rhs) is not equal to the value of this
           *      TargetTypeEnum enumeration, false otherwise
           */
         bool operator!=(const TargetTypeEnum& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() != rhs.getValue();
         }


         /** Tests whether the value of the TargetTypeEnum enumeration on the right hand side of the < operator is less than the
           * value as this TargetTypeEnum enumeration.
           *
           * @param rhs The TargetTypeEnum enumeration whose value is to be compared against the value of this TargetTypeEnum
           *      enumeration.
           * @return True if the value of the specified TargetTypeEnum enumeration (rhs) is less than the value of this
           *      TargetTypeEnum enumeration, false otherwise
           */
         bool operator<(const TargetTypeEnum& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() < rhs.getValue();
         }


         /** Tests whether the value of the TargetTypeEnum enumeration on the right hand side of the <= operator is less than or
           * equal to the value as this TargetTypeEnum enumeration.
           *
           * @param rhs The TargetTypeEnum enumeration whose value is to be compared against the value of this TargetTypeEnum
           *      enumeration.
           * @return True if the value of the specified TargetTypeEnum enumeration (rhs) is less than or equal to the value of
           *      this TargetTypeEnum enumeration, false otherwise
           */
         bool operator<=(const TargetTypeEnum& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() <= rhs.getValue();
         }


         /** Tests whether the value of the TargetTypeEnum enumeration on the right hand side of the > operator is greater than
           * the value as this TargetTypeEnum enumeration.
           *
           * @param rhs The TargetTypeEnum enumeration whose value is to be compared against the value of this TargetTypeEnum
           *      enumeration.
           * @return True if the value of the specified TargetTypeEnum enumeration (rhs) is greater than the value of this
           *      TargetTypeEnum enumeration, false otherwise
           */
         bool operator>(const TargetTypeEnum& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() > rhs.getValue();
         }


         /** Tests whether the value of the TargetTypeEnum enumeration on the right hand side of the >= operator is greater than
           * or equal to the value as this TargetTypeEnum enumeration.
           *
           * @param rhs The TargetTypeEnum enumeration whose value is to be compared against the value of this TargetTypeEnum
           *      enumeration.
           * @return True if the value of the specified TargetTypeEnum enumeration (rhs) is greater than or equal to the value of
           *      this TargetTypeEnum enumeration, false otherwise
           */
         bool operator>=(const TargetTypeEnum& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() >= rhs.getValue();
         }


         /** Tests whether the value of this TargetTypeEnum enumeration is equal to the value of the specified EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this TargetTypeEnum enumeration.
           * @return True if the value of the EnumerationItem(rhs) is equal to the value of this TargetTypeEnum enumeration, false
           *      otherwise
           */
         bool operator==(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() == rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is equal to the value of the specified TargetTypeEnum
           * enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified TargetTypeEnum
           *      enumeration.
           * @param rhs The TargetTypeEnum enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is equal to the value of the TargetTypeEnum enumeration (rhs),
           *      false otherwise
           */
         friend bool operator==(EnumerationItem lhs, const TargetTypeEnum& rhs)
         {
            testForValidItem(lhs);
            return lhs == rhs.getValue();
         }


         /** Tests whether the value of this TargetTypeEnum enumeration is not equal to the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this TargetTypeEnum enumeration.
           * @return True if the value of the EnumerationItem(rhs) is not equal to the value of this TargetTypeEnum enumeration,
           *      false otherwise
           */
         bool operator!=(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() != rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is not equal to the value of the specified TargetTypeEnum
           * enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified TargetTypeEnum
           *      enumeration.
           * @param rhs The TargetTypeEnum enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is not equal to the value of the TargetTypeEnum enumeration
           *      (rhs), false otherwise
           */
         friend bool operator!=(EnumerationItem lhs, const TargetTypeEnum& rhs)
         {
            testForValidItem(lhs);
            return lhs != rhs.getValue();
         }


         /** Tests whether the value of this TargetTypeEnum enumeration is less than the value of the specified EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this TargetTypeEnum enumeration.
           * @return True if the value of the EnumerationItem(rhs) is less than the value of this TargetTypeEnum enumeration,
           *      false otherwise
           */
         bool operator<(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() < rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is less than the value of the specified TargetTypeEnum
           * enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified TargetTypeEnum
           *      enumeration.
           * @param rhs The TargetTypeEnum enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is less than the value of the TargetTypeEnum enumeration (rhs),
           *      false otherwise
           */
         friend bool operator<(EnumerationItem lhs, const TargetTypeEnum& rhs)
         {
            testForValidItem(lhs);
            return lhs < rhs.getValue();
         }


         /** Tests whether the value of this TargetTypeEnum enumeration is less than or equal to the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this TargetTypeEnum enumeration.
           * @return True if the value of the EnumerationItem(rhs) is less than or equal to the value of this TargetTypeEnum
           *      enumeration, false otherwise
           */
         bool operator<=(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() <= rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is less than or equal to the value of the specified
           * TargetTypeEnum enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified TargetTypeEnum
           *      enumeration.
           * @param rhs The TargetTypeEnum enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is less than or equal to the value of the TargetTypeEnum
           *      enumeration (rhs), false otherwise
           */
         friend bool operator<=(EnumerationItem lhs, const TargetTypeEnum& rhs)
         {
            testForValidItem(lhs);
            return lhs <= rhs.getValue();
         }


         /** Tests whether the value of this TargetTypeEnum enumeration is greater than the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this TargetTypeEnum enumeration.
           * @return True if the value of the EnumerationItem(rhs) is greater than the value of this TargetTypeEnum enumeration,
           *      false otherwise
           */
         bool operator>(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() > rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is greater than the value of the specified TargetTypeEnum
           * enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified TargetTypeEnum
           *      enumeration.
           * @param rhs The TargetTypeEnum enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is greater than the value of the TargetTypeEnum enumeration
           *      (rhs), false otherwise
           */
         friend bool operator>(EnumerationItem lhs, const TargetTypeEnum& rhs)
         {
            testForValidItem(lhs);
            return lhs > rhs.getValue();
         }


         /** Tests whether the value of this TargetTypeEnum enumeration is greater than or equal to the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this TargetTypeEnum enumeration.
           * @return True if the value of the EnumerationItem(rhs) is greater than or equal to the value of this TargetTypeEnum
           *      enumeration, false otherwise
           */
         bool operator>=(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() >= rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is greater than or equal to the value of the specified
           * TargetTypeEnum enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified TargetTypeEnum
           *      enumeration.
           * @param rhs The TargetTypeEnum enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is greater than or equal to the value of the TargetTypeEnum
           *      enumeration (rhs), false otherwise
           */
         friend bool operator>=(EnumerationItem lhs, const TargetTypeEnum& rhs)
         {
            testForValidItem(lhs);
            return lhs >= rhs.getValue();
         }


         /** Returns the name of the specified enumeration item.
           *
           * @param item The enumeration item whose name is to be returned.
           * @return The enumeration item's name if valid
           */
         static const std::string& toName(EnumerationItem item)
            throw(uci::base::UCIException)
         {
            initializeTables();

            ToNameTable::const_iterator iter = m_toNameTable.find(item);
            if (iter == m_toNameTable.end()) {
               std::ostringstream oStream;
               oStream << "Failed to convert enumerated item to its name, item not found in mapping: " << item;
               throw uci::base::UCIException(oStream.str(), 0);
            }

            return iter->second;
         }


         /** Returns the enumeration item having the specified name.
           *
           * @param itemName The name of the enumeration item to be returned.
           * @return The enumeration item having the specified name
           */
         static EnumerationItem fromName(const std::string& itemName)
            throw(uci::base::UCIException)
         {
            initializeTables();

            FromNameTable::const_iterator iter = m_fromNameTable.find(itemName);
            if (iter == m_fromNameTable.end()) {
               std::ostringstream oStream;
               oStream << "Failed to return enumerated item having specified name, name not found in mapping: " << itemName;
               throw uci::base::UCIException(oStream.str(), 0);
            }

            return iter->second;
         }


         /** Return the name of the enumeration item that the value of this TargetTypeEnum enumeration is set to.
           *
           * @return The name of the enumeration item.
           */
         virtual const std::string& toName() const
            throw(uci::base::UCIException)
         {
            return toName(getValue());
         }


         /** Sets the value of this TargetTypeEnum enumeration to the value of the enumeration item having the specified name.
           *
           * @param itemName The name of the enumeration item whose value is to be used to set this TargetTypeEnum enumeration to
           */
         virtual void setValueFromName(const std::string& itemName)
            throw(uci::base::UCIException)
         {
            setValue(fromName(itemName));
         }



      protected:

         /** The default constructor. In addition to constructing a new TargetTypeEnum, this constructor also makes sure the
           * conversion tables are initialized
           */
         TargetTypeEnum()
         {
            initializeTables();
         }


         /** Tests that the specified enumeration item is valid or not, throwing an exception if the item is not valid.
           *
           * @param additionalDescription An additional description to include in the error message should an exception be thrown
           * @param item The enumeration item to test
           */
         static void testForValidItem(const char* additionalDescription, EnumerationItem item)
            throw(uci::base::UCIException)
         {
            if (!isValid(item)) {
               std::ostringstream oStream;
               oStream << additionalDescription << ": " << item << " (item must be between " << enumNotSet << " and "<< enumMaxExclusive << " exclusive)";
               throw uci::base::UCIException(oStream.str(), 0);
            }

         }


         /** Tests that the specified enumeration item is valid or not, throwing an exception if the item is not valid.
           *
           * @param item The enumeration item to test
           */
         static void testForValidItem(EnumerationItem item)
            throw(uci::base::UCIException)
         {
            testForValidItem("Invalid enumerated item specified", item);
         }



      private:

         //  The type of table that is used to lookup the name of a enumeration item given that enumeration item.
         typedef std::map<EnumerationItem, const std::string> ToNameTable;

         //  The type of table that is used to lookup an enumeration item given its name.
         typedef std::map<const std::string, EnumerationItem> FromNameTable;

         //  The table that is used to lookup the name of a enumeration item given that enumeration item.
         static ToNameTable m_toNameTable;

         //  The type of table that is used to lookup an enumeration item given its name.
         static FromNameTable m_fromNameTable;

         //  Indicates whether the tables has been initialized (true) or not (false).
         static bool m_tablesInitialized;

         //  Initializes the translation tables
         static void initializeTables();


      protected:

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The TargetTypeEnum to copy from
           */
         TargetTypeEnum(const TargetTypeEnum& rhs)
         {
            (void)rhs;
         }

      }; // TargetTypeEnum


   } // Namespace: type
} // Namespace: uci
template<typename charT, typename traits>
std::basic_ostream<charT, traits>&
operator<<(std::basic_ostream<charT, traits>& oStream, const uci::type::TargetTypeEnum& enumeration)
{
   oStream << enumeration.toName();
   return oStream;
}

#endif // Uci__Type__TargetTypeEnum_h

