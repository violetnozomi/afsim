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
#ifndef Uci__Type__MSTGTA_Type_h
#define Uci__Type__MSTGTA_Type_h 1

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

      /** This is the MSTGTA_Type sequence accessor class */
      class MSTGTA_Type : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~MSTGTA_Type()
         { }

         /** Returns this accessor's type constant, i.e. MSTGTA_Type
           *
           * @return This accessor's type constant, i.e. MSTGTA_Type
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::mSTGTA_Type;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const MSTGTA_Type& accessor)
            throw(uci::base::UCIException) = 0;


         /** [White space: preserve] [Length: 6] [Pattern: "[0-9 ]{6}"] */
         typedef xs::String TGT_NUM;

         /** [White space: preserve] [Length: 4] [Pattern: "[0-9 ]{4}"] */
         typedef xs::String TGT_PRI;

         /** [White space: preserve] [Length: 13] [Pattern: "[0-9 ]{13}"] */
         typedef xs::String TGT_LTIOV;

         /** [White space: preserve] [Length: 2] [Pattern: "[0-9 ]{2}"] */
         typedef xs::String TGT_COLL;

         /** [White space: preserve] [Length: 6] [Pattern: "[1-9][0-9 ]{5}"] */
         typedef xs::String TGT_CAT;

         /** [White space: preserve] [Length: 8] [Pattern: "[0-9]{6}Z "] */
         typedef xs::String TGT_UTC;

         /** Returns the value of the string data type that is identified by the TGT_NUM.
           *
           * @return The value of the string data type identified by TGT_NUM.
           */
         virtual const uci::type::MSTGTA_Type::TGT_NUM& getTGT_NUM() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the TGT_NUM.
           *
           * @return The value of the string data type identified by TGT_NUM.
           */
         virtual uci::type::MSTGTA_Type::TGT_NUM& getTGT_NUM()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the TGT_NUM to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setTGT_NUM(const uci::type::MSTGTA_Type::TGT_NUM& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the TGT_NUM to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setTGT_NUM(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the TGT_NUM to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setTGT_NUM(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TGT_NUM exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TGT_NUM is emabled or not
           */
         virtual bool hasTGT_NUM() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TGT_NUM
           *
           * @param type = uci::type::mSTGTA_Type::accessorType::tGT_NUM This Accessor's accessor type
           */
         virtual void enableTGT_NUM(uci::base::accessorType::AccessorType type = uci::type::mSTGTA_Type::accessorType::tGT_NUM)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TGT_NUM */
         virtual void clearTGT_NUM()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the TGT_PRI.
           *
           * @return The value of the string data type identified by TGT_PRI.
           */
         virtual const uci::type::MSTGTA_Type::TGT_PRI& getTGT_PRI() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the TGT_PRI.
           *
           * @return The value of the string data type identified by TGT_PRI.
           */
         virtual uci::type::MSTGTA_Type::TGT_PRI& getTGT_PRI()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the TGT_PRI to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setTGT_PRI(const uci::type::MSTGTA_Type::TGT_PRI& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the TGT_PRI to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setTGT_PRI(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the TGT_PRI to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setTGT_PRI(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TGT_PRI exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TGT_PRI is emabled or not
           */
         virtual bool hasTGT_PRI() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TGT_PRI
           *
           * @param type = uci::type::mSTGTA_Type::accessorType::tGT_PRI This Accessor's accessor type
           */
         virtual void enableTGT_PRI(uci::base::accessorType::AccessorType type = uci::type::mSTGTA_Type::accessorType::tGT_PRI)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TGT_PRI */
         virtual void clearTGT_PRI()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the TGT_LTIOV.
           *
           * @return The value of the string data type identified by TGT_LTIOV.
           */
         virtual const uci::type::MSTGTA_Type::TGT_LTIOV& getTGT_LTIOV() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the TGT_LTIOV.
           *
           * @return The value of the string data type identified by TGT_LTIOV.
           */
         virtual uci::type::MSTGTA_Type::TGT_LTIOV& getTGT_LTIOV()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the TGT_LTIOV to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setTGT_LTIOV(const uci::type::MSTGTA_Type::TGT_LTIOV& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the TGT_LTIOV to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setTGT_LTIOV(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the TGT_LTIOV to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setTGT_LTIOV(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TGT_LTIOV exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TGT_LTIOV is emabled or not
           */
         virtual bool hasTGT_LTIOV() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TGT_LTIOV
           *
           * @param type = uci::type::mSTGTA_Type::accessorType::tGT_LTIOV This Accessor's accessor type
           */
         virtual void enableTGT_LTIOV(uci::base::accessorType::AccessorType type = uci::type::mSTGTA_Type::accessorType::tGT_LTIOV)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TGT_LTIOV */
         virtual void clearTGT_LTIOV()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the TGT_COLL.
           *
           * @return The value of the string data type identified by TGT_COLL.
           */
         virtual const uci::type::MSTGTA_Type::TGT_COLL& getTGT_COLL() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the TGT_COLL.
           *
           * @return The value of the string data type identified by TGT_COLL.
           */
         virtual uci::type::MSTGTA_Type::TGT_COLL& getTGT_COLL()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the TGT_COLL to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setTGT_COLL(const uci::type::MSTGTA_Type::TGT_COLL& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the TGT_COLL to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setTGT_COLL(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the TGT_COLL to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setTGT_COLL(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TGT_COLL exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TGT_COLL is emabled or not
           */
         virtual bool hasTGT_COLL() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TGT_COLL
           *
           * @param type = uci::type::mSTGTA_Type::accessorType::tGT_COLL This Accessor's accessor type
           */
         virtual void enableTGT_COLL(uci::base::accessorType::AccessorType type = uci::type::mSTGTA_Type::accessorType::tGT_COLL)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TGT_COLL */
         virtual void clearTGT_COLL()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the TGT_CAT.
           *
           * @return The value of the string data type identified by TGT_CAT.
           */
         virtual const uci::type::MSTGTA_Type::TGT_CAT& getTGT_CAT() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the TGT_CAT.
           *
           * @return The value of the string data type identified by TGT_CAT.
           */
         virtual uci::type::MSTGTA_Type::TGT_CAT& getTGT_CAT()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the TGT_CAT to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setTGT_CAT(const uci::type::MSTGTA_Type::TGT_CAT& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the TGT_CAT to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setTGT_CAT(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the TGT_CAT to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setTGT_CAT(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TGT_CAT exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TGT_CAT is emabled or not
           */
         virtual bool hasTGT_CAT() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TGT_CAT
           *
           * @param type = uci::type::mSTGTA_Type::accessorType::tGT_CAT This Accessor's accessor type
           */
         virtual void enableTGT_CAT(uci::base::accessorType::AccessorType type = uci::type::mSTGTA_Type::accessorType::tGT_CAT)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TGT_CAT */
         virtual void clearTGT_CAT()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the TGT_UTC.
           *
           * @return The value of the string data type identified by TGT_UTC.
           */
         virtual const uci::type::MSTGTA_Type::TGT_UTC& getTGT_UTC() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the TGT_UTC.
           *
           * @return The value of the string data type identified by TGT_UTC.
           */
         virtual uci::type::MSTGTA_Type::TGT_UTC& getTGT_UTC()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the TGT_UTC to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setTGT_UTC(const uci::type::MSTGTA_Type::TGT_UTC& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the TGT_UTC to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setTGT_UTC(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the TGT_UTC to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setTGT_UTC(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TGT_UTC exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TGT_UTC is emabled or not
           */
         virtual bool hasTGT_UTC() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TGT_UTC
           *
           * @param type = uci::type::mSTGTA_Type::accessorType::tGT_UTC This Accessor's accessor type
           */
         virtual void enableTGT_UTC(uci::base::accessorType::AccessorType type = uci::type::mSTGTA_Type::accessorType::tGT_UTC)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TGT_UTC */
         virtual void clearTGT_UTC()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         MSTGTA_Type()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The MSTGTA_Type to copy from
           */
         MSTGTA_Type(const MSTGTA_Type& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this MSTGTA_Type to the contents of the MSTGTA_Type on the right hand
           * side (rhs) of the assignment operator.MSTGTA_Type [only available to derived classes]
           *
           * @param rhs The MSTGTA_Type on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::MSTGTA_Type
           * @return A constant reference to this MSTGTA_Type.
           */
         const MSTGTA_Type& operator=(const MSTGTA_Type& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // MSTGTA_Type


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__MSTGTA_Type_h

