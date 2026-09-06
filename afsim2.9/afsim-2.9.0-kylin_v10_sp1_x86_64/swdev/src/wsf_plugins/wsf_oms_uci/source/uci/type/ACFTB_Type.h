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
#ifndef Uci__Type__ACFTB_Type_h
#define Uci__Type__ACFTB_Type_h 1

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

      /** This is the ACFTB_Type sequence accessor class */
      class ACFTB_Type : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ACFTB_Type()
         { }

         /** Returns this accessor's type constant, i.e. ACFTB_Type
           *
           * @return This accessor's type constant, i.e. ACFTB_Type
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::aCFTB_Type;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ACFTB_Type& accessor)
            throw(uci::base::UCIException) = 0;


         /** [White space: preserve] [Length: 11] [Pattern: "[A-Za-z0-9 ]{11}"] */
         typedef xs::String AC_MSN_ID;

         /** [White space: preserve] [Length: 11] [Pattern: "[0-9A-Za-z ]{11}"] */
         typedef xs::String AC_TAIL_NO;

         /** [White space: preserve] [Length: 13] [Pattern: "[0-9 ]{13}"] */
         typedef xs::String AC_TO;

         /** [White space: preserve] [Length: 7] [Pattern: "[A-Za-z0-9 ]{7}"] */
         typedef xs::String SENSOR_ID;

         /** [White space: preserve] [Length: 2] [Pattern: "[0-9 ]{2}"] */
         typedef xs::String SCENE_SOURCE;

         /** [White space: preserve] [Length: 7] [Pattern: "[0-9 ]{7}"] */
         typedef xs::String SCNUM;

         /** [White space: preserve] [Length: 7] [Pattern: "[0-9 ]{7}"] */
         typedef xs::String IMHOSTNO;

         /** [White space: preserve] [Length: 6] [Pattern: "[0-9 ]{6}"] */
         typedef xs::String IMREQID;

         /** Returns the value of the string data type that is identified by the AC_MSN_ID.
           *
           * @return The value of the string data type identified by AC_MSN_ID.
           */
         virtual const uci::type::ACFTB_Type::AC_MSN_ID& getAC_MSN_ID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the AC_MSN_ID.
           *
           * @return The value of the string data type identified by AC_MSN_ID.
           */
         virtual uci::type::ACFTB_Type::AC_MSN_ID& getAC_MSN_ID()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the AC_MSN_ID to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setAC_MSN_ID(const uci::type::ACFTB_Type::AC_MSN_ID& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the AC_MSN_ID to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setAC_MSN_ID(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the AC_MSN_ID to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setAC_MSN_ID(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AC_MSN_ID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AC_MSN_ID is emabled or not
           */
         virtual bool hasAC_MSN_ID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AC_MSN_ID
           *
           * @param type = uci::type::aCFTB_Type::accessorType::aC_MSN_ID This Accessor's accessor type
           */
         virtual void enableAC_MSN_ID(uci::base::accessorType::AccessorType type = uci::type::aCFTB_Type::accessorType::aC_MSN_ID)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AC_MSN_ID */
         virtual void clearAC_MSN_ID()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the AC_TAIL_NO.
           *
           * @return The value of the string data type identified by AC_TAIL_NO.
           */
         virtual const uci::type::ACFTB_Type::AC_TAIL_NO& getAC_TAIL_NO() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the AC_TAIL_NO.
           *
           * @return The value of the string data type identified by AC_TAIL_NO.
           */
         virtual uci::type::ACFTB_Type::AC_TAIL_NO& getAC_TAIL_NO()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the AC_TAIL_NO to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setAC_TAIL_NO(const uci::type::ACFTB_Type::AC_TAIL_NO& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the AC_TAIL_NO to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setAC_TAIL_NO(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the AC_TAIL_NO to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setAC_TAIL_NO(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AC_TAIL_NO exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AC_TAIL_NO is emabled or not
           */
         virtual bool hasAC_TAIL_NO() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AC_TAIL_NO
           *
           * @param type = uci::type::aCFTB_Type::accessorType::aC_TAIL_NO This Accessor's accessor type
           */
         virtual void enableAC_TAIL_NO(uci::base::accessorType::AccessorType type = uci::type::aCFTB_Type::accessorType::aC_TAIL_NO)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AC_TAIL_NO */
         virtual void clearAC_TAIL_NO()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the AC_TO.
           *
           * @return The value of the string data type identified by AC_TO.
           */
         virtual const uci::type::ACFTB_Type::AC_TO& getAC_TO() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the AC_TO.
           *
           * @return The value of the string data type identified by AC_TO.
           */
         virtual uci::type::ACFTB_Type::AC_TO& getAC_TO()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the AC_TO to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setAC_TO(const uci::type::ACFTB_Type::AC_TO& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the AC_TO to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setAC_TO(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the AC_TO to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setAC_TO(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AC_TO exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AC_TO is emabled or not
           */
         virtual bool hasAC_TO() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AC_TO
           *
           * @param type = uci::type::aCFTB_Type::accessorType::aC_TO This Accessor's accessor type
           */
         virtual void enableAC_TO(uci::base::accessorType::AccessorType type = uci::type::aCFTB_Type::accessorType::aC_TO)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AC_TO */
         virtual void clearAC_TO()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the SENSOR_ID.
           *
           * @return The value of the string data type identified by SENSOR_ID.
           */
         virtual const uci::type::ACFTB_Type::SENSOR_ID& getSENSOR_ID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the SENSOR_ID.
           *
           * @return The value of the string data type identified by SENSOR_ID.
           */
         virtual uci::type::ACFTB_Type::SENSOR_ID& getSENSOR_ID()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SENSOR_ID to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setSENSOR_ID(const uci::type::ACFTB_Type::SENSOR_ID& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SENSOR_ID to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSENSOR_ID(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SENSOR_ID to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSENSOR_ID(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SENSOR_ID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SENSOR_ID is emabled or not
           */
         virtual bool hasSENSOR_ID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SENSOR_ID
           *
           * @param type = uci::type::aCFTB_Type::accessorType::sENSOR_ID This Accessor's accessor type
           */
         virtual void enableSENSOR_ID(uci::base::accessorType::AccessorType type = uci::type::aCFTB_Type::accessorType::sENSOR_ID)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SENSOR_ID */
         virtual void clearSENSOR_ID()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the SCENE_SOURCE.
           *
           * @return The value of the string data type identified by SCENE_SOURCE.
           */
         virtual const uci::type::ACFTB_Type::SCENE_SOURCE& getSCENE_SOURCE() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the SCENE_SOURCE.
           *
           * @return The value of the string data type identified by SCENE_SOURCE.
           */
         virtual uci::type::ACFTB_Type::SCENE_SOURCE& getSCENE_SOURCE()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SCENE_SOURCE to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setSCENE_SOURCE(const uci::type::ACFTB_Type::SCENE_SOURCE& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SCENE_SOURCE to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSCENE_SOURCE(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SCENE_SOURCE to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSCENE_SOURCE(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SCENE_SOURCE exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SCENE_SOURCE is emabled or not
           */
         virtual bool hasSCENE_SOURCE() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SCENE_SOURCE
           *
           * @param type = uci::type::aCFTB_Type::accessorType::sCENE_SOURCE This Accessor's accessor type
           */
         virtual void enableSCENE_SOURCE(uci::base::accessorType::AccessorType type = uci::type::aCFTB_Type::accessorType::sCENE_SOURCE)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SCENE_SOURCE */
         virtual void clearSCENE_SOURCE()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the SCNUM.
           *
           * @return The value of the string data type identified by SCNUM.
           */
         virtual const uci::type::ACFTB_Type::SCNUM& getSCNUM() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the SCNUM.
           *
           * @return The value of the string data type identified by SCNUM.
           */
         virtual uci::type::ACFTB_Type::SCNUM& getSCNUM()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SCNUM to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setSCNUM(const uci::type::ACFTB_Type::SCNUM& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SCNUM to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSCNUM(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SCNUM to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSCNUM(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SCNUM exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SCNUM is emabled or not
           */
         virtual bool hasSCNUM() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SCNUM
           *
           * @param type = uci::type::aCFTB_Type::accessorType::sCNUM This Accessor's accessor type
           */
         virtual void enableSCNUM(uci::base::accessorType::AccessorType type = uci::type::aCFTB_Type::accessorType::sCNUM)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SCNUM */
         virtual void clearSCNUM()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the IMHOSTNO.
           *
           * @return The value of the string data type identified by IMHOSTNO.
           */
         virtual const uci::type::ACFTB_Type::IMHOSTNO& getIMHOSTNO() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the IMHOSTNO.
           *
           * @return The value of the string data type identified by IMHOSTNO.
           */
         virtual uci::type::ACFTB_Type::IMHOSTNO& getIMHOSTNO()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the IMHOSTNO to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setIMHOSTNO(const uci::type::ACFTB_Type::IMHOSTNO& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the IMHOSTNO to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setIMHOSTNO(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the IMHOSTNO to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setIMHOSTNO(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by IMHOSTNO exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by IMHOSTNO is emabled or not
           */
         virtual bool hasIMHOSTNO() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by IMHOSTNO
           *
           * @param type = uci::type::aCFTB_Type::accessorType::iMHOSTNO This Accessor's accessor type
           */
         virtual void enableIMHOSTNO(uci::base::accessorType::AccessorType type = uci::type::aCFTB_Type::accessorType::iMHOSTNO)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by IMHOSTNO */
         virtual void clearIMHOSTNO()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the IMREQID.
           *
           * @return The value of the string data type identified by IMREQID.
           */
         virtual const uci::type::ACFTB_Type::IMREQID& getIMREQID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the IMREQID.
           *
           * @return The value of the string data type identified by IMREQID.
           */
         virtual uci::type::ACFTB_Type::IMREQID& getIMREQID()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the IMREQID to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setIMREQID(const uci::type::ACFTB_Type::IMREQID& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the IMREQID to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setIMREQID(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the IMREQID to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setIMREQID(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by IMREQID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by IMREQID is emabled or not
           */
         virtual bool hasIMREQID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by IMREQID
           *
           * @param type = uci::type::aCFTB_Type::accessorType::iMREQID This Accessor's accessor type
           */
         virtual void enableIMREQID(uci::base::accessorType::AccessorType type = uci::type::aCFTB_Type::accessorType::iMREQID)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by IMREQID */
         virtual void clearIMREQID()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ACFTB_Type()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ACFTB_Type to copy from
           */
         ACFTB_Type(const ACFTB_Type& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ACFTB_Type to the contents of the ACFTB_Type on the right hand
           * side (rhs) of the assignment operator.ACFTB_Type [only available to derived classes]
           *
           * @param rhs The ACFTB_Type on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::ACFTB_Type
           * @return A constant reference to this ACFTB_Type.
           */
         const ACFTB_Type& operator=(const ACFTB_Type& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ACFTB_Type


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ACFTB_Type_h

