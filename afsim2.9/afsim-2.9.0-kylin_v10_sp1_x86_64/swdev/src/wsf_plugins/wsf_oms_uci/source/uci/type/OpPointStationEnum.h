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
#ifndef Uci__Type__OpPointStationEnum_h
#define Uci__Type__OpPointStationEnum_h 1

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

      /** The type of a station point */
      class OpPointStationEnum : public virtual uci::base::Accessor {
      public:

         //  The following are the enumerated items that comprise this enumeration
         enum EnumerationItem {
            //  Enumerated item that indicates that the enumeration has not been set
            enumNotSet,
            //  UCI_GENERAL_STATION's ordinal value */
            UCI_GENERAL_STATION,

            //  UCI_TOMCAT's ordinal value */
            UCI_TOMCAT,

            //  UCI_PICKET's ordinal value */
            UCI_PICKET,

            //  UCI_RENDEZVOUS's ordinal value */
            UCI_RENDEZVOUS,

            //  UCI_REPLENISHMENT's ordinal value */
            UCI_REPLENISHMENT,

            //  UCI_RESCUE's ordinal value */
            UCI_RESCUE,

            //  UCI_COMBAT_AIR_PATROL's ordinal value */
            UCI_COMBAT_AIR_PATROL,

            //  UCI_AIRBORNE_EARLY_WARNING's ordinal value */
            UCI_AIRBORNE_EARLY_WARNING,

            //  UCI_ASW_FIXED_WING's ordinal value */
            UCI_ASW_FIXED_WING,

            //  UCI_ASW_HELO's ordinal value */
            UCI_ASW_HELO,

            //  UCI_REPLENISHMENT_AIR's ordinal value */
            UCI_REPLENISHMENT_AIR,

            //  UCI_STRIKE_IP's ordinal value */
            UCI_STRIKE_IP,

            //  UCI_TACAN's ordinal value */
            UCI_TACAN,

            //  UCI_TANKER's ordinal value */
            UCI_TANKER,

            //  UCI_ORBIT_RACE_TRACK's ordinal value */
            UCI_ORBIT_RACE_TRACK,

            //  UCI_ORBIT_FIGURE_EIGHT's ordinal value */
            UCI_ORBIT_FIGURE_EIGHT,

            //  UCI_ORBIT_RANDOM_CLOSED's ordinal value */
            UCI_ORBIT_RANDOM_CLOSED,

            //  UCI_ORBIT_POINT's ordinal value */
            UCI_ORBIT_POINT,

            //  UCI_OTHER's ordinal value */
            UCI_OTHER,

            //  Enumerated item that indicates maximum enumerated item exclusive of this item
            enumMaxExclusive
         };


         /** The destructor */
         virtual ~OpPointStationEnum()
         { }

         /** Returns this accessor's type constant, i.e. opPointStationEnum
           *
           * @return This accessor's type constant, i.e. opPointStationEnum
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::opPointStationEnum;
         }


         /** The assignment operator. Sets the contents of this OpPointStationEnum to the contents of the OpPointStationEnum on
           * the right hand side (rhs) of the assignment operator.OpPointStationEnum
           *
           * @param rhs The OpPointStationEnum on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this OpPointStationEnum
           * @return A constant reference to this OpPointStationEnum.
           */
         const OpPointStationEnum& operator=(const OpPointStationEnum& rhs)
         {
            setValue(rhs.getValue());
            return *this;
         }


         /** The assignment operator. Sets the value of this OpPointStationEnum enumeration to the EnumerationItem on the right
           * hand side of the assignment operator.
           *
           * @param rhs The EnumerationItemwhose value is to be assigned to this OpPointStationEnum enumeration
           * @return A reference to this OpPointStationEnum enumeration
           */
         const OpPointStationEnum& operator=(EnumerationItem rhs)
         {
            setValue(rhs);
            return *this;
         }


         /** Sets the value of this OpPointStationEnum enumeration to the value of the specified enumeration item.
           *
           * @param item The EnumerationItemwhose value is to be used to set the value of this OpPointStationEnum enumeration
           */
         virtual void setValue(EnumerationItem item)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of this OpPointStationEnum enumeration.
           *
           * @param testForValidity=true Specifies whether this OpPointStationEnum enumeration should be validated (true) or not
           *      (false) as part of retrieving its value.
           * @return The value of this OpPointStationEnum enumeration.
           */
         virtual EnumerationItem getValue(bool testForValidity=true) const
            throw(uci::base::UCIException) = 0;


         /** Returns the number of items in this OpPointStationEnum enumeration.
           *
           * @return The number of items in this OpPointStationEnum enumeration.
           */
         int getNumberOfItems() const
         {
            return 19;
         }


         /** Returns whether this OpPointStationEnum enumeration is valid or not. A OpPointStationEnum enumeration is valid if its
           * value is one of the enumeration items other than enumNotSet and enumMaxExclusive.
           *
           * @return A flag indicating whether this OpPointStationEnum enumeration is valid (true) or not (false).
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


         /** Tests whether the value of the OpPointStationEnum enumeration on the right hand side of the == operator is equal to
           * the value as this OpPointStationEnum enumeration.
           *
           * @param rhs The OpPointStationEnum enumeration whose value is to be compared against the value of this
           *      OpPointStationEnum enumeration.
           * @return True if the value of the specified OpPointStationEnum enumeration (rhs) is equal to the value of this
           *      OpPointStationEnum enumeration, false otherwise
           */
         bool operator==(const OpPointStationEnum& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() == rhs.getValue();
         }


         /** Tests whether the value of the OpPointStationEnum enumeration on the right hand side of the != operator is not equal
           * to the value as this OpPointStationEnum enumeration.
           *
           * @param rhs The OpPointStationEnum enumeration whose value is to be compared against the value of this
           *      OpPointStationEnum enumeration.
           * @return True if the value of the specified OpPointStationEnum enumeration (rhs) is not equal to the value of this
           *      OpPointStationEnum enumeration, false otherwise
           */
         bool operator!=(const OpPointStationEnum& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() != rhs.getValue();
         }


         /** Tests whether the value of the OpPointStationEnum enumeration on the right hand side of the < operator is less than
           * the value as this OpPointStationEnum enumeration.
           *
           * @param rhs The OpPointStationEnum enumeration whose value is to be compared against the value of this
           *      OpPointStationEnum enumeration.
           * @return True if the value of the specified OpPointStationEnum enumeration (rhs) is less than the value of this
           *      OpPointStationEnum enumeration, false otherwise
           */
         bool operator<(const OpPointStationEnum& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() < rhs.getValue();
         }


         /** Tests whether the value of the OpPointStationEnum enumeration on the right hand side of the <= operator is less than
           * or equal to the value as this OpPointStationEnum enumeration.
           *
           * @param rhs The OpPointStationEnum enumeration whose value is to be compared against the value of this
           *      OpPointStationEnum enumeration.
           * @return True if the value of the specified OpPointStationEnum enumeration (rhs) is less than or equal to the value of
           *      this OpPointStationEnum enumeration, false otherwise
           */
         bool operator<=(const OpPointStationEnum& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() <= rhs.getValue();
         }


         /** Tests whether the value of the OpPointStationEnum enumeration on the right hand side of the > operator is greater
           * than the value as this OpPointStationEnum enumeration.
           *
           * @param rhs The OpPointStationEnum enumeration whose value is to be compared against the value of this
           *      OpPointStationEnum enumeration.
           * @return True if the value of the specified OpPointStationEnum enumeration (rhs) is greater than the value of this
           *      OpPointStationEnum enumeration, false otherwise
           */
         bool operator>(const OpPointStationEnum& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() > rhs.getValue();
         }


         /** Tests whether the value of the OpPointStationEnum enumeration on the right hand side of the >= operator is greater
           * than or equal to the value as this OpPointStationEnum enumeration.
           *
           * @param rhs The OpPointStationEnum enumeration whose value is to be compared against the value of this
           *      OpPointStationEnum enumeration.
           * @return True if the value of the specified OpPointStationEnum enumeration (rhs) is greater than or equal to the value
           *      of this OpPointStationEnum enumeration, false otherwise
           */
         bool operator>=(const OpPointStationEnum& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() >= rhs.getValue();
         }


         /** Tests whether the value of this OpPointStationEnum enumeration is equal to the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this OpPointStationEnum
           *      enumeration.
           * @return True if the value of the EnumerationItem(rhs) is equal to the value of this OpPointStationEnum enumeration,
           *      false otherwise
           */
         bool operator==(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() == rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is equal to the value of the specified OpPointStationEnum
           * enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified OpPointStationEnum
           *      enumeration.
           * @param rhs The OpPointStationEnum enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is equal to the value of the OpPointStationEnum enumeration
           *      (rhs), false otherwise
           */
         friend bool operator==(EnumerationItem lhs, const OpPointStationEnum& rhs)
         {
            testForValidItem(lhs);
            return lhs == rhs.getValue();
         }


         /** Tests whether the value of this OpPointStationEnum enumeration is not equal to the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this OpPointStationEnum
           *      enumeration.
           * @return True if the value of the EnumerationItem(rhs) is not equal to the value of this OpPointStationEnum
           *      enumeration, false otherwise
           */
         bool operator!=(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() != rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is not equal to the value of the specified
           * OpPointStationEnum enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified OpPointStationEnum
           *      enumeration.
           * @param rhs The OpPointStationEnum enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is not equal to the value of the OpPointStationEnum enumeration
           *      (rhs), false otherwise
           */
         friend bool operator!=(EnumerationItem lhs, const OpPointStationEnum& rhs)
         {
            testForValidItem(lhs);
            return lhs != rhs.getValue();
         }


         /** Tests whether the value of this OpPointStationEnum enumeration is less than the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this OpPointStationEnum
           *      enumeration.
           * @return True if the value of the EnumerationItem(rhs) is less than the value of this OpPointStationEnum enumeration,
           *      false otherwise
           */
         bool operator<(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() < rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is less than the value of the specified OpPointStationEnum
           * enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified OpPointStationEnum
           *      enumeration.
           * @param rhs The OpPointStationEnum enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is less than the value of the OpPointStationEnum enumeration
           *      (rhs), false otherwise
           */
         friend bool operator<(EnumerationItem lhs, const OpPointStationEnum& rhs)
         {
            testForValidItem(lhs);
            return lhs < rhs.getValue();
         }


         /** Tests whether the value of this OpPointStationEnum enumeration is less than or equal to the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this OpPointStationEnum
           *      enumeration.
           * @return True if the value of the EnumerationItem(rhs) is less than or equal to the value of this OpPointStationEnum
           *      enumeration, false otherwise
           */
         bool operator<=(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() <= rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is less than or equal to the value of the specified
           * OpPointStationEnum enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified OpPointStationEnum
           *      enumeration.
           * @param rhs The OpPointStationEnum enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is less than or equal to the value of the OpPointStationEnum
           *      enumeration (rhs), false otherwise
           */
         friend bool operator<=(EnumerationItem lhs, const OpPointStationEnum& rhs)
         {
            testForValidItem(lhs);
            return lhs <= rhs.getValue();
         }


         /** Tests whether the value of this OpPointStationEnum enumeration is greater than the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this OpPointStationEnum
           *      enumeration.
           * @return True if the value of the EnumerationItem(rhs) is greater than the value of this OpPointStationEnum
           *      enumeration, false otherwise
           */
         bool operator>(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() > rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is greater than the value of the specified
           * OpPointStationEnum enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified OpPointStationEnum
           *      enumeration.
           * @param rhs The OpPointStationEnum enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is greater than the value of the OpPointStationEnum enumeration
           *      (rhs), false otherwise
           */
         friend bool operator>(EnumerationItem lhs, const OpPointStationEnum& rhs)
         {
            testForValidItem(lhs);
            return lhs > rhs.getValue();
         }


         /** Tests whether the value of this OpPointStationEnum enumeration is greater than or equal to the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this OpPointStationEnum
           *      enumeration.
           * @return True if the value of the EnumerationItem(rhs) is greater than or equal to the value of this
           *      OpPointStationEnum enumeration, false otherwise
           */
         bool operator>=(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() >= rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is greater than or equal to the value of the specified
           * OpPointStationEnum enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified OpPointStationEnum
           *      enumeration.
           * @param rhs The OpPointStationEnum enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is greater than or equal to the value of the OpPointStationEnum
           *      enumeration (rhs), false otherwise
           */
         friend bool operator>=(EnumerationItem lhs, const OpPointStationEnum& rhs)
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


         /** Return the name of the enumeration item that the value of this OpPointStationEnum enumeration is set to.
           *
           * @return The name of the enumeration item.
           */
         virtual const std::string& toName() const
            throw(uci::base::UCIException)
         {
            return toName(getValue());
         }


         /** Sets the value of this OpPointStationEnum enumeration to the value of the enumeration item having the specified name.
           *
           * @param itemName The name of the enumeration item whose value is to be used to set this OpPointStationEnum enumeration
           *      to
           */
         virtual void setValueFromName(const std::string& itemName)
            throw(uci::base::UCIException)
         {
            setValue(fromName(itemName));
         }



      protected:

         /** The default constructor. In addition to constructing a new OpPointStationEnum, this constructor also makes sure the
           * conversion tables are initialized
           */
         OpPointStationEnum()
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
           * @param rhs The OpPointStationEnum to copy from
           */
         OpPointStationEnum(const OpPointStationEnum& rhs)
         {
            (void)rhs;
         }

      }; // OpPointStationEnum


   } // Namespace: type
} // Namespace: uci
template<typename charT, typename traits>
std::basic_ostream<charT, traits>&
operator<<(std::basic_ostream<charT, traits>& oStream, const uci::type::OpPointStationEnum& enumeration)
{
   oStream << enumeration.toName();
   return oStream;
}

#endif // Uci__Type__OpPointStationEnum_h

