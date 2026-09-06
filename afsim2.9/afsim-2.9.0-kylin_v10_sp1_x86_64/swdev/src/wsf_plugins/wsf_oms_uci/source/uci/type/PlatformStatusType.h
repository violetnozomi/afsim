// This file was generated  on 12/5/2018 at 1:1:47 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__PlatformStatusType_h
#define Uci__Type__PlatformStatusType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__PlatformFunctionStatusType_h)
# include "uci/type/PlatformFunctionStatusType.h"
#endif

#if !defined(Uci__Type__PlatformStatusSAM_Type_h)
# include "uci/type/PlatformStatusSAM_Type.h"
#endif

#if !defined(Uci__Type__SurfaceRecoveryType_h)
# include "uci/type/SurfaceRecoveryType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the PlatformStatusType sequence accessor class */
      class PlatformStatusType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~PlatformStatusType()
         { }

         /** Returns this accessor's type constant, i.e. PlatformStatusType
           *
           * @return This accessor's type constant, i.e. PlatformStatusType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::platformStatusType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PlatformStatusType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Not a subsitude for Capability and CapabilityStatus, this field allows for high level platform SA rollup. The data in
           * this filed will always be superseeded by Capability and CapabilityStatus. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::PlatformFunctionStatusType, uci::type::accessorType::platformFunctionStatusType> FunctionStatus;

         /** Returns the value of the SimplePrimitive data type that is identified by the BailoutIndicator.
           *
           * @return The value of the simple primitive data type identified by BailoutIndicator.
           */
         virtual xs::Boolean getBailoutIndicator() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the BailoutIndicator.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setBailoutIndicator(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by BailoutIndicator exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by BailoutIndicator is emabled or not
           */
         virtual bool hasBailoutIndicator() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by BailoutIndicator
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableBailoutIndicator(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by BailoutIndicator */
         virtual void clearBailoutIndicator()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the CommandAndControlIndicator.
           *
           * @return The value of the simple primitive data type identified by CommandAndControlIndicator.
           */
         virtual xs::Boolean getCommandAndControlIndicator() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the CommandAndControlIndicator.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setCommandAndControlIndicator(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CommandAndControlIndicator exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CommandAndControlIndicator is emabled or not
           */
         virtual bool hasCommandAndControlIndicator() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CommandAndControlIndicator
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableCommandAndControlIndicator(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CommandAndControlIndicator */
         virtual void clearCommandAndControlIndicator()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the EmergencyIndicator.
           *
           * @return The value of the simple primitive data type identified by EmergencyIndicator.
           */
         virtual xs::Boolean getEmergencyIndicator() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the EmergencyIndicator.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setEmergencyIndicator(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EmergencyIndicator exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EmergencyIndicator is emabled or not
           */
         virtual bool hasEmergencyIndicator() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EmergencyIndicator
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableEmergencyIndicator(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EmergencyIndicator */
         virtual void clearEmergencyIndicator()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the FunctionStatus.
           *
           * @return The bounded list identified by FunctionStatus.
           */
         virtual const uci::type::PlatformStatusType::FunctionStatus& getFunctionStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the FunctionStatus.
           *
           * @return The bounded list identified by FunctionStatus.
           */
         virtual uci::type::PlatformStatusType::FunctionStatus& getFunctionStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the FunctionStatus.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setFunctionStatus(const uci::type::PlatformStatusType::FunctionStatus& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SAM_Status.
           *
           * @return The acecssor that provides access to the complex content that is identified by SAM_Status.
           */
         virtual const uci::type::PlatformStatusSAM_Type& getSAM_Status() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SAM_Status.
           *
           * @return The acecssor that provides access to the complex content that is identified by SAM_Status.
           */
         virtual uci::type::PlatformStatusSAM_Type& getSAM_Status()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SAM_Status to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SAM_Status
           */
         virtual void setSAM_Status(const uci::type::PlatformStatusSAM_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SAM_Status exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SAM_Status is emabled or not
           */
         virtual bool hasSAM_Status() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SAM_Status
           *
           * @param type = uci::type::accessorType::platformStatusSAM_Type This Accessor's accessor type
           */
         virtual void enableSAM_Status(uci::base::accessorType::AccessorType type = uci::type::accessorType::platformStatusSAM_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SAM_Status */
         virtual void clearSAM_Status()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SurfaceRecovery.
           *
           * @return The acecssor that provides access to the complex content that is identified by SurfaceRecovery.
           */
         virtual const uci::type::SurfaceRecoveryType& getSurfaceRecovery() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SurfaceRecovery.
           *
           * @return The acecssor that provides access to the complex content that is identified by SurfaceRecovery.
           */
         virtual uci::type::SurfaceRecoveryType& getSurfaceRecovery()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SurfaceRecovery to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SurfaceRecovery
           */
         virtual void setSurfaceRecovery(const uci::type::SurfaceRecoveryType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SurfaceRecovery exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SurfaceRecovery is emabled or not
           */
         virtual bool hasSurfaceRecovery() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SurfaceRecovery
           *
           * @param type = uci::type::accessorType::surfaceRecoveryType This Accessor's accessor type
           */
         virtual void enableSurfaceRecovery(uci::base::accessorType::AccessorType type = uci::type::accessorType::surfaceRecoveryType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SurfaceRecovery */
         virtual void clearSurfaceRecovery()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PlatformStatusType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PlatformStatusType to copy from
           */
         PlatformStatusType(const PlatformStatusType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PlatformStatusType to the contents of the PlatformStatusType on
           * the right hand side (rhs) of the assignment operator.PlatformStatusType [only available to derived classes]
           *
           * @param rhs The PlatformStatusType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::PlatformStatusType
           * @return A constant reference to this PlatformStatusType.
           */
         const PlatformStatusType& operator=(const PlatformStatusType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PlatformStatusType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PlatformStatusType_h

