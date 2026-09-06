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
#ifndef Uci__Type__ModulationEnum_h
#define Uci__Type__ModulationEnum_h 1

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

      /** Indicates the type of signal modulation. */
      class ModulationEnum : public virtual uci::base::Accessor {
      public:

         //  The following are the enumerated items that comprise this enumeration
         enum EnumerationItem {
            //  Enumerated item that indicates that the enumeration has not been set
            enumNotSet,
            //  UCI_AM's ordinal value */
            UCI_AM,

            //  UCI_FM's ordinal value */
            UCI_FM,

            //  UCI_PM's ordinal value */
            UCI_PM,

            //  UCI_TONEAM's ordinal value */
            UCI_TONEAM,

            //  UCI_TONEFM's ordinal value */
            UCI_TONEFM,

            //  UCI_PSK's ordinal value */
            UCI_PSK,

            //  UCI_BPSK's ordinal value */
            UCI_BPSK,

            //  UCI_QPSK's ordinal value */
            UCI_QPSK,

            //  UCI_PSK8's ordinal value */
            UCI_PSK8,

            //  UCI_PSK16's ordinal value */
            UCI_PSK16,

            //  UCI_PSK32's ordinal value */
            UCI_PSK32,

            //  UCI_PSK64's ordinal value */
            UCI_PSK64,

            //  UCI_DPSK's ordinal value */
            UCI_DPSK,

            //  UCI_OQPSK's ordinal value */
            UCI_OQPSK,

            //  UCI_PI4QPSK's ordinal value */
            UCI_PI4QPSK,

            //  UCI_FSK's ordinal value */
            UCI_FSK,

            //  UCI_BFSK's ordinal value */
            UCI_BFSK,

            //  UCI_QFSK's ordinal value */
            UCI_QFSK,

            //  UCI_FSK8's ordinal value */
            UCI_FSK8,

            //  UCI_FSK16's ordinal value */
            UCI_FSK16,

            //  UCI_FSK32's ordinal value */
            UCI_FSK32,

            //  UCI_FSK64's ordinal value */
            UCI_FSK64,

            //  UCI_AFSK's ordinal value */
            UCI_AFSK,

            //  UCI_MFSK's ordinal value */
            UCI_MFSK,

            //  UCI_DTMF's ordinal value */
            UCI_DTMF,

            //  UCI_ASK's ordinal value */
            UCI_ASK,

            //  UCI_OOK's ordinal value */
            UCI_OOK,

            //  UCI_QAM's ordinal value */
            UCI_QAM,

            //  UCI_CPM's ordinal value */
            UCI_CPM,

            //  UCI_MSK's ordinal value */
            UCI_MSK,

            //  UCI_BMSK's ordinal value */
            UCI_BMSK,

            //  UCI_QMSK's ordinal value */
            UCI_QMSK,

            //  UCI_MSK8's ordinal value */
            UCI_MSK8,

            //  UCI_MSK16's ordinal value */
            UCI_MSK16,

            //  UCI_MSK32's ordinal value */
            UCI_MSK32,

            //  UCI_MSK64's ordinal value */
            UCI_MSK64,

            //  UCI_GMSK's ordinal value */
            UCI_GMSK,

            //  UCI_CPFSK's ordinal value */
            UCI_CPFSK,

            //  UCI_OFDM's ordinal value */
            UCI_OFDM,

            //  UCI_DMT's ordinal value */
            UCI_DMT,

            //  UCI_WAVELET's ordinal value */
            UCI_WAVELET,

            //  UCI_TCM's ordinal value */
            UCI_TCM,

            //  UCI_DSSS's ordinal value */
            UCI_DSSS,

            //  UCI_CSS's ordinal value */
            UCI_CSS,

            //  UCI_FHSS's ordinal value */
            UCI_FHSS,

            //  UCI_SIM's ordinal value */
            UCI_SIM,

            //  UCI_SSB's ordinal value */
            UCI_SSB,

            //  UCI_SSBU's ordinal value */
            UCI_SSBU,

            //  UCI_SSBL's ordinal value */
            UCI_SSBL,

            //  UCI_PULSED's ordinal value */
            UCI_PULSED,

            //  UCI_CW's ordinal value */
            UCI_CW,

            //  UCI_PULSE_MODULATION's ordinal value */
            UCI_PULSE_MODULATION,

            //  UCI_PULSE_STAGGER's ordinal value */
            UCI_PULSE_STAGGER,

            //  UCI_PULSE_DOPPLER's ordinal value */
            UCI_PULSE_DOPPLER,

            //  UCI_NOISE_JAM's ordinal value */
            UCI_NOISE_JAM,

            //  UCI_CODE_MODULATION_PULSE's ordinal value */
            UCI_CODE_MODULATION_PULSE,

            //  UCI_TIME_DIVISION's ordinal value */
            UCI_TIME_DIVISION,

            //  UCI_VENDORSPECIFIC's ordinal value */
            UCI_VENDORSPECIFIC,

            //  UCI_NONE's ordinal value */
            UCI_NONE,

            //  Enumerated item that indicates maximum enumerated item exclusive of this item
            enumMaxExclusive
         };


         /** The destructor */
         virtual ~ModulationEnum()
         { }

         /** Returns this accessor's type constant, i.e. modulationEnum
           *
           * @return This accessor's type constant, i.e. modulationEnum
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::modulationEnum;
         }


         /** The assignment operator. Sets the contents of this ModulationEnum to the contents of the ModulationEnum on the right
           * hand side (rhs) of the assignment operator.ModulationEnum
           *
           * @param rhs The ModulationEnum on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this ModulationEnum
           * @return A constant reference to this ModulationEnum.
           */
         const ModulationEnum& operator=(const ModulationEnum& rhs)
         {
            setValue(rhs.getValue());
            return *this;
         }


         /** The assignment operator. Sets the value of this ModulationEnum enumeration to the EnumerationItem on the right hand
           * side of the assignment operator.
           *
           * @param rhs The EnumerationItemwhose value is to be assigned to this ModulationEnum enumeration
           * @return A reference to this ModulationEnum enumeration
           */
         const ModulationEnum& operator=(EnumerationItem rhs)
         {
            setValue(rhs);
            return *this;
         }


         /** Sets the value of this ModulationEnum enumeration to the value of the specified enumeration item.
           *
           * @param item The EnumerationItemwhose value is to be used to set the value of this ModulationEnum enumeration
           */
         virtual void setValue(EnumerationItem item)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of this ModulationEnum enumeration.
           *
           * @param testForValidity=true Specifies whether this ModulationEnum enumeration should be validated (true) or not
           *      (false) as part of retrieving its value.
           * @return The value of this ModulationEnum enumeration.
           */
         virtual EnumerationItem getValue(bool testForValidity=true) const
            throw(uci::base::UCIException) = 0;


         /** Returns the number of items in this ModulationEnum enumeration.
           *
           * @return The number of items in this ModulationEnum enumeration.
           */
         int getNumberOfItems() const
         {
            return 59;
         }


         /** Returns whether this ModulationEnum enumeration is valid or not. A ModulationEnum enumeration is valid if its value
           * is one of the enumeration items other than enumNotSet and enumMaxExclusive.
           *
           * @return A flag indicating whether this ModulationEnum enumeration is valid (true) or not (false).
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


         /** Tests whether the value of the ModulationEnum enumeration on the right hand side of the == operator is equal to the
           * value as this ModulationEnum enumeration.
           *
           * @param rhs The ModulationEnum enumeration whose value is to be compared against the value of this ModulationEnum
           *      enumeration.
           * @return True if the value of the specified ModulationEnum enumeration (rhs) is equal to the value of this
           *      ModulationEnum enumeration, false otherwise
           */
         bool operator==(const ModulationEnum& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() == rhs.getValue();
         }


         /** Tests whether the value of the ModulationEnum enumeration on the right hand side of the != operator is not equal to
           * the value as this ModulationEnum enumeration.
           *
           * @param rhs The ModulationEnum enumeration whose value is to be compared against the value of this ModulationEnum
           *      enumeration.
           * @return True if the value of the specified ModulationEnum enumeration (rhs) is not equal to the value of this
           *      ModulationEnum enumeration, false otherwise
           */
         bool operator!=(const ModulationEnum& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() != rhs.getValue();
         }


         /** Tests whether the value of the ModulationEnum enumeration on the right hand side of the < operator is less than the
           * value as this ModulationEnum enumeration.
           *
           * @param rhs The ModulationEnum enumeration whose value is to be compared against the value of this ModulationEnum
           *      enumeration.
           * @return True if the value of the specified ModulationEnum enumeration (rhs) is less than the value of this
           *      ModulationEnum enumeration, false otherwise
           */
         bool operator<(const ModulationEnum& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() < rhs.getValue();
         }


         /** Tests whether the value of the ModulationEnum enumeration on the right hand side of the <= operator is less than or
           * equal to the value as this ModulationEnum enumeration.
           *
           * @param rhs The ModulationEnum enumeration whose value is to be compared against the value of this ModulationEnum
           *      enumeration.
           * @return True if the value of the specified ModulationEnum enumeration (rhs) is less than or equal to the value of
           *      this ModulationEnum enumeration, false otherwise
           */
         bool operator<=(const ModulationEnum& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() <= rhs.getValue();
         }


         /** Tests whether the value of the ModulationEnum enumeration on the right hand side of the > operator is greater than
           * the value as this ModulationEnum enumeration.
           *
           * @param rhs The ModulationEnum enumeration whose value is to be compared against the value of this ModulationEnum
           *      enumeration.
           * @return True if the value of the specified ModulationEnum enumeration (rhs) is greater than the value of this
           *      ModulationEnum enumeration, false otherwise
           */
         bool operator>(const ModulationEnum& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() > rhs.getValue();
         }


         /** Tests whether the value of the ModulationEnum enumeration on the right hand side of the >= operator is greater than
           * or equal to the value as this ModulationEnum enumeration.
           *
           * @param rhs The ModulationEnum enumeration whose value is to be compared against the value of this ModulationEnum
           *      enumeration.
           * @return True if the value of the specified ModulationEnum enumeration (rhs) is greater than or equal to the value of
           *      this ModulationEnum enumeration, false otherwise
           */
         bool operator>=(const ModulationEnum& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() >= rhs.getValue();
         }


         /** Tests whether the value of this ModulationEnum enumeration is equal to the value of the specified EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this ModulationEnum enumeration.
           * @return True if the value of the EnumerationItem(rhs) is equal to the value of this ModulationEnum enumeration, false
           *      otherwise
           */
         bool operator==(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() == rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is equal to the value of the specified ModulationEnum
           * enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified ModulationEnum
           *      enumeration.
           * @param rhs The ModulationEnum enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is equal to the value of the ModulationEnum enumeration (rhs),
           *      false otherwise
           */
         friend bool operator==(EnumerationItem lhs, const ModulationEnum& rhs)
         {
            testForValidItem(lhs);
            return lhs == rhs.getValue();
         }


         /** Tests whether the value of this ModulationEnum enumeration is not equal to the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this ModulationEnum enumeration.
           * @return True if the value of the EnumerationItem(rhs) is not equal to the value of this ModulationEnum enumeration,
           *      false otherwise
           */
         bool operator!=(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() != rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is not equal to the value of the specified ModulationEnum
           * enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified ModulationEnum
           *      enumeration.
           * @param rhs The ModulationEnum enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is not equal to the value of the ModulationEnum enumeration
           *      (rhs), false otherwise
           */
         friend bool operator!=(EnumerationItem lhs, const ModulationEnum& rhs)
         {
            testForValidItem(lhs);
            return lhs != rhs.getValue();
         }


         /** Tests whether the value of this ModulationEnum enumeration is less than the value of the specified EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this ModulationEnum enumeration.
           * @return True if the value of the EnumerationItem(rhs) is less than the value of this ModulationEnum enumeration,
           *      false otherwise
           */
         bool operator<(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() < rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is less than the value of the specified ModulationEnum
           * enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified ModulationEnum
           *      enumeration.
           * @param rhs The ModulationEnum enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is less than the value of the ModulationEnum enumeration (rhs),
           *      false otherwise
           */
         friend bool operator<(EnumerationItem lhs, const ModulationEnum& rhs)
         {
            testForValidItem(lhs);
            return lhs < rhs.getValue();
         }


         /** Tests whether the value of this ModulationEnum enumeration is less than or equal to the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this ModulationEnum enumeration.
           * @return True if the value of the EnumerationItem(rhs) is less than or equal to the value of this ModulationEnum
           *      enumeration, false otherwise
           */
         bool operator<=(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() <= rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is less than or equal to the value of the specified
           * ModulationEnum enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified ModulationEnum
           *      enumeration.
           * @param rhs The ModulationEnum enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is less than or equal to the value of the ModulationEnum
           *      enumeration (rhs), false otherwise
           */
         friend bool operator<=(EnumerationItem lhs, const ModulationEnum& rhs)
         {
            testForValidItem(lhs);
            return lhs <= rhs.getValue();
         }


         /** Tests whether the value of this ModulationEnum enumeration is greater than the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this ModulationEnum enumeration.
           * @return True if the value of the EnumerationItem(rhs) is greater than the value of this ModulationEnum enumeration,
           *      false otherwise
           */
         bool operator>(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() > rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is greater than the value of the specified ModulationEnum
           * enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified ModulationEnum
           *      enumeration.
           * @param rhs The ModulationEnum enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is greater than the value of the ModulationEnum enumeration
           *      (rhs), false otherwise
           */
         friend bool operator>(EnumerationItem lhs, const ModulationEnum& rhs)
         {
            testForValidItem(lhs);
            return lhs > rhs.getValue();
         }


         /** Tests whether the value of this ModulationEnum enumeration is greater than or equal to the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this ModulationEnum enumeration.
           * @return True if the value of the EnumerationItem(rhs) is greater than or equal to the value of this ModulationEnum
           *      enumeration, false otherwise
           */
         bool operator>=(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() >= rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is greater than or equal to the value of the specified
           * ModulationEnum enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified ModulationEnum
           *      enumeration.
           * @param rhs The ModulationEnum enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is greater than or equal to the value of the ModulationEnum
           *      enumeration (rhs), false otherwise
           */
         friend bool operator>=(EnumerationItem lhs, const ModulationEnum& rhs)
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


         /** Return the name of the enumeration item that the value of this ModulationEnum enumeration is set to.
           *
           * @return The name of the enumeration item.
           */
         virtual const std::string& toName() const
            throw(uci::base::UCIException)
         {
            return toName(getValue());
         }


         /** Sets the value of this ModulationEnum enumeration to the value of the enumeration item having the specified name.
           *
           * @param itemName The name of the enumeration item whose value is to be used to set this ModulationEnum enumeration to
           */
         virtual void setValueFromName(const std::string& itemName)
            throw(uci::base::UCIException)
         {
            setValue(fromName(itemName));
         }



      protected:

         /** The default constructor. In addition to constructing a new ModulationEnum, this constructor also makes sure the
           * conversion tables are initialized
           */
         ModulationEnum()
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
           * @param rhs The ModulationEnum to copy from
           */
         ModulationEnum(const ModulationEnum& rhs)
         {
            (void)rhs;
         }

      }; // ModulationEnum


   } // Namespace: type
} // Namespace: uci
template<typename charT, typename traits>
std::basic_ostream<charT, traits>&
operator<<(std::basic_ostream<charT, traits>& oStream, const uci::type::ModulationEnum& enumeration)
{
   oStream << enumeration.toName();
   return oStream;
}

#endif // Uci__Type__ModulationEnum_h

