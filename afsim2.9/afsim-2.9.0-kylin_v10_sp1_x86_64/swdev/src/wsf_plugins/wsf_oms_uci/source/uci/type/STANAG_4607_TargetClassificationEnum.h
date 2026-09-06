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
#ifndef Uci__Type__STANAG_4607_TargetClassificationEnum_h
#define Uci__Type__STANAG_4607_TargetClassificationEnum_h 1

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

      /** Surface Moving Target Indicator Target Classification from STANAG 4607 Edition 3 Field D32.10 */
      class STANAG_4607_TargetClassificationEnum : public virtual uci::base::Accessor {
      public:

         //  The following are the enumerated items that comprise this enumeration
         enum EnumerationItem {
            //  Enumerated item that indicates that the enumeration has not been set
            enumNotSet,
            //  UCI_NO_INFORMATION_LIVE_TARGET's ordinal value */
            UCI_NO_INFORMATION_LIVE_TARGET,

            //  UCI_TRACKED_VEHICLE_LIVE_TARGET's ordinal value */
            UCI_TRACKED_VEHICLE_LIVE_TARGET,

            //  UCI_WHEELED_VEHICLE_LIVE_TARGET's ordinal value */
            UCI_WHEELED_VEHICLE_LIVE_TARGET,

            //  UCI_ROTARY_WING_AIRCRAFT_LIVE_TARGET's ordinal value */
            UCI_ROTARY_WING_AIRCRAFT_LIVE_TARGET,

            //  UCI_FIXED_WING_AIRCRAFT_LIVE_TARGET's ordinal value */
            UCI_FIXED_WING_AIRCRAFT_LIVE_TARGET,

            //  UCI_STATIONARY_ROTATOR_LIVE_TARGET's ordinal value */
            UCI_STATIONARY_ROTATOR_LIVE_TARGET,

            //  UCI_MARITIME_LIVE_TARGET's ordinal value */
            UCI_MARITIME_LIVE_TARGET,

            //  UCI_BEACON_LIVE_TARGET's ordinal value */
            UCI_BEACON_LIVE_TARGET,

            //  UCI_AMPHIBIOUS_LIVE_TARGET's ordinal value */
            UCI_AMPHIBIOUS_LIVE_TARGET,

            //  UCI_PERSON_LIVE_TARGET's ordinal value */
            UCI_PERSON_LIVE_TARGET,

            //  UCI_VEHICLE_LIVE_TARGET's ordinal value */
            UCI_VEHICLE_LIVE_TARGET,

            //  UCI_ANIMAL_LIVE_TARGET's ordinal value */
            UCI_ANIMAL_LIVE_TARGET,

            //  UCI_LARGE_MULTIPLE_RETURN_LIVE_LAND_TARGET's ordinal value */
            UCI_LARGE_MULTIPLE_RETURN_LIVE_LAND_TARGET,

            //  UCI_LARGE_MULTIPLE_RETURN_LIVE_MARITIME_TARGET's ordinal value */
            UCI_LARGE_MULTIPLE_RETURN_LIVE_MARITIME_TARGET,

            //  UCI_OTHER_LIVE_TARGET's ordinal value */
            UCI_OTHER_LIVE_TARGET,

            //  UCI_UNKNOWN_LIVE_TARGET's ordinal value */
            UCI_UNKNOWN_LIVE_TARGET,

            //  UCI_NO_INFORMATION_SIMULATED_TARGET's ordinal value */
            UCI_NO_INFORMATION_SIMULATED_TARGET,

            //  UCI_TRACKED_VEHICLE_SIMULATED_TARGET's ordinal value */
            UCI_TRACKED_VEHICLE_SIMULATED_TARGET,

            //  UCI_WHEELED_VEHICLE_SIMULATED_TARGET's ordinal value */
            UCI_WHEELED_VEHICLE_SIMULATED_TARGET,

            //  UCI_ROTARY_WING_AIRCRAFT_SIMULATED_TARGET's ordinal value */
            UCI_ROTARY_WING_AIRCRAFT_SIMULATED_TARGET,

            //  UCI_FIXED_WING_AIRCRAFT_SIMULATED_TARGET's ordinal value */
            UCI_FIXED_WING_AIRCRAFT_SIMULATED_TARGET,

            //  UCI_STATIONARY_ROTATOR_SIMULATED_TARGET's ordinal value */
            UCI_STATIONARY_ROTATOR_SIMULATED_TARGET,

            //  UCI_MARITIME_SIMULATED_TARGET's ordinal value */
            UCI_MARITIME_SIMULATED_TARGET,

            //  UCI_BEACON_SIMULATED_TARGET's ordinal value */
            UCI_BEACON_SIMULATED_TARGET,

            //  UCI_AMPHIBIOUS_SIMULATED_TARGET's ordinal value */
            UCI_AMPHIBIOUS_SIMULATED_TARGET,

            //  UCI_PERSON_SIMULATED_TARGET's ordinal value */
            UCI_PERSON_SIMULATED_TARGET,

            //  UCI_VEHICLE_SIMULATED_TARGET's ordinal value */
            UCI_VEHICLE_SIMULATED_TARGET,

            //  UCI_ANIMAL_SIMULATED_TARGET's ordinal value */
            UCI_ANIMAL_SIMULATED_TARGET,

            //  UCI_LARGE_MULTIPLE_RETURN_SIMULATED_LAND_TARGET's ordinal value */
            UCI_LARGE_MULTIPLE_RETURN_SIMULATED_LAND_TARGET,

            //  UCI_LARGE_MULTIPLE_RETURN_SIMULATED_MARITIME_TARGET's ordinal value */
            UCI_LARGE_MULTIPLE_RETURN_SIMULATED_MARITIME_TARGET,

            //  UCI_OTHER_SIMULATED_TARGET's ordinal value */
            UCI_OTHER_SIMULATED_TARGET,

            //  UCI_UNKNOWN_SIMULATED_TARGET's ordinal value */
            UCI_UNKNOWN_SIMULATED_TARGET,

            //  Enumerated item that indicates maximum enumerated item exclusive of this item
            enumMaxExclusive
         };


         /** The destructor */
         virtual ~STANAG_4607_TargetClassificationEnum()
         { }

         /** Returns this accessor's type constant, i.e. sTANAG_4607_TargetClassificationEnum
           *
           * @return This accessor's type constant, i.e. sTANAG_4607_TargetClassificationEnum
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::sTANAG_4607_TargetClassificationEnum;
         }


         /** The assignment operator. Sets the contents of this STANAG_4607_TargetClassificationEnum to the contents of the
           * STANAG_4607_TargetClassificationEnum on the right hand side (rhs) of the assignment
           * operator.STANAG_4607_TargetClassificationEnum
           *
           * @param rhs The STANAG_4607_TargetClassificationEnum on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this STANAG_4607_TargetClassificationEnum
           * @return A constant reference to this STANAG_4607_TargetClassificationEnum.
           */
         const STANAG_4607_TargetClassificationEnum& operator=(const STANAG_4607_TargetClassificationEnum& rhs)
         {
            setValue(rhs.getValue());
            return *this;
         }


         /** The assignment operator. Sets the value of this STANAG_4607_TargetClassificationEnum enumeration to the
           * EnumerationItem on the right hand side of the assignment operator.
           *
           * @param rhs The EnumerationItemwhose value is to be assigned to this STANAG_4607_TargetClassificationEnum enumeration
           * @return A reference to this STANAG_4607_TargetClassificationEnum enumeration
           */
         const STANAG_4607_TargetClassificationEnum& operator=(EnumerationItem rhs)
         {
            setValue(rhs);
            return *this;
         }


         /** Sets the value of this STANAG_4607_TargetClassificationEnum enumeration to the value of the specified enumeration
           * item.
           *
           * @param item The EnumerationItemwhose value is to be used to set the value of this
           *      STANAG_4607_TargetClassificationEnum enumeration
           */
         virtual void setValue(EnumerationItem item)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of this STANAG_4607_TargetClassificationEnum enumeration.
           *
           * @param testForValidity=true Specifies whether this STANAG_4607_TargetClassificationEnum enumeration should be
           *      validated (true) or not (false) as part of retrieving its value.
           * @return The value of this STANAG_4607_TargetClassificationEnum enumeration.
           */
         virtual EnumerationItem getValue(bool testForValidity=true) const
            throw(uci::base::UCIException) = 0;


         /** Returns the number of items in this STANAG_4607_TargetClassificationEnum enumeration.
           *
           * @return The number of items in this STANAG_4607_TargetClassificationEnum enumeration.
           */
         int getNumberOfItems() const
         {
            return 32;
         }


         /** Returns whether this STANAG_4607_TargetClassificationEnum enumeration is valid or not. A
           * STANAG_4607_TargetClassificationEnum enumeration is valid if its value is one of the enumeration items other than
           * enumNotSet and enumMaxExclusive.
           *
           * @return A flag indicating whether this STANAG_4607_TargetClassificationEnum enumeration is valid (true) or not
           *      (false).
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


         /** Tests whether the value of the STANAG_4607_TargetClassificationEnum enumeration on the right hand side of the ==
           * operator is equal to the value as this STANAG_4607_TargetClassificationEnum enumeration.
           *
           * @param rhs The STANAG_4607_TargetClassificationEnum enumeration whose value is to be compared against the value of
           *      this STANAG_4607_TargetClassificationEnum enumeration.
           * @return True if the value of the specified STANAG_4607_TargetClassificationEnum enumeration (rhs) is equal to the
           *      value of this STANAG_4607_TargetClassificationEnum enumeration, false otherwise
           */
         bool operator==(const STANAG_4607_TargetClassificationEnum& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() == rhs.getValue();
         }


         /** Tests whether the value of the STANAG_4607_TargetClassificationEnum enumeration on the right hand side of the !=
           * operator is not equal to the value as this STANAG_4607_TargetClassificationEnum enumeration.
           *
           * @param rhs The STANAG_4607_TargetClassificationEnum enumeration whose value is to be compared against the value of
           *      this STANAG_4607_TargetClassificationEnum enumeration.
           * @return True if the value of the specified STANAG_4607_TargetClassificationEnum enumeration (rhs) is not equal to the
           *      value of this STANAG_4607_TargetClassificationEnum enumeration, false otherwise
           */
         bool operator!=(const STANAG_4607_TargetClassificationEnum& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() != rhs.getValue();
         }


         /** Tests whether the value of the STANAG_4607_TargetClassificationEnum enumeration on the right hand side of the <
           * operator is less than the value as this STANAG_4607_TargetClassificationEnum enumeration.
           *
           * @param rhs The STANAG_4607_TargetClassificationEnum enumeration whose value is to be compared against the value of
           *      this STANAG_4607_TargetClassificationEnum enumeration.
           * @return True if the value of the specified STANAG_4607_TargetClassificationEnum enumeration (rhs) is less than the
           *      value of this STANAG_4607_TargetClassificationEnum enumeration, false otherwise
           */
         bool operator<(const STANAG_4607_TargetClassificationEnum& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() < rhs.getValue();
         }


         /** Tests whether the value of the STANAG_4607_TargetClassificationEnum enumeration on the right hand side of the <=
           * operator is less than or equal to the value as this STANAG_4607_TargetClassificationEnum enumeration.
           *
           * @param rhs The STANAG_4607_TargetClassificationEnum enumeration whose value is to be compared against the value of
           *      this STANAG_4607_TargetClassificationEnum enumeration.
           * @return True if the value of the specified STANAG_4607_TargetClassificationEnum enumeration (rhs) is less than or
           *      equal to the value of this STANAG_4607_TargetClassificationEnum enumeration, false otherwise
           */
         bool operator<=(const STANAG_4607_TargetClassificationEnum& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() <= rhs.getValue();
         }


         /** Tests whether the value of the STANAG_4607_TargetClassificationEnum enumeration on the right hand side of the >
           * operator is greater than the value as this STANAG_4607_TargetClassificationEnum enumeration.
           *
           * @param rhs The STANAG_4607_TargetClassificationEnum enumeration whose value is to be compared against the value of
           *      this STANAG_4607_TargetClassificationEnum enumeration.
           * @return True if the value of the specified STANAG_4607_TargetClassificationEnum enumeration (rhs) is greater than the
           *      value of this STANAG_4607_TargetClassificationEnum enumeration, false otherwise
           */
         bool operator>(const STANAG_4607_TargetClassificationEnum& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() > rhs.getValue();
         }


         /** Tests whether the value of the STANAG_4607_TargetClassificationEnum enumeration on the right hand side of the >=
           * operator is greater than or equal to the value as this STANAG_4607_TargetClassificationEnum enumeration.
           *
           * @param rhs The STANAG_4607_TargetClassificationEnum enumeration whose value is to be compared against the value of
           *      this STANAG_4607_TargetClassificationEnum enumeration.
           * @return True if the value of the specified STANAG_4607_TargetClassificationEnum enumeration (rhs) is greater than or
           *      equal to the value of this STANAG_4607_TargetClassificationEnum enumeration, false otherwise
           */
         bool operator>=(const STANAG_4607_TargetClassificationEnum& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() >= rhs.getValue();
         }


         /** Tests whether the value of this STANAG_4607_TargetClassificationEnum enumeration is equal to the value of the
           * specified EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this
           *      STANAG_4607_TargetClassificationEnum enumeration.
           * @return True if the value of the EnumerationItem(rhs) is equal to the value of this
           *      STANAG_4607_TargetClassificationEnum enumeration, false otherwise
           */
         bool operator==(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() == rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is equal to the value of the specified
           * STANAG_4607_TargetClassificationEnum enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified
           *      STANAG_4607_TargetClassificationEnum enumeration.
           * @param rhs The STANAG_4607_TargetClassificationEnum enumeration whose value is to be compared against the value of
           *      the specified EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is equal to the value of the
           *      STANAG_4607_TargetClassificationEnum enumeration (rhs), false otherwise
           */
         friend bool operator==(EnumerationItem lhs, const STANAG_4607_TargetClassificationEnum& rhs)
         {
            testForValidItem(lhs);
            return lhs == rhs.getValue();
         }


         /** Tests whether the value of this STANAG_4607_TargetClassificationEnum enumeration is not equal to the value of the
           * specified EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this
           *      STANAG_4607_TargetClassificationEnum enumeration.
           * @return True if the value of the EnumerationItem(rhs) is not equal to the value of this
           *      STANAG_4607_TargetClassificationEnum enumeration, false otherwise
           */
         bool operator!=(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() != rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is not equal to the value of the specified
           * STANAG_4607_TargetClassificationEnum enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified
           *      STANAG_4607_TargetClassificationEnum enumeration.
           * @param rhs The STANAG_4607_TargetClassificationEnum enumeration whose value is to be compared against the value of
           *      the specified EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is not equal to the value of the
           *      STANAG_4607_TargetClassificationEnum enumeration (rhs), false otherwise
           */
         friend bool operator!=(EnumerationItem lhs, const STANAG_4607_TargetClassificationEnum& rhs)
         {
            testForValidItem(lhs);
            return lhs != rhs.getValue();
         }


         /** Tests whether the value of this STANAG_4607_TargetClassificationEnum enumeration is less than the value of the
           * specified EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this
           *      STANAG_4607_TargetClassificationEnum enumeration.
           * @return True if the value of the EnumerationItem(rhs) is less than the value of this
           *      STANAG_4607_TargetClassificationEnum enumeration, false otherwise
           */
         bool operator<(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() < rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is less than the value of the specified
           * STANAG_4607_TargetClassificationEnum enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified
           *      STANAG_4607_TargetClassificationEnum enumeration.
           * @param rhs The STANAG_4607_TargetClassificationEnum enumeration whose value is to be compared against the value of
           *      the specified EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is less than the value of the
           *      STANAG_4607_TargetClassificationEnum enumeration (rhs), false otherwise
           */
         friend bool operator<(EnumerationItem lhs, const STANAG_4607_TargetClassificationEnum& rhs)
         {
            testForValidItem(lhs);
            return lhs < rhs.getValue();
         }


         /** Tests whether the value of this STANAG_4607_TargetClassificationEnum enumeration is less than or equal to the value
           * of the specified EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this
           *      STANAG_4607_TargetClassificationEnum enumeration.
           * @return True if the value of the EnumerationItem(rhs) is less than or equal to the value of this
           *      STANAG_4607_TargetClassificationEnum enumeration, false otherwise
           */
         bool operator<=(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() <= rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is less than or equal to the value of the specified
           * STANAG_4607_TargetClassificationEnum enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified
           *      STANAG_4607_TargetClassificationEnum enumeration.
           * @param rhs The STANAG_4607_TargetClassificationEnum enumeration whose value is to be compared against the value of
           *      the specified EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is less than or equal to the value of the
           *      STANAG_4607_TargetClassificationEnum enumeration (rhs), false otherwise
           */
         friend bool operator<=(EnumerationItem lhs, const STANAG_4607_TargetClassificationEnum& rhs)
         {
            testForValidItem(lhs);
            return lhs <= rhs.getValue();
         }


         /** Tests whether the value of this STANAG_4607_TargetClassificationEnum enumeration is greater than the value of the
           * specified EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this
           *      STANAG_4607_TargetClassificationEnum enumeration.
           * @return True if the value of the EnumerationItem(rhs) is greater than the value of this
           *      STANAG_4607_TargetClassificationEnum enumeration, false otherwise
           */
         bool operator>(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() > rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is greater than the value of the specified
           * STANAG_4607_TargetClassificationEnum enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified
           *      STANAG_4607_TargetClassificationEnum enumeration.
           * @param rhs The STANAG_4607_TargetClassificationEnum enumeration whose value is to be compared against the value of
           *      the specified EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is greater than the value of the
           *      STANAG_4607_TargetClassificationEnum enumeration (rhs), false otherwise
           */
         friend bool operator>(EnumerationItem lhs, const STANAG_4607_TargetClassificationEnum& rhs)
         {
            testForValidItem(lhs);
            return lhs > rhs.getValue();
         }


         /** Tests whether the value of this STANAG_4607_TargetClassificationEnum enumeration is greater than or equal to the
           * value of the specified EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this
           *      STANAG_4607_TargetClassificationEnum enumeration.
           * @return True if the value of the EnumerationItem(rhs) is greater than or equal to the value of this
           *      STANAG_4607_TargetClassificationEnum enumeration, false otherwise
           */
         bool operator>=(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() >= rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is greater than or equal to the value of the specified
           * STANAG_4607_TargetClassificationEnum enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified
           *      STANAG_4607_TargetClassificationEnum enumeration.
           * @param rhs The STANAG_4607_TargetClassificationEnum enumeration whose value is to be compared against the value of
           *      the specified EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is greater than or equal to the value of the
           *      STANAG_4607_TargetClassificationEnum enumeration (rhs), false otherwise
           */
         friend bool operator>=(EnumerationItem lhs, const STANAG_4607_TargetClassificationEnum& rhs)
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


         /** Return the name of the enumeration item that the value of this STANAG_4607_TargetClassificationEnum enumeration is
           * set to.
           *
           * @return The name of the enumeration item.
           */
         virtual const std::string& toName() const
            throw(uci::base::UCIException)
         {
            return toName(getValue());
         }


         /** Sets the value of this STANAG_4607_TargetClassificationEnum enumeration to the value of the enumeration item having
           * the specified name.
           *
           * @param itemName The name of the enumeration item whose value is to be used to set this
           *      STANAG_4607_TargetClassificationEnum enumeration to
           */
         virtual void setValueFromName(const std::string& itemName)
            throw(uci::base::UCIException)
         {
            setValue(fromName(itemName));
         }



      protected:

         /** The default constructor. In addition to constructing a new STANAG_4607_TargetClassificationEnum, this constructor
           * also makes sure the conversion tables are initialized
           */
         STANAG_4607_TargetClassificationEnum()
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
           * @param rhs The STANAG_4607_TargetClassificationEnum to copy from
           */
         STANAG_4607_TargetClassificationEnum(const STANAG_4607_TargetClassificationEnum& rhs)
         {
            (void)rhs;
         }

      }; // STANAG_4607_TargetClassificationEnum


   } // Namespace: type
} // Namespace: uci
template<typename charT, typename traits>
std::basic_ostream<charT, traits>&
operator<<(std::basic_ostream<charT, traits>& oStream, const uci::type::STANAG_4607_TargetClassificationEnum& enumeration)
{
   oStream << enumeration.toName();
   return oStream;
}

#endif // Uci__Type__STANAG_4607_TargetClassificationEnum_h

