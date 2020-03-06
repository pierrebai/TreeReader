#include "FilterEditor.h"
#include "TreeFilterListWidget.h"
#include "TreeFilterHelpers.h"
#include "QtUtilities.h"

#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qgridlayout.h>
#include <QtWidgets/qcombobox.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qheaderview.h>
#include <QtWidgets/qscrollarea.h>

#include <algorithm>
#include <typeindex>

namespace TreeReaderApp
{
   using namespace TreeReader;
   using namespace QtAdditions;
   using namespace std;

   namespace
   {
      namespace L
      {
         inline const wchar_t* t(const wchar_t* text)
         {
            return text;
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   //
   // A QWidget to select and order filters.

   class FiltersEditorUI
   {
   public:
      FiltersEditorUI(FilterEditor& parent)
      : _editor(parent)
      {
         BuildUI(parent);
         ConnectUI();
      }

      TreeFilterPtr GetEdited()
      {
         UpdateEditedFromUI();
         return _edited;
      }

      void SetEdited(const TreeFilterPtr& ed)
      {
         if (ed == _edited)
            return;

         _edited = ed;

         FillUI();
      }

   private:

      void BuildUI(FilterEditor& parent)
      {
         QVBoxLayout* layout = new QVBoxLayout(&parent);
         layout->setContentsMargins(0, 0, 0, 0);

         _filterList = new TreeFilterListWidget([self = this](TreeFilterWidget* panel)
         {
            delete panel;
         });

         _filterList->setAcceptDrops(true);
         layout->addWidget(_filterList);

         _filterList->setEnabled(true);
      }

      void ConnectUI()
      {
      }

      void FillUI()
      {
         DisableFeedback df(_filterList, _disableFeedback);

         _filterList->Clear();
         TreeReader::VisitFilters(_edited, true, [self = this](const TreeFilterPtr& filter) -> bool
         {
            self->_filterList->AddTreeFilter(filter);
            return true;
         });
      }

      TreeFilterPtr DisconnectFilter(TreeFilterPtr filter)
      {
         if (auto delegate = dynamic_pointer_cast<DelegateTreeFilter>(filter))
         {
            delegate->Filter = nullptr;
         }
         else if (auto combine = dynamic_pointer_cast<CombineTreeFilter>(filter))
         {
            combine->Filters.clear();
         }
         return filter;
      }

      void UpdateEditedFromUI()
      {
         TreeFilterPtr root;
         vector<TreeFilterPtr> previous;

         for (auto& filter : _filterList->GetTreeFilters())
         {
            filter = DisconnectFilter(filter->Clone());
            if (!root)
            {
               root = filter;
            }
            else
            {
               bool placed = false;
               while (previous.size() > 0)
               {
                  if (auto delegate = dynamic_pointer_cast<DelegateTreeFilter>(previous.back()))
                  {
                     if (!delegate->Filter)
                     {
                        delegate->Filter = filter;
                        placed = true;
                        break;
                     }
                  }
                  else if (auto combine = dynamic_pointer_cast<CombineTreeFilter>(previous.back()))
                  {
                     combine->Filters.push_back(filter);
                     placed = true;
                     break;
                  }

                  previous.pop_back();
               }

               if (!placed)
               {
                  auto newRoot = And(root, filter);
                  root = newRoot;
                  previous.push_back(root);
               }
            }

            previous.push_back(filter);
         }

         _edited = root;
      }

      FilterEditor& _editor;
      TreeFilterPtr _edited;

      TreeFilterListWidget* _filterList;

      int _disableFeedback = 0;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // A QWidget to select and order filters.

   FilterEditor::FilterEditor(QWidget* parent)
   : QWidget(parent), _ui(make_unique<FiltersEditorUI>(*this))
   {
   }

   void FilterEditor::SetEdited(const TreeFilterPtr& edited)
   {
      if (!_ui)
         return;

      _ui->SetEdited(edited);
   }

   TreeFilterPtr FilterEditor::GetEdited() const
   {
      if (!_ui)
         return {};

      return _ui->GetEdited();
   }
}

// vim: sw=3 : sts=3 : et : sta : 
