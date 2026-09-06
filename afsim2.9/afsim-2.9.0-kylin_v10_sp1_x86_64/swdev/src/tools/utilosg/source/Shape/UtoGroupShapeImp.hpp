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
// UtoGroupShapeImp.hpp : header file

#if !defined(_UTOGROUPSHAPEIMP_HPP_)
#define _UTOGROUPSHAPEIMP_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include "UtoShapeImp.hpp"
#include "UtoTypes.hpp"

class UtoGroupShapeImp : public UtoShapeImp
{
public:
   UtoGroupShapeImp();
   UtoGroupShapeImp(const UtoGroupShapeImp&);
   ~UtoGroupShapeImp() override;

   // UtoShapeMap& Shapes () {return m_Shapes;}
   UtoShape*          FindShape(const std::string& name) const;
   UtoShape*          Bind(const std::string& name, const UtoShape& shape);
   bool               UnbindShape(const std::string& rName);
   const UtoShapeMap& Shapes() const { return m_Shapes; }

   bool EnableRecursiveProperties(bool enable);
   bool IsRecursivePropertiesEnabled() const { return m_RecursiveEnabled; }

   void SetColor(const UtoColor& Color, unsigned int nClrMask, unsigned int nReserved = 0) override;

   // sets the owner for the shape
   void SetOwner(UtoEntity* pOwner) override;

protected:
   virtual void OnShapeBound(UtoShape& shape);
   virtual void OnShapeUnbound(UtoShape& shape);

private:
   UtoGroupShapeImp& operator=(const UtoGroupShapeImp&);

   UtoShapeMap m_Shapes;
   bool        m_RecursiveEnabled;
};

#endif // !defined(_UTOGROUPSHAPEIMP_HPP_)
