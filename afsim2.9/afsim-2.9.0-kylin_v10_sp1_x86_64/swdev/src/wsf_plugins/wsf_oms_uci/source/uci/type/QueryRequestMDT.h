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
#ifndef Uci__Type__QueryRequestMDT_h
#define Uci__Type__QueryRequestMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__QueryRequestID_Type_h)
# include "uci/type/QueryRequestID_Type.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__EmptyType_h)
# include "uci/type/EmptyType.h"
#endif

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the QueryRequestMDT sequence accessor class */
      class QueryRequestMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~QueryRequestMDT()
         { }

         /** Returns this accessor's type constant, i.e. QueryRequestMDT
           *
           * @return This accessor's type constant, i.e. QueryRequestMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::queryRequestMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const QueryRequestMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the QueryRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by QueryRequestID.
           */
         virtual const uci::type::QueryRequestID_Type& getQueryRequestID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the QueryRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by QueryRequestID.
           */
         virtual uci::type::QueryRequestID_Type& getQueryRequestID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the QueryRequestID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by QueryRequestID
           */
         virtual void setQueryRequestID(const uci::type::QueryRequestID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ResultsInNativeMessage.
           *
           * @return The value of the simple primitive data type identified by ResultsInNativeMessage.
           */
         virtual xs::Boolean getResultsInNativeMessage() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ResultsInNativeMessage.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setResultsInNativeMessage(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the QueryAll.
           *
           * @return The value of the simple primitive data type identified by QueryAll.
           */
         virtual xs::Boolean getQueryAll() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the QueryAll.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setQueryAll(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the QueryIdentifiersOnly.
           *
           * @return The value of the string data type identified by QueryIdentifiersOnly.
           */
         virtual const uci::type::EmptyType& getQueryIdentifiersOnly() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the QueryIdentifiersOnly.
           *
           * @return The value of the string data type identified by QueryIdentifiersOnly.
           */
         virtual uci::type::EmptyType& getQueryIdentifiersOnly()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the QueryIdentifiersOnly to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setQueryIdentifiersOnly(const uci::type::EmptyType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the QueryIdentifiersOnly to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setQueryIdentifiersOnly(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the QueryIdentifiersOnly to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setQueryIdentifiersOnly(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by QueryIdentifiersOnly exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by QueryIdentifiersOnly is emabled or not
           */
         virtual bool hasQueryIdentifiersOnly() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by QueryIdentifiersOnly
           *
           * @param type = uci::type::accessorType::emptyType This Accessor's accessor type
           */
         virtual void enableQueryIdentifiersOnly(uci::base::accessorType::AccessorType type = uci::type::accessorType::emptyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by QueryIdentifiersOnly */
         virtual void clearQueryIdentifiersOnly()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RequestFilter.
           *
           * @return The acecssor that provides access to the complex content that is identified by RequestFilter.
           */
         virtual const uci::type::QueryFilterPET& getRequestFilter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RequestFilter.
           *
           * @return The acecssor that provides access to the complex content that is identified by RequestFilter.
           */
         virtual uci::type::QueryFilterPET& getRequestFilter()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RequestFilter to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RequestFilter
           */
         virtual void setRequestFilter(const uci::type::QueryFilterPET& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SystemID.
           */
         virtual const uci::type::SystemID_Type& getSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SystemID.
           */
         virtual uci::type::SystemID_Type& getSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SystemID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SystemID
           */
         virtual void setSystemID(const uci::type::SystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SystemID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SystemID is emabled or not
           */
         virtual bool hasSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SystemID
           *
           * @param type = uci::type::accessorType::systemID_Type This Accessor's accessor type
           */
         virtual void enableSystemID(uci::base::accessorType::AccessorType type = uci::type::accessorType::systemID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SystemID */
         virtual void clearSystemID()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         QueryRequestMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The QueryRequestMDT to copy from
           */
         QueryRequestMDT(const QueryRequestMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this QueryRequestMDT to the contents of the QueryRequestMDT on the
           * right hand side (rhs) of the assignment operator.QueryRequestMDT [only available to derived classes]
           *
           * @param rhs The QueryRequestMDT on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::QueryRequestMDT
           * @return A constant reference to this QueryRequestMDT.
           */
         const QueryRequestMDT& operator=(const QueryRequestMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // QueryRequestMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__QueryRequestMDT_h

