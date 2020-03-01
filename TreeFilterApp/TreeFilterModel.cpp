#include "TreeFilterModel.h"
#include "TreeFilterHelpers.h"

#include <QtCore/qmimedata.h>
#include <QtCore/qdatastream.h>

namespace TreeReaderApp
{
   using namespace std;
   using namespace TreeReader;

   /////////////////////////////////////////////////////////////////////////
   //
   // Internal helpers.

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

      int GetChildIndexInParent(const TreeFilter* parent, const TreeFilter* child)
      {
         if (auto delegate = dynamic_cast<const DelegateTreeFilter*>(parent))
         {
            if (delegate->Filter.get() == child)
               return 0;
         }
         else if (auto combine = dynamic_cast<const CombineTreeFilter*>(parent))
         {
            for (size_t indexInParent = 0; indexInParent < combine->Filters.size(); ++indexInParent)
               if (combine->Filters[indexInParent].get() == child)
                  return int(indexInParent);
         }

         return -1;
      }

      pair<TreeFilter*,int> GetParentFilter(const TreeFilterPtr& root, const TreeFilter* child)
      {
         TreeFilter* parent = nullptr;
         int indexInParent = -1;

         VisitFilters(root.get(), [child, &parent, &indexInParent](TreeFilter* filter)
         {
            indexInParent = GetChildIndexInParent(filter, child);
            if (indexInParent != -1)
            {
               parent = filter;
               return false;
            }

            return true;
         });

         return make_pair(parent, indexInParent);
      }

      TreeFilterPtr FindFilter(TreeFilterPtr filter, TreeFilter* rawFilter)
      {
         while (filter)
         {
            if (filter.get() == rawFilter)
               break;

            if (auto delegate = dynamic_pointer_cast<DelegateTreeFilter>(filter))
            {
               filter = delegate->Filter;
            }
            else if (auto combine = dynamic_pointer_cast<CombineTreeFilter>(filter))
            {
               for (size_t i = 0; i < combine->Filters.size(); ++i)
               {
                  if (TreeFilterPtr found = FindFilter(combine->Filters[i], rawFilter))
                  {
                     return found;
                  }
               }
               filter = nullptr;
            }
            else
            {
               filter = nullptr;
            }
         }
         return filter;
      }

      bool CanAcceptChildFilter(TreeFilter* parent)
      {
         if (auto delegate = dynamic_cast<const DelegateTreeFilter*>(parent))
         {
            // TODO: we need to check if dropped filter could accept existing filter? Of we simply lose the filter?
            return true;
         }
         else if (auto combine = dynamic_cast<const CombineTreeFilter*>(parent))
         {
            return true;
         }

         return false;
      }

      TreeFilterPtr InsertUnderFilter(TreeFilterPtr root, TreeFilter* parent, TreeFilterPtr filter)
      {
         // TODO: make sure we don't insert a filter under itself.
         if (!parent)
         {
            if (auto delegate = dynamic_pointer_cast<DelegateTreeFilter>(filter))
            {
               delegate->Filter = root;
               return delegate;
            }
            else if (auto combine = dynamic_pointer_cast<CombineTreeFilter>(filter))
            {
               if (root)
                  combine->Filters.push_back(root);
               return combine;
            }
            else
            {
               root = make_shared<AndTreeFilter>(vector<TreeFilterPtr>{ root });
               parent = root.get();
            }
         }

         if (auto delegate = dynamic_cast<DelegateTreeFilter*>(parent))
         {
            // TODO: we need to check if dropped filter could accept existing filter? Of we simply lose the filter?
            delegate->Filter = filter;
         }
         else if (auto combine = dynamic_cast<CombineTreeFilter*>(parent))
         {
            combine->Filters.push_back(filter);
         }

         return root;
      }

      struct TreeFilterMimeData : QMimeData
      {
         vector<TreeFilterPtr> Filters;
      };
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Basic model support.

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

   /////////////////////////////////////////////////////////////////////////
   //
   // Drag and drop support.

   Qt::ItemFlags TreeFilterModel::flags(const QModelIndex& index) const
   {
      const TreeFilter* filter = index.isValid() ? static_cast<TreeFilter*>(index.internalPointer()) : Filter.get();
      if (!filter)
         return Qt::ItemFlag::NoItemFlags;

      return Qt::ItemFlag::ItemIsDragEnabled
         | Qt::ItemFlag::ItemIsDropEnabled
         | Qt::ItemFlag::ItemIsEnabled
         | Qt::ItemFlag::ItemIsSelectable;
   }

   Qt::DropActions TreeFilterModel::supportedDropActions() const
   {
      return Qt::MoveAction;
   }

   QStringList TreeFilterModel::mimeTypes() const
   {
      return QStringList("x-tree-reader-tree-filter");
   }

   QMimeData* TreeFilterModel::mimeData(const QModelIndexList& indexes) const
   {
      TreeFilterMimeData* mimeData = new TreeFilterMimeData;

      for (const QModelIndex& index : indexes)
      {
         TreeFilter* rawFilter = index.isValid() ? static_cast<TreeFilter*>(index.internalPointer()) : Filter.get();
         TreeFilterPtr filter = FindFilter(Filter, rawFilter);
         if (filter)
            mimeData->Filters.push_back(filter);
      }

      return mimeData;
   }

   bool TreeFilterModel::canDropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) const
   {
      if (!dynamic_cast<const TreeFilterMimeData*>(data))
         return false;

      if (column > 0)
         return false;

      // TODO: use row to validate the drop

      TreeFilter* parentFilter = parent.isValid() ? static_cast<TreeFilter*>(parent.internalPointer()) : nullptr;
      if (!parentFilter)
         return true;

      return CanAcceptChildFilter(parentFilter);
   }

   bool TreeFilterModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent)
   {
      if (!canDropMimeData(data, action, row, column, parent))
         return false;

      if (action == Qt::IgnoreAction)
         return true;

      // TODO: use row to position the drop

      const TreeFilterMimeData* filterData = dynamic_cast<const TreeFilterMimeData*>(data);

      TreeFilter* parentFilter = parent.isValid() ? static_cast<TreeFilter*>(parent.internalPointer()) : nullptr;
      for (TreeFilterPtr filter : filterData->Filters)
         Filter = InsertUnderFilter(Filter, parentFilter, filter);

      layoutChanged();

      return true;
   }
}
