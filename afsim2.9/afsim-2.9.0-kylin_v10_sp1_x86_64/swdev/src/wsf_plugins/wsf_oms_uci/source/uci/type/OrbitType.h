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
#ifndef Uci__Type__OrbitType_h
#define Uci__Type__OrbitType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__OrbitEnum_h)
# include "uci/type/OrbitEnum.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__CircleType_h)
# include "uci/type/CircleType.h"
#endif

#if !defined(Uci__Type__RelativeDirectionEnum_h)
# include "uci/type/RelativeDirectionEnum.h"
#endif

#if !defined(Uci__Type__OrbitDurationType_h)
# include "uci/type/OrbitDurationType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the OrbitType sequence accessor class */
      class OrbitType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~OrbitType()
         { }

         /** Returns this accessor's type constant, i.e. OrbitType
           *
           * @return This accessor's type constant, i.e. OrbitType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::orbitType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const OrbitType& accessor)
            throw(uci::base::UCIException) = 0;


         /** A single point will be provided if and only if OrbitType is Circle. [Maximum occurrences: 2] */
         typedef uci::base::BoundedList<uci::type::CircleType, uci::type::accessorType::circleType> Circle;

         /** Returns the value of the enumeration that is identified by the OrbitType.
           *
           * @return The value of the enumeration identified by OrbitType.
           */
         virtual const uci::type::OrbitEnum& getOrbitType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the OrbitType.
           *
           * @return The value of the enumeration identified by OrbitType.
           */
         virtual uci::type::OrbitEnum& getOrbitType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the OrbitType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setOrbitType(const uci::type::OrbitEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the OrbitType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setOrbitType(uci::type::OrbitEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Circle.
           *
           * @return The bounded list identified by Circle.
           */
         virtual const uci::type::OrbitType::Circle& getCircle() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Circle.
           *
           * @return The bounded list identified by Circle.
           */
         virtual uci::type::OrbitType::Circle& getCircle()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Circle.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCircle(const uci::type::OrbitType::Circle& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the TurnDirection.
           *
           * @return The value of the enumeration identified by TurnDirection.
           */
         virtual const uci::type::RelativeDirectionEnum& getTurnDirection() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the TurnDirection.
           *
           * @return The value of the enumeration identified by TurnDirection.
           */
         virtual uci::type::RelativeDirectionEnum& getTurnDirection()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the TurnDirection.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setTurnDirection(const uci::type::RelativeDirectionEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the TurnDirection.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setTurnDirection(uci::type::RelativeDirectionEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Duration.
           *
           * @return The acecssor that provides access to the complex content that is identified by Duration.
           */
         virtual const uci::type::OrbitDurationType& getDuration() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Duration.
           *
           * @return The acecssor that provides access to the complex content that is identified by Duration.
           */
         virtual uci::type::OrbitDurationType& getDuration()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Duration to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Duration
           */
         virtual void setDuration(const uci::type::OrbitDurationType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         OrbitType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The OrbitType to copy from
           */
         OrbitType(const OrbitType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this OrbitType to the contents of the OrbitType on the right hand side
           * (rhs) of the assignment operator.OrbitType [only available to derived classes]
           *
           * @param rhs The OrbitType on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::OrbitType
           * @return A constant reference to this OrbitType.
           */
         const OrbitType& operator=(const OrbitType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // OrbitType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__OrbitType_h

