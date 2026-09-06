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
#ifndef Uci__Type__DetectionRangeRingType_h
#define Uci__Type__DetectionRangeRingType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__AltitudeType_h)
# include "uci/type/AltitudeType.h"
#endif

#if !defined(Uci__Type__AnglePositiveType_h)
# include "uci/type/AnglePositiveType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__DistanceType_h)
# include "uci/type/DistanceType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Defines an azimuth cut in the global horizontal plane through the 3D detection envelope. */
      class DetectionRangeRingType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~DetectionRangeRingType()
         { }

         /** Returns this accessor's type constant, i.e. DetectionRangeRingType
           *
           * @return This accessor's type constant, i.e. DetectionRangeRingType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::detectionRangeRingType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DetectionRangeRingType& accessor)
            throw(uci::base::UCIException) = 0;


         /** A list of detection ranges in meters which define a detection envelope. Azimuth 0 is considered to be off the nose of
           * the aircraft. Range values are assumed to be evenly spaced between the start and end azimuth, going clockwise.
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::DistanceType, uci::type::accessorType::distanceType> DetectionRange;

         /** Returns the value of the SimplePrimitive data type that is identified by the AltitudeOffset.
           *
           * @return The value of the simple primitive data type identified by AltitudeOffset.
           */
         virtual uci::type::AltitudeTypeValue getAltitudeOffset() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the AltitudeOffset.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAltitudeOffset(uci::type::AltitudeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the StartAzimuth.
           *
           * @return The value of the simple primitive data type identified by StartAzimuth.
           */
         virtual uci::type::AnglePositiveTypeValue getStartAzimuth() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the StartAzimuth.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setStartAzimuth(uci::type::AnglePositiveTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by StartAzimuth exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by StartAzimuth is emabled or not
           */
         virtual bool hasStartAzimuth() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by StartAzimuth
           *
           * @param type = uci::type::accessorType::anglePositiveType This Accessor's accessor type
           */
         virtual void enableStartAzimuth(uci::base::accessorType::AccessorType type = uci::type::accessorType::anglePositiveType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by StartAzimuth */
         virtual void clearStartAzimuth()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the EndAzimuth.
           *
           * @return The value of the simple primitive data type identified by EndAzimuth.
           */
         virtual uci::type::AnglePositiveTypeValue getEndAzimuth() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the EndAzimuth.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setEndAzimuth(uci::type::AnglePositiveTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EndAzimuth exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EndAzimuth is emabled or not
           */
         virtual bool hasEndAzimuth() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EndAzimuth
           *
           * @param type = uci::type::accessorType::anglePositiveType This Accessor's accessor type
           */
         virtual void enableEndAzimuth(uci::base::accessorType::AccessorType type = uci::type::accessorType::anglePositiveType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EndAzimuth */
         virtual void clearEndAzimuth()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DetectionRange.
           *
           * @return The bounded list identified by DetectionRange.
           */
         virtual const uci::type::DetectionRangeRingType::DetectionRange& getDetectionRange() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DetectionRange.
           *
           * @return The bounded list identified by DetectionRange.
           */
         virtual uci::type::DetectionRangeRingType::DetectionRange& getDetectionRange()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the DetectionRange.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setDetectionRange(const uci::type::DetectionRangeRingType::DetectionRange& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DetectionRangeRingType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DetectionRangeRingType to copy from
           */
         DetectionRangeRingType(const DetectionRangeRingType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DetectionRangeRingType to the contents of the
           * DetectionRangeRingType on the right hand side (rhs) of the assignment operator.DetectionRangeRingType [only available
           * to derived classes]
           *
           * @param rhs The DetectionRangeRingType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::DetectionRangeRingType
           * @return A constant reference to this DetectionRangeRingType.
           */
         const DetectionRangeRingType& operator=(const DetectionRangeRingType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DetectionRangeRingType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DetectionRangeRingType_h

