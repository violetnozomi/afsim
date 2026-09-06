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
#ifndef Uci__Type__AllocatedTaskConstraintType_h
#define Uci__Type__AllocatedTaskConstraintType_h 1

#if !defined(Uci__Type__AllocatedTaskType_h)
# include "uci/type/AllocatedTaskType.h"
#endif

#if !defined(Uci__Type__TaskAllocationParametersType_h)
# include "uci/type/TaskAllocationParametersType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the AllocatedTaskConstraintType sequence accessor class */
      class AllocatedTaskConstraintType : public virtual uci::type::AllocatedTaskType {
      public:

         /** The destructor */
         virtual ~AllocatedTaskConstraintType()
         { }

         /** Returns this accessor's type constant, i.e. AllocatedTaskConstraintType
           *
           * @return This accessor's type constant, i.e. AllocatedTaskConstraintType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::allocatedTaskConstraintType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AllocatedTaskConstraintType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * AllocationConstraintOverride.
           *
           * @return The acecssor that provides access to the complex content that is identified by AllocationConstraintOverride.
           */
         virtual const uci::type::TaskAllocationParametersType& getAllocationConstraintOverride() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * AllocationConstraintOverride.
           *
           * @return The acecssor that provides access to the complex content that is identified by AllocationConstraintOverride.
           */
         virtual uci::type::TaskAllocationParametersType& getAllocationConstraintOverride()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AllocationConstraintOverride to the contents of the complex
           * content that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AllocationConstraintOverride
           */
         virtual void setAllocationConstraintOverride(const uci::type::TaskAllocationParametersType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AllocationConstraintOverride exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AllocationConstraintOverride is emabled or not
           */
         virtual bool hasAllocationConstraintOverride() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AllocationConstraintOverride
           *
           * @param type = uci::type::accessorType::taskAllocationParametersType This Accessor's accessor type
           */
         virtual void enableAllocationConstraintOverride(uci::base::accessorType::AccessorType type = uci::type::accessorType::taskAllocationParametersType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AllocationConstraintOverride */
         virtual void clearAllocationConstraintOverride()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AllocatedTaskConstraintType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AllocatedTaskConstraintType to copy from
           */
         AllocatedTaskConstraintType(const AllocatedTaskConstraintType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AllocatedTaskConstraintType to the contents of the
           * AllocatedTaskConstraintType on the right hand side (rhs) of the assignment operator.AllocatedTaskConstraintType [only
           * available to derived classes]
           *
           * @param rhs The AllocatedTaskConstraintType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::AllocatedTaskConstraintType
           * @return A constant reference to this AllocatedTaskConstraintType.
           */
         const AllocatedTaskConstraintType& operator=(const AllocatedTaskConstraintType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AllocatedTaskConstraintType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AllocatedTaskConstraintType_h

