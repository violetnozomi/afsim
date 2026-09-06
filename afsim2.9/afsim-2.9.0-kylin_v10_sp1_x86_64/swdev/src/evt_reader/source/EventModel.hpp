// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef RVEVENTTABLEMODEL_HPP
#define RVEVENTTABLEMODEL_HPP

#include <QAbstractTableModel>
#include <QFile>

class QTextStream;

class EventModel : public QAbstractTableModel
{
public:
   EventModel(QObject* aParent, const QString& aFilename);
   ~EventModel() override;

   int rowCount(const QModelIndex& aIndex) const override;
   int columnCount(const QModelIndex& aIndex) const override;
   QVariant data(const QModelIndex& aIndex, int aRole) const override;
   QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

   enum
   {
      FIELD_TYPE = 0,
      FIELD_OWNER = 1,
      FIELD_TARGET = 2,
      FIELD_THIRD_PARTY = 3,
      FIELD_DATA = 4
   };
private:
   bool IndexEvent(QTextStream& aStream,
                   std::size_t& aPlace);
   QString GetEventString(const std::size_t& aLoc) const;
   QString GetFullEventString(const std::size_t& aLoc) const; // includes line breaks
   QString GetUnlabeledValue(const unsigned int aIndex,
                             const std::size_t& aLoc) const;
   QString GetLabel(const unsigned int aIndex,
                    const std::size_t& aLoc) const;
   QString GetLabeledValue(const unsigned int aIndex,
                           const std::size_t& aLoc) const;

   std::vector<std::size_t> mFileIndex;
   std::size_t              mTerminatorLength;
   QFile                    mFile;
   QTextStream*             mStream;
   mutable std::size_t      mLoc;

};

#endif
