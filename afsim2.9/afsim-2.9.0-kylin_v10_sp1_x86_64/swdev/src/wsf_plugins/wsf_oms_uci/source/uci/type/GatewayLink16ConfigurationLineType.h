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
#ifndef Uci__Type__GatewayLink16ConfigurationLineType_h
#define Uci__Type__GatewayLink16ConfigurationLineType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__Point2D_Type_h)
# include "uci/type/Point2D_Type.h"
#endif

#if !defined(Uci__Type__GatewayLink16ConfigurationCompassQuadrantEnum_h)
# include "uci/type/GatewayLink16ConfigurationCompassQuadrantEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Describes a line-based Link 16 filter geographic area */
      class GatewayLink16ConfigurationLineType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~GatewayLink16ConfigurationLineType()
         { }

         /** Returns this accessor's type constant, i.e. GatewayLink16ConfigurationLineType
           *
           * @return This accessor's type constant, i.e. GatewayLink16ConfigurationLineType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::gatewayLink16ConfigurationLineType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const GatewayLink16ConfigurationLineType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Geographic locations of the endpoints of one or more connected line segments. The entire area extending infinitely
           * outward and from both ends of the line will be filtered. [Minimum occurrences: 2] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::Point2D_Type, uci::type::accessorType::point2D_Type> Point;

         /** Returns the bounded list that is identified by the Point.
           *
           * @return The bounded list identified by Point.
           */
         virtual const uci::type::GatewayLink16ConfigurationLineType::Point& getPoint() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Point.
           *
           * @return The bounded list identified by Point.
           */
         virtual uci::type::GatewayLink16ConfigurationLineType::Point& getPoint()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Point.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setPoint(const uci::type::GatewayLink16ConfigurationLineType::Point& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the FilteredNormal.
           *
           * @return The value of the enumeration identified by FilteredNormal.
           */
         virtual const uci::type::GatewayLink16ConfigurationCompassQuadrantEnum& getFilteredNormal() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the FilteredNormal.
           *
           * @return The value of the enumeration identified by FilteredNormal.
           */
         virtual uci::type::GatewayLink16ConfigurationCompassQuadrantEnum& getFilteredNormal()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FilteredNormal.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFilteredNormal(const uci::type::GatewayLink16ConfigurationCompassQuadrantEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FilteredNormal.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFilteredNormal(uci::type::GatewayLink16ConfigurationCompassQuadrantEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         GatewayLink16ConfigurationLineType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The GatewayLink16ConfigurationLineType to copy from
           */
         GatewayLink16ConfigurationLineType(const GatewayLink16ConfigurationLineType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this GatewayLink16ConfigurationLineType to the contents of the
           * GatewayLink16ConfigurationLineType on the right hand side (rhs) of the assignment
           * operator.GatewayLink16ConfigurationLineType [only available to derived classes]
           *
           * @param rhs The GatewayLink16ConfigurationLineType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::GatewayLink16ConfigurationLineType
           * @return A constant reference to this GatewayLink16ConfigurationLineType.
           */
         const GatewayLink16ConfigurationLineType& operator=(const GatewayLink16ConfigurationLineType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // GatewayLink16ConfigurationLineType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__GatewayLink16ConfigurationLineType_h

