// This file was generated  on 12/5/2018 at 1:1:44 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__CapabilityAssessmentActivityDetailsType_h
#define Uci__Type__CapabilityAssessmentActivityDetailsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__CapabilityAssessmentActivityType_h)
# include "uci/type/CapabilityAssessmentActivityType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CapabilityAssessmentActivityDetailsType sequence accessor class */
      class CapabilityAssessmentActivityDetailsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CapabilityAssessmentActivityDetailsType()
         { }

         /** Returns this accessor's type constant, i.e. CapabilityAssessmentActivityDetailsType
           *
           * @return This accessor's type constant, i.e. CapabilityAssessmentActivityDetailsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::capabilityAssessmentActivityDetailsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CapabilityAssessmentActivityDetailsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ActivityApproved.
           *
           * @return The value of the simple primitive data type identified by ActivityApproved.
           */
         virtual xs::Boolean getActivityApproved() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ActivityApproved.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setActivityApproved(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Activity.
           *
           * @return The acecssor that provides access to the complex content that is identified by Activity.
           */
         virtual const uci::type::CapabilityAssessmentActivityType& getActivity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Activity.
           *
           * @return The acecssor that provides access to the complex content that is identified by Activity.
           */
         virtual uci::type::CapabilityAssessmentActivityType& getActivity()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Activity to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Activity
           */
         virtual void setActivity(const uci::type::CapabilityAssessmentActivityType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CapabilityAssessmentActivityDetailsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CapabilityAssessmentActivityDetailsType to copy from
           */
         CapabilityAssessmentActivityDetailsType(const CapabilityAssessmentActivityDetailsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CapabilityAssessmentActivityDetailsType to the contents of the
           * CapabilityAssessmentActivityDetailsType on the right hand side (rhs) of the assignment
           * operator.CapabilityAssessmentActivityDetailsType [only available to derived classes]
           *
           * @param rhs The CapabilityAssessmentActivityDetailsType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::CapabilityAssessmentActivityDetailsType
           * @return A constant reference to this CapabilityAssessmentActivityDetailsType.
           */
         const CapabilityAssessmentActivityDetailsType& operator=(const CapabilityAssessmentActivityDetailsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CapabilityAssessmentActivityDetailsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CapabilityAssessmentActivityDetailsType_h

