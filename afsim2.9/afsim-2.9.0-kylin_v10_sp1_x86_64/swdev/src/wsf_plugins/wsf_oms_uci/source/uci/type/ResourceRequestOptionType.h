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
#ifndef Uci__Type__ResourceRequestOptionType_h
#define Uci__Type__ResourceRequestOptionType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ResourceRequestType_h)
# include "uci/type/ResourceRequestType.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ResourceRequestOptionType sequence accessor class */
      class ResourceRequestOptionType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ResourceRequestOptionType()
         { }

         /** Returns this accessor's type constant, i.e. ResourceRequestOptionType
           *
           * @return This accessor's type constant, i.e. ResourceRequestOptionType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::resourceRequestOptionType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ResourceRequestOptionType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Each entry in this array represents the resource required for this option. To fill the request all synchronous and
           * asynchronous entries must be allocated. [Maximum occurrences: 2]
           */
         typedef uci::base::BoundedList<uci::type::ResourceRequestType, uci::type::accessorType::resourceRequestType> ResourceRequest;

         /** Returns the bounded list that is identified by the ResourceRequest.
           *
           * @return The bounded list identified by ResourceRequest.
           */
         virtual const uci::type::ResourceRequestOptionType::ResourceRequest& getResourceRequest() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ResourceRequest.
           *
           * @return The bounded list identified by ResourceRequest.
           */
         virtual uci::type::ResourceRequestOptionType::ResourceRequest& getResourceRequest()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ResourceRequest.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setResourceRequest(const uci::type::ResourceRequestOptionType::ResourceRequest& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Merit.
           *
           * @return The value of the simple primitive data type identified by Merit.
           */
         virtual xs::UnsignedInt getMerit() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Merit.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMerit(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Merit exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Merit is emabled or not
           */
         virtual bool hasMerit() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Merit
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enableMerit(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Merit */
         virtual void clearMerit()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ResourceRequestOptionType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ResourceRequestOptionType to copy from
           */
         ResourceRequestOptionType(const ResourceRequestOptionType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ResourceRequestOptionType to the contents of the
           * ResourceRequestOptionType on the right hand side (rhs) of the assignment operator.ResourceRequestOptionType [only
           * available to derived classes]
           *
           * @param rhs The ResourceRequestOptionType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::ResourceRequestOptionType
           * @return A constant reference to this ResourceRequestOptionType.
           */
         const ResourceRequestOptionType& operator=(const ResourceRequestOptionType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ResourceRequestOptionType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ResourceRequestOptionType_h

