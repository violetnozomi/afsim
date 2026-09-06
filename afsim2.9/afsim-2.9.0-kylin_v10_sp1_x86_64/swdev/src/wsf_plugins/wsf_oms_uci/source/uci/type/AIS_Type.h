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
#ifndef Uci__Type__AIS_Type_h
#define Uci__Type__AIS_Type_h 1

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

      /** This is the AIS_Type sequence accessor class */
      class AIS_Type : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~AIS_Type()
         { }

         /** Returns this accessor's type constant, i.e. AIS_Type
           *
           * @return This accessor's type constant, i.e. AIS_Type
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::aIS_Type;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AIS_Type& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Length: 9] [Pattern: "[0-9]{9}"] */
         typedef xs::String MMSI_Number;

         /** [Length: 10] [Pattern: "IMO[0-9]{7}"] */
         typedef xs::String IMO_Number;

         /** [White space: preserve] [Length: 20] [Pattern: "[A-Za-z0-9 \-_]{20}"] */
         typedef xs::String VesselName;

         /** [Maximum length: 20] [White space: preserve] [Minimum length: 0] [Pattern: "[A-Za-z0-9 \-_]{0,20}"] */
         typedef xs::String CallSign;

         /** Returns the value of the string data type that is identified by the MMSI_Number.
           *
           * @return The value of the string data type identified by MMSI_Number.
           */
         virtual const uci::type::AIS_Type::MMSI_Number& getMMSI_Number() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the MMSI_Number.
           *
           * @return The value of the string data type identified by MMSI_Number.
           */
         virtual uci::type::AIS_Type::MMSI_Number& getMMSI_Number()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the MMSI_Number to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setMMSI_Number(const uci::type::AIS_Type::MMSI_Number& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the MMSI_Number to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setMMSI_Number(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the MMSI_Number to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setMMSI_Number(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MMSI_Number exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MMSI_Number is emabled or not
           */
         virtual bool hasMMSI_Number() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MMSI_Number
           *
           * @param type = uci::type::aIS_Type::accessorType::mMSI_Number This Accessor's accessor type
           */
         virtual void enableMMSI_Number(uci::base::accessorType::AccessorType type = uci::type::aIS_Type::accessorType::mMSI_Number)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MMSI_Number */
         virtual void clearMMSI_Number()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the IMO_Number.
           *
           * @return The value of the string data type identified by IMO_Number.
           */
         virtual const uci::type::AIS_Type::IMO_Number& getIMO_Number() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the IMO_Number.
           *
           * @return The value of the string data type identified by IMO_Number.
           */
         virtual uci::type::AIS_Type::IMO_Number& getIMO_Number()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the IMO_Number to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setIMO_Number(const uci::type::AIS_Type::IMO_Number& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the IMO_Number to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setIMO_Number(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the IMO_Number to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setIMO_Number(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by IMO_Number exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by IMO_Number is emabled or not
           */
         virtual bool hasIMO_Number() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by IMO_Number
           *
           * @param type = uci::type::aIS_Type::accessorType::iMO_Number This Accessor's accessor type
           */
         virtual void enableIMO_Number(uci::base::accessorType::AccessorType type = uci::type::aIS_Type::accessorType::iMO_Number)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by IMO_Number */
         virtual void clearIMO_Number()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the VesselName.
           *
           * @return The value of the string data type identified by VesselName.
           */
         virtual const uci::type::AIS_Type::VesselName& getVesselName() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the VesselName.
           *
           * @return The value of the string data type identified by VesselName.
           */
         virtual uci::type::AIS_Type::VesselName& getVesselName()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the VesselName to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setVesselName(const uci::type::AIS_Type::VesselName& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the VesselName to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setVesselName(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the VesselName to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setVesselName(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by VesselName exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by VesselName is emabled or not
           */
         virtual bool hasVesselName() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by VesselName
           *
           * @param type = uci::type::aIS_Type::accessorType::vesselName This Accessor's accessor type
           */
         virtual void enableVesselName(uci::base::accessorType::AccessorType type = uci::type::aIS_Type::accessorType::vesselName)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by VesselName */
         virtual void clearVesselName()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the CallSign.
           *
           * @return The value of the string data type identified by CallSign.
           */
         virtual const uci::type::AIS_Type::CallSign& getCallSign() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the CallSign.
           *
           * @return The value of the string data type identified by CallSign.
           */
         virtual uci::type::AIS_Type::CallSign& getCallSign()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the CallSign to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setCallSign(const uci::type::AIS_Type::CallSign& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the CallSign to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setCallSign(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the CallSign to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setCallSign(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CallSign exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CallSign is emabled or not
           */
         virtual bool hasCallSign() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CallSign
           *
           * @param type = uci::type::aIS_Type::accessorType::callSign This Accessor's accessor type
           */
         virtual void enableCallSign(uci::base::accessorType::AccessorType type = uci::type::aIS_Type::accessorType::callSign)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CallSign */
         virtual void clearCallSign()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AIS_Type()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AIS_Type to copy from
           */
         AIS_Type(const AIS_Type& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AIS_Type to the contents of the AIS_Type on the right hand side
           * (rhs) of the assignment operator.AIS_Type [only available to derived classes]
           *
           * @param rhs The AIS_Type on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::AIS_Type
           * @return A constant reference to this AIS_Type.
           */
         const AIS_Type& operator=(const AIS_Type& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AIS_Type


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AIS_Type_h

