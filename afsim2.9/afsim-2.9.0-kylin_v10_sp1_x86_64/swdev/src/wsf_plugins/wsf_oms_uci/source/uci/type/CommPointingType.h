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
#ifndef Uci__Type__CommPointingType_h
#define Uci__Type__CommPointingType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CommResourceType_h)
# include "uci/type/CommResourceType.h"
#endif

#if !defined(Uci__Type__Point3D_Type_h)
# include "uci/type/Point3D_Type.h"
#endif

#if !defined(Uci__Type__Point4D_Type_h)
# include "uci/type/Point4D_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__PointingVectorParameterType_h)
# include "uci/type/PointingVectorParameterType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** The ID type for UCI IDs that correspond to Pointing plans. */
      class CommPointingType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CommPointingType()
         { }

         /** Returns this accessor's type constant, i.e. CommPointingType
           *
           * @return This accessor's type constant, i.e. CommPointingType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commPointingType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommPointingType& accessor)
            throw(uci::base::UCIException) = 0;


         /** The list of different coverage parameters based on bandwidth/service type. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::PointingVectorParameterType, uci::type::accessorType::pointingVectorParameterType> PointingVectorParameters;

         /** Returns the accessor that provides access to the complex content that is identified by the CommSystem.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommSystem.
           */
         virtual const uci::type::CommResourceType& getCommSystem() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommSystem.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommSystem.
           */
         virtual uci::type::CommResourceType& getCommSystem()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CommSystem to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CommSystem
           */
         virtual void setCommSystem(const uci::type::CommResourceType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CommSystem exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CommSystem is emabled or not
           */
         virtual bool hasCommSystem() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CommSystem
           *
           * @param type = uci::type::accessorType::commResourceType This Accessor's accessor type
           */
         virtual void enableCommSystem(uci::base::accessorType::AccessorType type = uci::type::accessorType::commResourceType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CommSystem */
         virtual void clearCommSystem()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommSystemPosition.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommSystemPosition.
           */
         virtual const uci::type::Point3D_Type& getCommSystemPosition() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommSystemPosition.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommSystemPosition.
           */
         virtual uci::type::Point3D_Type& getCommSystemPosition()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CommSystemPosition to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CommSystemPosition
           */
         virtual void setCommSystemPosition(const uci::type::Point3D_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CommSystemPosition exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CommSystemPosition is emabled or not
           */
         virtual bool hasCommSystemPosition() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CommSystemPosition
           *
           * @param type = uci::type::accessorType::point3D_Type This Accessor's accessor type
           */
         virtual void enableCommSystemPosition(uci::base::accessorType::AccessorType type = uci::type::accessorType::point3D_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CommSystemPosition */
         virtual void clearCommSystemPosition()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PointingVectorCenterpoint.
           *
           * @return The acecssor that provides access to the complex content that is identified by PointingVectorCenterpoint.
           */
         virtual const uci::type::Point4D_Type& getPointingVectorCenterpoint() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PointingVectorCenterpoint.
           *
           * @return The acecssor that provides access to the complex content that is identified by PointingVectorCenterpoint.
           */
         virtual uci::type::Point4D_Type& getPointingVectorCenterpoint()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PointingVectorCenterpoint to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PointingVectorCenterpoint
           */
         virtual void setPointingVectorCenterpoint(const uci::type::Point4D_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the PointingVectorParameters.
           *
           * @return The bounded list identified by PointingVectorParameters.
           */
         virtual const uci::type::CommPointingType::PointingVectorParameters& getPointingVectorParameters() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the PointingVectorParameters.
           *
           * @return The bounded list identified by PointingVectorParameters.
           */
         virtual uci::type::CommPointingType::PointingVectorParameters& getPointingVectorParameters()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the PointingVectorParameters.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setPointingVectorParameters(const uci::type::CommPointingType::PointingVectorParameters& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommPointingType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommPointingType to copy from
           */
         CommPointingType(const CommPointingType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommPointingType to the contents of the CommPointingType on the
           * right hand side (rhs) of the assignment operator.CommPointingType [only available to derived classes]
           *
           * @param rhs The CommPointingType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::CommPointingType
           * @return A constant reference to this CommPointingType.
           */
         const CommPointingType& operator=(const CommPointingType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommPointingType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommPointingType_h

