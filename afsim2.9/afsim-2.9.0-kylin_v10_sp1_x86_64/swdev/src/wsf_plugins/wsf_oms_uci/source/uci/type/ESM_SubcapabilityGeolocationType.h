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
#ifndef Uci__Type__ESM_SubcapabilityGeolocationType_h
#define Uci__Type__ESM_SubcapabilityGeolocationType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SignalGeolocationControlEnum_h)
# include "uci/type/SignalGeolocationControlEnum.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SignalGeolocationMethodEnum_h)
# include "uci/type/SignalGeolocationMethodEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ESM_SubcapabilityGeolocationType sequence accessor class */
      class ESM_SubcapabilityGeolocationType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ESM_SubcapabilityGeolocationType()
         { }

         /** Returns this accessor's type constant, i.e. ESM_SubcapabilityGeolocationType
           *
           * @return This accessor's type constant, i.e. ESM_SubcapabilityGeolocationType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::eSM_SubcapabilityGeolocationType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ESM_SubcapabilityGeolocationType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a commanded geolocation type/technique to use. [Minimum occurrences: 0] [Maximum occurrences: 8] */
         typedef uci::base::BoundedList<uci::type::SignalGeolocationMethodEnum, uci::type::accessorType::signalGeolocationMethodEnum> GeolocationType;

         /** Returns the value of the enumeration that is identified by the GeolocationState.
           *
           * @return The value of the enumeration identified by GeolocationState.
           */
         virtual const uci::type::SignalGeolocationControlEnum& getGeolocationState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the GeolocationState.
           *
           * @return The value of the enumeration identified by GeolocationState.
           */
         virtual uci::type::SignalGeolocationControlEnum& getGeolocationState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the GeolocationState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setGeolocationState(const uci::type::SignalGeolocationControlEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the GeolocationState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setGeolocationState(uci::type::SignalGeolocationControlEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the GeolocationType.
           *
           * @return The bounded list identified by GeolocationType.
           */
         virtual const uci::type::ESM_SubcapabilityGeolocationType::GeolocationType& getGeolocationType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the GeolocationType.
           *
           * @return The bounded list identified by GeolocationType.
           */
         virtual uci::type::ESM_SubcapabilityGeolocationType::GeolocationType& getGeolocationType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the GeolocationType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setGeolocationType(const uci::type::ESM_SubcapabilityGeolocationType::GeolocationType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ESM_SubcapabilityGeolocationType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ESM_SubcapabilityGeolocationType to copy from
           */
         ESM_SubcapabilityGeolocationType(const ESM_SubcapabilityGeolocationType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ESM_SubcapabilityGeolocationType to the contents of the
           * ESM_SubcapabilityGeolocationType on the right hand side (rhs) of the assignment
           * operator.ESM_SubcapabilityGeolocationType [only available to derived classes]
           *
           * @param rhs The ESM_SubcapabilityGeolocationType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::ESM_SubcapabilityGeolocationType
           * @return A constant reference to this ESM_SubcapabilityGeolocationType.
           */
         const ESM_SubcapabilityGeolocationType& operator=(const ESM_SubcapabilityGeolocationType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ESM_SubcapabilityGeolocationType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ESM_SubcapabilityGeolocationType_h

