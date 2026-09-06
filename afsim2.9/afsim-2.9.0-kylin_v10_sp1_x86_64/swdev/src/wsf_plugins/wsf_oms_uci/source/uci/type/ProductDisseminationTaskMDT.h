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
#ifndef Uci__Type__ProductDisseminationTaskMDT_h
#define Uci__Type__ProductDisseminationTaskMDT_h 1

#if !defined(Uci__Type__ProductManagementTaskDataType_h)
# include "uci/type/ProductManagementTaskDataType.h"
#endif

#if !defined(Uci__Type__DisseminationRequirementsType_h)
# include "uci/type/DisseminationRequirementsType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ProductDisseminationTaskMDT sequence accessor class */
      class ProductDisseminationTaskMDT : public virtual uci::type::ProductManagementTaskDataType {
      public:

         /** The destructor */
         virtual ~ProductDisseminationTaskMDT()
         { }

         /** Returns this accessor's type constant, i.e. ProductDisseminationTaskMDT
           *
           * @return This accessor's type constant, i.e. ProductDisseminationTaskMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::productDisseminationTaskMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ProductDisseminationTaskMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DisseminationRequirements.
           *
           * @return The acecssor that provides access to the complex content that is identified by DisseminationRequirements.
           */
         virtual const uci::type::DisseminationRequirementsType& getDisseminationRequirements() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DisseminationRequirements.
           *
           * @return The acecssor that provides access to the complex content that is identified by DisseminationRequirements.
           */
         virtual uci::type::DisseminationRequirementsType& getDisseminationRequirements()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DisseminationRequirements to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DisseminationRequirements
           */
         virtual void setDisseminationRequirements(const uci::type::DisseminationRequirementsType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ProductDisseminationTaskMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ProductDisseminationTaskMDT to copy from
           */
         ProductDisseminationTaskMDT(const ProductDisseminationTaskMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ProductDisseminationTaskMDT to the contents of the
           * ProductDisseminationTaskMDT on the right hand side (rhs) of the assignment operator.ProductDisseminationTaskMDT [only
           * available to derived classes]
           *
           * @param rhs The ProductDisseminationTaskMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::ProductDisseminationTaskMDT
           * @return A constant reference to this ProductDisseminationTaskMDT.
           */
         const ProductDisseminationTaskMDT& operator=(const ProductDisseminationTaskMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ProductDisseminationTaskMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ProductDisseminationTaskMDT_h

