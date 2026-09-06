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
#ifndef Uci__Type__RemoteDestinationType_h
#define Uci__Type__RemoteDestinationType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__EntityID_Type_h)
# include "uci/type/EntityID_Type.h"
#endif

#if !defined(Uci__Type__Point4D_Type_h)
# include "uci/type/Point4D_Type.h"
#endif

#if !defined(Uci__Type__TLE_Type_h)
# include "uci/type/TLE_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the RemoteDestinationType sequence accessor class */
      class RemoteDestinationType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~RemoteDestinationType()
         { }

         /** Returns this accessor's type constant, i.e. RemoteDestinationType
           *
           * @return This accessor's type constant, i.e. RemoteDestinationType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::remoteDestinationType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RemoteDestinationType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RemoteSystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by RemoteSystemID.
           */
         virtual const uci::type::SystemID_Type& getRemoteSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RemoteSystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by RemoteSystemID.
           */
         virtual uci::type::SystemID_Type& getRemoteSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RemoteSystemID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RemoteSystemID
           */
         virtual void setRemoteSystemID(const uci::type::SystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RemoteSystemID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RemoteSystemID is emabled or not
           */
         virtual bool hasRemoteSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RemoteSystemID
           *
           * @param type = uci::type::accessorType::systemID_Type This Accessor's accessor type
           */
         virtual void enableRemoteSystemID(uci::base::accessorType::AccessorType type = uci::type::accessorType::systemID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RemoteSystemID */
         virtual void clearRemoteSystemID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RemoteEntityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by RemoteEntityID.
           */
         virtual const uci::type::EntityID_Type& getRemoteEntityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RemoteEntityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by RemoteEntityID.
           */
         virtual uci::type::EntityID_Type& getRemoteEntityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RemoteEntityID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RemoteEntityID
           */
         virtual void setRemoteEntityID(const uci::type::EntityID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RemoteEntityID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RemoteEntityID is emabled or not
           */
         virtual bool hasRemoteEntityID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RemoteEntityID
           *
           * @param type = uci::type::accessorType::entityID_Type This Accessor's accessor type
           */
         virtual void enableRemoteEntityID(uci::base::accessorType::AccessorType type = uci::type::accessorType::entityID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RemoteEntityID */
         virtual void clearRemoteEntityID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RemotePosition.
           *
           * @return The acecssor that provides access to the complex content that is identified by RemotePosition.
           */
         virtual const uci::type::Point4D_Type& getRemotePosition() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RemotePosition.
           *
           * @return The acecssor that provides access to the complex content that is identified by RemotePosition.
           */
         virtual uci::type::Point4D_Type& getRemotePosition()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RemotePosition to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RemotePosition
           */
         virtual void setRemotePosition(const uci::type::Point4D_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RemotePosition exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RemotePosition is emabled or not
           */
         virtual bool hasRemotePosition() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RemotePosition
           *
           * @param type = uci::type::accessorType::point4D_Type This Accessor's accessor type
           */
         virtual void enableRemotePosition(uci::base::accessorType::AccessorType type = uci::type::accessorType::point4D_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RemotePosition */
         virtual void clearRemotePosition()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RemoteTLE.
           *
           * @return The acecssor that provides access to the complex content that is identified by RemoteTLE.
           */
         virtual const uci::type::TLE_Type& getRemoteTLE() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RemoteTLE.
           *
           * @return The acecssor that provides access to the complex content that is identified by RemoteTLE.
           */
         virtual uci::type::TLE_Type& getRemoteTLE()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RemoteTLE to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RemoteTLE
           */
         virtual void setRemoteTLE(const uci::type::TLE_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RemoteTLE exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RemoteTLE is emabled or not
           */
         virtual bool hasRemoteTLE() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RemoteTLE
           *
           * @param type = uci::type::accessorType::tLE_Type This Accessor's accessor type
           */
         virtual void enableRemoteTLE(uci::base::accessorType::AccessorType type = uci::type::accessorType::tLE_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RemoteTLE */
         virtual void clearRemoteTLE()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RemoteDestinationType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RemoteDestinationType to copy from
           */
         RemoteDestinationType(const RemoteDestinationType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RemoteDestinationType to the contents of the RemoteDestinationType
           * on the right hand side (rhs) of the assignment operator.RemoteDestinationType [only available to derived classes]
           *
           * @param rhs The RemoteDestinationType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::RemoteDestinationType
           * @return A constant reference to this RemoteDestinationType.
           */
         const RemoteDestinationType& operator=(const RemoteDestinationType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RemoteDestinationType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RemoteDestinationType_h

