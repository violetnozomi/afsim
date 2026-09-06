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
#ifndef Uci__Type__MessageType_h
#define Uci__Type__MessageType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__HeaderType_h)
# include "uci/type/HeaderType.h"
#endif

#if !defined(Ism__Type__SecurityAttributesGroup_h)
# include "ism/type/SecurityAttributesGroup.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the MessageType sequence accessor class */
      class MessageType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~MessageType()
         { }

         /** Returns this accessor's type constant, i.e. MessageType
           *
           * @return This accessor's type constant, i.e. MessageType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::messageType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const MessageType& accessor)
            throw(uci::base::UCIException) = 0;


         /** This is the SequenceGroup sequence accessor class */
         class SequenceGroup : public virtual uci::base::Accessor {
         public:

            /** The destructor */
            virtual ~SequenceGroup()
            { }

            /** Returns this accessor's type constant, i.e. SequenceGroup
              *
              * @return This accessor's type constant, i.e. SequenceGroup
              */
            virtual uci::base::accessorType::AccessorType getAccessorType() const
               throw()
            {
               return uci::type::messageType::accessorType::sequenceGroup;
            }


            /** Initializes the contents of this accessor by copying the contents of the specified accessor
              *
              * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
              */
            virtual void copy(const SequenceGroup& accessor)
               throw(uci::base::UCIException) = 0;


            /** Returns the accessor that provides access to the complex content that is identified by the MessageHeader.
              *
              * @return The acecssor that provides access to the complex content that is identified by MessageHeader.
              */
            virtual const uci::type::HeaderType& getMessageHeader() const
               throw(uci::base::UCIException) = 0;


            /** Returns the accessor that provides access to the complex content that is identified by the MessageHeader.
              *
              * @return The acecssor that provides access to the complex content that is identified by MessageHeader.
              */
            virtual uci::type::HeaderType& getMessageHeader()
               throw(uci::base::UCIException) = 0;


            /** Sets the complex content that is identified by the MessageHeader to the contents of the complex content that is
              * accessed by the specified accessor.
              *
              * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
              *      the sequence identified by MessageHeader
              */
            virtual void setMessageHeader(const uci::type::HeaderType& value)
               throw(uci::base::UCIException) = 0;



         protected:

            /** The constructor [only available to derived classes]. */
            SequenceGroup()
            { }

            /** The copy constructor [only available to derived classes]
              *
              * @param rhs The SequenceGroup to copy from
              */
            SequenceGroup(const SequenceGroup& rhs)
            {
               (void)rhs;
            }

            /** The assignment operator. Sets the contents of this SequenceGroup to the contents of the SequenceGroup on the right
              * hand side (rhs) of the assignment operator.SequenceGroup [only available to derived classes]
              *
              * @param rhs The SequenceGroup on the right hand side (rhs) of the assignment operator whose contents are used to set
              *      the contents of this uci::type::MessageType::SequenceGroup
              * @return A constant reference to this SequenceGroup.
              */
            const SequenceGroup& operator=(const SequenceGroup& rhs)
            {
               (void)rhs;

               return *this;
            }


         }; // SequenceGroup


         /** Returns the accessor that provides access to the complex content that is identified by the SequenceGroup.
           *
           * @return The acecssor that provides access to the complex content that is identified by SequenceGroup.
           */
         virtual const uci::type::MessageType::SequenceGroup& getSequenceGroup() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SequenceGroup.
           *
           * @return The acecssor that provides access to the complex content that is identified by SequenceGroup.
           */
         virtual uci::type::MessageType::SequenceGroup& getSequenceGroup()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SequenceGroup to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SequenceGroup
           */
         virtual void setSequenceGroup(const uci::type::MessageType::SequenceGroup& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SecurityAttributesGroup.
           *
           * @return The acecssor that provides access to the complex content that is identified by SecurityAttributesGroup.
           */
         virtual const ism::type::SecurityAttributesGroup& getSecurityAttributesGroup() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SecurityAttributesGroup.
           *
           * @return The acecssor that provides access to the complex content that is identified by SecurityAttributesGroup.
           */
         virtual ism::type::SecurityAttributesGroup& getSecurityAttributesGroup()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SecurityAttributesGroup to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SecurityAttributesGroup
           */
         virtual void setSecurityAttributesGroup(const ism::type::SecurityAttributesGroup& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         MessageType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The MessageType to copy from
           */
         MessageType(const MessageType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this MessageType to the contents of the MessageType on the right hand
           * side (rhs) of the assignment operator.MessageType [only available to derived classes]
           *
           * @param rhs The MessageType on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::MessageType
           * @return A constant reference to this MessageType.
           */
         const MessageType& operator=(const MessageType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // MessageType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__MessageType_h

