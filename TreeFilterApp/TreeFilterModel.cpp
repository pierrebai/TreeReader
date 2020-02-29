#include "TreeFilterModel.h"
#include "TreeFilterHelpers.h"

namespace TreeReaderApp
{
   using namespace std;
   using namespace TreeReader;

   namespace
   {
      TreeFilter* GetChildFilter(const TreeFilter* parent, int row)
      {
         if (auto delegate = dynamic_cast<const DelegateTreeFilter*>(parent))
         {
            if (0 == row)
               return delegate->Filter.get();
         }
         else if (auto combine = dynamic_cast<const CombineTreeFilter*>(parent))
         {
            if (row >= 0 && row < combine->Filters.size())
               return combine->Filters[row].get();
         }

         return nullptr;
      }

      int GetChildrenCount(const TreeFilter* parent)
      {
         if (auto delegate = dynamic_cast<const DelegateTreeFilter*>(parent))
         {
            return delegate->Filter ? 1 : 0;
         }
         else if (auto combine = dynamic_cast<const CombineTreeFilter*>(parent))
         {
            return int(combine->Filters.size());
         }

         return 0;
      }

      int GetChildIndex(const TreeFilter* parent, const TreeFilter* child)
      {
         if (auto delegate = dynamic_cast<const DelegateTreeFilter*>(parent))
         {
            if (delegate->Filter.get() == child)
               return 0;
         }
         else if (auto combine = dynamic_cast<const CombineTreeFilter*>(parent))
         {
            for (size_t i = 0; i < combine->Filters.size(); ++i)
               if (combine->Filters[i].get() == child)
                  return int(i);
         }

         return -1;
      }

      pair<TreeFilter*,int> GetParentFilter(const TreeFilterPtr& root, const TreeFilter* child)
      {
         TreeFilter* parent = nullptr;
         int indexInParent = -1;

         VisitFilters(root.get(), [child, &parent, &indexInParent](TreeFilter* filter)
         {
            if (int index = GetChildIndex(filter, child); index != -1)
            {
               parent = filter;
               indexInParent = index;
               return false;
            }

            return true;
         });

         return make_pair(parent, indexInParent);
      }
   }

   QVariant TreeFilterModel::data(const QModelIndex& index, int role) const
   {
      const TreeFilter* filter = index.isValid() ? static_cast<TreeFilter*>(index.internalPointer()) : Filter.get();
      if (!filter)
         return QVariant();

      wstring text;
      switch (role)
      {
         case Qt::DisplayRole: text = filter->GetName(); break;
         case Qt::ToolTipRole: text = filter->GetDescription(); break;
         default: return QVariant();
      }
      return QVariant(QString::fromStdWString(text));
   }

   QVariant TreeFilterModel::headerData(int section, Qt::Orientation orientation, int role) const
   {
      return QVariant();
   }

   QModelIndex TreeFilterModel::index(int row, int column, const QModelIndex& parent) const
   {
      TreeFilter* parentFilter = parent.isValid() ? static_cast<TreeFilter*>(parent.internalPointer()) : nullptr;
      TreeFilter* child = parentFilter ? GetChildFilter(parentFilter, row) : Filter.get();
      if (!child)
         return QModelIndex();

      return createIndex(row, column, static_cast<void*>(child));
   }

   QModelIndex TreeFilterModel::parent(const QModelIndex& index) const
   {
      TreeFilter* filter = index.isValid() ? static_cast<TreeFilter*>(index.internalPointer()) : Filter.get();
      if (!filter)
         return QModelIndex();

      auto [parent, indexInParent] = GetParentFilter(Filter, filter);
      if (!parent)
         return QModelIndex();

      return createIndex(indexInParent, 0, static_cast<void*>(parent));
   }

   int TreeFilterModel::rowCount(const QModelIndex& parent) const
   {
      TreeFilter* parentFilter = parent.isValid() ? static_cast<TreeFilter*>(parent.internalPointer()) : nullptr;
      if (!parentFilter)
         return Filter ? 1 : 0;

      return GetChildrenCount(parentFilter);
   }
   int TreeFilterModel::columnCount(const QModelIndex& parent) const
   {
      return Filter ? 1 : 0;
   }
}
