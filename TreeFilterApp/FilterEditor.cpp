#include "FilterEditor.h"
#include "TreeFilterListWidget.h"
#include "QtUtilities.h"
#include "QWidgetScrollListWidget.h"

#include "dak/tree_reader/tree_filter.h"
#include "dak/tree_reader/tree_filter_helpers.h"
#include "dak/tree_reader/named_filters.h"
#include "dak/utility/undo_stack.h"
#include "dak/utility/exceptions.h"
#include "dak/tree_reader/tree_filter_maker.h"

#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qgridlayout.h>
#include <QtWidgets/qcombobox.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qheaderview.h>
#include <QtWidgets/qscrollarea.h>

#include <algorithm>
#include <typeindex>

namespace dak::tree_reader::app
{
   using namespace dak::tree_reader;
   using namespace Qtadditions;
   using namespace std;

   ////////////////////////////////////////////////////////////////////////////
   //
   // A QWidget to select and order filters.

   class FiltersEditorUI
   {
   public:
      FiltersEditorUI(const named_filters& known, undo_stack& undoRedo, FilterEditor& parent)
      : _known_filters(known), _undo_redo(undoRedo), _editor(parent)
      {
         BuildUI(parent);
         ConnectUI();
      }

      tree_filter_ptr getEdited()
      {
         UpdateEditedFromUI();
         return _edited;
      }

      wstring getEditedName() const
      {
         return _filterName;
      }

      void SetEdited(const tree_filter_ptr& ed, const wstring& name, bool forced)
      {
         if (ed == _edited && !forced)
            return;

         _edited = ed;
         _filterName = name;

         FillUI();

         CommitFilterChangeToUndo();
      }

   private:

      /////////////////////////////////////////////////////////////////////////
      //
      // UI setup.

      void BuildUI(FilterEditor& parent)
      {
         QVBoxLayout* layout = new QVBoxLayout(&parent);
         layout->setContentsMargins(0, 0, 0, 0);

         auto delCallback = [self = this](TreeFilterListItem* panel)
         {
            // When a filter gets deleted, update the edited filter if it is no
            // longer the root.
            delete panel;
            auto filters = self->_filterList->getTreeFilters();
            if (filters.size() > 0)
               self->_edited = filters.back();
            else
               self->_edited = nullptr;

            self->CommitFilterChangeToUndo();
         };

         auto listChangedCallback = [self = this](QWidgetListWidget* list)
         {
            self->CommitFilterChangeToUndo();
         };

         _filterList = new TreeFilterListWidget(delCallback, {}, listChangedCallback);

         _filterList->setAcceptDrops(true);
         _filterList->setEnabled(true);

         _scrollFilterList = new QWidgetScrollListWidget(_filterList);
         layout->addWidget(_scrollFilterList);

         // note: allow undoing back to an empty filter list. To enable this, there must be an empty initial commit.
         _undo_redo.commit({ 0, nullptr, [self = this](const any&) { self->AwakenToEmptyFilters(); } });
      }

      void ConnectUI()
      {
      }

      static bool IsUnder(const tree_filter_ptr& filter, const tree_filter_ptr& child)
      {
         if (filter == child)
            return true;

         if (auto combine = dynamic_pointer_cast<combine_tree_filter>(filter))
         {
            for (const auto& c : combine->named_filters)
               if (IsUnder(c, child))
                  return true;
         }
         else if (auto delegate = dynamic_pointer_cast<delegate_tree_filter>(filter))
         {
            return IsUnder(delegate->filter, child);
         }

         return false;
      }

      void FillUI()
      {
         DisableFeedback df(_filterList, _disableFeedback);

         deque<pair<shared_ptr<combine_tree_filter>, TreeFilterListWidget*>> combineFilters;

         _filterList->clear();
         dak::tree_reader::visit_filters(_edited, true, [self = this, &combineFilters](const tree_filter_ptr& filter) -> bool
         {
            QWidgetListItem* widget = nullptr;
            for (auto& [combineFilter, combineWidget] : combineFilters)
            {
               if (IsUnder(combineFilter, filter))
               {
                  widget = combineWidget->addTreeFilter(filter);
                  break;
               }
            }


            if (!widget)
               widget = self->_filterList->addTreeFilter(filter);

            if (TreeFilterListItem* filterWidget = dynamic_cast<TreeFilterListItem*>(widget))
            {
               if (filterWidget->SubList)
               {
                  if (auto combine = dynamic_pointer_cast<combine_tree_filter>(filter))
                  {
                     combineFilters.emplace_front(combine, filterWidget->SubList);
                  }
               }
            }

            return true;
         });
      }

      /////////////////////////////////////////////////////////////////////////
      //
      // Transfer from UI to data.

      static tree_filter_ptr DisconnectFilter(tree_filter_ptr filter)
      {
         if (auto delegate = dynamic_pointer_cast<delegate_tree_filter>(filter))
         {
            delegate->filter = nullptr;
         }
         else if (auto combine = dynamic_pointer_cast<combine_tree_filter>(filter))
         {
            combine->named_filters.clear();
         }
         return filter;
      }

      static void FillFiltersFromUI(const std::vector<QWidgetListItem*>& widgets, tree_filter_ptr& root, vector<tree_filter_ptr>& previous)
      {
         for (auto& widget : widgets)
         {
            auto tfw = dynamic_cast<TreeFilterListItem*>(widget);
            if (!tfw)
               continue;
            if (!tfw->filter)
               continue;

            auto filter = DisconnectFilter(tfw->filter->clone());

            if (!root)
            {
               root = filter;
            }
            else
            {
               bool placed = false;
               while (previous.size() > 0)
               {
                  if (auto delegate = dynamic_pointer_cast<delegate_tree_filter>(previous.back()))
                  {
                     if (!delegate->filter)
                     {
                        delegate->filter = filter;
                        placed = true;
                        break;
                     }
                  }
                  else if (auto combine = dynamic_pointer_cast<combine_tree_filter>(previous.back()))
                  {
                     combine->named_filters.push_back(filter);
                     placed = true;
                     break;
                  }

                  previous.pop_back();
               }

               if (!placed)
               {
                  auto newRoot = and(root, filter);
                  root = newRoot;
                  previous.push_back(root);
               }
            }

            if (tfw->SubList)
            {
               vector<tree_filter_ptr> subPrevious;
               subPrevious.push_back(filter);
               FillFiltersFromUI(tfw->SubList->getItems(), filter, subPrevious);

               // note: when we have an explicit sub-list, we don't use it as
               //       the previous implicit combine to put other filters not
               //       under it in.
               //
               //       So we don't put it in the previous stack in this
               //       level.
            }
            else
            {
               previous.push_back(filter);
            }
         }
      }

      void UpdateEditedFromUI()
      {
         vector<tree_filter_ptr> previous;
         tree_filter_ptr root;
         FillFiltersFromUI(_filterList->getItems(), root, previous);
         _edited = root;
      }

      /////////////////////////////////////////////////////////////////////////
      //
      // undo/redo.

      void CommitFilterChangeToUndo()
      {
         if (_disableFeedback > 0)
            return;

         UpdateEditedFromUI();
         _undo_redo.commit(
            {
               convert_filter_to_text(_edited),
               [self = this](any& data) { self->deaden_filters(data); },
               [self = this](const any& data) { self->awaken_filters(data); }
            });
      }

      void deaden_filters(any& data)
      {
         UpdateEditedFromUI();
         data = convert_filter_to_text(_edited);
      }

      void awaken_filters(const any& data)
      {
         _edited = convert_text_to_filter(any_cast<wstring>(data), _known_filters);
         FillUI();
      }

      void AwakenToEmptyFilters()
      {
         _edited = nullptr;
         FillUI();
      }


      /////////////////////////////////////////////////////////////////////////
      //
      // data.

      const named_filters& _known_filters;
      undo_stack& _undo_redo;
      FilterEditor& _editor;
      tree_filter_ptr _edited;
      wstring _filterName;

      TreeFilterListWidget* _filterList;
      QWidgetScrollListWidget* _scrollFilterList;

      int _disableFeedback = 0;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // A QWidget to select and order filters.

   FilterEditor::FilterEditor(const named_filters& known, undo_stack& undoRedo, QWidget* parent)
   : QWidget(parent), _ui(make_unique<FiltersEditorUI>(known, undoRedo, *this))
   {
   }

   void FilterEditor::SetEdited(const tree_filter_ptr& edited, const wstring& name, bool forced)
   {
      if (!_ui)
         return;

      _ui->SetEdited(edited, name, forced);
   }

   tree_filter_ptr FilterEditor::getEdited() const
   {
      if (!_ui)
         return {};

      return _ui->getEdited();
   }

   wstring FilterEditor::getEditedName() const
   {
      if (!_ui)
         return {};

      return _ui->getEditedName();
   }
}

// vim: sw=3 : sts=3 : et : sta : 
