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
#ifndef Uci__Type__RunwayCoordinatesType_h
#define Uci__Type__RunwayCoordinatesType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__Point3D_Type_h)
# include "uci/type/Point3D_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Specifies runway-specific coordinates. These coordinates are specific to either takeoffs or landings as identified in
        * RunwayMDT.
        */
      class RunwayCoordinatesType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~RunwayCoordinatesType()
         { }

         /** Returns this accessor's type constant, i.e. RunwayCoordinatesType
           *
           * @return This accessor's type constant, i.e. RunwayCoordinatesType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::runwayCoordinatesType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RunwayCoordinatesType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Start.
           *
           * @return The acecssor that provides access to the complex content that is identified by Start.
           */
         virtual const uci::type::Point3D_Type& getStart() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Start.
           *
           * @return The acecssor that provides access to the complex content that is identified by Start.
           */
         virtual uci::type::Point3D_Type& getStart()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Start to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Start
           */
         virtual void setStart(const uci::type::Point3D_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Threshold.
           *
           * @return The acecssor that provides access to the complex content that is identified by Threshold.
           */
         virtual const uci::type::Point3D_Type& getThreshold() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Threshold.
           *
           * @return The acecssor that provides access to the complex content that is identified by Threshold.
           */
         virtual uci::type::Point3D_Type& getThreshold()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Threshold to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Threshold
           */
         virtual void setThreshold(const uci::type::Point3D_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Threshold exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Threshold is emabled or not
           */
         virtual bool hasThreshold() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Threshold
           *
           * @param type = uci::type::accessorType::point3D_Type This Accessor's accessor type
           */
         virtual void enableThreshold(uci::base::accessorType::AccessorType type = uci::type::accessorType::point3D_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Threshold */
         virtual void clearThreshold()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Limit.
           *
           * @return The acecssor that provides access to the complex content that is identified by Limit.
           */
         virtual const uci::type::Point3D_Type& getLimit() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Limit.
           *
           * @return The acecssor that provides access to the complex content that is identified by Limit.
           */
         virtual uci::type::Point3D_Type& getLimit()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Limit to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Limit
           */
         virtual void setLimit(const uci::type::Point3D_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Limit exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Limit is emabled or not
           */
         virtual bool hasLimit() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Limit
           *
           * @param type = uci::type::accessorType::point3D_Type This Accessor's accessor type
           */
         virtual void enableLimit(uci::base::accessorType::AccessorType type = uci::type::accessorType::point3D_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Limit */
         virtual void clearLimit()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RunwayCoordinatesType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RunwayCoordinatesType to copy from
           */
         RunwayCoordinatesType(const RunwayCoordinatesType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RunwayCoordinatesType to the contents of the RunwayCoordinatesType
           * on the right hand side (rhs) of the assignment operator.RunwayCoordinatesType [only available to derived classes]
           *
           * @param rhs The RunwayCoordinatesType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::RunwayCoordinatesType
           * @return A constant reference to this RunwayCoordinatesType.
           */
         const RunwayCoordinatesType& operator=(const RunwayCoordinatesType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RunwayCoordinatesType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RunwayCoordinatesType_h

