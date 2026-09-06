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
#ifndef Uci__Type__TurnPointType_h
#define Uci__Type__TurnPointType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__Point2D_Type_h)
# include "uci/type/Point2D_Type.h"
#endif

#if !defined(Uci__Type__TurnPointTypeEnum_h)
# include "uci/type/TurnPointTypeEnum.h"
#endif

#if !defined(Uci__Type__AngleType_h)
# include "uci/type/AngleType.h"
#endif

#if !defined(Uci__Type__DistanceType_h)
# include "uci/type/DistanceType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the TurnPointType sequence accessor class */
      class TurnPointType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~TurnPointType()
         { }

         /** Returns this accessor's type constant, i.e. TurnPointType
           *
           * @return This accessor's type constant, i.e. TurnPointType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::turnPointType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const TurnPointType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Waypoint.
           *
           * @return The acecssor that provides access to the complex content that is identified by Waypoint.
           */
         virtual const uci::type::Point2D_Type& getWaypoint() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Waypoint.
           *
           * @return The acecssor that provides access to the complex content that is identified by Waypoint.
           */
         virtual uci::type::Point2D_Type& getWaypoint()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Waypoint to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Waypoint
           */
         virtual void setWaypoint(const uci::type::Point2D_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the TurnPointType.
           *
           * @return The value of the enumeration identified by TurnPointType.
           */
         virtual const uci::type::TurnPointTypeEnum& getTurnPointType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the TurnPointType.
           *
           * @return The value of the enumeration identified by TurnPointType.
           */
         virtual uci::type::TurnPointTypeEnum& getTurnPointType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the TurnPointType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setTurnPointType(const uci::type::TurnPointTypeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the TurnPointType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setTurnPointType(uci::type::TurnPointTypeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Course.
           *
           * @return The value of the simple primitive data type identified by Course.
           */
         virtual uci::type::AngleTypeValue getCourse() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Course.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setCourse(uci::type::AngleTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Course exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Course is emabled or not
           */
         virtual bool hasCourse() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Course
           *
           * @param type = uci::type::accessorType::angleType This Accessor's accessor type
           */
         virtual void enableCourse(uci::base::accessorType::AccessorType type = uci::type::accessorType::angleType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Course */
         virtual void clearCourse()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the TurnRadius.
           *
           * @return The value of the simple primitive data type identified by TurnRadius.
           */
         virtual uci::type::DistanceTypeValue getTurnRadius() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the TurnRadius.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTurnRadius(uci::type::DistanceTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TurnRadius exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TurnRadius is emabled or not
           */
         virtual bool hasTurnRadius() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TurnRadius
           *
           * @param type = uci::type::accessorType::distanceType This Accessor's accessor type
           */
         virtual void enableTurnRadius(uci::base::accessorType::AccessorType type = uci::type::accessorType::distanceType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TurnRadius */
         virtual void clearTurnRadius()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         TurnPointType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The TurnPointType to copy from
           */
         TurnPointType(const TurnPointType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this TurnPointType to the contents of the TurnPointType on the right
           * hand side (rhs) of the assignment operator.TurnPointType [only available to derived classes]
           *
           * @param rhs The TurnPointType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::TurnPointType
           * @return A constant reference to this TurnPointType.
           */
         const TurnPointType& operator=(const TurnPointType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // TurnPointType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__TurnPointType_h

