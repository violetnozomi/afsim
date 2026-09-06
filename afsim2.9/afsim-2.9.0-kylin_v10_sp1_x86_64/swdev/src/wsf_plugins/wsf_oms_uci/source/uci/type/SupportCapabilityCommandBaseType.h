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
#ifndef Uci__Type__SupportCapabilityCommandBaseType_h
#define Uci__Type__SupportCapabilityCommandBaseType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SupportCapabilityCommandID_Type_h)
# include "uci/type/SupportCapabilityCommandID_Type.h"
#endif

#if !defined(Uci__Type__SupportCapabilityID_Type_h)
# include "uci/type/SupportCapabilityID_Type.h"
#endif

#if !defined(Uci__Type__MessageStateEnum_h)
# include "uci/type/MessageStateEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SupportCapabilityCommandBaseType sequence accessor class */
      class SupportCapabilityCommandBaseType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SupportCapabilityCommandBaseType()
         { }

         /** Returns this accessor's type constant, i.e. SupportCapabilityCommandBaseType
           *
           * @return This accessor's type constant, i.e. SupportCapabilityCommandBaseType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::supportCapabilityCommandBaseType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SupportCapabilityCommandBaseType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommandID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommandID.
           */
         virtual const uci::type::SupportCapabilityCommandID_Type& getCommandID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommandID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommandID.
           */
         virtual uci::type::SupportCapabilityCommandID_Type& getCommandID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CommandID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CommandID
           */
         virtual void setCommandID(const uci::type::SupportCapabilityCommandID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SupportCapabilityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SupportCapabilityID.
           */
         virtual const uci::type::SupportCapabilityID_Type& getSupportCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SupportCapabilityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SupportCapabilityID.
           */
         virtual uci::type::SupportCapabilityID_Type& getSupportCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SupportCapabilityID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SupportCapabilityID
           */
         virtual void setSupportCapabilityID(const uci::type::SupportCapabilityID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CommandState.
           *
           * @return The value of the enumeration identified by CommandState.
           */
         virtual const uci::type::MessageStateEnum& getCommandState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CommandState.
           *
           * @return The value of the enumeration identified by CommandState.
           */
         virtual uci::type::MessageStateEnum& getCommandState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CommandState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCommandState(const uci::type::MessageStateEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CommandState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCommandState(uci::type::MessageStateEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SupportCapabilityCommandBaseType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SupportCapabilityCommandBaseType to copy from
           */
         SupportCapabilityCommandBaseType(const SupportCapabilityCommandBaseType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SupportCapabilityCommandBaseType to the contents of the
           * SupportCapabilityCommandBaseType on the right hand side (rhs) of the assignment
           * operator.SupportCapabilityCommandBaseType [only available to derived classes]
           *
           * @param rhs The SupportCapabilityCommandBaseType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::SupportCapabilityCommandBaseType
           * @return A constant reference to this SupportCapabilityCommandBaseType.
           */
         const SupportCapabilityCommandBaseType& operator=(const SupportCapabilityCommandBaseType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SupportCapabilityCommandBaseType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SupportCapabilityCommandBaseType_h

