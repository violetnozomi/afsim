// This file was generated  on 12/5/2018 at 1:1:48 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__StrikeCapabilityType_h
#define Uci__Type__StrikeCapabilityType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CapabilityID_Type_h)
# include "uci/type/CapabilityID_Type.h"
#endif

#if !defined(Uci__Type__StoreType_h)
# include "uci/type/StoreType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__CapabilityControlInterfacesEnum_h)
# include "uci/type/CapabilityControlInterfacesEnum.h"
#endif

#if !defined(Uci__Type__StrikeCapabilityOptionsType_h)
# include "uci/type/StrikeCapabilityOptionsType.h"
#endif

#if !defined(Uci__Type__AO_CodeType_h)
# include "uci/type/AO_CodeType.h"
#endif

#if !defined(Uci__Type__StrikeMessageOutputsEnum_h)
# include "uci/type/StrikeMessageOutputsEnum.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__StoreInformationType_h)
# include "uci/type/StoreInformationType.h"
#endif

#if !defined(Uci__Type__StoreVerificationStatusType_h)
# include "uci/type/StoreVerificationStatusType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the StrikeCapabilityType sequence accessor class */
      class StrikeCapabilityType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~StrikeCapabilityType()
         { }

         /** Returns this accessor's type constant, i.e. StrikeCapabilityType
           *
           * @return This accessor's type constant, i.e. StrikeCapabilityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::strikeCapabilityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const StrikeCapabilityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates an accepted control interface for the Capability. See enumeration annotations for further details. [Maximum
           * occurrences: 7]
           */
         typedef uci::base::BoundedList<uci::type::CapabilityControlInterfacesEnum, uci::type::accessorType::capabilityControlInterfacesEnum> AcceptedInterface;

         /** For optical/laser guided weapons, indicates a PRF and/or PIM code supported by the Capability. [Minimum occurrences:
           * 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::AO_CodeType, uci::type::accessorType::aO_CodeType> SupportedCode;

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


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityType.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityType.
           */
         virtual const uci::type::StoreType& getCapabilityType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityType.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityType.
           */
         virtual uci::type::StoreType& getCapabilityType()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CapabilityType to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CapabilityType
           */
         virtual void setCapabilityType(const uci::type::StoreType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AcceptedInterface.
           *
           * @return The bounded list identified by AcceptedInterface.
           */
         virtual const uci::type::StrikeCapabilityType::AcceptedInterface& getAcceptedInterface() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AcceptedInterface.
           *
           * @return The bounded list identified by AcceptedInterface.
           */
         virtual uci::type::StrikeCapabilityType::AcceptedInterface& getAcceptedInterface()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AcceptedInterface.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAcceptedInterface(const uci::type::StrikeCapabilityType::AcceptedInterface& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityOptions.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityOptions.
           */
         virtual const uci::type::StrikeCapabilityOptionsType& getCapabilityOptions() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityOptions.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityOptions.
           */
         virtual uci::type::StrikeCapabilityOptionsType& getCapabilityOptions()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CapabilityOptions to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CapabilityOptions
           */
         virtual void setCapabilityOptions(const uci::type::StrikeCapabilityOptionsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SupportedCode.
           *
           * @return The bounded list identified by SupportedCode.
           */
         virtual const uci::type::StrikeCapabilityType::SupportedCode& getSupportedCode() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SupportedCode.
           *
           * @return The bounded list identified by SupportedCode.
           */
         virtual uci::type::StrikeCapabilityType::SupportedCode& getSupportedCode()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SupportedCode.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSupportedCode(const uci::type::StrikeCapabilityType::SupportedCode& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the MessageOutput.
           *
           * @return The value of the enumeration identified by MessageOutput.
           */
         virtual const uci::type::StrikeMessageOutputsEnum& getMessageOutput() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the MessageOutput.
           *
           * @return The value of the enumeration identified by MessageOutput.
           */
         virtual uci::type::StrikeMessageOutputsEnum& getMessageOutput()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the MessageOutput.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setMessageOutput(const uci::type::StrikeMessageOutputsEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the MessageOutput.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setMessageOutput(uci::type::StrikeMessageOutputsEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield MessageOutputis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasMessageOutput() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getMessageOutput will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::strikeMessageOutputsEnum This Accessor's accessor type
           */
         virtual void enableMessageOutput(uci::base::accessorType::AccessorType type = uci::type::accessorType::strikeMessageOutputsEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearMessageOutput()
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


         /** Returns the accessor that provides access to the complex content that is identified by the StoreInformation.
           *
           * @return The acecssor that provides access to the complex content that is identified by StoreInformation.
           */
         virtual const uci::type::StoreInformationType& getStoreInformation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the StoreInformation.
           *
           * @return The acecssor that provides access to the complex content that is identified by StoreInformation.
           */
         virtual uci::type::StoreInformationType& getStoreInformation()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the StoreInformation to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by StoreInformation
           */
         virtual void setStoreInformation(const uci::type::StoreInformationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by StoreInformation exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by StoreInformation is emabled or not
           */
         virtual bool hasStoreInformation() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by StoreInformation
           *
           * @param type = uci::type::accessorType::storeInformationType This Accessor's accessor type
           */
         virtual void enableStoreInformation(uci::base::accessorType::AccessorType type = uci::type::accessorType::storeInformationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by StoreInformation */
         virtual void clearStoreInformation()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Verification.
           *
           * @return The acecssor that provides access to the complex content that is identified by Verification.
           */
         virtual const uci::type::StoreVerificationStatusType& getVerification() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Verification.
           *
           * @return The acecssor that provides access to the complex content that is identified by Verification.
           */
         virtual uci::type::StoreVerificationStatusType& getVerification()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Verification to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Verification
           */
         virtual void setVerification(const uci::type::StoreVerificationStatusType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Verification exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Verification is emabled or not
           */
         virtual bool hasVerification() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Verification
           *
           * @param type = uci::type::accessorType::storeVerificationStatusType This Accessor's accessor type
           */
         virtual void enableVerification(uci::base::accessorType::AccessorType type = uci::type::accessorType::storeVerificationStatusType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Verification */
         virtual void clearVerification()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         StrikeCapabilityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The StrikeCapabilityType to copy from
           */
         StrikeCapabilityType(const StrikeCapabilityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this StrikeCapabilityType to the contents of the StrikeCapabilityType
           * on the right hand side (rhs) of the assignment operator.StrikeCapabilityType [only available to derived classes]
           *
           * @param rhs The StrikeCapabilityType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::StrikeCapabilityType
           * @return A constant reference to this StrikeCapabilityType.
           */
         const StrikeCapabilityType& operator=(const StrikeCapabilityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // StrikeCapabilityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__StrikeCapabilityType_h

