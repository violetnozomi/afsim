// This file was generated  on 10/18/2017 at 5:35:28 AM by the Boeing OMS CAL generation tools
// @warning  This file was automatically generated, edit at your own risk

/**
* Unclassified               U N C L A S S I F I E D               Unclassified
*
* DEVELOPMENT:
*    This document wholly developed with USG funds.
*    For additional information, contact the AFRCO.
*
* DISTRIBUTION STATEMENT D.  Distribution authorized to the Department 
*    of Defense and U.S. DoD contractors only; Critical Technology; 26 Mar 13.
*    Other requests shall be referred to the Air Force Rapid Capabilities 
*    Office, 202-404-5497 or 202-404-5498.
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
#ifndef Uci__Type__CommPointingPlanType_h
#define Uci__Type__CommPointingPlanType_h 1

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

      /** This is the CommPointingPlanType sequence accessor class */
      class CommPointingPlanType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CommPointingPlanType()
         { }

         /** Returns this accessor's type constant, i.e. CommPointingPlanType
           *
           * @return This accessor's type constant, i.e. CommPointingPlanType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commPointingPlanType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommPointingPlanType& accessor)
            throw(uci::base::UCIException) = 0;


         /** The List of Time bounded Windows supported by this Pointing Plan. [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::PointingPlanWindowType, uci::type::accessorType::pointingPlanWindowType> Window;

         /** Returns the bounded list that is identified by the Window.
           *
           * @return The bounded list identified by Window.
           */
         virtual const uci::type::CommPointingPlanType::Window& getWindow() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Window.
           *
           * @return The bounded list identified by Window.
           */
         virtual uci::type::CommPointingPlanType::Window& getWindow()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Window.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setWindow(const uci::type::CommPointingPlanType::Window& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommPointingPlanType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommPointingPlanType to copy from
           */
         CommPointingPlanType(const CommPointingPlanType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommPointingPlanType to the contents of the CommPointingPlanType
           * on the right hand side (rhs) of the assignment operator.CommPointingPlanType [only available to derived classes]
           *
           * @param rhs The CommPointingPlanType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::CommPointingPlanType
           * @return A constant reference to this CommPointingPlanType.
           */
         const CommPointingPlanType& operator=(const CommPointingPlanType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommPointingPlanType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommPointingPlanType_h

