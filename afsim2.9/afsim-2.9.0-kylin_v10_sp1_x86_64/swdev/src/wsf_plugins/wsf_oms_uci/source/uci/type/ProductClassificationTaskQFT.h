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
#ifndef Uci__Type__ProductClassificationTaskQFT_h
#define Uci__Type__ProductClassificationTaskQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__TaskID_Type_h)
# include "uci/type/TaskID_Type.h"
#endif

#if !defined(Uci__Type__DestinationID_Type_h)
# include "uci/type/DestinationID_Type.h"
#endif

#if !defined(Uci__Type__TraceabilityType_h)
# include "uci/type/TraceabilityType.h"
#endif

#if !defined(Uci__Type__ProductManagementTaskStateEnum_h)
# include "uci/type/ProductManagementTaskStateEnum.h"
#endif

#if !defined(Uci__Type__ProductReferenceType_h)
# include "uci/type/ProductReferenceType.h"
#endif

#if !defined(Uci__Type__ProductTypeEnum_h)
# include "uci/type/ProductTypeEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ProductClassificationTaskQFT sequence accessor class */
      class ProductClassificationTaskQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~ProductClassificationTaskQFT()
         { }

         /** Returns this accessor's type constant, i.e. ProductClassificationTaskQFT
           *
           * @return This accessor's type constant, i.e. ProductClassificationTaskQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::productClassificationTaskQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ProductClassificationTaskQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Filter on the System ID [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::SystemID_Type, uci::type::accessorType::systemID_Type> SystemID;

         /** Filter on the Task ID [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::TaskID_Type, uci::type::accessorType::taskID_Type> TaskID;

         /** Filter on the Destination ID [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::DestinationID_Type, uci::type::accessorType::destinationID_Type> DestinationID;

         /** Filter on the parent task ID [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::TaskID_Type, uci::type::accessorType::taskID_Type> ParentTaskID;

         /** Filter on the dependent task ID [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::TaskID_Type, uci::type::accessorType::taskID_Type> DependentTaskID;

         /** Identifies the current state of a task. [Minimum occurrences: 0] [Maximum occurrences: 6] */
         typedef uci::base::BoundedList<uci::type::ProductManagementTaskStateEnum, uci::type::accessorType::productManagementTaskStateEnum> TaskState;

         /** Filter on the product reference elements [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::ProductReferenceType, uci::type::accessorType::productReferenceType> Product;

         /** Filter on the type of product [Minimum occurrences: 0] [Maximum occurrences: 78] */
         typedef uci::base::BoundedList<uci::type::ProductTypeEnum, uci::type::accessorType::productTypeEnum> ProductType;

         /** Returns the value of the SimplePrimitive data type that is identified by the IncludeStatus.
           *
           * @return The value of the simple primitive data type identified by IncludeStatus.
           */
         virtual xs::Boolean getIncludeStatus() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the IncludeStatus.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setIncludeStatus(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SystemID.
           *
           * @return The bounded list identified by SystemID.
           */
         virtual const uci::type::ProductClassificationTaskQFT::SystemID& getSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SystemID.
           *
           * @return The bounded list identified by SystemID.
           */
         virtual uci::type::ProductClassificationTaskQFT::SystemID& getSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SystemID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSystemID(const uci::type::ProductClassificationTaskQFT::SystemID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TaskID.
           *
           * @return The bounded list identified by TaskID.
           */
         virtual const uci::type::ProductClassificationTaskQFT::TaskID& getTaskID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TaskID.
           *
           * @return The bounded list identified by TaskID.
           */
         virtual uci::type::ProductClassificationTaskQFT::TaskID& getTaskID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TaskID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTaskID(const uci::type::ProductClassificationTaskQFT::TaskID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DestinationID.
           *
           * @return The bounded list identified by DestinationID.
           */
         virtual const uci::type::ProductClassificationTaskQFT::DestinationID& getDestinationID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DestinationID.
           *
           * @return The bounded list identified by DestinationID.
           */
         virtual uci::type::ProductClassificationTaskQFT::DestinationID& getDestinationID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the DestinationID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setDestinationID(const uci::type::ProductClassificationTaskQFT::DestinationID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Traceability.
           *
           * @return The acecssor that provides access to the complex content that is identified by Traceability.
           */
         virtual const uci::type::TraceabilityType& getTraceability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Traceability.
           *
           * @return The acecssor that provides access to the complex content that is identified by Traceability.
           */
         virtual uci::type::TraceabilityType& getTraceability()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Traceability to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Traceability
           */
         virtual void setTraceability(const uci::type::TraceabilityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Traceability exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Traceability is emabled or not
           */
         virtual bool hasTraceability() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Traceability
           *
           * @param type = uci::type::accessorType::traceabilityType This Accessor's accessor type
           */
         virtual void enableTraceability(uci::base::accessorType::AccessorType type = uci::type::accessorType::traceabilityType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Traceability */
         virtual void clearTraceability()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ParentTaskID.
           *
           * @return The bounded list identified by ParentTaskID.
           */
         virtual const uci::type::ProductClassificationTaskQFT::ParentTaskID& getParentTaskID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ParentTaskID.
           *
           * @return The bounded list identified by ParentTaskID.
           */
         virtual uci::type::ProductClassificationTaskQFT::ParentTaskID& getParentTaskID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ParentTaskID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setParentTaskID(const uci::type::ProductClassificationTaskQFT::ParentTaskID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DependentTaskID.
           *
           * @return The bounded list identified by DependentTaskID.
           */
         virtual const uci::type::ProductClassificationTaskQFT::DependentTaskID& getDependentTaskID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DependentTaskID.
           *
           * @return The bounded list identified by DependentTaskID.
           */
         virtual uci::type::ProductClassificationTaskQFT::DependentTaskID& getDependentTaskID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the DependentTaskID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setDependentTaskID(const uci::type::ProductClassificationTaskQFT::DependentTaskID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TaskState.
           *
           * @return The bounded list identified by TaskState.
           */
         virtual const uci::type::ProductClassificationTaskQFT::TaskState& getTaskState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TaskState.
           *
           * @return The bounded list identified by TaskState.
           */
         virtual uci::type::ProductClassificationTaskQFT::TaskState& getTaskState()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TaskState.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTaskState(const uci::type::ProductClassificationTaskQFT::TaskState& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Product.
           *
           * @return The bounded list identified by Product.
           */
         virtual const uci::type::ProductClassificationTaskQFT::Product& getProduct() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Product.
           *
           * @return The bounded list identified by Product.
           */
         virtual uci::type::ProductClassificationTaskQFT::Product& getProduct()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Product.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setProduct(const uci::type::ProductClassificationTaskQFT::Product& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductType.
           *
           * @return The bounded list identified by ProductType.
           */
         virtual const uci::type::ProductClassificationTaskQFT::ProductType& getProductType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductType.
           *
           * @return The bounded list identified by ProductType.
           */
         virtual uci::type::ProductClassificationTaskQFT::ProductType& getProductType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ProductType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setProductType(const uci::type::ProductClassificationTaskQFT::ProductType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ProductClassificationTaskQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ProductClassificationTaskQFT to copy from
           */
         ProductClassificationTaskQFT(const ProductClassificationTaskQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ProductClassificationTaskQFT to the contents of the
           * ProductClassificationTaskQFT on the right hand side (rhs) of the assignment operator.ProductClassificationTaskQFT
           * [only available to derived classes]
           *
           * @param rhs The ProductClassificationTaskQFT on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::ProductClassificationTaskQFT
           * @return A constant reference to this ProductClassificationTaskQFT.
           */
         const ProductClassificationTaskQFT& operator=(const ProductClassificationTaskQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ProductClassificationTaskQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ProductClassificationTaskQFT_h

