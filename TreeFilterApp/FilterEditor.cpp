#include "FilterEditor.h"
#include "TreeFilterItem.h"
#include "TreeFilterHelpers.h"
#include "QtUtilities.h"

#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qgridlayout.h>
#include <QtWidgets/qcombobox.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qheaderview.h>
#include <QtWidgets/qlistwidget.h>

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

         auto selected = GetSelection();
         FillUI(selected);
      }

      void UpdateListContent()
      {
         _disableFeedback++;
         _filterList->blockSignals(_disableFeedback > 0);

         _filterList->clear();
         TreeReader::VisitFilters(_edited, true, [self=this](const TreeFilterPtr& filter) -> bool
         {
            AddTreeFilterItem(self->_filterList.get(), filter);
            return true;
         });

         _disableFeedback--;
         _filterList->blockSignals(_disableFeedback > 0);
      }

   private:

      static unique_ptr<QPushButton> MakeButton(int icon, const wchar_t* tooltip)
      {
         unique_ptr<QPushButton> button = make_unique<QPushButton>();
         button->setIcon(QIcon(CreatePixmapFromResource(icon)));
         button->setToolTip(QString::fromWCharArray(tooltip));
         return move(button);
      }

      void BuildUI(FilterEditor& parent, int copy_icon, int add_icon, int remove_icon, int move_up_icon, int move_down_icon)
      {
         QVBoxLayout* layout = new QVBoxLayout(&parent);
         layout->setContentsMargins(0, 0, 0, 0);

         QWidget* button_panel = new QWidget(&parent);
            QGridLayout* button_layout = new QGridLayout(button_panel);
            button_layout->setContentsMargins(0, 0, 0, 0);
            _removeFiltersButton = MakeButton(remove_icon, L::t(L"Remove"));
            button_layout->addWidget(_removeFiltersButton.get(), 0, 2);
         layout->addWidget(button_panel);

         _filterList = make_unique<QListWidget>();
         //_filterList->setUniformRowHeights(true);
         //_filterList->setHeaderHidden(true);
         _filterList->setIconSize(QSize(64, 32));
         _filterList->setAcceptDrops(true);
         _filterList->setDragEnabled(true);
         _filterList->setDragDropMode(QListWidget::DragDrop);
         _filterList->setDefaultDropAction(Qt::MoveAction);
         _filterList->setDropIndicatorShown(true);
         layout->addWidget(_filterList.get());

         _filterList->setEnabled(true);
         _removeFiltersButton->setEnabled(false);
      }

      void ConnectUI()
      {
         _filterList->connect(_filterList->selectionModel(), &QItemSelectionModel::selectionChanged, [&](const QItemSelection& selected, const QItemSelection& deselected)
         {
            UpdateSelection(selected, deselected);
         });

         _removeFiltersButton->connect(_removeFiltersButton.get(), &QPushButton::clicked, [&]() { RemoveFilters(); });
      }

      void FillUI(const QItemSelection& selected)
      {
         _disableFeedback++;
         _filterList->blockSignals(_disableFeedback > 0);

         UpdateListContent();

         SetSelection(selected);

         UpdateEnabled();

         _disableFeedback--;
         _filterList->blockSignals(_disableFeedback > 0);
      }

      void UpdateEnabled()
      {
         auto selected = GetSelection();

         _removeFiltersButton->setEnabled(selected.size() > 0);
      }

      void UpdateSelection(const QItemSelection& selected, const QItemSelection& deselected)
      {
         UpdateEnabled();

         if (_disableFeedback)
            return;

         if (_editor.SelectionChanged)
            _editor.SelectionChanged(_edited);
      }

      void UpdateFilters()
      {
         UpdateEnabled();

         // Note: used to avoid re-calculating the layer when just setting its value in the UI.
         if (_disableFeedback)
            return;

         if (_editor.FilterChanged)
            _editor.FilterChanged(_edited);
      }

      void SetSelection(const QItemSelection& selection)
      {
         QItemSelectionModel* selModel = _filterList->selectionModel();
         if (!selModel)
            return;

         selModel->select(selection, QItemSelectionModel::SelectionFlag::ClearAndSelect);
      }

      QItemSelection GetSelection() const
      {
         QItemSelectionModel* selModel = _filterList->selectionModel();
         if (!selModel)
            return {};

         return selModel->selection();
      };

      void RemoveFilters()
      {
         auto model = _filterList->model();
         if (!model)
            return;

         _disableFeedback++;
         _filterList->blockSignals(_disableFeedback > 0);

         // Note: remove in reverse index order to avoid changing indexes before processing them.
         auto selected = GetSelection().indexes();
         const auto end = selected.rend();
         for (auto pos = selected.rbegin(); pos != end; ++pos)
         {
            model->removeRow(pos->row(), pos->parent());
         }

         FillUI({});
         UpdateFilters();

         _disableFeedback--;
         _filterList->blockSignals(_disableFeedback > 0);
      }

      FilterEditor& _editor;
      TreeFilterPtr _edited;

      std::unique_ptr<QListWidget> _filterList;
      std::unique_ptr<QPushButton> _removeFiltersButton;

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
