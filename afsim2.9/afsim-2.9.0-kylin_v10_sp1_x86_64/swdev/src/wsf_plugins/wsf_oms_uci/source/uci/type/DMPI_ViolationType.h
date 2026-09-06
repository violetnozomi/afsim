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
#ifndef Uci__Type__DMPI_ViolationType_h
#define Uci__Type__DMPI_ViolationType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DMPI_ViolationChoiceType_h)
# include "uci/type/DMPI_ViolationChoiceType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__DMPI_ID_Type_h)
# include "uci/type/DMPI_ID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the DMPI_ViolationType sequence accessor class */
      class DMPI_ViolationType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~DMPI_ViolationType()
         { }

         /** Returns this accessor's type constant, i.e. DMPI_ViolationType
           *
           * @return This accessor's type constant, i.e. DMPI_ViolationType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::dMPI_ViolationType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DMPI_ViolationType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates which DMPI(s) violate the given corridor or NoFire zone. [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::DMPI_ID_Type, uci::type::accessorType::dMPI_ID_Type> DMPI_ID;

         /** Returns the accessor that provides access to the complex content that is identified by the ViolationType.
           *
           * @return The acecssor that provides access to the complex content that is identified by ViolationType.
           */
         virtual const uci::type::DMPI_ViolationChoiceType& getViolationType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ViolationType.
           *
           * @return The acecssor that provides access to the complex content that is identified by ViolationType.
           */
         virtual uci::type::DMPI_ViolationChoiceType& getViolationType()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ViolationType to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ViolationType
           */
         virtual void setViolationType(const uci::type::DMPI_ViolationChoiceType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DMPI_ID.
           *
           * @return The bounded list identified by DMPI_ID.
           */
         virtual const uci::type::DMPI_ViolationType::DMPI_ID& getDMPI_ID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DMPI_ID.
           *
           * @return The bounded list identified by DMPI_ID.
           */
         virtual uci::type::DMPI_ViolationType::DMPI_ID& getDMPI_ID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the DMPI_ID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setDMPI_ID(const uci::type::DMPI_ViolationType::DMPI_ID& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DMPI_ViolationType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DMPI_ViolationType to copy from
           */
         DMPI_ViolationType(const DMPI_ViolationType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DMPI_ViolationType to the contents of the DMPI_ViolationType on
           * the right hand side (rhs) of the assignment operator.DMPI_ViolationType [only available to derived classes]
           *
           * @param rhs The DMPI_ViolationType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::DMPI_ViolationType
           * @return A constant reference to this DMPI_ViolationType.
           */
         const DMPI_ViolationType& operator=(const DMPI_ViolationType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DMPI_ViolationType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DMPI_ViolationType_h

