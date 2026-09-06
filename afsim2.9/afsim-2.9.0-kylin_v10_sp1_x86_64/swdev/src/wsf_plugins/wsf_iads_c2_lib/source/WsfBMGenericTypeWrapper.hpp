// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// Classification: UNCLASSIFIED

#ifndef WSF_BM_GENERIC_TYPE_WRAPPER_HPP
#define WSF_BM_GENERIC_TYPE_WRAPPER_HPP

#include <memory>

#include <WsfBMCommon.hpp>
#include <WsfObject.hpp>

// This class is a generic data wrapper for bridging between AFSIM and the core data structures.
// It is not meant to be used alone, see the managed and unmanaged specializations below.
template <typename tWrappedType, typename tPointerType>
class WsfBMTypeWrapper : public WsfObject
{
   public:
      typedef tPointerType tType;

      WsfBMTypeWrapper(const std::string& script_class_name)
         : m_script_class_name(script_class_name)
      {}

      WsfBMTypeWrapper(const std::string& script_class_name, tPointerType data_to_wrap)
         : m_script_class_name(script_class_name)
         , m_wrapped_ptr(data_to_wrap)
      {}

      virtual WsfBMTypeWrapper* Clone() const
      {
         return new WsfBMTypeWrapper(*this);
      }

      virtual const char* GetScriptClassName() const { return m_script_class_name.c_str(); }
      virtual tType GetRawPtr() const { return m_wrapped_ptr; }

      virtual bool IsValidRef() const { return m_wrapped_ptr ? true : false; }

   protected:
      std::string m_script_class_name;
      tPointerType m_wrapped_ptr;
};

// Use this type when the data is truly shared and maintained between the core models and AFSIM. This is typically used for things that may be
// created and passed around (but data copied around) and whose lifecycle isn't well defined ahead of time (messages for instance). The data pointer
// will be reference counted and attempted to be deleted upon last owner.
template <typename tWrappedType>
class WsfBMManagedTypeWrapper : public WsfBMTypeWrapper<tWrappedType, std::shared_ptr<tWrappedType>>
{
   public:
      WsfBMManagedTypeWrapper(const std::string& script_class_name)
         : WsfBMTypeWrapper<tWrappedType, std::shared_ptr<tWrappedType>>(script_class_name)
      {
         WsfBMTypeWrapper<tWrappedType, std::shared_ptr<tWrappedType>>::m_wrapped_ptr.reset(new tWrappedType);
      }

      WsfBMManagedTypeWrapper(const std::string& script_class_name, const std::shared_ptr<tWrappedType>& data_to_wrap)
         : WsfBMTypeWrapper<tWrappedType, std::shared_ptr<tWrappedType>>(script_class_name, data_to_wrap)
      {}

      virtual void SetTable(std::shared_ptr<tWrappedType>& data_to_wrap)
      {
         WsfBMTypeWrapper<tWrappedType, std::shared_ptr<tWrappedType>>::m_wrapped_ptr = data_to_wrap;
      }
};

// Use this type when the data is really just a thin wrapper but the user won't own the data. This is typically passing internal data structures
// around (internal BM weapon table for instance) to modules within AFSIM that may need to reference them. The user will never own the data, therefore
// the data will be wrapped nut not attempted to be referenced counted or deleted.
template <typename tWrappedType>
class WsfBMUnmanagedTypeWrapper : public WsfBMTypeWrapper<tWrappedType, tWrappedType*>
{
   public:
      WsfBMUnmanagedTypeWrapper(const std::string& script_class_name)
         : WsfBMTypeWrapper<tWrappedType, tWrappedType *>(script_class_name)
      {
         WsfBMTypeWrapper<tWrappedType, tWrappedType*>::m_wrapped_ptr = nullptr;
      }

      WsfBMUnmanagedTypeWrapper(const std::string& script_class_name, tWrappedType* data_to_wrap)
         : WsfBMTypeWrapper<tWrappedType, tWrappedType *>(script_class_name, data_to_wrap)
      { }

      virtual void SetData(tWrappedType* data_to_wrap)
      {
         WsfBMTypeWrapper<tWrappedType, tWrappedType*>::m_wrapped_ptr = data_to_wrap;
      }
};

#endif
