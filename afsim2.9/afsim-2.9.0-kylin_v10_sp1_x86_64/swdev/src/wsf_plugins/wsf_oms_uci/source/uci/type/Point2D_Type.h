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
#ifndef Uci__Type__Point2D_Type_h
#define Uci__Type__Point2D_Type_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__AngleHalfType_h)
# include "uci/type/AngleHalfType.h"
#endif

#if !defined(Uci__Type__AngleType_h)
# include "uci/type/AngleType.h"
#endif

#if !defined(Uci__Type__AltitudeType_h)
# include "uci/type/AltitudeType.h"
#endif

#if !defined(Uci__Type__AltitudeRangeType_h)
# include "uci/type/AltitudeRangeType.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Positional data relative to WGS-84 ellipsoid with lat and long as mandatory entries. */
      class Point2D_Type : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~Point2D_Type()
         { }

         /** Returns this accessor's type constant, i.e. Point2D_Type
           *
           * @return This accessor's type constant, i.e. Point2D_Type
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::point2D_Type;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const Point2D_Type& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Latitude.
           *
           * @return The value of the simple primitive data type identified by Latitude.
           */
         virtual uci::type::AngleHalfTypeValue getLatitude() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Latitude.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setLatitude(uci::type::AngleHalfTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Longitude.
           *
           * @return The value of the simple primitive data type identified by Longitude.
           */
         virtual uci::type::AngleTypeValue getLongitude() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Longitude.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setLongitude(uci::type::AngleTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Altitude.
           *
           * @return The value of the simple primitive data type identified by Altitude.
           */
         virtual uci::type::AltitudeTypeValue getAltitude() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Altitude.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAltitude(uci::type::AltitudeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Altitude exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Altitude is emabled or not
           */
         virtual bool hasAltitude() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Altitude
           *
           * @param type = uci::type::accessorType::altitudeType This Accessor's accessor type
           */
         virtual void enableAltitude(uci::base::accessorType::AccessorType type = uci::type::accessorType::altitudeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Altitude */
         virtual void clearAltitude()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AltitudeRange.
           *
           * @return The acecssor that provides access to the complex content that is identified by AltitudeRange.
           */
         virtual const uci::type::AltitudeRangeType& getAltitudeRange() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AltitudeRange.
           *
           * @return The acecssor that provides access to the complex content that is identified by AltitudeRange.
           */
         virtual uci::type::AltitudeRangeType& getAltitudeRange()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AltitudeRange to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AltitudeRange
           */
         virtual void setAltitudeRange(const uci::type::AltitudeRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AltitudeRange exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AltitudeRange is emabled or not
           */
         virtual bool hasAltitudeRange() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AltitudeRange
           *
           * @param type = uci::type::accessorType::altitudeRangeType This Accessor's accessor type
           */
         virtual void enableAltitudeRange(uci::base::accessorType::AccessorType type = uci::type::accessorType::altitudeRangeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AltitudeRange */
         virtual void clearAltitudeRange()
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



      protected:

         /** The constructor [only available to derived classes]. */
         Point2D_Type()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The Point2D_Type to copy from
           */
         Point2D_Type(const Point2D_Type& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this Point2D_Type to the contents of the Point2D_Type on the right hand
           * side (rhs) of the assignment operator.Point2D_Type [only available to derived classes]
           *
           * @param rhs The Point2D_Type on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::Point2D_Type
           * @return A constant reference to this Point2D_Type.
           */
         const Point2D_Type& operator=(const Point2D_Type& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // Point2D_Type


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__Point2D_Type_h

