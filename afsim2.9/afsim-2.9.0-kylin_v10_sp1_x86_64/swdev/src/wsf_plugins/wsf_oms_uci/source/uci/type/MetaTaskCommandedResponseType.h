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
#ifndef Uci__Type__MetaTaskCommandedResponseType_h
#define Uci__Type__MetaTaskCommandedResponseType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ByReferenceType_h)
# include "uci/type/ByReferenceType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ResponseListType_h)
# include "uci/type/ResponseListType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the MetaTaskCommandedResponseType choice accessor class */
      class MetaTaskCommandedResponseType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~MetaTaskCommandedResponseType()
         { }

         /** Returns this accessor's type constant, i.e. MetaTaskCommandedResponseType
           *
           * @return This accessor's type constant, i.e. MetaTaskCommandedResponseType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::metaTaskCommandedResponseType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const MetaTaskCommandedResponseType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Defines a list of responses to be initiated when a Rule is satisfied. [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::ResponseListType, uci::type::accessorType::responseListType> Response;

         /** The following enumeration is used to identify which element of this Choice has been chosen. */
         enum MetaTaskCommandedResponseTypeChoice {
            METATASKCOMMANDEDRESPONSETYPE_CHOICE_NONE,
            METATASKCOMMANDEDRESPONSETYPE_CHOICE_BYREFERENCE,
            METATASKCOMMANDEDRESPONSETYPE_CHOICE_RESPONSE
         };


         /** This method returns this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active.
           *
           * @return The selected item's enumerated value
           */
         virtual MetaTaskCommandedResponseTypeChoice getMetaTaskCommandedResponseTypeChoiceOrdinal() const
            throw(uci::base::UCIException) = 0;


         /** This method is used to set this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active. There are two mechanisms that can be used to set a choice's "selection
           * ordinal." The first mechanism is by invoking this method. The second mechanism is by invoking one of the set methods
           * associated with one of the elements contained in this choice. Once this method is invoked, the value returned by
           * getMetaTaskCommandedResponseTypeChoiceOrdinal() will be the ordinal specified when this method was invoked. In
           * addition, the access methods associated with the chosen element will be enabled and will provide access to the chosen
           * element.
           *
           * @param chosenElementOrdinal The ordinal to set this choice's selected ordinal to.
           * @param type = uci::base::accessorType::null The type of data that is to be made available when the ordinal is set.
           *      The use of this parameter provides support for inheritable types. This parameter defaults to
           *      uci::base::accessorType::null that is used to indicate that the access methods associated with the chosen
           *      element shall provide access to data of the type that was specified for that element in the choice in the OMS
           *      schema, i.e. the chosen element's base type. If specified, this field must either be a type ID associated with
           *      the chosen element's base type or a type ID associated with a type that is derived from the chosen element's
           *      base type.
           */
         virtual void setMetaTaskCommandedResponseTypeChoiceOrdinal(MetaTaskCommandedResponseTypeChoice chosenElementOrdinal, uci::base::accessorType::AccessorType type = uci::base::accessorType::null)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ByReference.
           *
           * @return The acecssor that provides access to the complex content that is identified by ByReference.
           */
         virtual const uci::type::ByReferenceType& getByReference() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ByReference.
           *
           * @return The acecssor that provides access to the complex content that is identified by ByReference.
           */
         virtual uci::type::ByReferenceType& getByReference()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ByReference to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ByReference
           */
         virtual void setByReference(const uci::type::ByReferenceType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Response.
           *
           * @return The bounded list identified by Response.
           */
         virtual const uci::type::MetaTaskCommandedResponseType::Response& getResponse() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Response.
           *
           * @return The bounded list identified by Response.
           */
         virtual uci::type::MetaTaskCommandedResponseType::Response& getResponse()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Response.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setResponse(const uci::type::MetaTaskCommandedResponseType::Response& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         MetaTaskCommandedResponseType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The MetaTaskCommandedResponseType to copy from
           */
         MetaTaskCommandedResponseType(const MetaTaskCommandedResponseType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this MetaTaskCommandedResponseType to the contents of the
           * MetaTaskCommandedResponseType on the right hand side (rhs) of the assignment operator.MetaTaskCommandedResponseType
           * [only available to derived classes]
           *
           * @param rhs The MetaTaskCommandedResponseType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::MetaTaskCommandedResponseType
           * @return A constant reference to this MetaTaskCommandedResponseType.
           */
         const MetaTaskCommandedResponseType& operator=(const MetaTaskCommandedResponseType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // MetaTaskCommandedResponseType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__MetaTaskCommandedResponseType_h

