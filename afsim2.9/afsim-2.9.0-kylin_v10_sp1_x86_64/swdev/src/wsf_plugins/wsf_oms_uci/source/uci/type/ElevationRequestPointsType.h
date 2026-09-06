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
#ifndef Uci__Type__ElevationRequestPointsType_h
#define Uci__Type__ElevationRequestPointsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ElevationRequestID_Type_h)
# include "uci/type/ElevationRequestID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__Point2D_Type_h)
# include "uci/type/Point2D_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Data to contain the RequestID and a list of points for elevation in ElevationRequest message or for visibility
        * information in the ElevationVisibilityRequest message. This data is reused for the return message
        * ElevationRequestStatus and should have the optional Altitude element filled unless there is no elevation data
        * available in which case the field would remain empty.
        */
      class ElevationRequestPointsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ElevationRequestPointsType()
         { }

         /** Returns this accessor's type constant, i.e. ElevationRequestPointsType
           *
           * @return This accessor's type constant, i.e. ElevationRequestPointsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::elevationRequestPointsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ElevationRequestPointsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** List of points for which elevation data is needed. For ElevationRequestStatus, the Altitude element will be empty
           * when elevation data is unavailable at the requested location. For ElevationVisibilityRequest the Altitude element
           * could be filled in to check visibility at that height above ground level [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::Point2D_Type, uci::type::accessorType::point2D_Type> RequestPoint;

         /** Returns the accessor that provides access to the complex content that is identified by the RequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by RequestID.
           */
         virtual const uci::type::ElevationRequestID_Type& getRequestID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by RequestID.
           */
         virtual uci::type::ElevationRequestID_Type& getRequestID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RequestID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RequestID
           */
         virtual void setRequestID(const uci::type::ElevationRequestID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RequestPoint.
           *
           * @return The bounded list identified by RequestPoint.
           */
         virtual const uci::type::ElevationRequestPointsType::RequestPoint& getRequestPoint() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RequestPoint.
           *
           * @return The bounded list identified by RequestPoint.
           */
         virtual uci::type::ElevationRequestPointsType::RequestPoint& getRequestPoint()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the RequestPoint.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setRequestPoint(const uci::type::ElevationRequestPointsType::RequestPoint& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ElevationRequestPointsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ElevationRequestPointsType to copy from
           */
         ElevationRequestPointsType(const ElevationRequestPointsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ElevationRequestPointsType to the contents of the
           * ElevationRequestPointsType on the right hand side (rhs) of the assignment operator.ElevationRequestPointsType [only
           * available to derived classes]
           *
           * @param rhs The ElevationRequestPointsType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::ElevationRequestPointsType
           * @return A constant reference to this ElevationRequestPointsType.
           */
         const ElevationRequestPointsType& operator=(const ElevationRequestPointsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ElevationRequestPointsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ElevationRequestPointsType_h

