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
#ifndef Uci__Type__Point2D_ReportedType_h
#define Uci__Type__Point2D_ReportedType_h 1

#if !defined(Uci__Type__Point2D_Type_h)
# include "uci/type/Point2D_Type.h"
#endif

#if !defined(Uci__Type__AltitudeAdjustmentType_h)
# include "uci/type/AltitudeAdjustmentType.h"
#endif

#if !defined(Uci__Type__AltitudeSourceEnum_h)
# include "uci/type/AltitudeSourceEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the Point2D_ReportedType sequence accessor class */
      class Point2D_ReportedType : public virtual uci::type::Point2D_Type {
      public:

         /** The destructor */
         virtual ~Point2D_ReportedType()
         { }

         /** Returns this accessor's type constant, i.e. Point2D_ReportedType
           *
           * @return This accessor's type constant, i.e. Point2D_ReportedType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::point2D_ReportedType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const Point2D_ReportedType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the HAE_Adjustment.
           *
           * @return The value of the simple primitive data type identified by HAE_Adjustment.
           */
         virtual uci::type::AltitudeAdjustmentTypeValue getHAE_Adjustment() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the HAE_Adjustment.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setHAE_Adjustment(uci::type::AltitudeAdjustmentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by HAE_Adjustment exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by HAE_Adjustment is emabled or not
           */
         virtual bool hasHAE_Adjustment() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by HAE_Adjustment
           *
           * @param type = uci::type::accessorType::altitudeAdjustmentType This Accessor's accessor type
           */
         virtual void enableHAE_Adjustment(uci::base::accessorType::AccessorType type = uci::type::accessorType::altitudeAdjustmentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by HAE_Adjustment */
         virtual void clearHAE_Adjustment()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the AltitudeSource.
           *
           * @return The value of the enumeration identified by AltitudeSource.
           */
         virtual const uci::type::AltitudeSourceEnum& getAltitudeSource() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the AltitudeSource.
           *
           * @return The value of the enumeration identified by AltitudeSource.
           */
         virtual uci::type::AltitudeSourceEnum& getAltitudeSource()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the AltitudeSource.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setAltitudeSource(const uci::type::AltitudeSourceEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the AltitudeSource.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setAltitudeSource(uci::type::AltitudeSourceEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield AltitudeSourceis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasAltitudeSource() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getAltitudeSource will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::altitudeSourceEnum This Accessor's accessor type
           */
         virtual void enableAltitudeSource(uci::base::accessorType::AccessorType type = uci::type::accessorType::altitudeSourceEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearAltitudeSource()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         Point2D_ReportedType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The Point2D_ReportedType to copy from
           */
         Point2D_ReportedType(const Point2D_ReportedType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this Point2D_ReportedType to the contents of the Point2D_ReportedType
           * on the right hand side (rhs) of the assignment operator.Point2D_ReportedType [only available to derived classes]
           *
           * @param rhs The Point2D_ReportedType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::Point2D_ReportedType
           * @return A constant reference to this Point2D_ReportedType.
           */
         const Point2D_ReportedType& operator=(const Point2D_ReportedType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // Point2D_ReportedType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__Point2D_ReportedType_h

