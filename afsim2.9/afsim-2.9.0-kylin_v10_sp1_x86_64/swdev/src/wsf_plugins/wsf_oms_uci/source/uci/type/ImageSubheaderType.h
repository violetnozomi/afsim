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
#ifndef Uci__Type__ImageSubheaderType_h
#define Uci__Type__ImageSubheaderType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Xs__Type__StringXmlSchemaPrimitives_h)
# include "xs/type/stringXmlSchemaPrimitives.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ImageSubheaderType sequence accessor class */
      class ImageSubheaderType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ImageSubheaderType()
         { }

         /** Returns this accessor's type constant, i.e. ImageSubheaderType
           *
           * @return This accessor's type constant, i.e. ImageSubheaderType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::imageSubheaderType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ImageSubheaderType& accessor)
            throw(uci::base::UCIException) = 0;


         /** [White space: preserve] [Length: 17] [Pattern: "[A-Za-z0-9 \-_]{17}"] */
         typedef xs::String ImageSource;

         /** [White space: preserve] [Length: 3] [Pattern: "[A-Za-z0-9 \-_]{3}"] */
         typedef xs::String PrimaryTargetID;

         /** [Length: 11] [Pattern: "[A-Za-z0-9 \-_]{11}"] */
         typedef xs::String ImageID1;

         /** [White space: preserve] [Length: 9] [Pattern: "[A-Za-z0-9 \-_]{9}"] */
         typedef xs::String ImageID2;

         /** [White space: preserve] [Length: 21] [Pattern: "[A-Za-z0-9 \-_]{21}"] */
         typedef xs::String ICOMn;

         /** Returns the value of the string data type that is identified by the ImageSource.
           *
           * @return The value of the string data type identified by ImageSource.
           */
         virtual const uci::type::ImageSubheaderType::ImageSource& getImageSource() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ImageSource.
           *
           * @return The value of the string data type identified by ImageSource.
           */
         virtual uci::type::ImageSubheaderType::ImageSource& getImageSource()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ImageSource to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setImageSource(const uci::type::ImageSubheaderType::ImageSource& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ImageSource to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setImageSource(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ImageSource to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setImageSource(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ImageSource exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ImageSource is emabled or not
           */
         virtual bool hasImageSource() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ImageSource
           *
           * @param type = uci::type::imageSubheaderType::accessorType::imageSource This Accessor's accessor type
           */
         virtual void enableImageSource(uci::base::accessorType::AccessorType type = uci::type::imageSubheaderType::accessorType::imageSource)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ImageSource */
         virtual void clearImageSource()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the PrimaryTargetID.
           *
           * @return The value of the string data type identified by PrimaryTargetID.
           */
         virtual const uci::type::ImageSubheaderType::PrimaryTargetID& getPrimaryTargetID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the PrimaryTargetID.
           *
           * @return The value of the string data type identified by PrimaryTargetID.
           */
         virtual uci::type::ImageSubheaderType::PrimaryTargetID& getPrimaryTargetID()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the PrimaryTargetID to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setPrimaryTargetID(const uci::type::ImageSubheaderType::PrimaryTargetID& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the PrimaryTargetID to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setPrimaryTargetID(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the PrimaryTargetID to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setPrimaryTargetID(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PrimaryTargetID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PrimaryTargetID is emabled or not
           */
         virtual bool hasPrimaryTargetID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PrimaryTargetID
           *
           * @param type = uci::type::imageSubheaderType::accessorType::primaryTargetID This Accessor's accessor type
           */
         virtual void enablePrimaryTargetID(uci::base::accessorType::AccessorType type = uci::type::imageSubheaderType::accessorType::primaryTargetID)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PrimaryTargetID */
         virtual void clearPrimaryTargetID()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ImageID1.
           *
           * @return The value of the string data type identified by ImageID1.
           */
         virtual const uci::type::ImageSubheaderType::ImageID1& getImageID1() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ImageID1.
           *
           * @return The value of the string data type identified by ImageID1.
           */
         virtual uci::type::ImageSubheaderType::ImageID1& getImageID1()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ImageID1 to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setImageID1(const uci::type::ImageSubheaderType::ImageID1& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ImageID1 to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setImageID1(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ImageID1 to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setImageID1(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ImageID1 exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ImageID1 is emabled or not
           */
         virtual bool hasImageID1() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ImageID1
           *
           * @param type = uci::type::imageSubheaderType::accessorType::imageID1 This Accessor's accessor type
           */
         virtual void enableImageID1(uci::base::accessorType::AccessorType type = uci::type::imageSubheaderType::accessorType::imageID1)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ImageID1 */
         virtual void clearImageID1()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ImageID2.
           *
           * @return The value of the string data type identified by ImageID2.
           */
         virtual const uci::type::ImageSubheaderType::ImageID2& getImageID2() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ImageID2.
           *
           * @return The value of the string data type identified by ImageID2.
           */
         virtual uci::type::ImageSubheaderType::ImageID2& getImageID2()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ImageID2 to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setImageID2(const uci::type::ImageSubheaderType::ImageID2& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ImageID2 to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setImageID2(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ImageID2 to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setImageID2(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ImageID2 exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ImageID2 is emabled or not
           */
         virtual bool hasImageID2() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ImageID2
           *
           * @param type = uci::type::imageSubheaderType::accessorType::imageID2 This Accessor's accessor type
           */
         virtual void enableImageID2(uci::base::accessorType::AccessorType type = uci::type::imageSubheaderType::accessorType::imageID2)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ImageID2 */
         virtual void clearImageID2()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ICOMn.
           *
           * @return The value of the string data type identified by ICOMn.
           */
         virtual const uci::type::ImageSubheaderType::ICOMn& getICOMn() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ICOMn.
           *
           * @return The value of the string data type identified by ICOMn.
           */
         virtual uci::type::ImageSubheaderType::ICOMn& getICOMn()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ICOMn to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setICOMn(const uci::type::ImageSubheaderType::ICOMn& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ICOMn to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setICOMn(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ICOMn to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setICOMn(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ICOMn exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ICOMn is emabled or not
           */
         virtual bool hasICOMn() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ICOMn
           *
           * @param type = uci::type::imageSubheaderType::accessorType::iCOMn This Accessor's accessor type
           */
         virtual void enableICOMn(uci::base::accessorType::AccessorType type = uci::type::imageSubheaderType::accessorType::iCOMn)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ICOMn */
         virtual void clearICOMn()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ImageSubheaderType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ImageSubheaderType to copy from
           */
         ImageSubheaderType(const ImageSubheaderType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ImageSubheaderType to the contents of the ImageSubheaderType on
           * the right hand side (rhs) of the assignment operator.ImageSubheaderType [only available to derived classes]
           *
           * @param rhs The ImageSubheaderType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::ImageSubheaderType
           * @return A constant reference to this ImageSubheaderType.
           */
         const ImageSubheaderType& operator=(const ImageSubheaderType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ImageSubheaderType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ImageSubheaderType_h

