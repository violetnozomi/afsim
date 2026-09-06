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
#ifndef Uci__Type__CommPointingRequestMDT_h
#define Uci__Type__CommPointingRequestMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CommPointingRequestID_Type_h)
# include "uci/type/CommPointingRequestID_Type.h"
#endif

#if !defined(Uci__Type__CommPointingID_Type_h)
# include "uci/type/CommPointingID_Type.h"
#endif

#if !defined(Uci__Type__CommPointingRequestEnum_h)
# include "uci/type/CommPointingRequestEnum.h"
#endif

#if !defined(Uci__Type__CommPointingType_h)
# include "uci/type/CommPointingType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CommPointingRequestMDT sequence accessor class */
      class CommPointingRequestMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CommPointingRequestMDT()
         { }

         /** Returns this accessor's type constant, i.e. CommPointingRequestMDT
           *
           * @return This accessor's type constant, i.e. CommPointingRequestMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commPointingRequestMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommPointingRequestMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommPointingRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommPointingRequestID.
           */
         virtual const uci::type::CommPointingRequestID_Type& getCommPointingRequestID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommPointingRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommPointingRequestID.
           */
         virtual uci::type::CommPointingRequestID_Type& getCommPointingRequestID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CommPointingRequestID to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CommPointingRequestID
           */
         virtual void setCommPointingRequestID(const uci::type::CommPointingRequestID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommPointingID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommPointingID.
           */
         virtual const uci::type::CommPointingID_Type& getCommPointingID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommPointingID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommPointingID.
           */
         virtual uci::type::CommPointingID_Type& getCommPointingID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CommPointingID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CommPointingID
           */
         virtual void setCommPointingID(const uci::type::CommPointingID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PointingRequestState.
           *
           * @return The value of the enumeration identified by PointingRequestState.
           */
         virtual const uci::type::CommPointingRequestEnum& getPointingRequestState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PointingRequestState.
           *
           * @return The value of the enumeration identified by PointingRequestState.
           */
         virtual uci::type::CommPointingRequestEnum& getPointingRequestState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PointingRequestState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPointingRequestState(const uci::type::CommPointingRequestEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PointingRequestState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPointingRequestState(uci::type::CommPointingRequestEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PointingVector.
           *
           * @return The acecssor that provides access to the complex content that is identified by PointingVector.
           */
         virtual const uci::type::CommPointingType& getPointingVector() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PointingVector.
           *
           * @return The acecssor that provides access to the complex content that is identified by PointingVector.
           */
         virtual uci::type::CommPointingType& getPointingVector()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PointingVector to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PointingVector
           */
         virtual void setPointingVector(const uci::type::CommPointingType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PointingVector exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PointingVector is emabled or not
           */
         virtual bool hasPointingVector() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PointingVector
           *
           * @param type = uci::type::accessorType::commPointingType This Accessor's accessor type
           */
         virtual void enablePointingVector(uci::base::accessorType::AccessorType type = uci::type::accessorType::commPointingType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PointingVector */
         virtual void clearPointingVector()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommPointingRequestMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommPointingRequestMDT to copy from
           */
         CommPointingRequestMDT(const CommPointingRequestMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommPointingRequestMDT to the contents of the
           * CommPointingRequestMDT on the right hand side (rhs) of the assignment operator.CommPointingRequestMDT [only available
           * to derived classes]
           *
           * @param rhs The CommPointingRequestMDT on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::CommPointingRequestMDT
           * @return A constant reference to this CommPointingRequestMDT.
           */
         const CommPointingRequestMDT& operator=(const CommPointingRequestMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommPointingRequestMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommPointingRequestMDT_h

