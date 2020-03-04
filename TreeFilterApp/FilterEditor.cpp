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
      FiltersEditorUI(FilterEditor& parent, int copy_icon, int add_icon, int remove_icon, int move_up_icon, int move_down_icon)
      : _editor(parent)
      {
         BuildUI(parent, copy_icon, add_icon, remove_icon, move_up_icon, move_down_icon);
         ConnectUI();
      }

      TreeFilterPtr GetEdited() const
      {
         return _edited;
      }

      void SetEdited(const TreeFilterPtr& ed)
      {
         if (ed == _edited)
            return;

         _edited = ed;

         FillUI();
      }

      void UpdateListContent()
      {
         DisableFeedback df(_filterList, _disableFeedback);

         _filterList->Clear();
         TreeReader::VisitFilters(_edited, true, [self=this](const TreeFilterPtr& filter) -> bool
         {
            self->_filterList->AddTreeFilterListWidget(filter);
            return true;
         });
      }

   private:

      void BuildUI(FilterEditor& parent, int copy_icon, int add_icon, int remove_icon, int move_up_icon, int move_down_icon)
      {
         QVBoxLayout* layout = new QVBoxLayout(&parent);
         layout->setContentsMargins(0, 0, 0, 0);

         _filterList = new TreeFilterListWidget;
         // TODO: drag and drop.
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

         UpdateListContent();
      }

      void UpdateFilters()
      {
         // Note: used to avoid re-calculating the layer when just setting its value in the UI.
         if (_disableFeedback)
            return;

         if (_editor.FilterChanged)
            _editor.FilterChanged(_edited);
      }

      FilterEditor& _editor;
      TreeFilterPtr _edited;

      TreeFilterListWidget* _filterList;

      int _disableFeedback = 0;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // A QWidget to select and order filters.

   FilterEditor::FilterEditor(QWidget* parent, int copy_icon, int add_icon, int remove_icon, int move_up_icon, int move_down_icon)
   : QWidget(parent), _ui(make_unique<FiltersEditorUI>(*this, copy_icon, add_icon, remove_icon, move_up_icon, move_down_icon))
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

   void FilterEditor::UpdateListContent()
   {
      if (!_ui)
         return;

      return _ui->UpdateListContent();
   }
}

// vim: sw=3 : sts=3 : et : sta : 
