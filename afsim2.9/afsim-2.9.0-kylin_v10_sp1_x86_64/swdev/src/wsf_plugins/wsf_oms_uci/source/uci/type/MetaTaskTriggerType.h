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
#ifndef Uci__Type__MetaTaskTriggerType_h
#define Uci__Type__MetaTaskTriggerType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__MissionResponseListID_Type_h)
# include "uci/type/MissionResponseListID_Type.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__ResponseListID_Type_h)
# include "uci/type/ResponseListID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the MetaTaskTriggerType sequence accessor class */
      class MetaTaskTriggerType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~MetaTaskTriggerType()
         { }

         /** Returns this accessor's type constant, i.e. MetaTaskTriggerType
           *
           * @return This accessor's type constant, i.e. MetaTaskTriggerType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::metaTaskTriggerType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const MetaTaskTriggerType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MissionResponseListID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionResponseListID.
           */
         virtual const uci::type::MissionResponseListID_Type& getMissionResponseListID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MissionResponseListID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionResponseListID.
           */
         virtual uci::type::MissionResponseListID_Type& getMissionResponseListID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MissionResponseListID to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MissionResponseListID
           */
         virtual void setMissionResponseListID(const uci::type::MissionResponseListID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the RuleIndex.
           *
           * @return The value of the simple primitive data type identified by RuleIndex.
           */
         virtual xs::UnsignedInt getRuleIndex() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the RuleIndex.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRuleIndex(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ResponseListID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ResponseListID.
           */
         virtual const uci::type::ResponseListID_Type& getResponseListID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ResponseListID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ResponseListID.
           */
         virtual uci::type::ResponseListID_Type& getResponseListID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ResponseListID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ResponseListID
           */
         virtual void setResponseListID(const uci::type::ResponseListID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ResponseListID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ResponseListID is emabled or not
           */
         virtual bool hasResponseListID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ResponseListID
           *
           * @param type = uci::type::accessorType::responseListID_Type This Accessor's accessor type
           */
         virtual void enableResponseListID(uci::base::accessorType::AccessorType type = uci::type::accessorType::responseListID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ResponseListID */
         virtual void clearResponseListID()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         MetaTaskTriggerType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The MetaTaskTriggerType to copy from
           */
         MetaTaskTriggerType(const MetaTaskTriggerType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this MetaTaskTriggerType to the contents of the MetaTaskTriggerType on
           * the right hand side (rhs) of the assignment operator.MetaTaskTriggerType [only available to derived classes]
           *
           * @param rhs The MetaTaskTriggerType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::MetaTaskTriggerType
           * @return A constant reference to this MetaTaskTriggerType.
           */
         const MetaTaskTriggerType& operator=(const MetaTaskTriggerType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // MetaTaskTriggerType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__MetaTaskTriggerType_h

