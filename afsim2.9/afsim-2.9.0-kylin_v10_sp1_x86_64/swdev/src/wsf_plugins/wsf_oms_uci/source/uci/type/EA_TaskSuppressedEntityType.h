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
#ifndef Uci__Type__EA_TaskSuppressedEntityType_h
#define Uci__Type__EA_TaskSuppressedEntityType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__EntityID_Type_h)
# include "uci/type/EntityID_Type.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EA_TaskSuppressedEntityType sequence accessor class */
      class EA_TaskSuppressedEntityType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EA_TaskSuppressedEntityType()
         { }

         /** Returns this accessor's type constant, i.e. EA_TaskSuppressedEntityType
           *
           * @return This accessor's type constant, i.e. EA_TaskSuppressedEntityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::eA_TaskSuppressedEntityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EA_TaskSuppressedEntityType& accessor)
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


         /** Returns the value of the SimplePrimitive data type that is identified by the Priority.
           *
           * @return The value of the simple primitive data type identified by Priority.
           */
         virtual xs::UnsignedInt getPriority() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Priority.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPriority(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Priority exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Priority is emabled or not
           */
         virtual bool hasPriority() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Priority
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enablePriority(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Priority */
         virtual void clearPriority()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EA_TaskSuppressedEntityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EA_TaskSuppressedEntityType to copy from
           */
         EA_TaskSuppressedEntityType(const EA_TaskSuppressedEntityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EA_TaskSuppressedEntityType to the contents of the
           * EA_TaskSuppressedEntityType on the right hand side (rhs) of the assignment operator.EA_TaskSuppressedEntityType [only
           * available to derived classes]
           *
           * @param rhs The EA_TaskSuppressedEntityType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::EA_TaskSuppressedEntityType
           * @return A constant reference to this EA_TaskSuppressedEntityType.
           */
         const EA_TaskSuppressedEntityType& operator=(const EA_TaskSuppressedEntityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EA_TaskSuppressedEntityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EA_TaskSuppressedEntityType_h

