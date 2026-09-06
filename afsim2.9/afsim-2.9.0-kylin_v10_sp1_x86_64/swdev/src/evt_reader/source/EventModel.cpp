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

#include "EventModel.hpp"

#include <QRegularExpression>
#include <QTextStream>

EventModel::EventModel(QObject*                aParent,
                       const QString&          aFilename)
   : QAbstractTableModel(aParent)
   , mTerminatorLength(0)
   , mFile(aFilename)
   , mStream(nullptr)
   , mLoc(0)
{
   mFile.open(QIODevice::ReadOnly | QFile::Text);
   if (mFile.isOpen())
   {
      size_t place = 0;
      mStream = new QTextStream(&mFile);
      while (IndexEvent(*mStream, place));
   }
}

EventModel::~EventModel()
{
   delete mStream;
   mFile.close();
}

int EventModel::rowCount(const QModelIndex& aIndex) const
{
   return (int)mFileIndex.size();
}

int EventModel::columnCount(const QModelIndex& aIndex) const
{
   return FIELD_DATA + 100;
}

QVariant EventModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   if (role == Qt::DisplayRole)
   {
      if (orientation == Qt::Horizontal)
      {
         switch (section)
         {
         case (FIELD_TYPE):
            return "type";
         case (FIELD_OWNER):
            return "platform";
         case(FIELD_TARGET):
            return "interactor";
         case(FIELD_THIRD_PARTY):
            return "third_party";
         }
      }
      else
      {
         return GetUnlabeledValue(0, mFileIndex[section]);
      }
      return section + 1;
   }
   return QVariant();
}

bool EventModel::IndexEvent(QTextStream& aStream,
                            std::size_t& aPlace)
{
   if (aStream.atEnd())
   {
      return false;
   }
   mFileIndex.emplace_back(aPlace);// aStream.pos());
   QString line = aStream.readLine();
   aPlace += line.size() + mTerminatorLength;
   if (mTerminatorLength == 0)
   {
      // ok, we are assuming a uniform terminator length... this seems pretty safe,
      // and if it isn't the case, maybe you shouldn't be doing whatever it is you did
      mTerminatorLength = aStream.pos() - aPlace;
      aPlace += mTerminatorLength;
   }
   while (line.endsWith('\\') && !aStream.atEnd())
   {
      line = aStream.readLine();
      aPlace += line.size() + mTerminatorLength;
   }
   mLoc = aPlace;
   return true;
}

QString EventModel::GetEventString(const std::size_t& aLoc) const
{
   if (mLoc != aLoc)
   {
      // seek is expensive, this optimization assumes that Qt is going to be frequently hitting events sequentially
      // caching would be an alternative, but I think Qt's behavior is reliable enough to exploit
      // this got filtering on a 2.4 GB file from 210 seconds down to 20 seconds.
      mStream->seek(aLoc);
      mLoc = aLoc;
   }
   QString e;
   QString line;

   e = mStream->readLine();
   mLoc += e.size() + mTerminatorLength;
   while (e.endsWith('\\') && !mStream->atEnd())
   {
      e.chop(1);
      line = mStream->readLine();
      mLoc += line.size() + mTerminatorLength;
      e += line;
   }

   return e;
}

QString EventModel::GetFullEventString(const std::size_t& aLoc) const
{
   if (mLoc != aLoc)
   {
      // seek is expensive, this optimization assumes that Qt is going to be frequently hitting events sequentially
      // caching would be an alternative, but I think Qt's behavior is reliable enough to exploit
      // this got filtering on a 2.4 GB file from 210 seconds down to 20 seconds.
      mStream->seek(aLoc);
      mLoc = aLoc;
   }
   QString e;
   QString line;

   e = mStream->readLine();
   mLoc += e.size() + mTerminatorLength;
   while (e.endsWith('\\') && !mStream->atEnd())
   {
      line = mStream->readLine();
      mLoc += line.size() + mTerminatorLength;
      e += "\n" + line;
   }
   e += "\n";
   return e;
}

QString EventModel::GetUnlabeledValue(const unsigned int aIndex,
                                      const std::size_t& aLoc) const
{
   QString eventString = GetEventString(aLoc);
   QVector<QStringRef> vals = eventString.splitRef(" ");
   if (aIndex >= static_cast<unsigned int>(vals.size()))
   {
      return QString();
   }
   else
   {
      for (unsigned int i = 2; i <= aIndex; ++i) // we start at two, because we can assume the presence of a time and type before we get to labels
      {
         if (vals[i].endsWith(':'))
         {
            return QString();
         }
      }
      return vals[aIndex].toString();
   }
}

QString EventModel::GetLabel(const unsigned int aIndex, const std::size_t& aLoc) const
{
   QString eventString = GetEventString(aLoc);

   int idx = 0;
   QRegularExpression r("([^:\\s]+):\\s");
   QRegularExpressionMatch m;
   for (unsigned int i = 0; i <= aIndex; ++i)
   {
      if (idx >= 0)
      {
         idx = eventString.indexOf(r, idx, &m);
         if (idx >= 0)
         {
            idx += m.captured().length();
         }
      }
   }
   if (idx >= 0)
   {
      return m.captured();
   }
   else
   {
      return QString();
   }
}

QString EventModel::GetLabeledValue(const unsigned int aIndex, const std::size_t& aLoc) const
{
   QString eventString = GetEventString(aLoc);

   int idx = 0;
   QRegularExpression r("([^:\\s]+):\\s");
   QRegularExpressionMatch m;
   for (unsigned int i = 0; i <= aIndex; ++i)
   {
      if (idx >= 0)
      {
         idx = eventString.indexOf(r, idx, &m);
         if (idx >= 0)
         {
            idx += m.captured().length();
         }
      }
   }
   QString header = m.captured();
   int afterIdx = eventString.indexOf(r, idx, &m);
   if (afterIdx == idx)
   {
      if (afterIdx == -1)
      {
         return QString();
      }
      else
      {
         return header;
      }
   }
   else
   {
      return eventString.mid(idx, afterIdx - idx);
   }
}

QVariant EventModel::data(const QModelIndex& aIndex, int aRole) const
{
   if (aIndex.isValid())
   {
      if (aRole == Qt::UserRole) // user role will tell you the file location
      {
         return GetFullEventString(mFileIndex[aIndex.row()]);
      }
      if ((aRole == Qt::DisplayRole) || (aRole == Qt::ToolTipRole))// || (aRole == Qt::UserRole))
      {
         int c = aIndex.column();

         if (c == FIELD_TYPE)
         {
            if (aRole == Qt::ToolTipRole)
            {
               return "type";
            }
            return GetUnlabeledValue(1, mFileIndex[aIndex.row()]);
         }
         else if (c == FIELD_OWNER)
         {
            if (aRole == Qt::ToolTipRole)
            {
               return "owner";
            }
            return GetUnlabeledValue(2, mFileIndex[aIndex.row()]);
         }
         else if (c == FIELD_TARGET)
         {
            if (aRole == Qt::ToolTipRole)
            {
               return "target";
            }
            return GetUnlabeledValue(3, mFileIndex[aIndex.row()]);
         }
         else if (c == FIELD_THIRD_PARTY)
         {
            if (aRole == Qt::ToolTipRole)
            {
               return "third_party";
            }
            return GetUnlabeledValue(4, mFileIndex[aIndex.row()]);
         }
         else
         {
            if (aRole == Qt::ToolTipRole)
            {
               return GetLabel(c - FIELD_DATA, mFileIndex[aIndex.row()]);
            }
            return GetLabeledValue(c - FIELD_DATA, mFileIndex[aIndex.row()]);
         }
      }
   }
   return QVariant();
}
