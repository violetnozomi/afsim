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
#ifndef Uci__Type__Link16DataLinkIdentifierType_h
#define Uci__Type__Link16DataLinkIdentifierType_h 1

#if !defined(Uci__Type__DataLinkIdentifierPET_h)
# include "uci/type/DataLinkIdentifierPET.h"
#endif

#if !defined(Uci__Type__VoiceCallSignType_h)
# include "uci/type/VoiceCallSignType.h"
#endif

#if !defined(Uci__Type__Link16TrackIdentifierType_h)
# include "uci/type/Link16TrackIdentifierType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is a polymorphic extension which allows for the extension of the base DataLinkIdentifierPET. This represents
        * data used to identify a Link 16 participant.
        */
      class Link16DataLinkIdentifierType : public virtual uci::type::DataLinkIdentifierPET {
      public:

         /** The destructor */
         virtual ~Link16DataLinkIdentifierType()
         { }

         /** Returns this accessor's type constant, i.e. Link16DataLinkIdentifierType
           *
           * @return This accessor's type constant, i.e. Link16DataLinkIdentifierType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::link16DataLinkIdentifierType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const Link16DataLinkIdentifierType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the VoiceCallSign.
           *
           * @return The value of the string data type identified by VoiceCallSign.
           */
         virtual const uci::type::VoiceCallSignType& getVoiceCallSign() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the VoiceCallSign.
           *
           * @return The value of the string data type identified by VoiceCallSign.
           */
         virtual uci::type::VoiceCallSignType& getVoiceCallSign()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the VoiceCallSign to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setVoiceCallSign(const uci::type::VoiceCallSignType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the VoiceCallSign to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setVoiceCallSign(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the VoiceCallSign to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setVoiceCallSign(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by VoiceCallSign exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by VoiceCallSign is emabled or not
           */
         virtual bool hasVoiceCallSign() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by VoiceCallSign
           *
           * @param type = uci::type::accessorType::voiceCallSignType This Accessor's accessor type
           */
         virtual void enableVoiceCallSign(uci::base::accessorType::AccessorType type = uci::type::accessorType::voiceCallSignType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by VoiceCallSign */
         virtual void clearVoiceCallSign()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TrackID.
           *
           * @return The acecssor that provides access to the complex content that is identified by TrackID.
           */
         virtual const uci::type::Link16TrackIdentifierType& getTrackID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TrackID.
           *
           * @return The acecssor that provides access to the complex content that is identified by TrackID.
           */
         virtual uci::type::Link16TrackIdentifierType& getTrackID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TrackID to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TrackID
           */
         virtual void setTrackID(const uci::type::Link16TrackIdentifierType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TrackID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TrackID is emabled or not
           */
         virtual bool hasTrackID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TrackID
           *
           * @param type = uci::type::accessorType::link16TrackIdentifierType This Accessor's accessor type
           */
         virtual void enableTrackID(uci::base::accessorType::AccessorType type = uci::type::accessorType::link16TrackIdentifierType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TrackID */
         virtual void clearTrackID()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         Link16DataLinkIdentifierType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The Link16DataLinkIdentifierType to copy from
           */
         Link16DataLinkIdentifierType(const Link16DataLinkIdentifierType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this Link16DataLinkIdentifierType to the contents of the
           * Link16DataLinkIdentifierType on the right hand side (rhs) of the assignment operator.Link16DataLinkIdentifierType
           * [only available to derived classes]
           *
           * @param rhs The Link16DataLinkIdentifierType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::Link16DataLinkIdentifierType
           * @return A constant reference to this Link16DataLinkIdentifierType.
           */
         const Link16DataLinkIdentifierType& operator=(const Link16DataLinkIdentifierType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // Link16DataLinkIdentifierType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__Link16DataLinkIdentifierType_h

