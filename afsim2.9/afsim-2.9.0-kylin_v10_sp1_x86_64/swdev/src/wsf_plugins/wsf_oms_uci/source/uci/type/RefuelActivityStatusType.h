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
#ifndef Uci__Type__RefuelActivityStatusType_h
#define Uci__Type__RefuelActivityStatusType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__EntityID_Type_h)
# include "uci/type/EntityID_Type.h"
#endif

#if !defined(Uci__Type__EmptyType_h)
# include "uci/type/EmptyType.h"
#endif

#if !defined(Uci__Type__RefuelActivityDrogueStatusType_h)
# include "uci/type/RefuelActivityDrogueStatusType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the RefuelActivityStatusType sequence accessor class */
      class RefuelActivityStatusType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~RefuelActivityStatusType()
         { }

         /** Returns this accessor's type constant, i.e. RefuelActivityStatusType
           *
           * @return This accessor's type constant, i.e. RefuelActivityStatusType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::refuelActivityStatusType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RefuelActivityStatusType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ReceiverID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ReceiverID.
           */
         virtual const uci::type::EntityID_Type& getReceiverID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ReceiverID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ReceiverID.
           */
         virtual uci::type::EntityID_Type& getReceiverID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ReceiverID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ReceiverID
           */
         virtual void setReceiverID(const uci::type::EntityID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ReceiverID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ReceiverID is emabled or not
           */
         virtual bool hasReceiverID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ReceiverID
           *
           * @param type = uci::type::accessorType::entityID_Type This Accessor's accessor type
           */
         virtual void enableReceiverID(uci::base::accessorType::AccessorType type = uci::type::accessorType::entityID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ReceiverID */
         virtual void clearReceiverID()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Boom.
           *
           * @return The value of the string data type identified by Boom.
           */
         virtual const uci::type::EmptyType& getBoom() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Boom.
           *
           * @return The value of the string data type identified by Boom.
           */
         virtual uci::type::EmptyType& getBoom()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Boom to the string accessed by the specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setBoom(const uci::type::EmptyType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Boom to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setBoom(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Boom to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setBoom(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Boom exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Boom is emabled or not
           */
         virtual bool hasBoom() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Boom
           *
           * @param type = uci::type::accessorType::emptyType This Accessor's accessor type
           */
         virtual void enableBoom(uci::base::accessorType::AccessorType type = uci::type::accessorType::emptyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Boom */
         virtual void clearBoom()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Drogue.
           *
           * @return The acecssor that provides access to the complex content that is identified by Drogue.
           */
         virtual const uci::type::RefuelActivityDrogueStatusType& getDrogue() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Drogue.
           *
           * @return The acecssor that provides access to the complex content that is identified by Drogue.
           */
         virtual uci::type::RefuelActivityDrogueStatusType& getDrogue()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Drogue to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Drogue
           */
         virtual void setDrogue(const uci::type::RefuelActivityDrogueStatusType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Drogue exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Drogue is emabled or not
           */
         virtual bool hasDrogue() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Drogue
           *
           * @param type = uci::type::accessorType::refuelActivityDrogueStatusType This Accessor's accessor type
           */
         virtual void enableDrogue(uci::base::accessorType::AccessorType type = uci::type::accessorType::refuelActivityDrogueStatusType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Drogue */
         virtual void clearDrogue()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RefuelActivityStatusType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RefuelActivityStatusType to copy from
           */
         RefuelActivityStatusType(const RefuelActivityStatusType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RefuelActivityStatusType to the contents of the
           * RefuelActivityStatusType on the right hand side (rhs) of the assignment operator.RefuelActivityStatusType [only
           * available to derived classes]
           *
           * @param rhs The RefuelActivityStatusType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::RefuelActivityStatusType
           * @return A constant reference to this RefuelActivityStatusType.
           */
         const RefuelActivityStatusType& operator=(const RefuelActivityStatusType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RefuelActivityStatusType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RefuelActivityStatusType_h

