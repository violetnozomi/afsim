// This file was generated  on 12/5/2018 at 1:1:46 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__IdentityBasicType_h
#define Uci__Type__IdentityBasicType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__StandardIdentityType_h)
# include "uci/type/StandardIdentityType.h"
#endif

#if !defined(Uci__Type__PlatformIdentityType_h)
# include "uci/type/PlatformIdentityType.h"
#endif

#if !defined(Uci__Type__SpecificIdentityType_h)
# include "uci/type/SpecificIdentityType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This defines up to three methods ("Standard", "Platform", "Specific") for Target Identity based on MIL-STD-6016. */
      class IdentityBasicType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~IdentityBasicType()
         { }

         /** Returns this accessor's type constant, i.e. IdentityBasicType
           *
           * @return This accessor's type constant, i.e. IdentityBasicType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::identityBasicType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const IdentityBasicType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the "standard identity" of the Entity. Can establish capability or be commanded according to the
           * availability for up to 60 allowable options. [Minimum occurrences: 0] [Maximum occurrences: 60]
           */
         typedef uci::base::BoundedList<uci::type::StandardIdentityType, uci::type::accessorType::standardIdentityType> Standard;

         /** Indicates the "platform identity" of the Entity. Can establish capability or be commanded according to the
           * availability for up to 60 allowable options. [Minimum occurrences: 0] [Maximum occurrences: 60]
           */
         typedef uci::base::BoundedList<uci::type::PlatformIdentityType, uci::type::accessorType::platformIdentityType> Platform;

         /** Indicates a "specific identity" of the Entity. Can establish capability or be commanded according to the availability
           * for up to 60 allowable options. [Minimum occurrences: 0] [Maximum occurrences: 60]
           */
         typedef uci::base::BoundedList<uci::type::SpecificIdentityType, uci::type::accessorType::specificIdentityType> Specific;

         /** Returns the bounded list that is identified by the Standard.
           *
           * @return The bounded list identified by Standard.
           */
         virtual const uci::type::IdentityBasicType::Standard& getStandard() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Standard.
           *
           * @return The bounded list identified by Standard.
           */
         virtual uci::type::IdentityBasicType::Standard& getStandard()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Standard.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setStandard(const uci::type::IdentityBasicType::Standard& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Platform.
           *
           * @return The bounded list identified by Platform.
           */
         virtual const uci::type::IdentityBasicType::Platform& getPlatform() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Platform.
           *
           * @return The bounded list identified by Platform.
           */
         virtual uci::type::IdentityBasicType::Platform& getPlatform()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Platform.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setPlatform(const uci::type::IdentityBasicType::Platform& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Specific.
           *
           * @return The bounded list identified by Specific.
           */
         virtual const uci::type::IdentityBasicType::Specific& getSpecific() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Specific.
           *
           * @return The bounded list identified by Specific.
           */
         virtual uci::type::IdentityBasicType::Specific& getSpecific()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Specific.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSpecific(const uci::type::IdentityBasicType::Specific& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         IdentityBasicType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The IdentityBasicType to copy from
           */
         IdentityBasicType(const IdentityBasicType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this IdentityBasicType to the contents of the IdentityBasicType on the
           * right hand side (rhs) of the assignment operator.IdentityBasicType [only available to derived classes]
           *
           * @param rhs The IdentityBasicType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::IdentityBasicType
           * @return A constant reference to this IdentityBasicType.
           */
         const IdentityBasicType& operator=(const IdentityBasicType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // IdentityBasicType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__IdentityBasicType_h

