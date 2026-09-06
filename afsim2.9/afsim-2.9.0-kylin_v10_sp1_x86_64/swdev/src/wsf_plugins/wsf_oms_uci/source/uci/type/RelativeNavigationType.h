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
#ifndef Uci__Type__RelativeNavigationType_h
#define Uci__Type__RelativeNavigationType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__Point3D_Type_h)
# include "uci/type/Point3D_Type.h"
#endif

#if !defined(Uci__Type__DistanceType_h)
# include "uci/type/DistanceType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the RelativeNavigationType sequence accessor class */
      class RelativeNavigationType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~RelativeNavigationType()
         { }

         /** Returns this accessor's type constant, i.e. RelativeNavigationType
           *
           * @return This accessor's type constant, i.e. RelativeNavigationType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::relativeNavigationType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RelativeNavigationType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RelativePoint.
           *
           * @return The acecssor that provides access to the complex content that is identified by RelativePoint.
           */
         virtual const uci::type::Point3D_Type& getRelativePoint() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RelativePoint.
           *
           * @return The acecssor that provides access to the complex content that is identified by RelativePoint.
           */
         virtual uci::type::Point3D_Type& getRelativePoint()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RelativePoint to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RelativePoint
           */
         virtual void setRelativePoint(const uci::type::Point3D_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the OffsetNorth.
           *
           * @return The value of the simple primitive data type identified by OffsetNorth.
           */
         virtual uci::type::DistanceTypeValue getOffsetNorth() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the OffsetNorth.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setOffsetNorth(uci::type::DistanceTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the OffsetEast.
           *
           * @return The value of the simple primitive data type identified by OffsetEast.
           */
         virtual uci::type::DistanceTypeValue getOffsetEast() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the OffsetEast.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setOffsetEast(uci::type::DistanceTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the OffsetDown.
           *
           * @return The value of the simple primitive data type identified by OffsetDown.
           */
         virtual uci::type::DistanceTypeValue getOffsetDown() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the OffsetDown.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setOffsetDown(uci::type::DistanceTypeValue value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RelativeNavigationType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RelativeNavigationType to copy from
           */
         RelativeNavigationType(const RelativeNavigationType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RelativeNavigationType to the contents of the
           * RelativeNavigationType on the right hand side (rhs) of the assignment operator.RelativeNavigationType [only available
           * to derived classes]
           *
           * @param rhs The RelativeNavigationType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::RelativeNavigationType
           * @return A constant reference to this RelativeNavigationType.
           */
         const RelativeNavigationType& operator=(const RelativeNavigationType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RelativeNavigationType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RelativeNavigationType_h

