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
#ifndef Uci__Type__CommUHF_SATCOM_CommWaveformActivityType_h
#define Uci__Type__CommUHF_SATCOM_CommWaveformActivityType_h 1

#if !defined(Uci__Type__CommWaveformActivityPET_h)
# include "uci/type/CommWaveformActivityPET.h"
#endif

#if !defined(Uci__Type__DurationType_h)
# include "uci/type/DurationType.h"
#endif

#if !defined(Uci__Type__CommRangeModeTypeEnum_h)
# include "uci/type/CommRangeModeTypeEnum.h"
#endif

#if !defined(Uci__Type__CommSATCOM_ServiceTypeEnum_h)
# include "uci/type/CommSATCOM_ServiceTypeEnum.h"
#endif

#if !defined(Uci__Type__ForeignKeyType_h)
# include "uci/type/ForeignKeyType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is a polymorphic extension which allows for the extension of the base CommWaveformActivityPET for communication
        * UHF SATCOM parameters.
        */
      class CommUHF_SATCOM_CommWaveformActivityType : public virtual uci::type::CommWaveformActivityPET {
      public:

         /** The destructor */
         virtual ~CommUHF_SATCOM_CommWaveformActivityType()
         { }

         /** Returns this accessor's type constant, i.e. CommUHF_SATCOM_CommWaveformActivityType
           *
           * @return This accessor's type constant, i.e. CommUHF_SATCOM_CommWaveformActivityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commUHF_SATCOM_CommWaveformActivityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommUHF_SATCOM_CommWaveformActivityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** A set of terminal addresses associated with each I/O port. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ForeignKeyType, uci::type::accessorType::foreignKeyType> GuardSlotID;

         /** Returns the value of the SimplePrimitive data type that is identified by the RangeDelay.
           *
           * @return The value of the simple primitive data type identified by RangeDelay.
           */
         virtual uci::type::DurationTypeValue getRangeDelay() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the RangeDelay.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRangeDelay(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the RangeMode.
           *
           * @return The value of the enumeration identified by RangeMode.
           */
         virtual const uci::type::CommRangeModeTypeEnum& getRangeMode() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the RangeMode.
           *
           * @return The value of the enumeration identified by RangeMode.
           */
         virtual uci::type::CommRangeModeTypeEnum& getRangeMode()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the RangeMode.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setRangeMode(const uci::type::CommRangeModeTypeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the RangeMode.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setRangeMode(uci::type::CommRangeModeTypeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield RangeModeis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasRangeMode() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getRangeMode will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::commRangeModeTypeEnum This Accessor's accessor type
           */
         virtual void enableRangeMode(uci::base::accessorType::AccessorType type = uci::type::accessorType::commRangeModeTypeEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearRangeMode()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ServiceType.
           *
           * @return The value of the enumeration identified by ServiceType.
           */
         virtual const uci::type::CommSATCOM_ServiceTypeEnum& getServiceType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ServiceType.
           *
           * @return The value of the enumeration identified by ServiceType.
           */
         virtual uci::type::CommSATCOM_ServiceTypeEnum& getServiceType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ServiceType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setServiceType(const uci::type::CommSATCOM_ServiceTypeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ServiceType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setServiceType(uci::type::CommSATCOM_ServiceTypeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield ServiceTypeis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasServiceType() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getServiceType will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::commSATCOM_ServiceTypeEnum This Accessor's accessor type
           */
         virtual void enableServiceType(uci::base::accessorType::AccessorType type = uci::type::accessorType::commSATCOM_ServiceTypeEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearServiceType()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ConfigurationCode.
           *
           * @return The acecssor that provides access to the complex content that is identified by ConfigurationCode.
           */
         virtual const uci::type::ForeignKeyType& getConfigurationCode() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ConfigurationCode.
           *
           * @return The acecssor that provides access to the complex content that is identified by ConfigurationCode.
           */
         virtual uci::type::ForeignKeyType& getConfigurationCode()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ConfigurationCode to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ConfigurationCode
           */
         virtual void setConfigurationCode(const uci::type::ForeignKeyType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ConfigurationCode exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ConfigurationCode is emabled or not
           */
         virtual bool hasConfigurationCode() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ConfigurationCode
           *
           * @param type = uci::type::accessorType::foreignKeyType This Accessor's accessor type
           */
         virtual void enableConfigurationCode(uci::base::accessorType::AccessorType type = uci::type::accessorType::foreignKeyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ConfigurationCode */
         virtual void clearConfigurationCode()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the OrderwireEncryption.
           *
           * @return The value of the simple primitive data type identified by OrderwireEncryption.
           */
         virtual xs::Boolean getOrderwireEncryption() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the OrderwireEncryption.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setOrderwireEncryption(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by OrderwireEncryption exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by OrderwireEncryption is emabled or not
           */
         virtual bool hasOrderwireEncryption() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by OrderwireEncryption
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableOrderwireEncryption(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by OrderwireEncryption */
         virtual void clearOrderwireEncryption()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SatelliteID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SatelliteID.
           */
         virtual const uci::type::ForeignKeyType& getSatelliteID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SatelliteID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SatelliteID.
           */
         virtual uci::type::ForeignKeyType& getSatelliteID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SatelliteID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SatelliteID
           */
         virtual void setSatelliteID(const uci::type::ForeignKeyType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SatelliteID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SatelliteID is emabled or not
           */
         virtual bool hasSatelliteID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SatelliteID
           *
           * @param type = uci::type::accessorType::foreignKeyType This Accessor's accessor type
           */
         virtual void enableSatelliteID(uci::base::accessorType::AccessorType type = uci::type::accessorType::foreignKeyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SatelliteID */
         virtual void clearSatelliteID()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the GuardSlotID.
           *
           * @return The bounded list identified by GuardSlotID.
           */
         virtual const uci::type::CommUHF_SATCOM_CommWaveformActivityType::GuardSlotID& getGuardSlotID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the GuardSlotID.
           *
           * @return The bounded list identified by GuardSlotID.
           */
         virtual uci::type::CommUHF_SATCOM_CommWaveformActivityType::GuardSlotID& getGuardSlotID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the GuardSlotID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setGuardSlotID(const uci::type::CommUHF_SATCOM_CommWaveformActivityType::GuardSlotID& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommUHF_SATCOM_CommWaveformActivityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommUHF_SATCOM_CommWaveformActivityType to copy from
           */
         CommUHF_SATCOM_CommWaveformActivityType(const CommUHF_SATCOM_CommWaveformActivityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommUHF_SATCOM_CommWaveformActivityType to the contents of the
           * CommUHF_SATCOM_CommWaveformActivityType on the right hand side (rhs) of the assignment
           * operator.CommUHF_SATCOM_CommWaveformActivityType [only available to derived classes]
           *
           * @param rhs The CommUHF_SATCOM_CommWaveformActivityType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::CommUHF_SATCOM_CommWaveformActivityType
           * @return A constant reference to this CommUHF_SATCOM_CommWaveformActivityType.
           */
         const CommUHF_SATCOM_CommWaveformActivityType& operator=(const CommUHF_SATCOM_CommWaveformActivityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommUHF_SATCOM_CommWaveformActivityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommUHF_SATCOM_CommWaveformActivityType_h

