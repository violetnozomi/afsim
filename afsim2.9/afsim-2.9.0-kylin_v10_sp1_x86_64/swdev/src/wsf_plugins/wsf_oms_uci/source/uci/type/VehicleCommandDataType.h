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
#ifndef Uci__Type__VehicleCommandDataType_h
#define Uci__Type__VehicleCommandDataType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__VehicleActionEnum_h)
# include "uci/type/VehicleActionEnum.h"
#endif

#if !defined(Uci__Type__IFF_Type_h)
# include "uci/type/IFF_Type.h"
#endif

#if !defined(Uci__Type__VehicleSurvivabilityModeEnum_h)
# include "uci/type/VehicleSurvivabilityModeEnum.h"
#endif

#if !defined(Uci__Type__DurationType_h)
# include "uci/type/DurationType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__VehicleLossOfLinkProcessingEnum_h)
# include "uci/type/VehicleLossOfLinkProcessingEnum.h"
#endif

#if !defined(Uci__Type__CommAllocationActionType_h)
# include "uci/type/CommAllocationActionType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the VehicleCommandDataType sequence accessor class */
      class VehicleCommandDataType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~VehicleCommandDataType()
         { }

         /** Returns this accessor's type constant, i.e. VehicleCommandDataType
           *
           * @return This accessor's type constant, i.e. VehicleCommandDataType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::vehicleCommandDataType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const VehicleCommandDataType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Action.
           *
           * @return The value of the enumeration identified by Action.
           */
         virtual const uci::type::VehicleActionEnum& getAction() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Action.
           *
           * @return The value of the enumeration identified by Action.
           */
         virtual uci::type::VehicleActionEnum& getAction()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Action.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setAction(const uci::type::VehicleActionEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Action.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setAction(uci::type::VehicleActionEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield Actionis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasAction() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getAction will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::vehicleActionEnum This Accessor's accessor type
           */
         virtual void enableAction(uci::base::accessorType::AccessorType type = uci::type::accessorType::vehicleActionEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearAction()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the IFF_Settings.
           *
           * @return The acecssor that provides access to the complex content that is identified by IFF_Settings.
           */
         virtual const uci::type::IFF_Type& getIFF_Settings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the IFF_Settings.
           *
           * @return The acecssor that provides access to the complex content that is identified by IFF_Settings.
           */
         virtual uci::type::IFF_Type& getIFF_Settings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the IFF_Settings to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by IFF_Settings
           */
         virtual void setIFF_Settings(const uci::type::IFF_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by IFF_Settings exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by IFF_Settings is emabled or not
           */
         virtual bool hasIFF_Settings() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by IFF_Settings
           *
           * @param type = uci::type::accessorType::iFF_Type This Accessor's accessor type
           */
         virtual void enableIFF_Settings(uci::base::accessorType::AccessorType type = uci::type::accessorType::iFF_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by IFF_Settings */
         virtual void clearIFF_Settings()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SurvivabilityMode.
           *
           * @return The value of the enumeration identified by SurvivabilityMode.
           */
         virtual const uci::type::VehicleSurvivabilityModeEnum& getSurvivabilityMode() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SurvivabilityMode.
           *
           * @return The value of the enumeration identified by SurvivabilityMode.
           */
         virtual uci::type::VehicleSurvivabilityModeEnum& getSurvivabilityMode()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SurvivabilityMode.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSurvivabilityMode(const uci::type::VehicleSurvivabilityModeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SurvivabilityMode.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSurvivabilityMode(uci::type::VehicleSurvivabilityModeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield SurvivabilityModeis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasSurvivabilityMode() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getSurvivabilityMode will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::vehicleSurvivabilityModeEnum This Accessor's accessor type
           */
         virtual void enableSurvivabilityMode(uci::base::accessorType::AccessorType type = uci::type::accessorType::vehicleSurvivabilityModeEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearSurvivabilityMode()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the LostCommTimeout.
           *
           * @return The value of the simple primitive data type identified by LostCommTimeout.
           */
         virtual uci::type::DurationTypeValue getLostCommTimeout() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the LostCommTimeout.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setLostCommTimeout(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by LostCommTimeout exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by LostCommTimeout is emabled or not
           */
         virtual bool hasLostCommTimeout() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by LostCommTimeout
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enableLostCommTimeout(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by LostCommTimeout */
         virtual void clearLostCommTimeout()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the LOS.
           *
           * @return The value of the simple primitive data type identified by LOS.
           */
         virtual xs::Boolean getLOS() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the LOS.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setLOS(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by LOS exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by LOS is emabled or not
           */
         virtual bool hasLOS() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by LOS
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableLOS(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by LOS */
         virtual void clearLOS()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the LossOfLinkProcessing.
           *
           * @return The value of the enumeration identified by LossOfLinkProcessing.
           */
         virtual const uci::type::VehicleLossOfLinkProcessingEnum& getLossOfLinkProcessing() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the LossOfLinkProcessing.
           *
           * @return The value of the enumeration identified by LossOfLinkProcessing.
           */
         virtual uci::type::VehicleLossOfLinkProcessingEnum& getLossOfLinkProcessing()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the LossOfLinkProcessing.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setLossOfLinkProcessing(const uci::type::VehicleLossOfLinkProcessingEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the LossOfLinkProcessing.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setLossOfLinkProcessing(uci::type::VehicleLossOfLinkProcessingEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield LossOfLinkProcessingis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasLossOfLinkProcessing() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getLossOfLinkProcessing will return the optional field and not throw an
           * exception when invoked.
           *
           * @param type = uci::type::accessorType::vehicleLossOfLinkProcessingEnum This Accessor's accessor type
           */
         virtual void enableLossOfLinkProcessing(uci::base::accessorType::AccessorType type = uci::type::accessorType::vehicleLossOfLinkProcessingEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearLossOfLinkProcessing()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommAllocationAction.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommAllocationAction.
           */
         virtual const uci::type::CommAllocationActionType& getCommAllocationAction() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommAllocationAction.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommAllocationAction.
           */
         virtual uci::type::CommAllocationActionType& getCommAllocationAction()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CommAllocationAction to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CommAllocationAction
           */
         virtual void setCommAllocationAction(const uci::type::CommAllocationActionType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CommAllocationAction exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CommAllocationAction is emabled or not
           */
         virtual bool hasCommAllocationAction() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CommAllocationAction
           *
           * @param type = uci::type::accessorType::commAllocationActionType This Accessor's accessor type
           */
         virtual void enableCommAllocationAction(uci::base::accessorType::AccessorType type = uci::type::accessorType::commAllocationActionType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CommAllocationAction */
         virtual void clearCommAllocationAction()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         VehicleCommandDataType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The VehicleCommandDataType to copy from
           */
         VehicleCommandDataType(const VehicleCommandDataType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this VehicleCommandDataType to the contents of the
           * VehicleCommandDataType on the right hand side (rhs) of the assignment operator.VehicleCommandDataType [only available
           * to derived classes]
           *
           * @param rhs The VehicleCommandDataType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::VehicleCommandDataType
           * @return A constant reference to this VehicleCommandDataType.
           */
         const VehicleCommandDataType& operator=(const VehicleCommandDataType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // VehicleCommandDataType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__VehicleCommandDataType_h

