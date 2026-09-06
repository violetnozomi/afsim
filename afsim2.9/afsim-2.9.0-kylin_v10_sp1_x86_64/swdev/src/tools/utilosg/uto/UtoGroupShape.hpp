// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

/////////////////////////////////////////////////////////////////////////////
// UtoGroupShape.hpp : header file

#if !defined(_UTOGROUPSHAPE_HPP_)
#define _UTOGROUPSHAPE_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include "utilosg_export.h"

#include "UtoShape.hpp"
#include "UtoTypes.hpp"
class UtoGroupShapeImp;

class UTILOSG_EXPORT UtoGroupShape : public UtoShape
{
private:
   // {secret}
   struct UTILOSG_EXPORT Policies
   {
      struct Impl;
      typedef Utok::NameShape value_type;
      typedef Impl*           impl_ptr;

      Policies();
      Policies(Impl* imp);
      Policies(const Policies& rhs);
      ~Policies();

      value_type* ptr();
      void        forward();
      bool        is_equal(const Policies&) const;
      void        swap(Policies&);

   private:
      Policies& operator=(const Policies& rhs);

      Impl* m_pImpl;
   };

public:
   // Provides access to a NameShape.
   typedef Utok::Iterator<Policies> iterator;
   // Provides access to the begin and end iterator.
   typedef Utok::IteratorPair<iterator> iterator_pair;

   UtoGroupShape();
   UtoGroupShape(const UtoGroupShape&);
   ~UtoGroupShape() override;

   UtoGroupShape&     operator=(const UtoGroupShape&);
   UtoShape*          FindShape(const std::string& name) const;
   UtoShape*          Bind(const std::string& name, const UtoShape& shape);
   bool               Unbind(const std::string& name);
   UtoShape*          Lookup(const std::string& name);
   bool               Rebind(const std::string& name, const UtoShape& shape);
   const UtoShapeMap& Shapes();
   // const_iterator_pair Shapes () const;

   bool EnableRecursiveProperties(bool enable);
   bool IsRecursivePropertiesEnabled() const;

   UtoShape* Clone() const override;
   void      Dump(std::ostream&) override;

protected:
#ifdef DOCJET // hide from DocJet or it is tied to the other constructor
#else
   UtoGroupShape(UtoGroupShapeImp*);
#endif

private:
};

#endif // !defined(_UTOGROUPSHAPE_HPP_)
