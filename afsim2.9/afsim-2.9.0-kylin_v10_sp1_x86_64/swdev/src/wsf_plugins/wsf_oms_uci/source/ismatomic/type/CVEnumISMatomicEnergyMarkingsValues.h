// This file was generated  on 12/5/2018 at 1:1:44 PM by the Boeing OMS CAL generation tools
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
#ifndef Ismatomic__Type__CVEnumISMatomicEnergyMarkingsValues_h
#define Ismatomic__Type__CVEnumISMatomicEnergyMarkingsValues_h 1

#if !defined(Xs__Type__StringXmlSchemaPrimitives_h)
# include "xs/type/stringXmlSchemaPrimitives.h"
#endif

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

//  
namespace ismatomic {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** (U) All currently valid Atomic Energy information markings from the published register PERMISSIBLE VALUES The
        * permissible values for this simple type are defined in the Controlled Value Enumeration:
        * CVEnumISMatomicEnergyMarkings.xml
        */
      class CVEnumISMatomicEnergyMarkingsValues : public virtual uci::base::Accessor {
      public:

         /** [Pattern: "RD-SG-((14)|(15)|(18)|(20))"] [Pattern: "FRD-SG-((14)|(15)|(18)|(20))"] */
         typedef xs::Token CVEnumISMatomicEnergyMarkingsValuesMember1;

         /** This is the CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration accessor class */
         class CVEnumISMatomicEnergyMarkingsValuesMember2 : public virtual uci::base::Accessor {
         public:

            //  The following are the enumerated items that comprise this enumeration
            enum EnumerationItem {
               //  Enumerated item that indicates that the enumeration has not been set
               enumNotSet,
               //  ISMATOMIC_RD's ordinal value */
               ISMATOMIC_RD,

               //  ISMATOMIC_RD_CNWDI's ordinal value */
               ISMATOMIC_RD_CNWDI,

               //  ISMATOMIC_FRD's ordinal value */
               ISMATOMIC_FRD,

               //  ISMATOMIC_DCNI's ordinal value */
               ISMATOMIC_DCNI,

               //  ISMATOMIC_UCNI's ordinal value */
               ISMATOMIC_UCNI,

               //  ISMATOMIC_TFNI's ordinal value */
               ISMATOMIC_TFNI,

               //  Enumerated item that indicates maximum enumerated item exclusive of this item
               enumMaxExclusive
            };


            /** The destructor */
            virtual ~CVEnumISMatomicEnergyMarkingsValuesMember2()
            { }

            /** Returns this accessor's type constant, i.e. cVEnumISMatomicEnergyMarkingsValuesMember2
              *
              * @return This accessor's type constant, i.e. cVEnumISMatomicEnergyMarkingsValuesMember2
              */
            virtual uci::base::accessorType::AccessorType getAccessorType() const
               throw()
            {
               return ismatomic::type::cVEnumISMatomicEnergyMarkingsValues::accessorType::cVEnumISMatomicEnergyMarkingsValuesMember2;
            }


            /** The assignment operator. Sets the contents of this CVEnumISMatomicEnergyMarkingsValuesMember2 to the contents of the
              * CVEnumISMatomicEnergyMarkingsValuesMember2 on the right hand side (rhs) of the assignment
              * operator.CVEnumISMatomicEnergyMarkingsValuesMember2
              *
              * @param rhs The CVEnumISMatomicEnergyMarkingsValuesMember2 on the right hand side (rhs) of the assignment operator
              *      whose contents are used to set the contents of this CVEnumISMatomicEnergyMarkingsValuesMember2
              * @return A constant reference to this CVEnumISMatomicEnergyMarkingsValuesMember2.
              */
            const CVEnumISMatomicEnergyMarkingsValuesMember2& operator=(const CVEnumISMatomicEnergyMarkingsValuesMember2& rhs)
            {
               setValue(rhs.getValue());
               return *this;
            }


            /** The assignment operator. Sets the value of this CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration to the
              * EnumerationItem on the right hand side of the assignment operator.
              *
              * @param rhs The EnumerationItemwhose value is to be assigned to this CVEnumISMatomicEnergyMarkingsValuesMember2
              *      enumeration
              * @return A reference to this CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration
              */
            const CVEnumISMatomicEnergyMarkingsValuesMember2& operator=(EnumerationItem rhs)
            {
               setValue(rhs);
               return *this;
            }


            /** Sets the value of this CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration to the value of the specified
              * enumeration item.
              *
              * @param item The EnumerationItemwhose value is to be used to set the value of this
              *      CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration
              */
            virtual void setValue(EnumerationItem item)
               throw(uci::base::UCIException) = 0;


            /** Returns the value of this CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration.
              *
              * @param testForValidity=true Specifies whether this CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration should be
              *      validated (true) or not (false) as part of retrieving its value.
              * @return The value of this CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration.
              */
            virtual EnumerationItem getValue(bool testForValidity=true) const
               throw(uci::base::UCIException) = 0;


            /** Returns the number of items in this CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration.
              *
              * @return The number of items in this CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration.
              */
            int getNumberOfItems() const
            {
               return 6;
            }


            /** Returns whether this CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration is valid or not. A
              * CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration is valid if its value is one of the enumeration items other
              * than enumNotSet and enumMaxExclusive.
              *
              * @return A flag indicating whether this CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration is valid (true) or not
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


            /** Tests whether the value of the CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration on the right hand side of the
              * == operator is equal to the value as this CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration.
              *
              * @param rhs The CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration whose value is to be compared against the value
              *      of this CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration.
              * @return True if the value of the specified CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration (rhs) is equal to
              *      the value of this CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration, false otherwise
              */
            bool operator==(const CVEnumISMatomicEnergyMarkingsValuesMember2& rhs) const
               throw(uci::base::UCIException)
            {
               return getValue() == rhs.getValue();
            }


            /** Tests whether the value of the CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration on the right hand side of the
              * != operator is not equal to the value as this CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration.
              *
              * @param rhs The CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration whose value is to be compared against the value
              *      of this CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration.
              * @return True if the value of the specified CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration (rhs) is not equal
              *      to the value of this CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration, false otherwise
              */
            bool operator!=(const CVEnumISMatomicEnergyMarkingsValuesMember2& rhs) const
               throw(uci::base::UCIException)
            {
               return getValue() != rhs.getValue();
            }


            /** Tests whether the value of the CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration on the right hand side of the <
              * operator is less than the value as this CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration.
              *
              * @param rhs The CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration whose value is to be compared against the value
              *      of this CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration.
              * @return True if the value of the specified CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration (rhs) is less than
              *      the value of this CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration, false otherwise
              */
            bool operator<(const CVEnumISMatomicEnergyMarkingsValuesMember2& rhs) const
               throw(uci::base::UCIException)
            {
               return getValue() < rhs.getValue();
            }


            /** Tests whether the value of the CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration on the right hand side of the
              * <= operator is less than or equal to the value as this CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration.
              *
              * @param rhs The CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration whose value is to be compared against the value
              *      of this CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration.
              * @return True if the value of the specified CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration (rhs) is less than
              *      or equal to the value of this CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration, false otherwise
              */
            bool operator<=(const CVEnumISMatomicEnergyMarkingsValuesMember2& rhs) const
               throw(uci::base::UCIException)
            {
               return getValue() <= rhs.getValue();
            }


            /** Tests whether the value of the CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration on the right hand side of the >
              * operator is greater than the value as this CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration.
              *
              * @param rhs The CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration whose value is to be compared against the value
              *      of this CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration.
              * @return True if the value of the specified CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration (rhs) is greater
              *      than the value of this CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration, false otherwise
              */
            bool operator>(const CVEnumISMatomicEnergyMarkingsValuesMember2& rhs) const
               throw(uci::base::UCIException)
            {
               return getValue() > rhs.getValue();
            }


            /** Tests whether the value of the CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration on the right hand side of the
              * >= operator is greater than or equal to the value as this CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration.
              *
              * @param rhs The CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration whose value is to be compared against the value
              *      of this CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration.
              * @return True if the value of the specified CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration (rhs) is greater
              *      than or equal to the value of this CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration, false otherwise
              */
            bool operator>=(const CVEnumISMatomicEnergyMarkingsValuesMember2& rhs) const
               throw(uci::base::UCIException)
            {
               return getValue() >= rhs.getValue();
            }


            /** Tests whether the value of this CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration is equal to the value of the
              * specified EnumerationItem.
              *
              * @param rhs The EnumerationItem whose value is to be compared against the value of this
              *      CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration.
              * @return True if the value of the EnumerationItem(rhs) is equal to the value of this
              *      CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration, false otherwise
              */
            bool operator==(EnumerationItem rhs) const
               throw(uci::base::UCIException)
            {
               testForValidItem(rhs);
               return getValue() == rhs;
            }


            /** Tests whether the value of the specified EnumerationItem is equal to the value of the specified
              * CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration.
              *
              * @param lhs The EnumerationItem whose value is to be compared against the value of the specified
              *      CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration.
              * @param rhs The CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration whose value is to be compared against the value
              *      of the specified EnumerationItem
              * @return True if the value of the EnumerationItem(lhs) is equal to the value of the
              *      CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration (rhs), false otherwise
              */
            friend bool operator==(EnumerationItem lhs, const CVEnumISMatomicEnergyMarkingsValuesMember2& rhs)
            {
               testForValidItem(lhs);
               return lhs == rhs.getValue();
            }


            /** Tests whether the value of this CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration is not equal to the value of
              * the specified EnumerationItem.
              *
              * @param rhs The EnumerationItem whose value is to be compared against the value of this
              *      CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration.
              * @return True if the value of the EnumerationItem(rhs) is not equal to the value of this
              *      CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration, false otherwise
              */
            bool operator!=(EnumerationItem rhs) const
               throw(uci::base::UCIException)
            {
               testForValidItem(rhs);
               return getValue() != rhs;
            }


            /** Tests whether the value of the specified EnumerationItem is not equal to the value of the specified
              * CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration.
              *
              * @param lhs The EnumerationItem whose value is to be compared against the value of the specified
              *      CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration.
              * @param rhs The CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration whose value is to be compared against the value
              *      of the specified EnumerationItem
              * @return True if the value of the EnumerationItem(lhs) is not equal to the value of the
              *      CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration (rhs), false otherwise
              */
            friend bool operator!=(EnumerationItem lhs, const CVEnumISMatomicEnergyMarkingsValuesMember2& rhs)
            {
               testForValidItem(lhs);
               return lhs != rhs.getValue();
            }


            /** Tests whether the value of this CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration is less than the value of the
              * specified EnumerationItem.
              *
              * @param rhs The EnumerationItem whose value is to be compared against the value of this
              *      CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration.
              * @return True if the value of the EnumerationItem(rhs) is less than the value of this
              *      CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration, false otherwise
              */
            bool operator<(EnumerationItem rhs) const
               throw(uci::base::UCIException)
            {
               testForValidItem(rhs);
               return getValue() < rhs;
            }


            /** Tests whether the value of the specified EnumerationItem is less than the value of the specified
              * CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration.
              *
              * @param lhs The EnumerationItem whose value is to be compared against the value of the specified
              *      CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration.
              * @param rhs The CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration whose value is to be compared against the value
              *      of the specified EnumerationItem
              * @return True if the value of the EnumerationItem(lhs) is less than the value of the
              *      CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration (rhs), false otherwise
              */
            friend bool operator<(EnumerationItem lhs, const CVEnumISMatomicEnergyMarkingsValuesMember2& rhs)
            {
               testForValidItem(lhs);
               return lhs < rhs.getValue();
            }


            /** Tests whether the value of this CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration is less than or equal to the
              * value of the specified EnumerationItem.
              *
              * @param rhs The EnumerationItem whose value is to be compared against the value of this
              *      CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration.
              * @return True if the value of the EnumerationItem(rhs) is less than or equal to the value of this
              *      CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration, false otherwise
              */
            bool operator<=(EnumerationItem rhs) const
               throw(uci::base::UCIException)
            {
               testForValidItem(rhs);
               return getValue() <= rhs;
            }


            /** Tests whether the value of the specified EnumerationItem is less than or equal to the value of the specified
              * CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration.
              *
              * @param lhs The EnumerationItem whose value is to be compared against the value of the specified
              *      CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration.
              * @param rhs The CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration whose value is to be compared against the value
              *      of the specified EnumerationItem
              * @return True if the value of the EnumerationItem(lhs) is less than or equal to the value of the
              *      CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration (rhs), false otherwise
              */
            friend bool operator<=(EnumerationItem lhs, const CVEnumISMatomicEnergyMarkingsValuesMember2& rhs)
            {
               testForValidItem(lhs);
               return lhs <= rhs.getValue();
            }


            /** Tests whether the value of this CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration is greater than the value of
              * the specified EnumerationItem.
              *
              * @param rhs The EnumerationItem whose value is to be compared against the value of this
              *      CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration.
              * @return True if the value of the EnumerationItem(rhs) is greater than the value of this
              *      CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration, false otherwise
              */
            bool operator>(EnumerationItem rhs) const
               throw(uci::base::UCIException)
            {
               testForValidItem(rhs);
               return getValue() > rhs;
            }


            /** Tests whether the value of the specified EnumerationItem is greater than the value of the specified
              * CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration.
              *
              * @param lhs The EnumerationItem whose value is to be compared against the value of the specified
              *      CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration.
              * @param rhs The CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration whose value is to be compared against the value
              *      of the specified EnumerationItem
              * @return True if the value of the EnumerationItem(lhs) is greater than the value of the
              *      CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration (rhs), false otherwise
              */
            friend bool operator>(EnumerationItem lhs, const CVEnumISMatomicEnergyMarkingsValuesMember2& rhs)
            {
               testForValidItem(lhs);
               return lhs > rhs.getValue();
            }


            /** Tests whether the value of this CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration is greater than or equal to
              * the value of the specified EnumerationItem.
              *
              * @param rhs The EnumerationItem whose value is to be compared against the value of this
              *      CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration.
              * @return True if the value of the EnumerationItem(rhs) is greater than or equal to the value of this
              *      CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration, false otherwise
              */
            bool operator>=(EnumerationItem rhs) const
               throw(uci::base::UCIException)
            {
               testForValidItem(rhs);
               return getValue() >= rhs;
            }


            /** Tests whether the value of the specified EnumerationItem is greater than or equal to the value of the specified
              * CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration.
              *
              * @param lhs The EnumerationItem whose value is to be compared against the value of the specified
              *      CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration.
              * @param rhs The CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration whose value is to be compared against the value
              *      of the specified EnumerationItem
              * @return True if the value of the EnumerationItem(lhs) is greater than or equal to the value of the
              *      CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration (rhs), false otherwise
              */
            friend bool operator>=(EnumerationItem lhs, const CVEnumISMatomicEnergyMarkingsValuesMember2& rhs)
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


            /** Return the name of the enumeration item that the value of this CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration
              * is set to.
              *
              * @return The name of the enumeration item.
              */
            virtual const std::string& toName() const
               throw(uci::base::UCIException)
            {
               return toName(getValue());
            }


            /** Sets the value of this CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration to the value of the enumeration item
              * having the specified name.
              *
              * @param itemName The name of the enumeration item whose value is to be used to set this
              *      CVEnumISMatomicEnergyMarkingsValuesMember2 enumeration to
              */
            virtual void setValueFromName(const std::string& itemName)
               throw(uci::base::UCIException)
            {
               setValue(fromName(itemName));
            }



         protected:

            /** The default constructor. In addition to constructing a new CVEnumISMatomicEnergyMarkingsValuesMember2, this
              * constructor also makes sure the conversion tables are initialized
              */
            CVEnumISMatomicEnergyMarkingsValuesMember2()
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
              * @param rhs The CVEnumISMatomicEnergyMarkingsValuesMember2 to copy from
              */
            CVEnumISMatomicEnergyMarkingsValuesMember2(const CVEnumISMatomicEnergyMarkingsValuesMember2& rhs)
            {
               (void)rhs;
            }

         }; // CVEnumISMatomicEnergyMarkingsValuesMember2


         /** The destructor */
         virtual ~CVEnumISMatomicEnergyMarkingsValues()
         { }

         /** Returns this accessor's type constant, i.e. CVEnumISMatomicEnergyMarkingsValues
           *
           * @return This accessor's type constant, i.e. CVEnumISMatomicEnergyMarkingsValues
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return ismatomic::type::accessorType::cVEnumISMatomicEnergyMarkingsValues;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CVEnumISMatomicEnergyMarkingsValues& accessor)
            throw(uci::base::UCIException) = 0;


         /** The following enumeration is used to identify which element of this Choice has been chosen. */
         enum CVEnumISMatomicEnergyMarkingsValuesMember {
            CVENUMISMATOMICENERGYMARKINGSVALUESMEMBER_NONE,
            CVENUMISMATOMICENERGYMARKINGSVALUESMEMBER_MEMBER1,
            CVENUMISMATOMICENERGYMARKINGSVALUESMEMBER_MEMBER2
         };


         /** This method returns this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active.
           *
           * @return The selected item's enumerated value
           */
         virtual CVEnumISMatomicEnergyMarkingsValuesMember getCVEnumISMatomicEnergyMarkingsValuesUnionOrdinal() const
            throw(uci::base::UCIException) = 0;


         /** This method is used to set this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active. There are two mechanisms that can be used to set a choice's "selection
           * ordinal." The first mechanism is by invoking this method. The second mechanism is by invoking one of the set methods
           * associated with one of the elements contained in this choice. Once this method is invoked, the value returned by
           * getCVEnumISMatomicEnergyMarkingsValuesChoiceOrdinal() will be the ordinal specified when this method was invoked. In
           * addition, the access methods associated with the chosen element will be enabled and will provide access to the chosen
           * element.
           *
           * @param ordinal The ordinal to set this choice's selected ordinal to.
           * @param type = uci::base::accessorType::null The type of data that is to be made available when the ordinal is set.
           *      The use of this parameter provides support for inheritable types. This parameter defaults to
           *      uci::base::accessorType::null that is used to indicate that the access methods associated with the chosen
           *      element shall provide access to data of the type that was specified for that element in the choice in the OMS
           *      schema, i.e. the chosen element's base type. If specified, this field must either be a type ID associated with
           *      the chosen element's base type or a type ID associated with a type that is derived from the chosen element's
           *      base type.
           */
         virtual void setCVEnumISMatomicEnergyMarkingsValuesUnionOrdinal(CVEnumISMatomicEnergyMarkingsValuesMember ordinal, uci::base::accessorType::AccessorType type = uci::base::accessorType::null)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Member1.
           *
           * @return The value of the string data type identified by Member1.
           */
         virtual const ismatomic::type::CVEnumISMatomicEnergyMarkingsValues::CVEnumISMatomicEnergyMarkingsValuesMember1& getMember1() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Member1.
           *
           * @return The value of the string data type identified by Member1.
           */
         virtual ismatomic::type::CVEnumISMatomicEnergyMarkingsValues::CVEnumISMatomicEnergyMarkingsValuesMember1& getMember1()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Member1 to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setMember1(const ismatomic::type::CVEnumISMatomicEnergyMarkingsValues::CVEnumISMatomicEnergyMarkingsValuesMember1& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Member1 to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setMember1(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Member1 to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setMember1(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Member2.
           *
           * @return The value of the enumeration identified by Member2.
           */
         virtual const ismatomic::type::CVEnumISMatomicEnergyMarkingsValues::CVEnumISMatomicEnergyMarkingsValuesMember2& getMember2() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Member2.
           *
           * @return The value of the enumeration identified by Member2.
           */
         virtual ismatomic::type::CVEnumISMatomicEnergyMarkingsValues::CVEnumISMatomicEnergyMarkingsValuesMember2& getMember2()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Member2.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setMember2(const ismatomic::type::CVEnumISMatomicEnergyMarkingsValues::CVEnumISMatomicEnergyMarkingsValuesMember2& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Member2.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setMember2(ismatomic::type::CVEnumISMatomicEnergyMarkingsValues::CVEnumISMatomicEnergyMarkingsValuesMember2::EnumerationItem value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CVEnumISMatomicEnergyMarkingsValues()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CVEnumISMatomicEnergyMarkingsValues to copy from
           */
         CVEnumISMatomicEnergyMarkingsValues(const CVEnumISMatomicEnergyMarkingsValues& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CVEnumISMatomicEnergyMarkingsValues to the contents of the
           * CVEnumISMatomicEnergyMarkingsValues on the right hand side (rhs) of the assignment
           * operator.CVEnumISMatomicEnergyMarkingsValues [only available to derived classes]
           *
           * @param rhs The CVEnumISMatomicEnergyMarkingsValues on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this ismatomic::type::CVEnumISMatomicEnergyMarkingsValues
           * @return A constant reference to this CVEnumISMatomicEnergyMarkingsValues.
           */
         const CVEnumISMatomicEnergyMarkingsValues& operator=(const CVEnumISMatomicEnergyMarkingsValues& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CVEnumISMatomicEnergyMarkingsValues


   } // Namespace: type
} // Namespace: ismatomic

#endif // Ismatomic__Type__CVEnumISMatomicEnergyMarkingsValues_h

