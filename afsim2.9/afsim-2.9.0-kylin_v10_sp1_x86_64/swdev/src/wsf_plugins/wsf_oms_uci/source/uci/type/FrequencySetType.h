// This file was generated  on 12/5/2018 at 1:1:46 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__FrequencySetType_h
#define Uci__Type__FrequencySetType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__FrequencyRangeType_h)
# include "uci/type/FrequencyRangeType.h"
#endif

#if !defined(Uci__Type__ModulationEnum_h)
# include "uci/type/ModulationEnum.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__FrequencyMultiChannelType_h)
# include "uci/type/FrequencyMultiChannelType.h"
#endif

#if !defined(Uci__Type__RF_SignalContentEnum_h)
# include "uci/type/RF_SignalContentEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Modeled after SensorSpecificData in the ACTDF standard. */
      class FrequencySetType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~FrequencySetType()
         { }

         /** Returns this accessor's type constant, i.e. FrequencySetType
           *
           * @return This accessor's type constant, i.e. FrequencySetType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::frequencySetType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const FrequencySetType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates frequency(ies), expressed by either a single frequency, frequency range, or any multiple and combination of
           * the two. [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::FrequencyRangeType, uci::type::accessorType::frequencyRangeType> FrequencyRange;

         /** Indicates the signal modulation. [Minimum occurrences: 0] [Maximum occurrences: 59] */
         typedef uci::base::BoundedList<uci::type::ModulationEnum, uci::type::accessorType::modulationEnum> Modulation;

         /** Returns the bounded list that is identified by the FrequencyRange.
           *
           * @return The bounded list identified by FrequencyRange.
           */
         virtual const uci::type::FrequencySetType::FrequencyRange& getFrequencyRange() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the FrequencyRange.
           *
           * @return The bounded list identified by FrequencyRange.
           */
         virtual uci::type::FrequencySetType::FrequencyRange& getFrequencyRange()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the FrequencyRange.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setFrequencyRange(const uci::type::FrequencySetType::FrequencyRange& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Modulation.
           *
           * @return The bounded list identified by Modulation.
           */
         virtual const uci::type::FrequencySetType::Modulation& getModulation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Modulation.
           *
           * @return The bounded list identified by Modulation.
           */
         virtual uci::type::FrequencySetType::Modulation& getModulation()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Modulation.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setModulation(const uci::type::FrequencySetType::Modulation& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Encrypted.
           *
           * @return The value of the simple primitive data type identified by Encrypted.
           */
         virtual xs::Boolean getEncrypted() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Encrypted.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setEncrypted(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Encrypted exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Encrypted is emabled or not
           */
         virtual bool hasEncrypted() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Encrypted
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableEncrypted(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Encrypted */
         virtual void clearEncrypted()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MultiChannel.
           *
           * @return The acecssor that provides access to the complex content that is identified by MultiChannel.
           */
         virtual const uci::type::FrequencyMultiChannelType& getMultiChannel() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MultiChannel.
           *
           * @return The acecssor that provides access to the complex content that is identified by MultiChannel.
           */
         virtual uci::type::FrequencyMultiChannelType& getMultiChannel()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MultiChannel to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MultiChannel
           */
         virtual void setMultiChannel(const uci::type::FrequencyMultiChannelType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MultiChannel exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MultiChannel is emabled or not
           */
         virtual bool hasMultiChannel() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MultiChannel
           *
           * @param type = uci::type::accessorType::frequencyMultiChannelType This Accessor's accessor type
           */
         virtual void enableMultiChannel(uci::base::accessorType::AccessorType type = uci::type::accessorType::frequencyMultiChannelType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MultiChannel */
         virtual void clearMultiChannel()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Content.
           *
           * @return The value of the enumeration identified by Content.
           */
         virtual const uci::type::RF_SignalContentEnum& getContent() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Content.
           *
           * @return The value of the enumeration identified by Content.
           */
         virtual uci::type::RF_SignalContentEnum& getContent()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Content.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setContent(const uci::type::RF_SignalContentEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Content.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setContent(uci::type::RF_SignalContentEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield Contentis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasContent() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getContent will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::rF_SignalContentEnum This Accessor's accessor type
           */
         virtual void enableContent(uci::base::accessorType::AccessorType type = uci::type::accessorType::rF_SignalContentEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearContent()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         FrequencySetType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The FrequencySetType to copy from
           */
         FrequencySetType(const FrequencySetType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this FrequencySetType to the contents of the FrequencySetType on the
           * right hand side (rhs) of the assignment operator.FrequencySetType [only available to derived classes]
           *
           * @param rhs The FrequencySetType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::FrequencySetType
           * @return A constant reference to this FrequencySetType.
           */
         const FrequencySetType& operator=(const FrequencySetType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // FrequencySetType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__FrequencySetType_h

