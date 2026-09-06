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
#ifndef Uci__Type__ProductProcessingClassificationType_h
#define Uci__Type__ProductProcessingClassificationType_h 1

#if !defined(Uci__Type__ProductProcessingType_h)
# include "uci/type/ProductProcessingType.h"
#endif

#if !defined(Uci__Type__SecurityInformationType_h)
# include "uci/type/SecurityInformationType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ProductProcessingClassificationType sequence accessor class */
      class ProductProcessingClassificationType : public virtual uci::type::ProductProcessingType {
      public:

         /** The destructor */
         virtual ~ProductProcessingClassificationType()
         { }

         /** Returns this accessor's type constant, i.e. ProductProcessingClassificationType
           *
           * @return This accessor's type constant, i.e. ProductProcessingClassificationType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::productProcessingClassificationType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ProductProcessingClassificationType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ProductClassification.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProductClassification.
           */
         virtual const uci::type::SecurityInformationType& getProductClassification() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ProductClassification.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProductClassification.
           */
         virtual uci::type::SecurityInformationType& getProductClassification()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ProductClassification to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ProductClassification
           */
         virtual void setProductClassification(const uci::type::SecurityInformationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ProductClassification exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ProductClassification is emabled or not
           */
         virtual bool hasProductClassification() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ProductClassification
           *
           * @param type = uci::type::accessorType::securityInformationType This Accessor's accessor type
           */
         virtual void enableProductClassification(uci::base::accessorType::AccessorType type = uci::type::accessorType::securityInformationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ProductClassification */
         virtual void clearProductClassification()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ProductProcessingClassificationType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ProductProcessingClassificationType to copy from
           */
         ProductProcessingClassificationType(const ProductProcessingClassificationType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ProductProcessingClassificationType to the contents of the
           * ProductProcessingClassificationType on the right hand side (rhs) of the assignment
           * operator.ProductProcessingClassificationType [only available to derived classes]
           *
           * @param rhs The ProductProcessingClassificationType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::ProductProcessingClassificationType
           * @return A constant reference to this ProductProcessingClassificationType.
           */
         const ProductProcessingClassificationType& operator=(const ProductProcessingClassificationType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ProductProcessingClassificationType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ProductProcessingClassificationType_h

