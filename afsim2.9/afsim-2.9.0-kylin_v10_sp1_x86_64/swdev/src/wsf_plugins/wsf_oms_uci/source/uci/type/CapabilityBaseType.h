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
#ifndef Uci__Type__CapabilityBaseType_h
#define Uci__Type__CapabilityBaseType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__MDF_Type_h)
# include "uci/type/MDF_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CapabilityBaseType sequence accessor class */
      class CapabilityBaseType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CapabilityBaseType()
         { }

         /** Returns this accessor's type constant, i.e. CapabilityBaseType
           *
           * @return This accessor's type constant, i.e. CapabilityBaseType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::capabilityBaseType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CapabilityBaseType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a Mission Data File (MDF) that is available for use. All MDFs currently available for use should be listed.
           * An MDF is a collection of data, organized into files, that is used to tailor, configure or otherwise affect the
           * behavior of a Capability. MDFs can vary from mission to mission or even by phases of a single mission. They are
           * generally developed in pre-mission planning and are unique for each Subsystem model. Their content is not
           * standardized by UCI, but the mechanism for advertising their availability and activating them is. Their content is
           * abstracted by the UCI concept of Capability. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::MDF_Type, uci::type::accessorType::mDF_Type> AvailableMDF;

         /** Returns the bounded list that is identified by the AvailableMDF.
           *
           * @return The bounded list identified by AvailableMDF.
           */
         virtual const uci::type::CapabilityBaseType::AvailableMDF& getAvailableMDF() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AvailableMDF.
           *
           * @return The bounded list identified by AvailableMDF.
           */
         virtual uci::type::CapabilityBaseType::AvailableMDF& getAvailableMDF()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AvailableMDF.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAvailableMDF(const uci::type::CapabilityBaseType::AvailableMDF& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CapabilityBaseType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CapabilityBaseType to copy from
           */
         CapabilityBaseType(const CapabilityBaseType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CapabilityBaseType to the contents of the CapabilityBaseType on
           * the right hand side (rhs) of the assignment operator.CapabilityBaseType [only available to derived classes]
           *
           * @param rhs The CapabilityBaseType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::CapabilityBaseType
           * @return A constant reference to this CapabilityBaseType.
           */
         const CapabilityBaseType& operator=(const CapabilityBaseType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CapabilityBaseType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CapabilityBaseType_h

