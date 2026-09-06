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
#ifndef Uci__Type__EOB_IdentityBaseType_h
#define Uci__Type__EOB_IdentityBaseType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__EOB_SitePIN_Type_h)
# include "uci/type/EOB_SitePIN_Type.h"
#endif

#if !defined(Uci__Type__EOB_SiteNameType_h)
# include "uci/type/EOB_SiteNameType.h"
#endif

#if !defined(Uci__Type__ID_Type_h)
# include "uci/type/ID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EOB_IdentityBaseType sequence accessor class */
      class EOB_IdentityBaseType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EOB_IdentityBaseType()
         { }

         /** Returns this accessor's type constant, i.e. EOB_IdentityBaseType
           *
           * @return This accessor's type constant, i.e. EOB_IdentityBaseType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::eOB_IdentityBaseType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EOB_IdentityBaseType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SitePIN.
           *
           * @return The acecssor that provides access to the complex content that is identified by SitePIN.
           */
         virtual const uci::type::EOB_SitePIN_Type& getSitePIN() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SitePIN.
           *
           * @return The acecssor that provides access to the complex content that is identified by SitePIN.
           */
         virtual uci::type::EOB_SitePIN_Type& getSitePIN()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SitePIN to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SitePIN
           */
         virtual void setSitePIN(const uci::type::EOB_SitePIN_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SitePIN exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SitePIN is emabled or not
           */
         virtual bool hasSitePIN() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SitePIN
           *
           * @param type = uci::type::accessorType::eOB_SitePIN_Type This Accessor's accessor type
           */
         virtual void enableSitePIN(uci::base::accessorType::AccessorType type = uci::type::accessorType::eOB_SitePIN_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SitePIN */
         virtual void clearSitePIN()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the SiteName.
           *
           * @return The value of the string data type identified by SiteName.
           */
         virtual const uci::type::EOB_SiteNameType& getSiteName() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the SiteName.
           *
           * @return The value of the string data type identified by SiteName.
           */
         virtual uci::type::EOB_SiteNameType& getSiteName()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SiteName to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setSiteName(const uci::type::EOB_SiteNameType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SiteName to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSiteName(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SiteName to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSiteName(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SiteName exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SiteName is emabled or not
           */
         virtual bool hasSiteName() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SiteName
           *
           * @param type = uci::type::accessorType::eOB_SiteNameType This Accessor's accessor type
           */
         virtual void enableSiteName(uci::base::accessorType::AccessorType type = uci::type::accessorType::eOB_SiteNameType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SiteName */
         virtual void clearSiteName()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the InternalID.
           *
           * @return The acecssor that provides access to the complex content that is identified by InternalID.
           */
         virtual const uci::type::ID_Type& getInternalID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the InternalID.
           *
           * @return The acecssor that provides access to the complex content that is identified by InternalID.
           */
         virtual uci::type::ID_Type& getInternalID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the InternalID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by InternalID
           */
         virtual void setInternalID(const uci::type::ID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by InternalID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by InternalID is emabled or not
           */
         virtual bool hasInternalID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by InternalID
           *
           * @param type = uci::type::accessorType::iD_Type This Accessor's accessor type
           */
         virtual void enableInternalID(uci::base::accessorType::AccessorType type = uci::type::accessorType::iD_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by InternalID */
         virtual void clearInternalID()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EOB_IdentityBaseType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EOB_IdentityBaseType to copy from
           */
         EOB_IdentityBaseType(const EOB_IdentityBaseType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EOB_IdentityBaseType to the contents of the EOB_IdentityBaseType
           * on the right hand side (rhs) of the assignment operator.EOB_IdentityBaseType [only available to derived classes]
           *
           * @param rhs The EOB_IdentityBaseType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::EOB_IdentityBaseType
           * @return A constant reference to this EOB_IdentityBaseType.
           */
         const EOB_IdentityBaseType& operator=(const EOB_IdentityBaseType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EOB_IdentityBaseType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EOB_IdentityBaseType_h

