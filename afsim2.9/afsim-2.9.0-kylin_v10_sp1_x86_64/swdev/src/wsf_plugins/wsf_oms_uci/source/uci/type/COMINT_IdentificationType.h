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
#ifndef Uci__Type__COMINT_IdentificationType_h
#define Uci__Type__COMINT_IdentificationType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__EntityID_Type_h)
# include "uci/type/EntityID_Type.h"
#endif

#if !defined(Uci__Type__IdentityConfidenceType_h)
# include "uci/type/IdentityConfidenceType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the COMINT_IdentificationType sequence accessor class */
      class COMINT_IdentificationType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~COMINT_IdentificationType()
         { }

         /** Returns this accessor's type constant, i.e. COMINT_IdentificationType
           *
           * @return This accessor's type constant, i.e. COMINT_IdentificationType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::cOMINT_IdentificationType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const COMINT_IdentificationType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EntityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by EntityID.
           */
         virtual const uci::type::EntityID_Type& getEntityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EntityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by EntityID.
           */
         virtual uci::type::EntityID_Type& getEntityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EntityID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EntityID
           */
         virtual void setEntityID(const uci::type::EntityID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the IdentificationResults.
           *
           * @return The acecssor that provides access to the complex content that is identified by IdentificationResults.
           */
         virtual const uci::type::IdentityConfidenceType& getIdentificationResults() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the IdentificationResults.
           *
           * @return The acecssor that provides access to the complex content that is identified by IdentificationResults.
           */
         virtual uci::type::IdentityConfidenceType& getIdentificationResults()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the IdentificationResults to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by IdentificationResults
           */
         virtual void setIdentificationResults(const uci::type::IdentityConfidenceType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by IdentificationResults exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by IdentificationResults is emabled or not
           */
         virtual bool hasIdentificationResults() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by IdentificationResults
           *
           * @param type = uci::type::accessorType::identityConfidenceType This Accessor's accessor type
           */
         virtual void enableIdentificationResults(uci::base::accessorType::AccessorType type = uci::type::accessorType::identityConfidenceType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by IdentificationResults */
         virtual void clearIdentificationResults()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         COMINT_IdentificationType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The COMINT_IdentificationType to copy from
           */
         COMINT_IdentificationType(const COMINT_IdentificationType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this COMINT_IdentificationType to the contents of the
           * COMINT_IdentificationType on the right hand side (rhs) of the assignment operator.COMINT_IdentificationType [only
           * available to derived classes]
           *
           * @param rhs The COMINT_IdentificationType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::COMINT_IdentificationType
           * @return A constant reference to this COMINT_IdentificationType.
           */
         const COMINT_IdentificationType& operator=(const COMINT_IdentificationType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // COMINT_IdentificationType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__COMINT_IdentificationType_h

