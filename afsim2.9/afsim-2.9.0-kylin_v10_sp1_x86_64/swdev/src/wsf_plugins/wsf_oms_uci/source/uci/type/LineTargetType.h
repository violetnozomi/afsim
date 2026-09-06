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
#ifndef Uci__Type__LineTargetType_h
#define Uci__Type__LineTargetType_h 1

#if !defined(Uci__Type__LineType_h)
# include "uci/type/LineType.h"
#endif

#if !defined(Xs__Type__StringXmlSchemaPrimitives_h)
# include "xs/type/stringXmlSchemaPrimitives.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the LineTargetType sequence accessor class */
      class LineTargetType : public virtual uci::type::LineType {
      public:

         /** The destructor */
         virtual ~LineTargetType()
         { }

         /** Returns this accessor's type constant, i.e. LineTargetType
           *
           * @return This accessor's type constant, i.e. LineTargetType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::lineTargetType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const LineTargetType& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Length: 15] [Pattern: "[A-Za-z0-9 ]{15}"] */
         typedef xs::String LOC_ID;

         /** Returns the value of the string data type that is identified by the LOC_ID.
           *
           * @return The value of the string data type identified by LOC_ID.
           */
         virtual const uci::type::LineTargetType::LOC_ID& getLOC_ID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the LOC_ID.
           *
           * @return The value of the string data type identified by LOC_ID.
           */
         virtual uci::type::LineTargetType::LOC_ID& getLOC_ID()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the LOC_ID to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setLOC_ID(const uci::type::LineTargetType::LOC_ID& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the LOC_ID to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setLOC_ID(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the LOC_ID to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setLOC_ID(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by LOC_ID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by LOC_ID is emabled or not
           */
         virtual bool hasLOC_ID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by LOC_ID
           *
           * @param type = uci::type::lineTargetType::accessorType::lOC_ID This Accessor's accessor type
           */
         virtual void enableLOC_ID(uci::base::accessorType::AccessorType type = uci::type::lineTargetType::accessorType::lOC_ID)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by LOC_ID */
         virtual void clearLOC_ID()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         LineTargetType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The LineTargetType to copy from
           */
         LineTargetType(const LineTargetType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this LineTargetType to the contents of the LineTargetType on the right
           * hand side (rhs) of the assignment operator.LineTargetType [only available to derived classes]
           *
           * @param rhs The LineTargetType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::LineTargetType
           * @return A constant reference to this LineTargetType.
           */
         const LineTargetType& operator=(const LineTargetType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // LineTargetType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__LineTargetType_h

