// This file was generated  on 12/5/2018 at 1:1:45 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__COMINT_CapabilityType_h
#define Uci__Type__COMINT_CapabilityType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CapabilityID_Type_h)
# include "uci/type/CapabilityID_Type.h"
#endif

#if !defined(Uci__Type__CapabilityInitiationEnum_h)
# include "uci/type/CapabilityInitiationEnum.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__COMINT_SubCapabilityEnum_h)
# include "uci/type/COMINT_SubCapabilityEnum.h"
#endif

#if !defined(Uci__Type__SignalGeolocationMethodEnum_h)
# include "uci/type/SignalGeolocationMethodEnum.h"
#endif

#if !defined(Uci__Type__CapabilityControlInterfacesEnum_h)
# include "uci/type/CapabilityControlInterfacesEnum.h"
#endif

#if !defined(Uci__Type__COMINT_CapabilityOptionsType_h)
# include "uci/type/COMINT_CapabilityOptionsType.h"
#endif

#if !defined(Uci__Type__ESM_DwellCoverageType_h)
# include "uci/type/ESM_DwellCoverageType.h"
#endif

#if !defined(Uci__Type__ProductOutputType_h)
# include "uci/type/ProductOutputType.h"
#endif

#if !defined(Uci__Type__ESM_MessageOutputsEnum_h)
# include "uci/type/ESM_MessageOutputsEnum.h"
#endif

#if !defined(Uci__Type__ForeignKeyType_h)
# include "uci/type/ForeignKeyType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the COMINT_CapabilityType sequence accessor class */
      class COMINT_CapabilityType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~COMINT_CapabilityType()
         { }

         /** Returns this accessor's type constant, i.e. COMINT_CapabilityType
           *
           * @return This accessor's type constant, i.e. COMINT_CapabilityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::cOMINT_CapabilityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const COMINT_CapabilityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a SubCapability of the Capability, the second tier in the taxonomy. The second tier is an indication of the
           * specific collection technique. See enumeration annotations for further details. [Maximum occurrences: 16]
           */
         typedef uci::base::BoundedList<uci::type::COMINT_SubCapabilityEnum, uci::type::accessorType::cOMINT_SubCapabilityEnum> SubCapabilityType;

         /** Indicates a geolocation type/technique supported by the Capability. This element is only expected when the sibling
           * SubCapabilityType indicates geolocation. [Minimum occurrences: 0] [Maximum occurrences: 8]
           */
         typedef uci::base::BoundedList<uci::type::SignalGeolocationMethodEnum, uci::type::accessorType::signalGeolocationMethodEnum> GeolocationType;

         /** Indicates an accepted control interface for the Capability. See enumeration annotations for further details. [Maximum
           * occurrences: 7]
           */
         typedef uci::base::BoundedList<uci::type::CapabilityControlInterfacesEnum, uci::type::accessorType::capabilityControlInterfacesEnum> AcceptedInterface;

         /** Indicates a summary of the expected coverage of a Dwell(s) associated with the Capability. This element does not
           * indicate an actual instantiated, scheduled Dwell of the Capability. This element is an abstraction of the MDF
           * representation and/or design implementation of a Dwell. Actual, instantiated, scheduled Dwells are reported via the
           * Activity message. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ESM_DwellCoverageType, uci::type::accessorType::eSM_DwellCoverageType> DwellCoverage;

         /** Indicates a Product that is an output of the Capability. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ProductOutputType, uci::type::accessorType::productOutputType> ProductOutput;

         /** Indicates a message that is an output of the Capability. See enumerated type annotations for details. [Minimum
           * occurrences: 0] [Maximum occurrences: 5]
           */
         typedef uci::base::BoundedList<uci::type::ESM_MessageOutputsEnum, uci::type::accessorType::eSM_MessageOutputsEnum> MessageOutput;

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
         virtual const uci::type::CapabilityInitiationEnum& getCapabilityType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CapabilityType.
           *
           * @return The value of the enumeration identified by CapabilityType.
           */
         virtual uci::type::CapabilityInitiationEnum& getCapabilityType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CapabilityType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCapabilityType(const uci::type::CapabilityInitiationEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CapabilityType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCapabilityType(uci::type::CapabilityInitiationEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SubCapabilityType.
           *
           * @return The bounded list identified by SubCapabilityType.
           */
         virtual const uci::type::COMINT_CapabilityType::SubCapabilityType& getSubCapabilityType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SubCapabilityType.
           *
           * @return The bounded list identified by SubCapabilityType.
           */
         virtual uci::type::COMINT_CapabilityType::SubCapabilityType& getSubCapabilityType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SubCapabilityType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSubCapabilityType(const uci::type::COMINT_CapabilityType::SubCapabilityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the GeolocationType.
           *
           * @return The bounded list identified by GeolocationType.
           */
         virtual const uci::type::COMINT_CapabilityType::GeolocationType& getGeolocationType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the GeolocationType.
           *
           * @return The bounded list identified by GeolocationType.
           */
         virtual uci::type::COMINT_CapabilityType::GeolocationType& getGeolocationType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the GeolocationType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setGeolocationType(const uci::type::COMINT_CapabilityType::GeolocationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AcceptedInterface.
           *
           * @return The bounded list identified by AcceptedInterface.
           */
         virtual const uci::type::COMINT_CapabilityType::AcceptedInterface& getAcceptedInterface() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AcceptedInterface.
           *
           * @return The bounded list identified by AcceptedInterface.
           */
         virtual uci::type::COMINT_CapabilityType::AcceptedInterface& getAcceptedInterface()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AcceptedInterface.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAcceptedInterface(const uci::type::COMINT_CapabilityType::AcceptedInterface& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityOptions.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityOptions.
           */
         virtual const uci::type::COMINT_CapabilityOptionsType& getCapabilityOptions() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityOptions.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityOptions.
           */
         virtual uci::type::COMINT_CapabilityOptionsType& getCapabilityOptions()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CapabilityOptions to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CapabilityOptions
           */
         virtual void setCapabilityOptions(const uci::type::COMINT_CapabilityOptionsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DwellCoverage.
           *
           * @return The bounded list identified by DwellCoverage.
           */
         virtual const uci::type::COMINT_CapabilityType::DwellCoverage& getDwellCoverage() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DwellCoverage.
           *
           * @return The bounded list identified by DwellCoverage.
           */
         virtual uci::type::COMINT_CapabilityType::DwellCoverage& getDwellCoverage()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the DwellCoverage.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setDwellCoverage(const uci::type::COMINT_CapabilityType::DwellCoverage& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductOutput.
           *
           * @return The bounded list identified by ProductOutput.
           */
         virtual const uci::type::COMINT_CapabilityType::ProductOutput& getProductOutput() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductOutput.
           *
           * @return The bounded list identified by ProductOutput.
           */
         virtual uci::type::COMINT_CapabilityType::ProductOutput& getProductOutput()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ProductOutput.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setProductOutput(const uci::type::COMINT_CapabilityType::ProductOutput& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MessageOutput.
           *
           * @return The bounded list identified by MessageOutput.
           */
         virtual const uci::type::COMINT_CapabilityType::MessageOutput& getMessageOutput() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MessageOutput.
           *
           * @return The bounded list identified by MessageOutput.
           */
         virtual uci::type::COMINT_CapabilityType::MessageOutput& getMessageOutput()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the MessageOutput.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setMessageOutput(const uci::type::COMINT_CapabilityType::MessageOutput& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ApplicationID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ApplicationID.
           */
         virtual const uci::type::ForeignKeyType& getApplicationID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ApplicationID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ApplicationID.
           */
         virtual uci::type::ForeignKeyType& getApplicationID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ApplicationID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ApplicationID
           */
         virtual void setApplicationID(const uci::type::ForeignKeyType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ApplicationID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ApplicationID is emabled or not
           */
         virtual bool hasApplicationID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ApplicationID
           *
           * @param type = uci::type::accessorType::foreignKeyType This Accessor's accessor type
           */
         virtual void enableApplicationID(uci::base::accessorType::AccessorType type = uci::type::accessorType::foreignKeyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ApplicationID */
         virtual void clearApplicationID()
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
         COMINT_CapabilityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The COMINT_CapabilityType to copy from
           */
         COMINT_CapabilityType(const COMINT_CapabilityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this COMINT_CapabilityType to the contents of the COMINT_CapabilityType
           * on the right hand side (rhs) of the assignment operator.COMINT_CapabilityType [only available to derived classes]
           *
           * @param rhs The COMINT_CapabilityType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::COMINT_CapabilityType
           * @return A constant reference to this COMINT_CapabilityType.
           */
         const COMINT_CapabilityType& operator=(const COMINT_CapabilityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // COMINT_CapabilityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__COMINT_CapabilityType_h

