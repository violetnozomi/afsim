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
#ifndef Uci__Type__Link16TrackIdentifierType_h
#define Uci__Type__Link16TrackIdentifierType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__TrackNumberType_h)
# include "uci/type/TrackNumberType.h"
#endif

#if !defined(Uci__Type__NetworkLinkID_Type_h)
# include "uci/type/NetworkLinkID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the Link16TrackIdentifierType sequence accessor class */
      class Link16TrackIdentifierType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~Link16TrackIdentifierType()
         { }

         /** Returns this accessor's type constant, i.e. Link16TrackIdentifierType
           *
           * @return This accessor's type constant, i.e. Link16TrackIdentifierType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::link16TrackIdentifierType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const Link16TrackIdentifierType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the TrackNumber.
           *
           * @return The value of the string data type identified by TrackNumber.
           */
         virtual const uci::type::TrackNumberType& getTrackNumber() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the TrackNumber.
           *
           * @return The value of the string data type identified by TrackNumber.
           */
         virtual uci::type::TrackNumberType& getTrackNumber()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the TrackNumber to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setTrackNumber(const uci::type::TrackNumberType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the TrackNumber to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setTrackNumber(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the TrackNumber to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setTrackNumber(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ExternalInterfaceID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ExternalInterfaceID.
           */
         virtual const uci::type::NetworkLinkID_Type& getExternalInterfaceID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ExternalInterfaceID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ExternalInterfaceID.
           */
         virtual uci::type::NetworkLinkID_Type& getExternalInterfaceID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ExternalInterfaceID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ExternalInterfaceID
           */
         virtual void setExternalInterfaceID(const uci::type::NetworkLinkID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ExternalInterfaceID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ExternalInterfaceID is emabled or not
           */
         virtual bool hasExternalInterfaceID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ExternalInterfaceID
           *
           * @param type = uci::type::accessorType::networkLinkID_Type This Accessor's accessor type
           */
         virtual void enableExternalInterfaceID(uci::base::accessorType::AccessorType type = uci::type::accessorType::networkLinkID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ExternalInterfaceID */
         virtual void clearExternalInterfaceID()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         Link16TrackIdentifierType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The Link16TrackIdentifierType to copy from
           */
         Link16TrackIdentifierType(const Link16TrackIdentifierType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this Link16TrackIdentifierType to the contents of the
           * Link16TrackIdentifierType on the right hand side (rhs) of the assignment operator.Link16TrackIdentifierType [only
           * available to derived classes]
           *
           * @param rhs The Link16TrackIdentifierType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::Link16TrackIdentifierType
           * @return A constant reference to this Link16TrackIdentifierType.
           */
         const Link16TrackIdentifierType& operator=(const Link16TrackIdentifierType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // Link16TrackIdentifierType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__Link16TrackIdentifierType_h

