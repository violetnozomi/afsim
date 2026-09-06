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
#ifndef Uci__Type__ElevationVisibilityRequestMDT_h
#define Uci__Type__ElevationVisibilityRequestMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ElevationRequestPointsType_h)
# include "uci/type/ElevationRequestPointsType.h"
#endif

#if !defined(Uci__Type__Point3D_Type_h)
# include "uci/type/Point3D_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This data contains the request for visibility from a given point (FromPoint) to a list of points (VisibilityPoint);
        * i.e. which points in the list are visible to the FromPoint.
        */
      class ElevationVisibilityRequestMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ElevationVisibilityRequestMDT()
         { }

         /** Returns this accessor's type constant, i.e. ElevationVisibilityRequestMDT
           *
           * @return This accessor's type constant, i.e. ElevationVisibilityRequestMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::elevationVisibilityRequestMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ElevationVisibilityRequestMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the VisibilityRequest.
           *
           * @return The acecssor that provides access to the complex content that is identified by VisibilityRequest.
           */
         virtual const uci::type::ElevationRequestPointsType& getVisibilityRequest() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the VisibilityRequest.
           *
           * @return The acecssor that provides access to the complex content that is identified by VisibilityRequest.
           */
         virtual uci::type::ElevationRequestPointsType& getVisibilityRequest()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the VisibilityRequest to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by VisibilityRequest
           */
         virtual void setVisibilityRequest(const uci::type::ElevationRequestPointsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FromPoint.
           *
           * @return The acecssor that provides access to the complex content that is identified by FromPoint.
           */
         virtual const uci::type::Point3D_Type& getFromPoint() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FromPoint.
           *
           * @return The acecssor that provides access to the complex content that is identified by FromPoint.
           */
         virtual uci::type::Point3D_Type& getFromPoint()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the FromPoint to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by FromPoint
           */
         virtual void setFromPoint(const uci::type::Point3D_Type& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ElevationVisibilityRequestMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ElevationVisibilityRequestMDT to copy from
           */
         ElevationVisibilityRequestMDT(const ElevationVisibilityRequestMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ElevationVisibilityRequestMDT to the contents of the
           * ElevationVisibilityRequestMDT on the right hand side (rhs) of the assignment operator.ElevationVisibilityRequestMDT
           * [only available to derived classes]
           *
           * @param rhs The ElevationVisibilityRequestMDT on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::ElevationVisibilityRequestMDT
           * @return A constant reference to this ElevationVisibilityRequestMDT.
           */
         const ElevationVisibilityRequestMDT& operator=(const ElevationVisibilityRequestMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ElevationVisibilityRequestMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ElevationVisibilityRequestMDT_h

