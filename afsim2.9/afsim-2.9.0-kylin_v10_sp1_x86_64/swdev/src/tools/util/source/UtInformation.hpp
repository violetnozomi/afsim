// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef UTINFORMATION_HPP
#define UTINFORMATION_HPP

#include "ut_export.h"

#include <string>

#include "UtAttribute.hpp"

template<typename T>
class UtInformation;

// typedefs
using UtIntInformation    = UtInformation<int>;
using UtBoolInformation   = UtInformation<bool>;
using UtDoubleInformation = UtInformation<double>;
using UtStringInformation = UtInformation<std::string>;

//! UtInformation represents an atomic piece of information.
//! This information can be of any type/class/structure, so
//! this class is implemented using generics.  Each piece
//! of information contains the data itself, a validity flag,
//! and either a quality or error value.  This value quantifies
//! how certain one is about the information.
//! Information is meant to be combined, filtered, fused, and to
//! provide the basis for inference.
template<typename T>
class UtInformation : public UtAttribute<T>
{
public:
   UtInformation()
      : UtAttribute<T>()
      , mIsValid(false)
      , mHasError(false)
      , mQuality(1.0)
   {
   }

   explicit UtInformation(UtStringId aName)
      : UtAttribute<T>(aName)
      , mIsValid(false)
      , mHasError(false)
      , mQuality(1.0)
   {
   }

   UtInformation(UtStringId aName, const T& aInitialValue)
      : UtAttribute<T>(aName, aInitialValue)
      , mIsValid(false)
      , mHasError(false)
      , mQuality(1.0)
   {
   }

   explicit UtInformation(int aNameId)
      : UtAttribute<T>(aNameId)
      , mIsValid(false)
      , mHasError(false)
      , mQuality(1.0)
   {
   }

   UtInformation(const UtInformation& aSrc)
      : UtAttribute<T>(aSrc)
      , mIsValid(aSrc.mIsValid)
      , mHasError(aSrc.mHasError)
      , mQuality(aSrc.mQuality)
   {
   }

   //! Make a copy of this attribute.
   //! Instead of calling this directly, users should use ut::clone.
   UtInformation<T>* Clone() const override { return new UtInformation<T>(*this); }

   UtInformation<T>& operator=(const UtInformation<T>& aRhs)
   {
      if (this != &aRhs)
      {
         this->mAttribute = aRhs.mAttribute;
         mIsValid         = aRhs.mIsValid;
         mQuality         = aRhs.mQuality;
         mHasError        = aRhs.mHasError;
      }
      return *this;
   }

   ~UtInformation() override = default;

   //! Return the "error" of the information.
   double GetError() const
   {
      if (!mHasError)
      {
         return 0.0;
      }
      else
      {
         return mError;
      }
   }

   //! Return whether this piece of information has error.
   //! @note Information either has error or quality, not both.
   bool HasError() const { return mHasError; }

   //! Return whether this piece of information has quality.
   //! @note Information either has error or quality, not both.
   bool HasQuality() const { return !mHasError; }

   //! Return the "quality" of the information.
   virtual double GetQuality() const
   {
      if (mHasError)
      {
         return 1.0;
      }
      else
      {
         return mQuality;
      }
   }

   //! Set the error of the information.
   void SetError(double aError)
   {
      mError    = aError;
      mHasError = true;
   }

   //! Set the quality of the information.
   void SetQuality(double aQuality)
   {
      mQuality  = aQuality;
      mHasError = false;
   }

   using UtAttribute<T>::Set;

   //! Set the value and the error.
   void Set(const T& aValue, double aError)
   {
      Set(aValue);
      SetError(aError);
   }

   //! Is the information valid?
   virtual bool IsValid() const { return mIsValid; }

   //! Set the validity
   void SetValid(bool aIsValid) { mIsValid = aIsValid; }

   void SetAndValidate(const T& aValue)
   {
      Set(aValue);
      mIsValid = true;
   }

   //! Clear and invalidate the information
   void Reset()
   {
      this->mAttribute = T();
      mIsValid         = false;
      mQuality         = 0.0;
   }

   //! For (de)serialization.
   template<typename Q>
   void Serialize(Q& aBuff)
   {
      aBuff& this->mAttribute;
      aBuff& mIsValid;
      aBuff& mQuality;
      aBuff& mHasError;
   }

private:
   // add validity and quality
   bool mIsValid;
   bool mHasError;

   union
   {
      double mQuality;
      double mError;
   };
};

//! Now declare a partially-specialized template for pointer types
template<typename T>
class UtInformation<T*> : public UtAttribute<T*>
{
public:
   UtInformation()
      : UtAttribute<T*>()
      , mIsValid(false)
      , mHasError(false)
      , mQuality(0.0)
   {
      this->mAttribute = nullptr;
   }

   UtInformation(const UtInformation& aSrc)
      : UtAttribute<T*>()
      , mIsValid(aSrc.mIsValid)
      , mHasError(false)
      , mQuality(aSrc.mQuality)
   {
      if (aSrc.mAttribute != nullptr)
      {
         this->mAttribute = new T(*aSrc.mAttribute);
      }
      else
      {
         this->mAttribute = nullptr;
      }
   }

   UtInformation(const std::string& aName, const T& aInitialValue)
      : UtAttribute<T*>()
      , mIsValid(false)
      , mHasError(false)
      , mQuality(0.0)
   {
      this->mAttribute = new T(aName, aInitialValue);
   }

   explicit UtInformation(int aNameId)
      : UtAttribute<T*>()
   {
      this->mAttribute = new T(aNameId);
   }

   //! Store the given value in this attribute.
   // Assume a valid pointer
   void Set(const T& aValue)
   {
      if (this->mAttribute != nullptr)
      {
         delete this->mAttribute;
      }
      this->mAttribute = new T(aValue);
      mIsValid         = true;
   }

   void Set(const UtInformation<T*>& aSrc)
   {
      if (aSrc.mAttribute != nullptr)
      {
         Set(*aSrc.mAttribute);
         mIsValid = aSrc.mIsValid;
         mError   = aSrc.mError;
      }
   }

   //! Make a copy of this attribute.
   //! Instead of calling this directly, users should use ut::clone.
   UtInformation<T*>* Clone() const override { return new UtInformation<T*>(*this); }

   ~UtInformation() override { delete this->mAttribute; }

   void CopyFrom(const UtInformation<T*>& aSrc)
   {
      if (this->mAttribute != nullptr)
      {
         Reset();
      }
      if (aSrc.mAttribute != nullptr)
      {
         this->mAttribute = new T(*aSrc.mAttribute);
      }
      mIsValid = aSrc.mIsValid;
      mQuality = aSrc.mQuality;
   }

   //! Clear and invalidate the information
   //! same as delete
   void Reset()
   {
      if (this->mAttribute != nullptr)
      {
         delete this->mAttribute; // This is a pointer
         this->mAttribute = nullptr;
      }
      mIsValid = false;
      mQuality = 0.0;
   }

   //! For (de)serialization.
   template<typename Q>
   void Serialize(Q& aBuff)
   {
      aBuff& this->mAttribute;
      aBuff& mIsValid;
      aBuff& mQuality;
      aBuff& mHasError;
   }

   //! Return the "error" of the information.
   double GetError() const { return mError; }

   bool HasError() const { return mHasError; }

   double GetQuality() const { return mQuality; }

   //! Set the error of the information.
   void SetError(double aError)
   {
      mError    = aError;
      mHasError = true;
   }

   //! Set the quality of the information.
   void SetQuality(double aQuality)
   {
      mQuality  = aQuality;
      mHasError = false;
   }

   //! Set the validity
   void SetValid(bool aIsValid)
   {
      // todo-mjm assert((mAttribute != nullptr) || (! aIsValid));
      mIsValid = aIsValid;
   }

   //! Is the information valid?
   virtual bool IsValid() const { return mIsValid; }

   void SetAndValidate(const T*& aValue)
   {
      assert(aValue != 0);
      Set(aValue);
      mIsValid = true;
   }

   void SetAndValidate(const T& aValue)
   {
      Set(aValue);
      mIsValid = true;
   }

private:
   // add validity and quality
   bool mIsValid;
   bool mHasError;

   union
   {
      double mQuality;
      double mError;
   };
};

#endif
