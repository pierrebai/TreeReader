#include "FilterEditor.h"
#include "TreeFilterModel.h"
#include "QtUtilities.h"

#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qgridlayout.h>
#include <QtWidgets/qcombobox.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qheaderview.h>
#include <QtWidgets/qtablewidget.h>
#include <QtWidgets/qtreeview.h>

#include <algorithm>
#include <typeindex>

namespace TreeReaderApp
{
   typedef std::vector<TreeFilterPtr> Filters;
   using namespace TreeReader;

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
   // A QWidget to select and order Filters.

   class FiltersEditorUI
   {
   public:
      FiltersEditorUI(FilterEditor& parent, int copy_icon, int add_icon, int remove_icon, int move_up_icon, int move_down_icon)
      : _editor(parent)
      {
         BuildUI(parent, copy_icon, add_icon, remove_icon, move_up_icon, move_down_icon);
      }

      TreeFilterPtr GetEdited() const
      {
         return _edited.size() > 0 ? _edited.front() : nullptr;
      }

      void SetEdited(const TreeFilterPtr& ed)
      {
         if (_edited.size() > 0 && ed == _edited[0])
            return;

         _edited.clear();
         _edited.push_back(ed);

         auto selected = GetSelection();
         FillUI(selected);
      }

      void UpdateListContent()
      {
         _disableFeedback++;
         _filtersTree->blockSignals(_disableFeedback > 0);

         TreeFilterModel* model = new TreeFilterModel;
         model->Filter = _edited.size() ? _edited.front() : nullptr;
         _filtersTree->setModel(model);
         _filtersTree->expandAll();

         _disableFeedback--;
         _filtersTree->blockSignals(_disableFeedback > 0);
      }

   private:

      static std::unique_ptr<QPushButton> MakeButton(int icon, const wchar_t* tooltip)
      {
         std::unique_ptr<QPushButton> button = std::make_unique<QPushButton>();
         button->setIcon(QIcon(CreatePixmapFromResource(icon)));
         button->setToolTip(QString::fromWCharArray(tooltip));
         return std::move(button);
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

         _filtersTree = std::make_unique<QTreeView>();
         _filtersTree->setUniformRowHeights(true);
         _filtersTree->setIconSize(QSize(64, 32));
         _filtersTree->setHeaderHidden(true);
         _filtersTree->setAcceptDrops(true);
         _filtersTree->setDragEnabled(true);
         _filtersTree->setDragDropMode(QTreeView::DragDrop);
         _filtersTree->setDropIndicatorShown(true);
         layout->addWidget(_filtersTree.get());

         _filtersTree->setEnabled(false);
         _removeFiltersButton->setEnabled(false);

         _filtersTree->connect(_filtersTree->selectionModel(), &QItemSelectionModel::selectionChanged, [&](const QItemSelection& selected, const QItemSelection& deselected)
         {
            UpdateSelection(selected, deselected);
         });

         _removeFiltersButton->connect(_removeFiltersButton.get(), &QPushButton::clicked, [&]() { RemoveFilters(); });
      }

      void FillUI(const QItemSelection& selected)
      {
         _disableFeedback++;
         _filtersTree->blockSignals(_disableFeedback > 0);

         UpdateListContent();

         SetSelection(selected);

         UpdateEnabled();

         _disableFeedback--;
         _filtersTree->blockSignals(_disableFeedback > 0);
      }

      void UpdateEnabled()
      {
         auto selected = GetSelection();

         _filtersTree->setEnabled(_edited.size() > 0);
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

         if (_editor.FiltersChanged)
            _editor.FiltersChanged(_edited);
      }

      void SetSelection(const QItemSelection& selection)
      {
         QItemSelectionModel* selModel = _filtersTree->selectionModel();
         if (!selModel)
            return;

         selModel->select(selection, QItemSelectionModel::SelectionFlag::ClearAndSelect);
      }

      QItemSelection GetSelection() const
      {
         QItemSelectionModel* selModel = _filtersTree->selectionModel();
         if (!selModel)
            return {};

         return selModel->selection();
      };

      void RemoveFilters()
      {
         // Note: clone in reverse index order to avoid changing indexes before processing them.
         auto selected = GetSelection();
         // TODO: filter editor: implement remove filter button
         //std::reverse(selected.begin(), selected.end());
         //for (int index : selected)
         //{
         //   _edited.erase(_edited.begin() + index);
         //}
         FillUI({});
         UpdateFilters();
      }

      FilterEditor& _editor;
      Filters _edited;

      std::unique_ptr<QTreeView> _filtersTree;
      std::unique_ptr<QPushButton> _removeFiltersButton;

      int _disableFeedback = 0;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // A QWidget to select and order filters.

   FilterEditor::FilterEditor(QWidget* parent, int copy_icon, int add_icon, int remove_icon, int move_up_icon, int move_down_icon)
   : QWidget(parent), _ui(std::make_unique<FiltersEditorUI>(*this, copy_icon, add_icon, remove_icon, move_up_icon, move_down_icon))
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
