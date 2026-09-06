// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCYBERATTACKPARAMETERS_HPP
#define WSFCYBERATTACKPARAMETERS_HPP

#include "wsf_cyber_export.h"

#include <unordered_map>
#include <vector>

#include "UtScriptAccessible.hpp"
#include "UtScriptClass.hpp"
#include "WsfAuxDataEnabled.hpp"
#include "script/WsfScriptAuxDataUtil.hpp"

namespace wsf
{
namespace cyber
{

//! This class is a simple wrapper for an AUX data container.
class WSF_CYBER_EXPORT AttackParameterObject : public UtScriptAccessible, public WsfAuxDataEnabled
{
public:
   AttackParameterObject() = default;
   explicit AttackParameterObject(bool aScriptUsage)
      : mScriptParameter(aScriptUsage)
   {
   }
   ~AttackParameterObject() override = default;

   virtual AttackParameterObject* Clone() const { return new AttackParameterObject(*this); }
   const char*                    GetScriptClassName() const override { return "WsfCyberAttackParameterObject"; }

   //! A wrapper for the underlying AuxData.Get() method that provides
   //! a parameter associated with an effect name (or any string), and
   //! handles the provision of that data regardless if it was originated
   //! in the script context or otherwise.
   template<class T>
   void GetParameter(const std::string& aEffectName, T*& aParameter) const
   {
      if (mScriptParameter)
      {
         // Data was provided via usage in the script context. Unpack as UtScriptData.
         UtScriptData data;
         GetAuxDataConst().Get(aEffectName, data);
         aParameter = data.GetPointer()->GetAppObject<T>();
      }
      else
      {
         // Data was provided programmatically, and can be unpacked directly.
         GetAuxDataConst().Get(aEffectName, aParameter);
      }
   }

private:
   // The origin of the parameter is important, as data provided in the script context
   // will have another level of indirection as a UtScriptData object. This value is
   // set to true when this parameter is constructed in the script context, and subsequent
   // usages at retrieving the data stored will require unpacking the UtScriptData object.
   // This is unfortunately required since UtAttribute get methods throw exceptions upon
   // failure to cast to a particular type instead of simply returning a nullptr.
   bool mScriptParameter{false};
};

class WSF_CYBER_EXPORT ScriptAttackParameterObjectClass : public UtScriptClass
{
public:
   ScriptAttackParameterObjectClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);
   ~ScriptAttackParameterObjectClass() override = default;

   void* Create(const UtScriptContext& aInstance) override;
   void* Clone(void* aObjectPtr) override;
   void  Destroy(void* aObjectPtr) override;

   UT_DECLARE_AUX_DATA_SCRIPT_METHODS()
};

//! The AttackParameters class is a container for various variables provided with
//! a cyber attack. These variables are intended for use with the effects associated
//! with an attack. Because cyber attacks may instantiate multiple effects, this
//! container associates each set of parameters with an effect. Thus, at the time
//! that an effect is instantiated during an attack, any required variables necessary
//! for that effect can be retrieved from this container.
//!
//! Entries for each effect in this container are wrapped aux data. Aux data is always
//! provided a string identifier. In this class, that identifier must correspond to
//! the name of the effect the data is associated with.
class WSF_CYBER_EXPORT AttackParameters : public UtScriptAccessible
{
public:
   using ParameterMap = std::unordered_map<std::string, std::vector<AttackParameterObject>>;

   AttackParameters()           = default;
   ~AttackParameters() override = default;

   virtual AttackParameters* Clone() const { return new AttackParameters(*this); }
   const char*               GetScriptClassName() const override { return "WsfCyberAttackParameters"; }

   std::vector<std::string>                  GetEffectNames() const;
   const std::vector<AttackParameterObject>* GetEffectEntries(const std::string& aEffectName) const;
   size_t                                    GetEffectEntryCount(const std::string& aEffectName) const;
   AttackParameterObject*                    GetEffectEntryByIndex(const std::string& aEffectName, size_t aIndex);

   //! Adds a parameter. Each parameter is a wrapped aux-data object (AttackParameterObject)
   //! that consists of a SINGLE variable associated with a string name corresponding to
   //! the effect name the parameter is valid for.
   //!
   //! Any AttackParameterObject that contains multiple variables are ignored. If multiple
   //! parameters are required, they should be added individually. Objects (such as structs,
   //! maps, or other AFSIM objects) are considered to be a single parameter (see how aux
   //! is implemented for details)
   //!
   //! In addition, any effect that requires more than one user supplied variable must
   //! be packed in the exact order that such data is expected to be consumed.
   void AddParameter(const AttackParameterObject& aParameter);

private:
   ParameterMap mMap{};
};

class WSF_CYBER_EXPORT ScriptAttackParametersClass : public UtScriptClass
{
public:
   ScriptAttackParametersClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);
   ~ScriptAttackParametersClass() override = default;

   void* Create(const UtScriptContext& aInstance) override;
   void* Clone(void* aObjectPtr) override;
   void  Destroy(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(GetEffectNames);
   UT_DECLARE_SCRIPT_METHOD(GetEffectEntryCount);
   UT_DECLARE_SCRIPT_METHOD(GetEffectEntryByIndex);
   UT_DECLARE_SCRIPT_METHOD(AddParameter);
};

} // namespace cyber
} // namespace wsf

#endif
