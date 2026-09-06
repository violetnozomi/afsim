// This file was generated  on 12/5/2018 at 1:1:45 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__CommCoverageUserDataType_h
#define Uci__Type__CommCoverageUserDataType_h 1

#if !defined(Uci__Type__CommCoverageDataType_h)
# include "uci/type/CommCoverageDataType.h"
#endif

#if !defined(Uci__Type__VisibleString256Type_h)
# include "uci/type/VisibleString256Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CommCoverageUserDataType sequence accessor class */
      class CommCoverageUserDataType : public virtual uci::type::CommCoverageDataType {
      public:

         /** The destructor */
         virtual ~CommCoverageUserDataType()
         { }

         /** Returns this accessor's type constant, i.e. CommCoverageUserDataType
           *
           * @return This accessor's type constant, i.e. CommCoverageUserDataType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commCoverageUserDataType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommCoverageUserDataType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the CommUserID.
           *
           * @return The value of the string data type identified by CommUserID.
           */
         virtual const uci::type::VisibleString256Type& getCommUserID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the CommUserID.
           *
           * @return The value of the string data type identified by CommUserID.
           */
         virtual uci::type::VisibleString256Type& getCommUserID()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the CommUserID to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setCommUserID(const uci::type::VisibleString256Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the CommUserID to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setCommUserID(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the CommUserID to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setCommUserID(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CommUserID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CommUserID is emabled or not
           */
         virtual bool hasCommUserID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CommUserID
           *
           * @param type = uci::type::accessorType::visibleString256Type This Accessor's accessor type
           */
         virtual void enableCommUserID(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString256Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CommUserID */
         virtual void clearCommUserID()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommCoverageUserDataType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommCoverageUserDataType to copy from
           */
         CommCoverageUserDataType(const CommCoverageUserDataType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommCoverageUserDataType to the contents of the
           * CommCoverageUserDataType on the right hand side (rhs) of the assignment operator.CommCoverageUserDataType [only
           * available to derived classes]
           *
           * @param rhs The CommCoverageUserDataType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::CommCoverageUserDataType
           * @return A constant reference to this CommCoverageUserDataType.
           */
         const CommCoverageUserDataType& operator=(const CommCoverageUserDataType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommCoverageUserDataType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommCoverageUserDataType_h

