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
#ifndef Uci__Type__EntityDataType_h
#define Uci__Type__EntityDataType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__EntityMDT_h)
# include "uci/type/EntityMDT.h"
#endif

#if !defined(Uci__Type__EntityMetadataMDT_h)
# include "uci/type/EntityMetadataMDT.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Provides the capability to pass an Entity and any associated metadata message in the same structure. This is
        * typically used when there is a need to pass the entirety of the information about an Entity at a given time as a part
        * of another message. For example, an OpPoint representing an emergency may include the associated Entity and
        * EntityMetadata from the time the emergency was declared.
        */
      class EntityDataType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EntityDataType()
         { }

         /** Returns this accessor's type constant, i.e. EntityDataType
           *
           * @return This accessor's type constant, i.e. EntityDataType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::entityDataType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EntityDataType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Entity.
           *
           * @return The acecssor that provides access to the complex content that is identified by Entity.
           */
         virtual const uci::type::EntityMDT& getEntity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Entity.
           *
           * @return The acecssor that provides access to the complex content that is identified by Entity.
           */
         virtual uci::type::EntityMDT& getEntity()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Entity to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Entity
           */
         virtual void setEntity(const uci::type::EntityMDT& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Metadata.
           *
           * @return The acecssor that provides access to the complex content that is identified by Metadata.
           */
         virtual const uci::type::EntityMetadataMDT& getMetadata() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Metadata.
           *
           * @return The acecssor that provides access to the complex content that is identified by Metadata.
           */
         virtual uci::type::EntityMetadataMDT& getMetadata()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Metadata to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Metadata
           */
         virtual void setMetadata(const uci::type::EntityMetadataMDT& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Metadata exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Metadata is emabled or not
           */
         virtual bool hasMetadata() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Metadata
           *
           * @param type = uci::type::accessorType::entityMetadataMDT This Accessor's accessor type
           */
         virtual void enableMetadata(uci::base::accessorType::AccessorType type = uci::type::accessorType::entityMetadataMDT)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Metadata */
         virtual void clearMetadata()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EntityDataType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EntityDataType to copy from
           */
         EntityDataType(const EntityDataType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EntityDataType to the contents of the EntityDataType on the right
           * hand side (rhs) of the assignment operator.EntityDataType [only available to derived classes]
           *
           * @param rhs The EntityDataType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::EntityDataType
           * @return A constant reference to this EntityDataType.
           */
         const EntityDataType& operator=(const EntityDataType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EntityDataType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EntityDataType_h

