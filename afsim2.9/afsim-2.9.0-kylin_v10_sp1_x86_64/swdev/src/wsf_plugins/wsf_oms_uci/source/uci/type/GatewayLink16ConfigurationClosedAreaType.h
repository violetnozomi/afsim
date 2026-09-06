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
#ifndef Uci__Type__GatewayLink16ConfigurationClosedAreaType_h
#define Uci__Type__GatewayLink16ConfigurationClosedAreaType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__Point2D_Type_h)
# include "uci/type/Point2D_Type.h"
#endif

#if !defined(Uci__Type__GatewayLink16ConfigurationSideEnum_h)
# include "uci/type/GatewayLink16ConfigurationSideEnum.h"
#endif

#if !defined(Uci__Type__GatewayLink16ConfigurationMotionType_h)
# include "uci/type/GatewayLink16ConfigurationMotionType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Describes common fields for closed Link 16 filter geographic area types */
      class GatewayLink16ConfigurationClosedAreaType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~GatewayLink16ConfigurationClosedAreaType()
         { }

         /** Returns this accessor's type constant, i.e. GatewayLink16ConfigurationClosedAreaType
           *
           * @return This accessor's type constant, i.e. GatewayLink16ConfigurationClosedAreaType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::gatewayLink16ConfigurationClosedAreaType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const GatewayLink16ConfigurationClosedAreaType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CenterPoint.
           *
           * @return The acecssor that provides access to the complex content that is identified by CenterPoint.
           */
         virtual const uci::type::Point2D_Type& getCenterPoint() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CenterPoint.
           *
           * @return The acecssor that provides access to the complex content that is identified by CenterPoint.
           */
         virtual uci::type::Point2D_Type& getCenterPoint()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CenterPoint to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CenterPoint
           */
         virtual void setCenterPoint(const uci::type::Point2D_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the FilteredSide.
           *
           * @return The value of the enumeration identified by FilteredSide.
           */
         virtual const uci::type::GatewayLink16ConfigurationSideEnum& getFilteredSide() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the FilteredSide.
           *
           * @return The value of the enumeration identified by FilteredSide.
           */
         virtual uci::type::GatewayLink16ConfigurationSideEnum& getFilteredSide()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FilteredSide.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFilteredSide(const uci::type::GatewayLink16ConfigurationSideEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FilteredSide.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFilteredSide(uci::type::GatewayLink16ConfigurationSideEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Motion.
           *
           * @return The acecssor that provides access to the complex content that is identified by Motion.
           */
         virtual const uci::type::GatewayLink16ConfigurationMotionType& getMotion() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Motion.
           *
           * @return The acecssor that provides access to the complex content that is identified by Motion.
           */
         virtual uci::type::GatewayLink16ConfigurationMotionType& getMotion()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Motion to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Motion
           */
         virtual void setMotion(const uci::type::GatewayLink16ConfigurationMotionType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Motion exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Motion is emabled or not
           */
         virtual bool hasMotion() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Motion
           *
           * @param type = uci::type::accessorType::gatewayLink16ConfigurationMotionType This Accessor's accessor type
           */
         virtual void enableMotion(uci::base::accessorType::AccessorType type = uci::type::accessorType::gatewayLink16ConfigurationMotionType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Motion */
         virtual void clearMotion()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         GatewayLink16ConfigurationClosedAreaType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The GatewayLink16ConfigurationClosedAreaType to copy from
           */
         GatewayLink16ConfigurationClosedAreaType(const GatewayLink16ConfigurationClosedAreaType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this GatewayLink16ConfigurationClosedAreaType to the contents of the
           * GatewayLink16ConfigurationClosedAreaType on the right hand side (rhs) of the assignment
           * operator.GatewayLink16ConfigurationClosedAreaType [only available to derived classes]
           *
           * @param rhs The GatewayLink16ConfigurationClosedAreaType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::GatewayLink16ConfigurationClosedAreaType
           * @return A constant reference to this GatewayLink16ConfigurationClosedAreaType.
           */
         const GatewayLink16ConfigurationClosedAreaType& operator=(const GatewayLink16ConfigurationClosedAreaType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // GatewayLink16ConfigurationClosedAreaType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__GatewayLink16ConfigurationClosedAreaType_h

