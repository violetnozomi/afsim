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
#ifndef Uci__Type__DamageAssessmentRequestStatusMDT_h
#define Uci__Type__DamageAssessmentRequestStatusMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DamageAssessmentRequestID_Type_h)
# include "uci/type/DamageAssessmentRequestID_Type.h"
#endif

#if !defined(Uci__Type__DamageAssessmentID_Type_h)
# include "uci/type/DamageAssessmentID_Type.h"
#endif

#if !defined(Uci__Type__ActivityRequestStatusEnum_h)
# include "uci/type/ActivityRequestStatusEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the DamageAssessmentRequestStatusMDT sequence accessor class */
      class DamageAssessmentRequestStatusMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~DamageAssessmentRequestStatusMDT()
         { }

         /** Returns this accessor's type constant, i.e. DamageAssessmentRequestStatusMDT
           *
           * @return This accessor's type constant, i.e. DamageAssessmentRequestStatusMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::damageAssessmentRequestStatusMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DamageAssessmentRequestStatusMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DamageAssessmentRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by DamageAssessmentRequestID.
           */
         virtual const uci::type::DamageAssessmentRequestID_Type& getDamageAssessmentRequestID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DamageAssessmentRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by DamageAssessmentRequestID.
           */
         virtual uci::type::DamageAssessmentRequestID_Type& getDamageAssessmentRequestID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DamageAssessmentRequestID to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DamageAssessmentRequestID
           */
         virtual void setDamageAssessmentRequestID(const uci::type::DamageAssessmentRequestID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DamageAssessmentID.
           *
           * @return The acecssor that provides access to the complex content that is identified by DamageAssessmentID.
           */
         virtual const uci::type::DamageAssessmentID_Type& getDamageAssessmentID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DamageAssessmentID.
           *
           * @return The acecssor that provides access to the complex content that is identified by DamageAssessmentID.
           */
         virtual uci::type::DamageAssessmentID_Type& getDamageAssessmentID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DamageAssessmentID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DamageAssessmentID
           */
         virtual void setDamageAssessmentID(const uci::type::DamageAssessmentID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DamageAssessmentID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DamageAssessmentID is emabled or not
           */
         virtual bool hasDamageAssessmentID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DamageAssessmentID
           *
           * @param type = uci::type::accessorType::damageAssessmentID_Type This Accessor's accessor type
           */
         virtual void enableDamageAssessmentID(uci::base::accessorType::AccessorType type = uci::type::accessorType::damageAssessmentID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DamageAssessmentID */
         virtual void clearDamageAssessmentID()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the State.
           *
           * @return The value of the enumeration identified by State.
           */
         virtual const uci::type::ActivityRequestStatusEnum& getState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the State.
           *
           * @return The value of the enumeration identified by State.
           */
         virtual uci::type::ActivityRequestStatusEnum& getState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the State.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setState(const uci::type::ActivityRequestStatusEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the State.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setState(uci::type::ActivityRequestStatusEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DamageAssessmentRequestStatusMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DamageAssessmentRequestStatusMDT to copy from
           */
         DamageAssessmentRequestStatusMDT(const DamageAssessmentRequestStatusMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DamageAssessmentRequestStatusMDT to the contents of the
           * DamageAssessmentRequestStatusMDT on the right hand side (rhs) of the assignment
           * operator.DamageAssessmentRequestStatusMDT [only available to derived classes]
           *
           * @param rhs The DamageAssessmentRequestStatusMDT on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::DamageAssessmentRequestStatusMDT
           * @return A constant reference to this DamageAssessmentRequestStatusMDT.
           */
         const DamageAssessmentRequestStatusMDT& operator=(const DamageAssessmentRequestStatusMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DamageAssessmentRequestStatusMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DamageAssessmentRequestStatusMDT_h

