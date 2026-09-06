// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WKFSTANDARDACTIONSOBJECT
#define WKFSTANDARDACTIONSOBJECT

#include <QList>
#include <QObject>

#include "WkfAction.hpp"

namespace wkf
{
class StandardActionsObject : public QObject
{
   Q_OBJECT

public:
   StandardActionsObject();
   ~StandardActionsObject() override;

   QList<Action*> GetActions() const { return mActionList; }

private:
   void HandleGroupAssignment(int aIndex);
   void HandleGroupSelection(int aIndex);

   QList<Action*>         mActionList;
   QMap<int, QStringList> mActionMap;
};
} // namespace wkf

#endif // !WKFSTANDARDACTIONSOBJECT
