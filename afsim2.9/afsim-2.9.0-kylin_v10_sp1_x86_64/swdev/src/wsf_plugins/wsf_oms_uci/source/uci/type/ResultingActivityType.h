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
#ifndef Uci__Type__ResultingActivityType_h
#define Uci__Type__ResultingActivityType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ActivityID_Type_h)
# include "uci/type/ActivityID_Type.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ResultingActivityType sequence accessor class */
      class ResultingActivityType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ResultingActivityType()
         { }

         /** Returns this accessor's type constant, i.e. ResultingActivityType
           *
           * @return This accessor's type constant, i.e. ResultingActivityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::resultingActivityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ResultingActivityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ActivityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ActivityID.
           */
         virtual const uci::type::ActivityID_Type& getActivityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ActivityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ActivityID.
           */
         virtual uci::type::ActivityID_Type& getActivityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ActivityID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ActivityID
           */
         virtual void setActivityID(const uci::type::ActivityID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the NewActivity.
           *
           * @return The value of the simple primitive data type identified by NewActivity.
           */
         virtual xs::Boolean getNewActivity() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the NewActivity.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setNewActivity(xs::Boolean value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ResultingActivityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ResultingActivityType to copy from
           */
         ResultingActivityType(const ResultingActivityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ResultingActivityType to the contents of the ResultingActivityType
           * on the right hand side (rhs) of the assignment operator.ResultingActivityType [only available to derived classes]
           *
           * @param rhs The ResultingActivityType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::ResultingActivityType
           * @return A constant reference to this ResultingActivityType.
           */
         const ResultingActivityType& operator=(const ResultingActivityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ResultingActivityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ResultingActivityType_h

