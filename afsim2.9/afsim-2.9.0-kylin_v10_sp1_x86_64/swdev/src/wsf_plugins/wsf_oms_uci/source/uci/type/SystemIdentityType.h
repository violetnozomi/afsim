// This file was generated  on 12/5/2018 at 1:1:48 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__SystemIdentityType_h
#define Uci__Type__SystemIdentityType_h 1

#if !defined(Uci__Type__IdentityType_h)
# include "uci/type/IdentityType.h"
#endif

#if !defined(Uci__Type__QualifyingTagsType_h)
# include "uci/type/QualifyingTagsType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__TimeFunctionType_h)
# include "uci/type/TimeFunctionType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** SystemIdentityType is an extension of IdentityType to include additional information specifically relevant to the
        * identity of a system.
        */
      class SystemIdentityType : public virtual uci::type::IdentityType {
      public:

         /** The destructor */
         virtual ~SystemIdentityType()
         { }

         /** Returns this accessor's type constant, i.e. SystemIdentityType
           *
           * @return This accessor's type constant, i.e. SystemIdentityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::systemIdentityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SystemIdentityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates time related events that are associated to the system such as when when operational or arriving at a
           * destination. [Minimum occurrences: 0] [Maximum occurrences: 7]
           */
         typedef uci::base::BoundedList<uci::type::TimeFunctionType, uci::type::accessorType::timeFunctionType> AssociatedTime;

         /** Returns the accessor that provides access to the complex content that is identified by the QualifyingTags.
           *
           * @return The acecssor that provides access to the complex content that is identified by QualifyingTags.
           */
         virtual const uci::type::QualifyingTagsType& getQualifyingTags() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the QualifyingTags.
           *
           * @return The acecssor that provides access to the complex content that is identified by QualifyingTags.
           */
         virtual uci::type::QualifyingTagsType& getQualifyingTags()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the QualifyingTags to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by QualifyingTags
           */
         virtual void setQualifyingTags(const uci::type::QualifyingTagsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by QualifyingTags exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by QualifyingTags is emabled or not
           */
         virtual bool hasQualifyingTags() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by QualifyingTags
           *
           * @param type = uci::type::accessorType::qualifyingTagsType This Accessor's accessor type
           */
         virtual void enableQualifyingTags(uci::base::accessorType::AccessorType type = uci::type::accessorType::qualifyingTagsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by QualifyingTags */
         virtual void clearQualifyingTags()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AssociatedTime.
           *
           * @return The bounded list identified by AssociatedTime.
           */
         virtual const uci::type::SystemIdentityType::AssociatedTime& getAssociatedTime() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AssociatedTime.
           *
           * @return The bounded list identified by AssociatedTime.
           */
         virtual uci::type::SystemIdentityType::AssociatedTime& getAssociatedTime()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AssociatedTime.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAssociatedTime(const uci::type::SystemIdentityType::AssociatedTime& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SystemIdentityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SystemIdentityType to copy from
           */
         SystemIdentityType(const SystemIdentityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SystemIdentityType to the contents of the SystemIdentityType on
           * the right hand side (rhs) of the assignment operator.SystemIdentityType [only available to derived classes]
           *
           * @param rhs The SystemIdentityType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::SystemIdentityType
           * @return A constant reference to this SystemIdentityType.
           */
         const SystemIdentityType& operator=(const SystemIdentityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SystemIdentityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SystemIdentityType_h

