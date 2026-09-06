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
#ifndef Uci__Type__CommPointingPlanDataType_h
#define Uci__Type__CommPointingPlanDataType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__PointingPlanWindowType_h)
# include "uci/type/PointingPlanWindowType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** The results of an assessment that determines Comms coverage for a set of Mission Plans. */
      class CommPointingPlanDataType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CommPointingPlanDataType()
         { }

         /** Returns this accessor's type constant, i.e. CommPointingPlanDataType
           *
           * @return This accessor's type constant, i.e. CommPointingPlanDataType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commPointingPlanDataType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommPointingPlanDataType& accessor)
            throw(uci::base::UCIException) = 0;


         /** The List of Time bounded Windows supported by this Pointing Plan. [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::PointingPlanWindowType, uci::type::accessorType::pointingPlanWindowType> Window;

         /** Returns the bounded list that is identified by the Window.
           *
           * @return The bounded list identified by Window.
           */
         virtual const uci::type::CommPointingPlanDataType::Window& getWindow() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Window.
           *
           * @return The bounded list identified by Window.
           */
         virtual uci::type::CommPointingPlanDataType::Window& getWindow()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Window.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setWindow(const uci::type::CommPointingPlanDataType::Window& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommPointingPlanDataType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommPointingPlanDataType to copy from
           */
         CommPointingPlanDataType(const CommPointingPlanDataType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommPointingPlanDataType to the contents of the
           * CommPointingPlanDataType on the right hand side (rhs) of the assignment operator.CommPointingPlanDataType [only
           * available to derived classes]
           *
           * @param rhs The CommPointingPlanDataType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::CommPointingPlanDataType
           * @return A constant reference to this CommPointingPlanDataType.
           */
         const CommPointingPlanDataType& operator=(const CommPointingPlanDataType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommPointingPlanDataType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommPointingPlanDataType_h

