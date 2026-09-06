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
#ifndef Uci__Type__DisseminationRequirementsType_h
#define Uci__Type__DisseminationRequirementsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__InputProductType_h)
# include "uci/type/InputProductType.h"
#endif

#if !defined(Uci__Type__DisseminationSubplanType_h)
# include "uci/type/DisseminationSubplanType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the DisseminationRequirementsType sequence accessor class */
      class DisseminationRequirementsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~DisseminationRequirementsType()
         { }

         /** Returns this accessor's type constant, i.e. DisseminationRequirementsType
           *
           * @return This accessor's type constant, i.e. DisseminationRequirementsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::disseminationRequirementsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DisseminationRequirementsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** This element specifies the product(s) to which this requirement applies. [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::InputProductType, uci::type::accessorType::inputProductType> InputProduct;

         /** Returns the bounded list that is identified by the InputProduct.
           *
           * @return The bounded list identified by InputProduct.
           */
         virtual const uci::type::DisseminationRequirementsType::InputProduct& getInputProduct() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the InputProduct.
           *
           * @return The bounded list identified by InputProduct.
           */
         virtual uci::type::DisseminationRequirementsType::InputProduct& getInputProduct()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the InputProduct.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setInputProduct(const uci::type::DisseminationRequirementsType::InputProduct& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DisseminationPlan.
           *
           * @return The acecssor that provides access to the complex content that is identified by DisseminationPlan.
           */
         virtual const uci::type::DisseminationSubplanType& getDisseminationPlan() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DisseminationPlan.
           *
           * @return The acecssor that provides access to the complex content that is identified by DisseminationPlan.
           */
         virtual uci::type::DisseminationSubplanType& getDisseminationPlan()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DisseminationPlan to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DisseminationPlan
           */
         virtual void setDisseminationPlan(const uci::type::DisseminationSubplanType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DisseminationPlan exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DisseminationPlan is emabled or not
           */
         virtual bool hasDisseminationPlan() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DisseminationPlan
           *
           * @param type = uci::type::accessorType::disseminationSubplanType This Accessor's accessor type
           */
         virtual void enableDisseminationPlan(uci::base::accessorType::AccessorType type = uci::type::accessorType::disseminationSubplanType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DisseminationPlan */
         virtual void clearDisseminationPlan()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DisseminationRequirementsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DisseminationRequirementsType to copy from
           */
         DisseminationRequirementsType(const DisseminationRequirementsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DisseminationRequirementsType to the contents of the
           * DisseminationRequirementsType on the right hand side (rhs) of the assignment operator.DisseminationRequirementsType
           * [only available to derived classes]
           *
           * @param rhs The DisseminationRequirementsType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::DisseminationRequirementsType
           * @return A constant reference to this DisseminationRequirementsType.
           */
         const DisseminationRequirementsType& operator=(const DisseminationRequirementsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DisseminationRequirementsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DisseminationRequirementsType_h

