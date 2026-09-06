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
#ifndef Uci__Type__RoutePlanningAutonomousRerouteType_h
#define Uci__Type__RoutePlanningAutonomousRerouteType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__RoutePlanningTypeEnum_h)
# include "uci/type/RoutePlanningTypeEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the RoutePlanningAutonomousRerouteType sequence accessor class */
      class RoutePlanningAutonomousRerouteType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~RoutePlanningAutonomousRerouteType()
         { }

         /** Returns this accessor's type constant, i.e. RoutePlanningAutonomousRerouteType
           *
           * @return This accessor's type constant, i.e. RoutePlanningAutonomousRerouteType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::routePlanningAutonomousRerouteType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RoutePlanningAutonomousRerouteType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the RerouteAllSegments.
           *
           * @return The value of the simple primitive data type identified by RerouteAllSegments.
           */
         virtual xs::Boolean getRerouteAllSegments() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the RerouteAllSegments.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRerouteAllSegments(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the RoutePlanningType.
           *
           * @return The value of the enumeration identified by RoutePlanningType.
           */
         virtual const uci::type::RoutePlanningTypeEnum& getRoutePlanningType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the RoutePlanningType.
           *
           * @return The value of the enumeration identified by RoutePlanningType.
           */
         virtual uci::type::RoutePlanningTypeEnum& getRoutePlanningType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the RoutePlanningType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setRoutePlanningType(const uci::type::RoutePlanningTypeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the RoutePlanningType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setRoutePlanningType(uci::type::RoutePlanningTypeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RoutePlanningAutonomousRerouteType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RoutePlanningAutonomousRerouteType to copy from
           */
         RoutePlanningAutonomousRerouteType(const RoutePlanningAutonomousRerouteType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RoutePlanningAutonomousRerouteType to the contents of the
           * RoutePlanningAutonomousRerouteType on the right hand side (rhs) of the assignment
           * operator.RoutePlanningAutonomousRerouteType [only available to derived classes]
           *
           * @param rhs The RoutePlanningAutonomousRerouteType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::RoutePlanningAutonomousRerouteType
           * @return A constant reference to this RoutePlanningAutonomousRerouteType.
           */
         const RoutePlanningAutonomousRerouteType& operator=(const RoutePlanningAutonomousRerouteType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RoutePlanningAutonomousRerouteType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RoutePlanningAutonomousRerouteType_h

