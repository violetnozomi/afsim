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
#ifndef Uci__Type__SignalLOB_Type_h
#define Uci__Type__SignalLOB_Type_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__LOS_Type_h)
# include "uci/type/LOS_Type.h"
#endif

#if !defined(Uci__Type__LOS_UncertaintyType_h)
# include "uci/type/LOS_UncertaintyType.h"
#endif

#if !defined(Uci__Type__ComponentID_Type_h)
# include "uci/type/ComponentID_Type.h"
#endif

#if !defined(Uci__Type__ReferenceObjectType_h)
# include "uci/type/ReferenceObjectType.h"
#endif

#if !defined(Uci__Type__SignalLOB_MethodEnum_h)
# include "uci/type/SignalLOB_MethodEnum.h"
#endif

#if !defined(Uci__Type__MeasurementStatusEnum_h)
# include "uci/type/MeasurementStatusEnum.h"
#endif

#if !defined(Uci__Type__SignalLOB_FlagEnum_h)
# include "uci/type/SignalLOB_FlagEnum.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__EW_BearingEnum_h)
# include "uci/type/EW_BearingEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SignalLOB_Type sequence accessor class */
      class SignalLOB_Type : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SignalLOB_Type()
         { }

         /** Returns this accessor's type constant, i.e. SignalLOB_Type
           *
           * @return This accessor's type constant, i.e. SignalLOB_Type
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::signalLOB_Type;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SignalLOB_Type& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LineOfBearing.
           *
           * @return The acecssor that provides access to the complex content that is identified by LineOfBearing.
           */
         virtual const uci::type::LOS_Type& getLineOfBearing() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LineOfBearing.
           *
           * @return The acecssor that provides access to the complex content that is identified by LineOfBearing.
           */
         virtual uci::type::LOS_Type& getLineOfBearing()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the LineOfBearing to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by LineOfBearing
           */
         virtual void setLineOfBearing(const uci::type::LOS_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by LineOfBearing exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by LineOfBearing is emabled or not
           */
         virtual bool hasLineOfBearing() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by LineOfBearing
           *
           * @param type = uci::type::accessorType::lOS_Type This Accessor's accessor type
           */
         virtual void enableLineOfBearing(uci::base::accessorType::AccessorType type = uci::type::accessorType::lOS_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by LineOfBearing */
         virtual void clearLineOfBearing()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LineOfBearingUncertainty.
           *
           * @return The acecssor that provides access to the complex content that is identified by LineOfBearingUncertainty.
           */
         virtual const uci::type::LOS_UncertaintyType& getLineOfBearingUncertainty() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LineOfBearingUncertainty.
           *
           * @return The acecssor that provides access to the complex content that is identified by LineOfBearingUncertainty.
           */
         virtual uci::type::LOS_UncertaintyType& getLineOfBearingUncertainty()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the LineOfBearingUncertainty to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by LineOfBearingUncertainty
           */
         virtual void setLineOfBearingUncertainty(const uci::type::LOS_UncertaintyType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by LineOfBearingUncertainty exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by LineOfBearingUncertainty is emabled or not
           */
         virtual bool hasLineOfBearingUncertainty() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by LineOfBearingUncertainty
           *
           * @param type = uci::type::accessorType::lOS_UncertaintyType This Accessor's accessor type
           */
         virtual void enableLineOfBearingUncertainty(uci::base::accessorType::AccessorType type = uci::type::accessorType::lOS_UncertaintyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by LineOfBearingUncertainty */
         virtual void clearLineOfBearingUncertainty()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ReferenceComponentID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ReferenceComponentID.
           */
         virtual const uci::type::ComponentID_Type& getReferenceComponentID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ReferenceComponentID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ReferenceComponentID.
           */
         virtual uci::type::ComponentID_Type& getReferenceComponentID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ReferenceComponentID to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ReferenceComponentID
           */
         virtual void setReferenceComponentID(const uci::type::ComponentID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ReferenceComponentID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ReferenceComponentID is emabled or not
           */
         virtual bool hasReferenceComponentID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ReferenceComponentID
           *
           * @param type = uci::type::accessorType::componentID_Type This Accessor's accessor type
           */
         virtual void enableReferenceComponentID(uci::base::accessorType::AccessorType type = uci::type::accessorType::componentID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ReferenceComponentID */
         virtual void clearReferenceComponentID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ReferenceID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ReferenceID.
           */
         virtual const uci::type::ReferenceObjectType& getReferenceID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ReferenceID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ReferenceID.
           */
         virtual uci::type::ReferenceObjectType& getReferenceID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ReferenceID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ReferenceID
           */
         virtual void setReferenceID(const uci::type::ReferenceObjectType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ReferenceID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ReferenceID is emabled or not
           */
         virtual bool hasReferenceID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ReferenceID
           *
           * @param type = uci::type::accessorType::referenceObjectType This Accessor's accessor type
           */
         virtual void enableReferenceID(uci::base::accessorType::AccessorType type = uci::type::accessorType::referenceObjectType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ReferenceID */
         virtual void clearReferenceID()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the LOB_Method.
           *
           * @return The value of the enumeration identified by LOB_Method.
           */
         virtual const uci::type::SignalLOB_MethodEnum& getLOB_Method() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the LOB_Method.
           *
           * @return The value of the enumeration identified by LOB_Method.
           */
         virtual uci::type::SignalLOB_MethodEnum& getLOB_Method()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the LOB_Method.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setLOB_Method(const uci::type::SignalLOB_MethodEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the LOB_Method.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setLOB_Method(uci::type::SignalLOB_MethodEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield LOB_Methodis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasLOB_Method() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getLOB_Method will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::signalLOB_MethodEnum This Accessor's accessor type
           */
         virtual void enableLOB_Method(uci::base::accessorType::AccessorType type = uci::type::accessorType::signalLOB_MethodEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearLOB_Method()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the LOB_Status.
           *
           * @return The value of the enumeration identified by LOB_Status.
           */
         virtual const uci::type::MeasurementStatusEnum& getLOB_Status() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the LOB_Status.
           *
           * @return The value of the enumeration identified by LOB_Status.
           */
         virtual uci::type::MeasurementStatusEnum& getLOB_Status()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the LOB_Status.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setLOB_Status(const uci::type::MeasurementStatusEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the LOB_Status.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setLOB_Status(uci::type::MeasurementStatusEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield LOB_Statusis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasLOB_Status() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getLOB_Status will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::measurementStatusEnum This Accessor's accessor type
           */
         virtual void enableLOB_Status(uci::base::accessorType::AccessorType type = uci::type::accessorType::measurementStatusEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearLOB_Status()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the LOB_Flag.
           *
           * @return The value of the enumeration identified by LOB_Flag.
           */
         virtual const uci::type::SignalLOB_FlagEnum& getLOB_Flag() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the LOB_Flag.
           *
           * @return The value of the enumeration identified by LOB_Flag.
           */
         virtual uci::type::SignalLOB_FlagEnum& getLOB_Flag()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the LOB_Flag.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setLOB_Flag(const uci::type::SignalLOB_FlagEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the LOB_Flag.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setLOB_Flag(uci::type::SignalLOB_FlagEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield LOB_Flagis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasLOB_Flag() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getLOB_Flag will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::signalLOB_FlagEnum This Accessor's accessor type
           */
         virtual void enableLOB_Flag(uci::base::accessorType::AccessorType type = uci::type::accessorType::signalLOB_FlagEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearLOB_Flag()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Timestamp.
           *
           * @return The value of the simple primitive data type identified by Timestamp.
           */
         virtual uci::type::DateTimeTypeValue getTimestamp() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Timestamp.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTimestamp(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Timestamp exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Timestamp is emabled or not
           */
         virtual bool hasTimestamp() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Timestamp
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableTimestamp(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Timestamp */
         virtual void clearTimestamp()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Suspect.
           *
           * @return The value of the simple primitive data type identified by Suspect.
           */
         virtual xs::Boolean getSuspect() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Suspect.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setSuspect(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Suspect exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Suspect is emabled or not
           */
         virtual bool hasSuspect() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Suspect
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableSuspect(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Suspect */
         virtual void clearSuspect()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ExcessiveAttitude.
           *
           * @return The value of the simple primitive data type identified by ExcessiveAttitude.
           */
         virtual xs::Boolean getExcessiveAttitude() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ExcessiveAttitude.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setExcessiveAttitude(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ExcessiveAttitude exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ExcessiveAttitude is emabled or not
           */
         virtual bool hasExcessiveAttitude() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ExcessiveAttitude
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableExcessiveAttitude(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ExcessiveAttitude */
         virtual void clearExcessiveAttitude()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the FringeDetection.
           *
           * @return The value of the simple primitive data type identified by FringeDetection.
           */
         virtual xs::Boolean getFringeDetection() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the FringeDetection.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setFringeDetection(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FringeDetection exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FringeDetection is emabled or not
           */
         virtual bool hasFringeDetection() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FringeDetection
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableFringeDetection(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FringeDetection */
         virtual void clearFringeDetection()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the EW_BearingType.
           *
           * @return The value of the enumeration identified by EW_BearingType.
           */
         virtual const uci::type::EW_BearingEnum& getEW_BearingType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the EW_BearingType.
           *
           * @return The value of the enumeration identified by EW_BearingType.
           */
         virtual uci::type::EW_BearingEnum& getEW_BearingType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the EW_BearingType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setEW_BearingType(const uci::type::EW_BearingEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the EW_BearingType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setEW_BearingType(uci::type::EW_BearingEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield EW_BearingTypeis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasEW_BearingType() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getEW_BearingType will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::eW_BearingEnum This Accessor's accessor type
           */
         virtual void enableEW_BearingType(uci::base::accessorType::AccessorType type = uci::type::accessorType::eW_BearingEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearEW_BearingType()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SignalLOB_Type()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SignalLOB_Type to copy from
           */
         SignalLOB_Type(const SignalLOB_Type& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SignalLOB_Type to the contents of the SignalLOB_Type on the right
           * hand side (rhs) of the assignment operator.SignalLOB_Type [only available to derived classes]
           *
           * @param rhs The SignalLOB_Type on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::SignalLOB_Type
           * @return A constant reference to this SignalLOB_Type.
           */
         const SignalLOB_Type& operator=(const SignalLOB_Type& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SignalLOB_Type


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SignalLOB_Type_h

