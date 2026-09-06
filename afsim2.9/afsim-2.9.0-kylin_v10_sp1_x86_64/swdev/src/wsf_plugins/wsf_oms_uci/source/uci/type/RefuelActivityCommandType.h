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
#ifndef Uci__Type__RefuelActivityCommandType_h
#define Uci__Type__RefuelActivityCommandType_h 1

#if !defined(Uci__Type__ActivityCommandBaseType_h)
# include "uci/type/ActivityCommandBaseType.h"
#endif

#if !defined(Uci__Type__EntityID_Type_h)
# include "uci/type/EntityID_Type.h"
#endif

#if !defined(Uci__Type__RefuelConnectionType_h)
# include "uci/type/RefuelConnectionType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the RefuelActivityCommandType sequence accessor class */
      class RefuelActivityCommandType : public virtual uci::type::ActivityCommandBaseType {
      public:

         /** The destructor */
         virtual ~RefuelActivityCommandType()
         { }

         /** Returns this accessor's type constant, i.e. RefuelActivityCommandType
           *
           * @return This accessor's type constant, i.e. RefuelActivityCommandType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::refuelActivityCommandType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RefuelActivityCommandType& accessor)
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


         /** Returns whether the Element that is identified by EntityID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EntityID is emabled or not
           */
         virtual bool hasEntityID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EntityID
           *
           * @param type = uci::type::accessorType::entityID_Type This Accessor's accessor type
           */
         virtual void enableEntityID(uci::base::accessorType::AccessorType type = uci::type::accessorType::entityID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EntityID */
         virtual void clearEntityID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RefuelConnection.
           *
           * @return The acecssor that provides access to the complex content that is identified by RefuelConnection.
           */
         virtual const uci::type::RefuelConnectionType& getRefuelConnection() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RefuelConnection.
           *
           * @return The acecssor that provides access to the complex content that is identified by RefuelConnection.
           */
         virtual uci::type::RefuelConnectionType& getRefuelConnection()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RefuelConnection to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RefuelConnection
           */
         virtual void setRefuelConnection(const uci::type::RefuelConnectionType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RefuelActivityCommandType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RefuelActivityCommandType to copy from
           */
         RefuelActivityCommandType(const RefuelActivityCommandType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RefuelActivityCommandType to the contents of the
           * RefuelActivityCommandType on the right hand side (rhs) of the assignment operator.RefuelActivityCommandType [only
           * available to derived classes]
           *
           * @param rhs The RefuelActivityCommandType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::RefuelActivityCommandType
           * @return A constant reference to this RefuelActivityCommandType.
           */
         const RefuelActivityCommandType& operator=(const RefuelActivityCommandType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RefuelActivityCommandType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RefuelActivityCommandType_h

