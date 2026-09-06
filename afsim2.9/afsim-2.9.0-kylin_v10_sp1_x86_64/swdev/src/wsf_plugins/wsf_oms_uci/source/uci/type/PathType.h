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
#ifndef Uci__Type__PathType_h
#define Uci__Type__PathType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__Point2D_Type_h)
# include "uci/type/Point2D_Type.h"
#endif

#if !defined(Uci__Type__LineProjectionEnum_h)
# include "uci/type/LineProjectionEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the PathType sequence accessor class */
      class PathType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~PathType()
         { }

         /** Returns this accessor's type constant, i.e. PathType
           *
           * @return This accessor's type constant, i.e. PathType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pathType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PathType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a vertex or waypoint along the desired path. Generally, services are encouraged to provide altitude and/or
           * time data whenever it is known. [Minimum occurrences: 2] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::Point2D_Type, uci::type::accessorType::point2D_Type> SegmentVertex;

         /** Returns the bounded list that is identified by the SegmentVertex.
           *
           * @return The bounded list identified by SegmentVertex.
           */
         virtual const uci::type::PathType::SegmentVertex& getSegmentVertex() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SegmentVertex.
           *
           * @return The bounded list identified by SegmentVertex.
           */
         virtual uci::type::PathType::SegmentVertex& getSegmentVertex()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SegmentVertex.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSegmentVertex(const uci::type::PathType::SegmentVertex& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the LineProjection.
           *
           * @return The value of the enumeration identified by LineProjection.
           */
         virtual const uci::type::LineProjectionEnum& getLineProjection() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the LineProjection.
           *
           * @return The value of the enumeration identified by LineProjection.
           */
         virtual uci::type::LineProjectionEnum& getLineProjection()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the LineProjection.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setLineProjection(const uci::type::LineProjectionEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the LineProjection.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setLineProjection(uci::type::LineProjectionEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PathType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PathType to copy from
           */
         PathType(const PathType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PathType to the contents of the PathType on the right hand side
           * (rhs) of the assignment operator.PathType [only available to derived classes]
           *
           * @param rhs The PathType on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::PathType
           * @return A constant reference to this PathType.
           */
         const PathType& operator=(const PathType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PathType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PathType_h

