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
#ifndef Uci__Type__TaskSetMDT_h
#define Uci__Type__TaskSetMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__TaskSetID_Type_h)
# include "uci/type/TaskSetID_Type.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__TaskID_Type_h)
# include "uci/type/TaskID_Type.h"
#endif

#if !defined(Uci__Type__TaskAssociationConstraintType_h)
# include "uci/type/TaskAssociationConstraintType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the TaskSetMDT sequence accessor class */
      class TaskSetMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~TaskSetMDT()
         { }

         /** Returns this accessor's type constant, i.e. TaskSetMDT
           *
           * @return This accessor's type constant, i.e. TaskSetMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::taskSetMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const TaskSetMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a list of current, active Tasks which are eligible for the next planning cycle. [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::TaskID_Type, uci::type::accessorType::taskID_Type> TaskID;

         /** Indicates an association between Tasks that is a constraint on how the Tasks are allocated and planned. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::TaskAssociationConstraintType, uci::type::accessorType::taskAssociationConstraintType> AssociationConstraint;

         /** Returns the accessor that provides access to the complex content that is identified by the TaskSetID.
           *
           * @return The acecssor that provides access to the complex content that is identified by TaskSetID.
           */
         virtual const uci::type::TaskSetID_Type& getTaskSetID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TaskSetID.
           *
           * @return The acecssor that provides access to the complex content that is identified by TaskSetID.
           */
         virtual uci::type::TaskSetID_Type& getTaskSetID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TaskSetID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TaskSetID
           */
         virtual void setTaskSetID(const uci::type::TaskSetID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ManagingSystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ManagingSystemID.
           */
         virtual const uci::type::SystemID_Type& getManagingSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ManagingSystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ManagingSystemID.
           */
         virtual uci::type::SystemID_Type& getManagingSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ManagingSystemID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ManagingSystemID
           */
         virtual void setManagingSystemID(const uci::type::SystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TaskID.
           *
           * @return The bounded list identified by TaskID.
           */
         virtual const uci::type::TaskSetMDT::TaskID& getTaskID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TaskID.
           *
           * @return The bounded list identified by TaskID.
           */
         virtual uci::type::TaskSetMDT::TaskID& getTaskID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TaskID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTaskID(const uci::type::TaskSetMDT::TaskID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AssociationConstraint.
           *
           * @return The bounded list identified by AssociationConstraint.
           */
         virtual const uci::type::TaskSetMDT::AssociationConstraint& getAssociationConstraint() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AssociationConstraint.
           *
           * @return The bounded list identified by AssociationConstraint.
           */
         virtual uci::type::TaskSetMDT::AssociationConstraint& getAssociationConstraint()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AssociationConstraint.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAssociationConstraint(const uci::type::TaskSetMDT::AssociationConstraint& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         TaskSetMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The TaskSetMDT to copy from
           */
         TaskSetMDT(const TaskSetMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this TaskSetMDT to the contents of the TaskSetMDT on the right hand
           * side (rhs) of the assignment operator.TaskSetMDT [only available to derived classes]
           *
           * @param rhs The TaskSetMDT on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::TaskSetMDT
           * @return A constant reference to this TaskSetMDT.
           */
         const TaskSetMDT& operator=(const TaskSetMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // TaskSetMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__TaskSetMDT_h

