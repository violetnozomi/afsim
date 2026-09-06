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
#ifndef Uci__Type__CommOutageType_h
#define Uci__Type__CommOutageType_h 1

#if !defined(Uci__Type__CommCoverageDataType_h)
# include "uci/type/CommCoverageDataType.h"
#endif

#if !defined(Uci__Base__IntAccessor_h)
# include "uci/base/IntAccessor.h"
#endif

#if !defined(Uci__Type__VisibleString1024Type_h)
# include "uci/type/VisibleString1024Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CommOutageType sequence accessor class */
      class CommOutageType : public virtual uci::type::CommCoverageDataType {
      public:

         /** The destructor */
         virtual ~CommOutageType()
         { }

         /** Returns this accessor's type constant, i.e. CommOutageType
           *
           * @return This accessor's type constant, i.e. CommOutageType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commOutageType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommOutageType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the OutageCode.
           *
           * @return The value of the simple primitive data type identified by OutageCode.
           */
         virtual xs::Int getOutageCode() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the OutageCode.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setOutageCode(xs::Int value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by OutageCode exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by OutageCode is emabled or not
           */
         virtual bool hasOutageCode() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by OutageCode
           *
           * @param type = uci::base::accessorType::intAccessor This Accessor's accessor type
           */
         virtual void enableOutageCode(uci::base::accessorType::AccessorType type = uci::base::accessorType::intAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by OutageCode */
         virtual void clearOutageCode()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the OutageDetails.
           *
           * @return The value of the string data type identified by OutageDetails.
           */
         virtual const uci::type::VisibleString1024Type& getOutageDetails() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the OutageDetails.
           *
           * @return The value of the string data type identified by OutageDetails.
           */
         virtual uci::type::VisibleString1024Type& getOutageDetails()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the OutageDetails to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setOutageDetails(const uci::type::VisibleString1024Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the OutageDetails to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setOutageDetails(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the OutageDetails to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setOutageDetails(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by OutageDetails exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by OutageDetails is emabled or not
           */
         virtual bool hasOutageDetails() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by OutageDetails
           *
           * @param type = uci::type::accessorType::visibleString1024Type This Accessor's accessor type
           */
         virtual void enableOutageDetails(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString1024Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by OutageDetails */
         virtual void clearOutageDetails()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommOutageType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommOutageType to copy from
           */
         CommOutageType(const CommOutageType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommOutageType to the contents of the CommOutageType on the right
           * hand side (rhs) of the assignment operator.CommOutageType [only available to derived classes]
           *
           * @param rhs The CommOutageType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::CommOutageType
           * @return A constant reference to this CommOutageType.
           */
         const CommOutageType& operator=(const CommOutageType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommOutageType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommOutageType_h

