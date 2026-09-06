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
#ifndef Uci__Type__EntitySourceType_h
#define Uci__Type__EntitySourceType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__ServiceID_Type_h)
# include "uci/type/ServiceID_Type.h"
#endif

#if !defined(Uci__Type__EntitySourceSpecificDataType_h)
# include "uci/type/EntitySourceSpecificDataType.h"
#endif

#if !defined(Uci__Type__EntitySourceEnum_h)
# include "uci/type/EntitySourceEnum.h"
#endif

#if !defined(Uci__Type__EntitySourceIdentifierType_h)
# include "uci/type/EntitySourceIdentifierType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EntitySourceType sequence accessor class */
      class EntitySourceType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EntitySourceType()
         { }

         /** Returns this accessor's type constant, i.e. EntitySourceType
           *
           * @return This accessor's type constant, i.e. EntitySourceType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::entitySourceType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EntitySourceType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SystemID.
           */
         virtual const uci::type::SystemID_Type& getSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SystemID.
           */
         virtual uci::type::SystemID_Type& getSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SystemID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SystemID
           */
         virtual void setSystemID(const uci::type::SystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ServiceID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ServiceID.
           */
         virtual const uci::type::ServiceID_Type& getServiceID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ServiceID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ServiceID.
           */
         virtual uci::type::ServiceID_Type& getServiceID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ServiceID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ServiceID
           */
         virtual void setServiceID(const uci::type::ServiceID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ServiceID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ServiceID is emabled or not
           */
         virtual bool hasServiceID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ServiceID
           *
           * @param type = uci::type::accessorType::serviceID_Type This Accessor's accessor type
           */
         virtual void enableServiceID(uci::base::accessorType::AccessorType type = uci::type::accessorType::serviceID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ServiceID */
         virtual void clearServiceID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SourceSpecificData.
           *
           * @return The acecssor that provides access to the complex content that is identified by SourceSpecificData.
           */
         virtual const uci::type::EntitySourceSpecificDataType& getSourceSpecificData() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SourceSpecificData.
           *
           * @return The acecssor that provides access to the complex content that is identified by SourceSpecificData.
           */
         virtual uci::type::EntitySourceSpecificDataType& getSourceSpecificData()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SourceSpecificData to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SourceSpecificData
           */
         virtual void setSourceSpecificData(const uci::type::EntitySourceSpecificDataType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SourceSpecificData exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SourceSpecificData is emabled or not
           */
         virtual bool hasSourceSpecificData() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SourceSpecificData
           *
           * @param type = uci::type::accessorType::entitySourceSpecificDataType This Accessor's accessor type
           */
         virtual void enableSourceSpecificData(uci::base::accessorType::AccessorType type = uci::type::accessorType::entitySourceSpecificDataType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SourceSpecificData */
         virtual void clearSourceSpecificData()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SourceType.
           *
           * @return The value of the enumeration identified by SourceType.
           */
         virtual const uci::type::EntitySourceEnum& getSourceType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SourceType.
           *
           * @return The value of the enumeration identified by SourceType.
           */
         virtual uci::type::EntitySourceEnum& getSourceType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SourceType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSourceType(const uci::type::EntitySourceEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SourceType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSourceType(uci::type::EntitySourceEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SourceTypeID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SourceTypeID.
           */
         virtual const uci::type::EntitySourceIdentifierType& getSourceTypeID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SourceTypeID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SourceTypeID.
           */
         virtual uci::type::EntitySourceIdentifierType& getSourceTypeID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SourceTypeID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SourceTypeID
           */
         virtual void setSourceTypeID(const uci::type::EntitySourceIdentifierType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SourceTypeID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SourceTypeID is emabled or not
           */
         virtual bool hasSourceTypeID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SourceTypeID
           *
           * @param type = uci::type::accessorType::entitySourceIdentifierType This Accessor's accessor type
           */
         virtual void enableSourceTypeID(uci::base::accessorType::AccessorType type = uci::type::accessorType::entitySourceIdentifierType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SourceTypeID */
         virtual void clearSourceTypeID()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EntitySourceType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EntitySourceType to copy from
           */
         EntitySourceType(const EntitySourceType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EntitySourceType to the contents of the EntitySourceType on the
           * right hand side (rhs) of the assignment operator.EntitySourceType [only available to derived classes]
           *
           * @param rhs The EntitySourceType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::EntitySourceType
           * @return A constant reference to this EntitySourceType.
           */
         const EntitySourceType& operator=(const EntitySourceType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EntitySourceType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EntitySourceType_h

