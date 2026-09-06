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
#ifndef Uci__Type__COMINT_GeolocationType_h
#define Uci__Type__COMINT_GeolocationType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__EntityID_Type_h)
# include "uci/type/EntityID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SignalGeolocationMethodEnum_h)
# include "uci/type/SignalGeolocationMethodEnum.h"
#endif

#if !defined(Uci__Type__SignalGeolocationControlEnum_h)
# include "uci/type/SignalGeolocationControlEnum.h"
#endif

#if !defined(Uci__Type__DurationType_h)
# include "uci/type/DurationType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the COMINT_GeolocationType sequence accessor class */
      class COMINT_GeolocationType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~COMINT_GeolocationType()
         { }

         /** Returns this accessor's type constant, i.e. COMINT_GeolocationType
           *
           * @return This accessor's type constant, i.e. COMINT_GeolocationType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::cOMINT_GeolocationType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const COMINT_GeolocationType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a geolocation type/technique being used. [Minimum occurrences: 0] [Maximum occurrences: 8] */
         typedef uci::base::BoundedList<uci::type::SignalGeolocationMethodEnum, uci::type::accessorType::signalGeolocationMethodEnum> GeolocationType;

         /** Returns the accessor that provides access to the complex content that is identified by the EntityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by EntityID.
           */
         virtual const uci::type::EntityID_Type& getEntityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EntityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by EntityID.
           */
         virtual uci::type::EntityID_Type& getEntityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EntityID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EntityID
           */
         virtual void setEntityID(const uci::type::EntityID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the GeolocationType.
           *
           * @return The bounded list identified by GeolocationType.
           */
         virtual const uci::type::COMINT_GeolocationType::GeolocationType& getGeolocationType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the GeolocationType.
           *
           * @return The bounded list identified by GeolocationType.
           */
         virtual uci::type::COMINT_GeolocationType::GeolocationType& getGeolocationType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the GeolocationType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setGeolocationType(const uci::type::COMINT_GeolocationType::GeolocationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the GeolocationControlState.
           *
           * @return The value of the enumeration identified by GeolocationControlState.
           */
         virtual const uci::type::SignalGeolocationControlEnum& getGeolocationControlState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the GeolocationControlState.
           *
           * @return The value of the enumeration identified by GeolocationControlState.
           */
         virtual uci::type::SignalGeolocationControlEnum& getGeolocationControlState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the GeolocationControlState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setGeolocationControlState(const uci::type::SignalGeolocationControlEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the GeolocationControlState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setGeolocationControlState(uci::type::SignalGeolocationControlEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield GeolocationControlStateis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasGeolocationControlState() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getGeolocationControlState will return the optional field and not throw an
           * exception when invoked.
           *
           * @param type = uci::type::accessorType::signalGeolocationControlEnum This Accessor's accessor type
           */
         virtual void enableGeolocationControlState(uci::base::accessorType::AccessorType type = uci::type::accessorType::signalGeolocationControlEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearGeolocationControlState()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ActualDuration.
           *
           * @return The value of the simple primitive data type identified by ActualDuration.
           */
         virtual uci::type::DurationTypeValue getActualDuration() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ActualDuration.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setActualDuration(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ActualDuration exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ActualDuration is emabled or not
           */
         virtual bool hasActualDuration() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ActualDuration
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enableActualDuration(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ActualDuration */
         virtual void clearActualDuration()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         COMINT_GeolocationType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The COMINT_GeolocationType to copy from
           */
         COMINT_GeolocationType(const COMINT_GeolocationType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this COMINT_GeolocationType to the contents of the
           * COMINT_GeolocationType on the right hand side (rhs) of the assignment operator.COMINT_GeolocationType [only available
           * to derived classes]
           *
           * @param rhs The COMINT_GeolocationType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::COMINT_GeolocationType
           * @return A constant reference to this COMINT_GeolocationType.
           */
         const COMINT_GeolocationType& operator=(const COMINT_GeolocationType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // COMINT_GeolocationType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__COMINT_GeolocationType_h

