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
#ifndef Uci__Type__CloudsType_h
#define Uci__Type__CloudsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__WeatherCloudCoverEnum_h)
# include "uci/type/WeatherCloudCoverEnum.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__AltitudeType_h)
# include "uci/type/AltitudeType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CloudsType sequence accessor class */
      class CloudsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CloudsType()
         { }

         /** Returns this accessor's type constant, i.e. CloudsType
           *
           * @return This accessor's type constant, i.e. CloudsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::cloudsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CloudsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Maximum inclusive: 8] */
         typedef uci::base::UnsignedIntAccessor CloudCoverAmplification;
         //  The type of primitive data accessed by CloudCoverAmplification
         typedef xs::UnsignedInt CloudCoverAmplificationValue;

         /** Returns the value of the enumeration that is identified by the CloudCover.
           *
           * @return The value of the enumeration identified by CloudCover.
           */
         virtual const uci::type::WeatherCloudCoverEnum& getCloudCover() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CloudCover.
           *
           * @return The value of the enumeration identified by CloudCover.
           */
         virtual uci::type::WeatherCloudCoverEnum& getCloudCover()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CloudCover.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCloudCover(const uci::type::WeatherCloudCoverEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CloudCover.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCloudCover(uci::type::WeatherCloudCoverEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield CloudCoveris available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasCloudCover() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getCloudCover will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::weatherCloudCoverEnum This Accessor's accessor type
           */
         virtual void enableCloudCover(uci::base::accessorType::AccessorType type = uci::type::accessorType::weatherCloudCoverEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearCloudCover()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the CloudCoverAmplification.
           *
           * @return The value of the simple primitive data type identified by CloudCoverAmplification.
           */
         virtual uci::type::CloudsType::CloudCoverAmplificationValue getCloudCoverAmplification() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the CloudCoverAmplification.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setCloudCoverAmplification(uci::type::CloudsType::CloudCoverAmplificationValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CloudCoverAmplification exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CloudCoverAmplification is emabled or not
           */
         virtual bool hasCloudCoverAmplification() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CloudCoverAmplification
           *
           * @param type = uci::type::cloudsType::accessorType::cloudCoverAmplification This Accessor's accessor type
           */
         virtual void enableCloudCoverAmplification(uci::base::accessorType::AccessorType type = uci::type::cloudsType::accessorType::cloudCoverAmplification)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CloudCoverAmplification */
         virtual void clearCloudCoverAmplification()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the CloudFloor.
           *
           * @return The value of the simple primitive data type identified by CloudFloor.
           */
         virtual uci::type::AltitudeTypeValue getCloudFloor() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the CloudFloor.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setCloudFloor(uci::type::AltitudeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CloudFloor exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CloudFloor is emabled or not
           */
         virtual bool hasCloudFloor() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CloudFloor
           *
           * @param type = uci::type::accessorType::altitudeType This Accessor's accessor type
           */
         virtual void enableCloudFloor(uci::base::accessorType::AccessorType type = uci::type::accessorType::altitudeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CloudFloor */
         virtual void clearCloudFloor()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the CloudCeiling.
           *
           * @return The value of the simple primitive data type identified by CloudCeiling.
           */
         virtual uci::type::AltitudeTypeValue getCloudCeiling() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the CloudCeiling.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setCloudCeiling(uci::type::AltitudeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CloudCeiling exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CloudCeiling is emabled or not
           */
         virtual bool hasCloudCeiling() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CloudCeiling
           *
           * @param type = uci::type::accessorType::altitudeType This Accessor's accessor type
           */
         virtual void enableCloudCeiling(uci::base::accessorType::AccessorType type = uci::type::accessorType::altitudeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CloudCeiling */
         virtual void clearCloudCeiling()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CloudsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CloudsType to copy from
           */
         CloudsType(const CloudsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CloudsType to the contents of the CloudsType on the right hand
           * side (rhs) of the assignment operator.CloudsType [only available to derived classes]
           *
           * @param rhs The CloudsType on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::CloudsType
           * @return A constant reference to this CloudsType.
           */
         const CloudsType& operator=(const CloudsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CloudsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CloudsType_h

