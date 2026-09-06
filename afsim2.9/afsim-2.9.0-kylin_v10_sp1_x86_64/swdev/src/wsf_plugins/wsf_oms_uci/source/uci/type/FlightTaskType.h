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
#ifndef Uci__Type__FlightTaskType_h
#define Uci__Type__FlightTaskType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__LoiterType_h)
# include "uci/type/LoiterType.h"
#endif

#if !defined(Uci__Type__EmptyType_h)
# include "uci/type/EmptyType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__MustFlyType_h)
# include "uci/type/MustFlyType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the FlightTaskType sequence accessor class */
      class FlightTaskType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~FlightTaskType()
         { }

         /** Returns this accessor's type constant, i.e. FlightTaskType
           *
           * @return This accessor's type constant, i.e. FlightTaskType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::flightTaskType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const FlightTaskType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Loiter.
           *
           * @return The acecssor that provides access to the complex content that is identified by Loiter.
           */
         virtual const uci::type::LoiterType& getLoiter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Loiter.
           *
           * @return The acecssor that provides access to the complex content that is identified by Loiter.
           */
         virtual uci::type::LoiterType& getLoiter()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Loiter to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Loiter
           */
         virtual void setLoiter(const uci::type::LoiterType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Loiter exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Loiter is emabled or not
           */
         virtual bool hasLoiter() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Loiter
           *
           * @param type = uci::type::accessorType::loiterType This Accessor's accessor type
           */
         virtual void enableLoiter(uci::base::accessorType::AccessorType type = uci::type::accessorType::loiterType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Loiter */
         virtual void clearLoiter()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Refuel.
           *
           * @return The value of the string data type identified by Refuel.
           */
         virtual const uci::type::EmptyType& getRefuel() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Refuel.
           *
           * @return The value of the string data type identified by Refuel.
           */
         virtual uci::type::EmptyType& getRefuel()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Refuel to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setRefuel(const uci::type::EmptyType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Refuel to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setRefuel(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Refuel to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setRefuel(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Refuel exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Refuel is emabled or not
           */
         virtual bool hasRefuel() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Refuel
           *
           * @param type = uci::type::accessorType::emptyType This Accessor's accessor type
           */
         virtual void enableRefuel(uci::base::accessorType::AccessorType type = uci::type::accessorType::emptyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Refuel */
         virtual void clearRefuel()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the RTB.
           *
           * @return The value of the simple primitive data type identified by RTB.
           */
         virtual xs::Boolean getRTB() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the RTB.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRTB(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RTB exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RTB is emabled or not
           */
         virtual bool hasRTB() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RTB
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableRTB(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RTB */
         virtual void clearRTB()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MustFly.
           *
           * @return The acecssor that provides access to the complex content that is identified by MustFly.
           */
         virtual const uci::type::MustFlyType& getMustFly() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MustFly.
           *
           * @return The acecssor that provides access to the complex content that is identified by MustFly.
           */
         virtual uci::type::MustFlyType& getMustFly()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MustFly to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MustFly
           */
         virtual void setMustFly(const uci::type::MustFlyType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MustFly exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MustFly is emabled or not
           */
         virtual bool hasMustFly() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MustFly
           *
           * @param type = uci::type::accessorType::mustFlyType This Accessor's accessor type
           */
         virtual void enableMustFly(uci::base::accessorType::AccessorType type = uci::type::accessorType::mustFlyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MustFly */
         virtual void clearMustFly()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         FlightTaskType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The FlightTaskType to copy from
           */
         FlightTaskType(const FlightTaskType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this FlightTaskType to the contents of the FlightTaskType on the right
           * hand side (rhs) of the assignment operator.FlightTaskType [only available to derived classes]
           *
           * @param rhs The FlightTaskType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::FlightTaskType
           * @return A constant reference to this FlightTaskType.
           */
         const FlightTaskType& operator=(const FlightTaskType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // FlightTaskType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__FlightTaskType_h

