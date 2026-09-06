// This file was generated  on 12/5/2018 at 1:1:44 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__ATO_TraceabilityType_h
#define Uci__Type__ATO_TraceabilityType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Xs__Type__StringXmlSchemaPrimitives_h)
# include "xs/type/stringXmlSchemaPrimitives.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ATO_TraceabilityType sequence accessor class */
      class ATO_TraceabilityType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ATO_TraceabilityType()
         { }

         /** Returns this accessor's type constant, i.e. ATO_TraceabilityType
           *
           * @return This accessor's type constant, i.e. ATO_TraceabilityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::aTO_TraceabilityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ATO_TraceabilityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** This element refers to the aircraft callsign corresponding to the mission ID.; [White space: preserve] [Length: 56]
           * [Pattern: "[A-Za-z0-9 \-_]{56}"]
           */
         typedef xs::String MissionCallSign;

         /** This element represents the unique ID of the mission specified by the ATO.; [White space: preserve] [Length: 10]
           * [Pattern: "[A-Za-z0-9 \-_]{10}"]
           */
         typedef xs::String MissionID;

         /** Returns the value of the string data type that is identified by the MissionCallSign.
           *
           * @return The value of the string data type identified by MissionCallSign.
           */
         virtual const uci::type::ATO_TraceabilityType::MissionCallSign& getMissionCallSign() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the MissionCallSign.
           *
           * @return The value of the string data type identified by MissionCallSign.
           */
         virtual uci::type::ATO_TraceabilityType::MissionCallSign& getMissionCallSign()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the MissionCallSign to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setMissionCallSign(const uci::type::ATO_TraceabilityType::MissionCallSign& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the MissionCallSign to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setMissionCallSign(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the MissionCallSign to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setMissionCallSign(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the MissionID.
           *
           * @return The value of the string data type identified by MissionID.
           */
         virtual const uci::type::ATO_TraceabilityType::MissionID& getMissionID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the MissionID.
           *
           * @return The value of the string data type identified by MissionID.
           */
         virtual uci::type::ATO_TraceabilityType::MissionID& getMissionID()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the MissionID to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setMissionID(const uci::type::ATO_TraceabilityType::MissionID& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the MissionID to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setMissionID(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the MissionID to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setMissionID(const char* value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ATO_TraceabilityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ATO_TraceabilityType to copy from
           */
         ATO_TraceabilityType(const ATO_TraceabilityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ATO_TraceabilityType to the contents of the ATO_TraceabilityType
           * on the right hand side (rhs) of the assignment operator.ATO_TraceabilityType [only available to derived classes]
           *
           * @param rhs The ATO_TraceabilityType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::ATO_TraceabilityType
           * @return A constant reference to this ATO_TraceabilityType.
           */
         const ATO_TraceabilityType& operator=(const ATO_TraceabilityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ATO_TraceabilityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ATO_TraceabilityType_h

