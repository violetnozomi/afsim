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
#ifndef Uci__Type__ProductManagementTaskStatusDataType_h
#define Uci__Type__ProductManagementTaskStatusDataType_h 1

#if !defined(Uci__Type__TaskStatusDataBaseType_h)
# include "uci/type/TaskStatusDataBaseType.h"
#endif

#if !defined(Uci__Type__ProductManagementTaskStateEnum_h)
# include "uci/type/ProductManagementTaskStateEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the base type for all ProductManagement Task status. */
      class ProductManagementTaskStatusDataType : public virtual uci::type::TaskStatusDataBaseType {
      public:

         /** The destructor */
         virtual ~ProductManagementTaskStatusDataType()
         { }

         /** Returns this accessor's type constant, i.e. ProductManagementTaskStatusDataType
           *
           * @return This accessor's type constant, i.e. ProductManagementTaskStatusDataType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::productManagementTaskStatusDataType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ProductManagementTaskStatusDataType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the TaskState.
           *
           * @return The value of the enumeration identified by TaskState.
           */
         virtual const uci::type::ProductManagementTaskStateEnum& getTaskState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the TaskState.
           *
           * @return The value of the enumeration identified by TaskState.
           */
         virtual uci::type::ProductManagementTaskStateEnum& getTaskState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the TaskState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setTaskState(const uci::type::ProductManagementTaskStateEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the TaskState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setTaskState(uci::type::ProductManagementTaskStateEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ProductManagementTaskStatusDataType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ProductManagementTaskStatusDataType to copy from
           */
         ProductManagementTaskStatusDataType(const ProductManagementTaskStatusDataType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ProductManagementTaskStatusDataType to the contents of the
           * ProductManagementTaskStatusDataType on the right hand side (rhs) of the assignment
           * operator.ProductManagementTaskStatusDataType [only available to derived classes]
           *
           * @param rhs The ProductManagementTaskStatusDataType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::ProductManagementTaskStatusDataType
           * @return A constant reference to this ProductManagementTaskStatusDataType.
           */
         const ProductManagementTaskStatusDataType& operator=(const ProductManagementTaskStatusDataType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ProductManagementTaskStatusDataType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ProductManagementTaskStatusDataType_h

