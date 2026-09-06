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
#ifndef Uci__Type__AMTI_CapabilityType_h
#define Uci__Type__AMTI_CapabilityType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CapabilityID_Type_h)
# include "uci/type/CapabilityID_Type.h"
#endif

#if !defined(Uci__Type__AMTI_CapabilityEnum_h)
# include "uci/type/AMTI_CapabilityEnum.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__AMTI_SubCapabilityEnum_h)
# include "uci/type/AMTI_SubCapabilityEnum.h"
#endif

#if !defined(Uci__Type__CapabilityControlInterfacesEnum_h)
# include "uci/type/CapabilityControlInterfacesEnum.h"
#endif

#if !defined(Uci__Type__AMTI_CapabilityOptionsType_h)
# include "uci/type/AMTI_CapabilityOptionsType.h"
#endif

#if !defined(Uci__Type__FrequencyRangeType_h)
# include "uci/type/FrequencyRangeType.h"
#endif

#if !defined(Uci__Type__ProductOutputType_h)
# include "uci/type/ProductOutputType.h"
#endif

#if !defined(Uci__Type__AMTI_MessageOutputsEnum_h)
# include "uci/type/AMTI_MessageOutputsEnum.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the AMTI_CapabilityType sequence accessor class */
      class AMTI_CapabilityType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~AMTI_CapabilityType()
         { }

         /** Returns this accessor's type constant, i.e. AMTI_CapabilityType
           *
           * @return This accessor's type constant, i.e. AMTI_CapabilityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::aMTI_CapabilityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AMTI_CapabilityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a SubCapability of the AMTI Capability; the second tier in the taxonomy of AMTI. For AMTI, the second tier
           * is waveform category. See enumeration annotations for further details. [Maximum occurrences: 3]
           */
         typedef uci::base::BoundedList<uci::type::AMTI_SubCapabilityEnum, uci::type::accessorType::aMTI_SubCapabilityEnum> SubCapabilityType;

         /** Indicates an accepted control interface for the Capability. See enumeration annotations for further details. [Maximum
           * occurrences: 7]
           */
         typedef uci::base::BoundedList<uci::type::CapabilityControlInterfacesEnum, uci::type::accessorType::capabilityControlInterfacesEnum> AcceptedInterface;

         /** Indicates a frequency range used by the Capability. [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::FrequencyRangeType, uci::type::accessorType::frequencyRangeType> SupportedFrequencies;

         /** Indicates a Product that is an output of the Capability. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ProductOutputType, uci::type::accessorType::productOutputType> ProductOutput;

         /** Indicates a message that is an output of the Capability. See enumerated type annotations for details. [Minimum
           * occurrences: 0] [Maximum occurrences: 2]
           */
         typedef uci::base::BoundedList<uci::type::AMTI_MessageOutputsEnum, uci::type::accessorType::aMTI_MessageOutputsEnum> MessageOutput;

         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityID.
           */
         virtual const uci::type::CapabilityID_Type& getCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityID.
           */
         virtual uci::type::CapabilityID_Type& getCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CapabilityID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CapabilityID
           */
         virtual void setCapabilityID(const uci::type::CapabilityID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CapabilityType.
           *
           * @return The value of the enumeration identified by CapabilityType.
           */
         virtual const uci::type::AMTI_CapabilityEnum& getCapabilityType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CapabilityType.
           *
           * @return The value of the enumeration identified by CapabilityType.
           */
         virtual uci::type::AMTI_CapabilityEnum& getCapabilityType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CapabilityType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCapabilityType(const uci::type::AMTI_CapabilityEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CapabilityType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCapabilityType(uci::type::AMTI_CapabilityEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SubCapabilityType.
           *
           * @return The bounded list identified by SubCapabilityType.
           */
         virtual const uci::type::AMTI_CapabilityType::SubCapabilityType& getSubCapabilityType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SubCapabilityType.
           *
           * @return The bounded list identified by SubCapabilityType.
           */
         virtual uci::type::AMTI_CapabilityType::SubCapabilityType& getSubCapabilityType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SubCapabilityType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSubCapabilityType(const uci::type::AMTI_CapabilityType::SubCapabilityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AcceptedInterface.
           *
           * @return The bounded list identified by AcceptedInterface.
           */
         virtual const uci::type::AMTI_CapabilityType::AcceptedInterface& getAcceptedInterface() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AcceptedInterface.
           *
           * @return The bounded list identified by AcceptedInterface.
           */
         virtual uci::type::AMTI_CapabilityType::AcceptedInterface& getAcceptedInterface()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AcceptedInterface.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAcceptedInterface(const uci::type::AMTI_CapabilityType::AcceptedInterface& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityOptions.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityOptions.
           */
         virtual const uci::type::AMTI_CapabilityOptionsType& getCapabilityOptions() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityOptions.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityOptions.
           */
         virtual uci::type::AMTI_CapabilityOptionsType& getCapabilityOptions()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CapabilityOptions to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CapabilityOptions
           */
         virtual void setCapabilityOptions(const uci::type::AMTI_CapabilityOptionsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SupportedFrequencies.
           *
           * @return The bounded list identified by SupportedFrequencies.
           */
         virtual const uci::type::AMTI_CapabilityType::SupportedFrequencies& getSupportedFrequencies() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SupportedFrequencies.
           *
           * @return The bounded list identified by SupportedFrequencies.
           */
         virtual uci::type::AMTI_CapabilityType::SupportedFrequencies& getSupportedFrequencies()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SupportedFrequencies.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSupportedFrequencies(const uci::type::AMTI_CapabilityType::SupportedFrequencies& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductOutput.
           *
           * @return The bounded list identified by ProductOutput.
           */
         virtual const uci::type::AMTI_CapabilityType::ProductOutput& getProductOutput() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductOutput.
           *
           * @return The bounded list identified by ProductOutput.
           */
         virtual uci::type::AMTI_CapabilityType::ProductOutput& getProductOutput()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ProductOutput.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setProductOutput(const uci::type::AMTI_CapabilityType::ProductOutput& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MessageOutput.
           *
           * @return The bounded list identified by MessageOutput.
           */
         virtual const uci::type::AMTI_CapabilityType::MessageOutput& getMessageOutput() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MessageOutput.
           *
           * @return The bounded list identified by MessageOutput.
           */
         virtual uci::type::AMTI_CapabilityType::MessageOutput& getMessageOutput()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the MessageOutput.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setMessageOutput(const uci::type::AMTI_CapabilityType::MessageOutput& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the CommsRequired.
           *
           * @return The value of the simple primitive data type identified by CommsRequired.
           */
         virtual xs::Boolean getCommsRequired() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the CommsRequired.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setCommsRequired(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CommsRequired exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CommsRequired is emabled or not
           */
         virtual bool hasCommsRequired() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CommsRequired
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableCommsRequired(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CommsRequired */
         virtual void clearCommsRequired()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AMTI_CapabilityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AMTI_CapabilityType to copy from
           */
         AMTI_CapabilityType(const AMTI_CapabilityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AMTI_CapabilityType to the contents of the AMTI_CapabilityType on
           * the right hand side (rhs) of the assignment operator.AMTI_CapabilityType [only available to derived classes]
           *
           * @param rhs The AMTI_CapabilityType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::AMTI_CapabilityType
           * @return A constant reference to this AMTI_CapabilityType.
           */
         const AMTI_CapabilityType& operator=(const AMTI_CapabilityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AMTI_CapabilityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AMTI_CapabilityType_h

