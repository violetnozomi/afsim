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
#ifndef Uci__Type__AMTI_TaskType_h
#define Uci__Type__AMTI_TaskType_h 1

#if !defined(Uci__Type__AMTI_TaskBaseType_h)
# include "uci/type/AMTI_TaskBaseType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__AMTI_TargetType_h)
# include "uci/type/AMTI_TargetType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the AMTI_TaskType sequence accessor class */
      class AMTI_TaskType : public virtual uci::type::AMTI_TaskBaseType {
      public:

         /** The destructor */
         virtual ~AMTI_TaskType()
         { }

         /** Returns this accessor's type constant, i.e. AMTI_TaskType
           *
           * @return This accessor's type constant, i.e. AMTI_TaskType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::aMTI_TaskType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AMTI_TaskType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the volume or specific Entity that defines the extents of the spatial region covered by the command. For
           * non-CUED AMTI Capabilities, one air volume instance of this element is expected. For CUED AMTI Capabilities, two
           * instances of this element are allowed: one with an Entity specified and a second with an air volume that define the
           * extents of the cue volume around the Entity. For CUED AMTI Capabilities, a single instance is allowed if it is an
           * Entity in which case the Entity location uncertainty defines the extents of the cue volume. [Maximum occurrences: 2]
           */
         typedef uci::base::BoundedList<uci::type::AMTI_TargetType, uci::type::accessorType::aMTI_TargetType> Target;

         /** Returns the bounded list that is identified by the Target.
           *
           * @return The bounded list identified by Target.
           */
         virtual const uci::type::AMTI_TaskType::Target& getTarget() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Target.
           *
           * @return The bounded list identified by Target.
           */
         virtual uci::type::AMTI_TaskType::Target& getTarget()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Target.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTarget(const uci::type::AMTI_TaskType::Target& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AMTI_TaskType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AMTI_TaskType to copy from
           */
         AMTI_TaskType(const AMTI_TaskType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AMTI_TaskType to the contents of the AMTI_TaskType on the right
           * hand side (rhs) of the assignment operator.AMTI_TaskType [only available to derived classes]
           *
           * @param rhs The AMTI_TaskType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::AMTI_TaskType
           * @return A constant reference to this AMTI_TaskType.
           */
         const AMTI_TaskType& operator=(const AMTI_TaskType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AMTI_TaskType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AMTI_TaskType_h

