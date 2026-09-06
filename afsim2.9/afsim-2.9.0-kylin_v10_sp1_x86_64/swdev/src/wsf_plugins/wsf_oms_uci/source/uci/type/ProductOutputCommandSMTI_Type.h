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
#ifndef Uci__Type__ProductOutputCommandSMTI_Type_h
#define Uci__Type__ProductOutputCommandSMTI_Type_h 1

#if !defined(Uci__Type__ProductOutputCommandBasicType_h)
# include "uci/type/ProductOutputCommandBasicType.h"
#endif

#if !defined(Uci__Type__STANAG_4607_PackingPlanType_h)
# include "uci/type/STANAG_4607_PackingPlanType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ProductOutputCommandSMTI_Type sequence accessor class */
      class ProductOutputCommandSMTI_Type : public virtual uci::type::ProductOutputCommandBasicType {
      public:

         /** The destructor */
         virtual ~ProductOutputCommandSMTI_Type()
         { }

         /** Returns this accessor's type constant, i.e. ProductOutputCommandSMTI_Type
           *
           * @return This accessor's type constant, i.e. ProductOutputCommandSMTI_Type
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::productOutputCommandSMTI_Type;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ProductOutputCommandSMTI_Type& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the STANAG_4607_PackingPlan.
           *
           * @return The acecssor that provides access to the complex content that is identified by STANAG_4607_PackingPlan.
           */
         virtual const uci::type::STANAG_4607_PackingPlanType& getSTANAG_4607_PackingPlan() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the STANAG_4607_PackingPlan.
           *
           * @return The acecssor that provides access to the complex content that is identified by STANAG_4607_PackingPlan.
           */
         virtual uci::type::STANAG_4607_PackingPlanType& getSTANAG_4607_PackingPlan()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the STANAG_4607_PackingPlan to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by STANAG_4607_PackingPlan
           */
         virtual void setSTANAG_4607_PackingPlan(const uci::type::STANAG_4607_PackingPlanType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by STANAG_4607_PackingPlan exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by STANAG_4607_PackingPlan is emabled or not
           */
         virtual bool hasSTANAG_4607_PackingPlan() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by STANAG_4607_PackingPlan
           *
           * @param type = uci::type::accessorType::sTANAG_4607_PackingPlanType This Accessor's accessor type
           */
         virtual void enableSTANAG_4607_PackingPlan(uci::base::accessorType::AccessorType type = uci::type::accessorType::sTANAG_4607_PackingPlanType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by STANAG_4607_PackingPlan */
         virtual void clearSTANAG_4607_PackingPlan()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ProductOutputCommandSMTI_Type()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ProductOutputCommandSMTI_Type to copy from
           */
         ProductOutputCommandSMTI_Type(const ProductOutputCommandSMTI_Type& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ProductOutputCommandSMTI_Type to the contents of the
           * ProductOutputCommandSMTI_Type on the right hand side (rhs) of the assignment operator.ProductOutputCommandSMTI_Type
           * [only available to derived classes]
           *
           * @param rhs The ProductOutputCommandSMTI_Type on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::ProductOutputCommandSMTI_Type
           * @return A constant reference to this ProductOutputCommandSMTI_Type.
           */
         const ProductOutputCommandSMTI_Type& operator=(const ProductOutputCommandSMTI_Type& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ProductOutputCommandSMTI_Type


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ProductOutputCommandSMTI_Type_h

