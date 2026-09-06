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
#ifndef Uci__Type__OpLineCategoryEnum_h
#define Uci__Type__OpLineCategoryEnum_h 1

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

      /** The type of an operational line */
      class OpLineCategoryEnum : public virtual uci::base::Accessor {
      public:

         //  The following are the enumerated items that comprise this enumeration
         enum EnumerationItem {
            //  Enumerated item that indicates that the enumeration has not been set
            enumNotSet,
            //  UCI_FORWARD_EDGE_OF_BATTLE_AREA's ordinal value */
            UCI_FORWARD_EDGE_OF_BATTLE_AREA,

            //  UCI_GUN_TARGET_LINE's ordinal value */
            UCI_GUN_TARGET_LINE,

            //  UCI_CORRIDOR's ordinal value */
            UCI_CORRIDOR,

            //  UCI_HOSTILE_BOUNDARY's ordinal value */
            UCI_HOSTILE_BOUNDARY,

            //  UCI_BUFFER_ZONE_BOUNDARY's ordinal value */
            UCI_BUFFER_ZONE_BOUNDARY,

            //  UCI_LOW_LEVEL_TRANSIT_ROUTE's ordinal value */
            UCI_LOW_LEVEL_TRANSIT_ROUTE,

            //  UCI_TACTICAL_ACTION_LINE's ordinal value */
            UCI_TACTICAL_ACTION_LINE,

            //  UCI_FIRE_SUPPORT_COORDINATION_LINE's ordinal value */
            UCI_FIRE_SUPPORT_COORDINATION_LINE,

            //  UCI_FORWARD_LINE_OF_OWN_TROOPS's ordinal value */
            UCI_FORWARD_LINE_OF_OWN_TROOPS,

            //  UCI_LINE_OF_COMMUNICATIONS's ordinal value */
            UCI_LINE_OF_COMMUNICATIONS,

            //  UCI_JETWAY's ordinal value */
            UCI_JETWAY,

            //  Enumerated item that indicates maximum enumerated item exclusive of this item
            enumMaxExclusive
         };


         /** The destructor */
         virtual ~OpLineCategoryEnum()
         { }

         /** Returns this accessor's type constant, i.e. opLineCategoryEnum
           *
           * @return This accessor's type constant, i.e. opLineCategoryEnum
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::opLineCategoryEnum;
         }


         /** The assignment operator. Sets the contents of this OpLineCategoryEnum to the contents of the OpLineCategoryEnum on
           * the right hand side (rhs) of the assignment operator.OpLineCategoryEnum
           *
           * @param rhs The OpLineCategoryEnum on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this OpLineCategoryEnum
           * @return A constant reference to this OpLineCategoryEnum.
           */
         const OpLineCategoryEnum& operator=(const OpLineCategoryEnum& rhs)
         {
            setValue(rhs.getValue());
            return *this;
         }


         /** The assignment operator. Sets the value of this OpLineCategoryEnum enumeration to the EnumerationItem on the right
           * hand side of the assignment operator.
           *
           * @param rhs The EnumerationItemwhose value is to be assigned to this OpLineCategoryEnum enumeration
           * @return A reference to this OpLineCategoryEnum enumeration
           */
         const OpLineCategoryEnum& operator=(EnumerationItem rhs)
         {
            setValue(rhs);
            return *this;
         }


         /** Sets the value of this OpLineCategoryEnum enumeration to the value of the specified enumeration item.
           *
           * @param item The EnumerationItemwhose value is to be used to set the value of this OpLineCategoryEnum enumeration
           */
         virtual void setValue(EnumerationItem item)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of this OpLineCategoryEnum enumeration.
           *
           * @param testForValidity=true Specifies whether this OpLineCategoryEnum enumeration should be validated (true) or not
           *      (false) as part of retrieving its value.
           * @return The value of this OpLineCategoryEnum enumeration.
           */
         virtual EnumerationItem getValue(bool testForValidity=true) const
            throw(uci::base::UCIException) = 0;


         /** Returns the number of items in this OpLineCategoryEnum enumeration.
           *
           * @return The number of items in this OpLineCategoryEnum enumeration.
           */
         int getNumberOfItems() const
         {
            return 11;
         }


         /** Returns whether this OpLineCategoryEnum enumeration is valid or not. A OpLineCategoryEnum enumeration is valid if its
           * value is one of the enumeration items other than enumNotSet and enumMaxExclusive.
           *
           * @return A flag indicating whether this OpLineCategoryEnum enumeration is valid (true) or not (false).
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


         /** Tests whether the value of the OpLineCategoryEnum enumeration on the right hand side of the == operator is equal to
           * the value as this OpLineCategoryEnum enumeration.
           *
           * @param rhs The OpLineCategoryEnum enumeration whose value is to be compared against the value of this
           *      OpLineCategoryEnum enumeration.
           * @return True if the value of the specified OpLineCategoryEnum enumeration (rhs) is equal to the value of this
           *      OpLineCategoryEnum enumeration, false otherwise
           */
         bool operator==(const OpLineCategoryEnum& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() == rhs.getValue();
         }


         /** Tests whether the value of the OpLineCategoryEnum enumeration on the right hand side of the != operator is not equal
           * to the value as this OpLineCategoryEnum enumeration.
           *
           * @param rhs The OpLineCategoryEnum enumeration whose value is to be compared against the value of this
           *      OpLineCategoryEnum enumeration.
           * @return True if the value of the specified OpLineCategoryEnum enumeration (rhs) is not equal to the value of this
           *      OpLineCategoryEnum enumeration, false otherwise
           */
         bool operator!=(const OpLineCategoryEnum& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() != rhs.getValue();
         }


         /** Tests whether the value of the OpLineCategoryEnum enumeration on the right hand side of the < operator is less than
           * the value as this OpLineCategoryEnum enumeration.
           *
           * @param rhs The OpLineCategoryEnum enumeration whose value is to be compared against the value of this
           *      OpLineCategoryEnum enumeration.
           * @return True if the value of the specified OpLineCategoryEnum enumeration (rhs) is less than the value of this
           *      OpLineCategoryEnum enumeration, false otherwise
           */
         bool operator<(const OpLineCategoryEnum& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() < rhs.getValue();
         }


         /** Tests whether the value of the OpLineCategoryEnum enumeration on the right hand side of the <= operator is less than
           * or equal to the value as this OpLineCategoryEnum enumeration.
           *
           * @param rhs The OpLineCategoryEnum enumeration whose value is to be compared against the value of this
           *      OpLineCategoryEnum enumeration.
           * @return True if the value of the specified OpLineCategoryEnum enumeration (rhs) is less than or equal to the value of
           *      this OpLineCategoryEnum enumeration, false otherwise
           */
         bool operator<=(const OpLineCategoryEnum& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() <= rhs.getValue();
         }


         /** Tests whether the value of the OpLineCategoryEnum enumeration on the right hand side of the > operator is greater
           * than the value as this OpLineCategoryEnum enumeration.
           *
           * @param rhs The OpLineCategoryEnum enumeration whose value is to be compared against the value of this
           *      OpLineCategoryEnum enumeration.
           * @return True if the value of the specified OpLineCategoryEnum enumeration (rhs) is greater than the value of this
           *      OpLineCategoryEnum enumeration, false otherwise
           */
         bool operator>(const OpLineCategoryEnum& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() > rhs.getValue();
         }


         /** Tests whether the value of the OpLineCategoryEnum enumeration on the right hand side of the >= operator is greater
           * than or equal to the value as this OpLineCategoryEnum enumeration.
           *
           * @param rhs The OpLineCategoryEnum enumeration whose value is to be compared against the value of this
           *      OpLineCategoryEnum enumeration.
           * @return True if the value of the specified OpLineCategoryEnum enumeration (rhs) is greater than or equal to the value
           *      of this OpLineCategoryEnum enumeration, false otherwise
           */
         bool operator>=(const OpLineCategoryEnum& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() >= rhs.getValue();
         }


         /** Tests whether the value of this OpLineCategoryEnum enumeration is equal to the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this OpLineCategoryEnum
           *      enumeration.
           * @return True if the value of the EnumerationItem(rhs) is equal to the value of this OpLineCategoryEnum enumeration,
           *      false otherwise
           */
         bool operator==(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() == rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is equal to the value of the specified OpLineCategoryEnum
           * enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified OpLineCategoryEnum
           *      enumeration.
           * @param rhs The OpLineCategoryEnum enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is equal to the value of the OpLineCategoryEnum enumeration
           *      (rhs), false otherwise
           */
         friend bool operator==(EnumerationItem lhs, const OpLineCategoryEnum& rhs)
         {
            testForValidItem(lhs);
            return lhs == rhs.getValue();
         }


         /** Tests whether the value of this OpLineCategoryEnum enumeration is not equal to the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this OpLineCategoryEnum
           *      enumeration.
           * @return True if the value of the EnumerationItem(rhs) is not equal to the value of this OpLineCategoryEnum
           *      enumeration, false otherwise
           */
         bool operator!=(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() != rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is not equal to the value of the specified
           * OpLineCategoryEnum enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified OpLineCategoryEnum
           *      enumeration.
           * @param rhs The OpLineCategoryEnum enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is not equal to the value of the OpLineCategoryEnum enumeration
           *      (rhs), false otherwise
           */
         friend bool operator!=(EnumerationItem lhs, const OpLineCategoryEnum& rhs)
         {
            testForValidItem(lhs);
            return lhs != rhs.getValue();
         }


         /** Tests whether the value of this OpLineCategoryEnum enumeration is less than the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this OpLineCategoryEnum
           *      enumeration.
           * @return True if the value of the EnumerationItem(rhs) is less than the value of this OpLineCategoryEnum enumeration,
           *      false otherwise
           */
         bool operator<(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() < rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is less than the value of the specified OpLineCategoryEnum
           * enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified OpLineCategoryEnum
           *      enumeration.
           * @param rhs The OpLineCategoryEnum enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is less than the value of the OpLineCategoryEnum enumeration
           *      (rhs), false otherwise
           */
         friend bool operator<(EnumerationItem lhs, const OpLineCategoryEnum& rhs)
         {
            testForValidItem(lhs);
            return lhs < rhs.getValue();
         }


         /** Tests whether the value of this OpLineCategoryEnum enumeration is less than or equal to the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this OpLineCategoryEnum
           *      enumeration.
           * @return True if the value of the EnumerationItem(rhs) is less than or equal to the value of this OpLineCategoryEnum
           *      enumeration, false otherwise
           */
         bool operator<=(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() <= rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is less than or equal to the value of the specified
           * OpLineCategoryEnum enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified OpLineCategoryEnum
           *      enumeration.
           * @param rhs The OpLineCategoryEnum enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is less than or equal to the value of the OpLineCategoryEnum
           *      enumeration (rhs), false otherwise
           */
         friend bool operator<=(EnumerationItem lhs, const OpLineCategoryEnum& rhs)
         {
            testForValidItem(lhs);
            return lhs <= rhs.getValue();
         }


         /** Tests whether the value of this OpLineCategoryEnum enumeration is greater than the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this OpLineCategoryEnum
           *      enumeration.
           * @return True if the value of the EnumerationItem(rhs) is greater than the value of this OpLineCategoryEnum
           *      enumeration, false otherwise
           */
         bool operator>(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() > rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is greater than the value of the specified
           * OpLineCategoryEnum enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified OpLineCategoryEnum
           *      enumeration.
           * @param rhs The OpLineCategoryEnum enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is greater than the value of the OpLineCategoryEnum enumeration
           *      (rhs), false otherwise
           */
         friend bool operator>(EnumerationItem lhs, const OpLineCategoryEnum& rhs)
         {
            testForValidItem(lhs);
            return lhs > rhs.getValue();
         }


         /** Tests whether the value of this OpLineCategoryEnum enumeration is greater than or equal to the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this OpLineCategoryEnum
           *      enumeration.
           * @return True if the value of the EnumerationItem(rhs) is greater than or equal to the value of this
           *      OpLineCategoryEnum enumeration, false otherwise
           */
         bool operator>=(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() >= rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is greater than or equal to the value of the specified
           * OpLineCategoryEnum enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified OpLineCategoryEnum
           *      enumeration.
           * @param rhs The OpLineCategoryEnum enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is greater than or equal to the value of the OpLineCategoryEnum
           *      enumeration (rhs), false otherwise
           */
         friend bool operator>=(EnumerationItem lhs, const OpLineCategoryEnum& rhs)
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


         /** Return the name of the enumeration item that the value of this OpLineCategoryEnum enumeration is set to.
           *
           * @return The name of the enumeration item.
           */
         virtual const std::string& toName() const
            throw(uci::base::UCIException)
         {
            return toName(getValue());
         }


         /** Sets the value of this OpLineCategoryEnum enumeration to the value of the enumeration item having the specified name.
           *
           * @param itemName The name of the enumeration item whose value is to be used to set this OpLineCategoryEnum enumeration
           *      to
           */
         virtual void setValueFromName(const std::string& itemName)
            throw(uci::base::UCIException)
         {
            setValue(fromName(itemName));
         }



      protected:

         /** The default constructor. In addition to constructing a new OpLineCategoryEnum, this constructor also makes sure the
           * conversion tables are initialized
           */
         OpLineCategoryEnum()
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
           * @param rhs The OpLineCategoryEnum to copy from
           */
         OpLineCategoryEnum(const OpLineCategoryEnum& rhs)
         {
            (void)rhs;
         }

      }; // OpLineCategoryEnum


   } // Namespace: type
} // Namespace: uci
template<typename charT, typename traits>
std::basic_ostream<charT, traits>&
operator<<(std::basic_ostream<charT, traits>& oStream, const uci::type::OpLineCategoryEnum& enumeration)
{
   oStream << enumeration.toName();
   return oStream;
}

#endif // Uci__Type__OpLineCategoryEnum_h

