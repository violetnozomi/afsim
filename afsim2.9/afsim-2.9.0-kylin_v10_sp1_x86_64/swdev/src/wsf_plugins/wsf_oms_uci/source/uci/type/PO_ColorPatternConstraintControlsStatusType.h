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
#ifndef Uci__Type__PO_ColorPatternConstraintControlsStatusType_h
#define Uci__Type__PO_ColorPatternConstraintControlsStatusType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ComponentControlsStatusType_h)
# include "uci/type/ComponentControlsStatusType.h"
#endif

#if !defined(Uci__Type__CollectionPatternEnum_h)
# include "uci/type/CollectionPatternEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the PO_ColorPatternConstraintControlsStatusType sequence accessor class */
      class PO_ColorPatternConstraintControlsStatusType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~PO_ColorPatternConstraintControlsStatusType()
         { }

         /** Returns this accessor's type constant, i.e. PO_ColorPatternConstraintControlsStatusType
           *
           * @return This accessor's type constant, i.e. PO_ColorPatternConstraintControlsStatusType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pO_ColorPatternConstraintControlsStatusType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PO_ColorPatternConstraintControlsStatusType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Status.
           *
           * @return The acecssor that provides access to the complex content that is identified by Status.
           */
         virtual const uci::type::ComponentControlsStatusType& getStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Status.
           *
           * @return The acecssor that provides access to the complex content that is identified by Status.
           */
         virtual uci::type::ComponentControlsStatusType& getStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Status to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Status
           */
         virtual void setStatus(const uci::type::ComponentControlsStatusType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Setting.
           *
           * @return The value of the enumeration identified by Setting.
           */
         virtual const uci::type::CollectionPatternEnum& getSetting() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Setting.
           *
           * @return The value of the enumeration identified by Setting.
           */
         virtual uci::type::CollectionPatternEnum& getSetting()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Setting.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSetting(const uci::type::CollectionPatternEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Setting.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSetting(uci::type::CollectionPatternEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PO_ColorPatternConstraintControlsStatusType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PO_ColorPatternConstraintControlsStatusType to copy from
           */
         PO_ColorPatternConstraintControlsStatusType(const PO_ColorPatternConstraintControlsStatusType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PO_ColorPatternConstraintControlsStatusType to the contents of the
           * PO_ColorPatternConstraintControlsStatusType on the right hand side (rhs) of the assignment
           * operator.PO_ColorPatternConstraintControlsStatusType [only available to derived classes]
           *
           * @param rhs The PO_ColorPatternConstraintControlsStatusType on the right hand side (rhs) of the assignment operator
           *      whose contents are used to set the contents of this uci::type::PO_ColorPatternConstraintControlsStatusType
           * @return A constant reference to this PO_ColorPatternConstraintControlsStatusType.
           */
         const PO_ColorPatternConstraintControlsStatusType& operator=(const PO_ColorPatternConstraintControlsStatusType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PO_ColorPatternConstraintControlsStatusType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PO_ColorPatternConstraintControlsStatusType_h

