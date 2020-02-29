#include "FiltersEditor.h"
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
      FiltersEditorUI(FiltersEditor& parent, int copy_icon, int add_icon, int remove_icon, int move_up_icon, int move_down_icon)
      : editor(parent)
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

         std::vector<int> selected = GetSelectedIndexes();
         if (selected.size() == 0 && _edited.size() == 1)
            selected.emplace_back(0);

         FillUI(selected);
      }

      Filters GetSelectedFilters() const
      {
         std::vector<TreeFilterPtr> selected;
         for (int index : GetSelectedIndexes())
         {
            selected.emplace_back(_edited[index]);
         }
         return selected;
      };

      void UpdateListContent()
      {
         disable_feedback++;
         _filtersList->blockSignals(disable_feedback > 0);

         TreeFilterModel* model = new TreeFilterModel;
         model->Filter = _edited.size() ? _edited.front() : nullptr;
         _filtersList->setModel(model);

         //int row = 0;
         //for (auto& filter : _edited)
         //{
         //   //const auto state = filter->Active ? Qt::CheckState::Checked : Qt::CheckState::Unchecked;
         //   const auto state = Qt::CheckState::Checked;
         //   auto filterActiveItem = _filtersList->item(row, FilterActiveColumn);
         //   filterActiveItem->setCheckState(state);

         //   // Note: make icon larger than what was set in the table view
         //   //       so that it gets scaled down with some smoothing.
         //   //const QIcon qicon = ui_qt::get_icon(mo_filter, 128, 64); TODO ICON
         //   const QString filterName = QString::fromWCharArray(GetFilterName(filter));
         //   auto filterNameItem = _filtersList->item(row, FilterNameColumn);
         //   //filterNameItem->setIcon(qicon); TODO ICON
         //   filterNameItem->setText(filterName);

         //   const QString filterDesc = QString::fromWCharArray(GetFilterName(filter)); // TODO
         //   auto descItem = _filtersList->item(row, FilterDescColumn);
         //   descItem->setText(filterDesc);

         //   row++;
         //}

         disable_feedback--;
         _filtersList->blockSignals(disable_feedback > 0);
      }

   private:

      static std::unique_ptr<QPushButton> MakeButton(int icon, const wchar_t* tooltip)
      {
         std::unique_ptr<QPushButton> button = std::make_unique<QPushButton>();
         button->setIcon(QIcon(CreatePixmapFromResource(icon)));
         button->setToolTip(QString::fromWCharArray(tooltip));
         return std::move(button);
      }

      void BuildUI(FiltersEditor& parent, int copy_icon, int add_icon, int remove_icon, int move_up_icon, int move_down_icon)
      {
         QVBoxLayout* layout = new QVBoxLayout(&parent);
         layout->setContentsMargins(0, 0, 0, 0);

         QWidget* button_panel = new QWidget(&parent);
            QGridLayout* button_layout = new QGridLayout(button_panel);
            button_layout->setContentsMargins(0, 0, 0, 0);
            CloneLayer_button = MakeButton(copy_icon, L::t(L"Copy"));
            button_layout->addWidget(CloneLayer_button.get(), 0, 0);
            AddFilter_button = MakeButton(add_icon, L::t(L"Add"));
            button_layout->addWidget(AddFilter_button.get(), 0, 1);
            RemoveFilters_button = MakeButton(remove_icon, L::t(L"Remove"));
            button_layout->addWidget(RemoveFilters_button.get(), 0, 2);
            MoveFiltersUp_button = MakeButton(move_up_icon, L::t(L"Move Up"));
            button_layout->addWidget(MoveFiltersUp_button.get(), 0, 3);
            MoveFiltersDown_button = MakeButton(move_down_icon, L::t(L"Move Down"));
            button_layout->addWidget(MoveFiltersDown_button.get(), 0, 4);
         layout->addWidget(button_panel);

         _filtersList = std::make_unique<QTreeView>();
         _filtersList->setIconSize(QSize(64, 32));
         // TODO
         //_filtersList->setColumnCount(3);
         //_filtersList->setHorizontalHeaderLabels(QStringList(
         //{
         //   QString::fromWCharArray(L::t(L"Active")),
         //   QString::fromWCharArray(L::t(L"Filter")),
         //   QString::fromWCharArray(L::t(L"Description"))
         //}));
         //_filtersList->setShowGrid(false);
         //_filtersList->horizontalHeader()->setSectionResizeMode(FilterNameColumn, QHeaderView::ResizeMode::Stretch);
         layout->addWidget(_filtersList.get());

         _filtersList->setEnabled(false);
         CloneLayer_button->setEnabled(false);
         AddFilter_button->setEnabled(true);
         RemoveFilters_button->setEnabled(false);
         MoveFiltersUp_button->setEnabled(false);
         MoveFiltersDown_button->setEnabled(false);

         // TODO
         //_filtersList->connect(_filtersList.get(), &QTreeView::itemSelectionChanged, [&]() { UpdateSelection(); });
         //_filtersList->connect(_filtersList.get(), &QTreeView::itemChanged, [&](QTableWidgetItem * item) { UpdateFilter(item); });

         CloneLayer_button->connect(CloneLayer_button.get(), &QPushButton::clicked, [&]() { CloneFilter(); });
         AddFilter_button->connect(AddFilter_button.get(), &QPushButton::clicked, [&]() { AddFilter(); });
         RemoveFilters_button->connect(RemoveFilters_button.get(), &QPushButton::clicked, [&]() { RemoveFilters(); });
         MoveFiltersUp_button->connect(MoveFiltersUp_button.get(), &QPushButton::clicked, [&]() { MoveFiltersUp(); });
         MoveFiltersDown_button->connect(MoveFiltersDown_button.get(), &QPushButton::clicked, [&]() { MoveFiltersDown(); });
      }

      void FillUI(const std::vector<int>& selected)
      {
         disable_feedback++;

         UpdateListContent();

         // TODO
         //_filtersList->resizeColumnsToContents();
         //_filtersList->horizontalHeader()->setSectionResizeMode(FilterNameColumn, QHeaderView::ResizeMode::Stretch);

         SetSelectedIndexes(selected);

         UpdateEnabled();

         disable_feedback--;
         _filtersList->blockSignals(disable_feedback > 0);
      }

      void UpdateEnabled()
      {
         auto selected = GetSelectedIndexes();

         _filtersList->setEnabled(_edited.size() > 0);
         CloneLayer_button->setEnabled(selected.size() > 0);
         AddFilter_button->setEnabled(true);
         RemoveFilters_button->setEnabled(selected.size() > 0);
         MoveFiltersUp_button->setEnabled(_edited.size() > 1 && selected.size() > 0);
         MoveFiltersDown_button->setEnabled(_edited.size() > 1 && selected.size() > 0);
      }

      void UpdateSelection()
      {
         UpdateEnabled();

         if (disable_feedback)
            return;

         if (editor.SelectionChanged)
            editor.SelectionChanged(_edited);
      }

      void UpdateFilter(QTableWidgetItem * item)
      {
         if (!item)
            return;

         switch (item->column())
         {
            case FilterActiveColumn:    return UpdateActive(item);
         }
      }

      void UpdateActive(QTableWidgetItem * item)
      {
         if (!item)
            return;

         const int row = item->row();
         if (row < 0 || row >= _edited.size())
            return;

         //_edited[row]->Active = (item->checkState() == Qt::CheckState::Unchecked); TODO ACTIVE FILTER

         UpdateFilters();
      }

      void UpdateFilters()
      {
         UpdateEnabled();

         // Note: used to avoid re-calculating the layer when just setting its value in the UI.
         if (disable_feedback)
            return;

         if (editor.FiltersChanged)
            editor.FiltersChanged(_edited);
      }

      void SetSelectedIndexes(const std::vector<int>& indexes)
      {
         _filtersList->clearSelection();
         for (const int row : indexes)
         {
            // TODO
            //for (int col = 0; col < _filtersList->columnCount(); ++col)
            //{
            //   const auto item = _filtersList->item(row, col);
            //   item->setSelected(true);
            //}
         }
      }

      std::vector<int> GetSelectedIndexes() const
      {
         std::vector<int> selected;
         // TODO
         //for (int row = 0; row < _filtersList->rowCount() && row < _edited.size(); ++row)
         //{
         //   for (int col = 0; col < _filtersList->columnCount(); ++col)
         //   {
         //      const auto item = _filtersList->item(row, col);
         //      if (item->isSelected())
         //      {
         //         selected.emplace_back(row);
         //         break;
         //      }
         //   }
         //}
         return selected;
      };

      void CloneFilter()
      {
         // Note: clone in reverse index order to avoid changing indexes before processing them.
         auto selected = GetSelectedIndexes();
         std::reverse(selected.begin(), selected.end());
         for (int index : selected)
         {
            //_edited.emplace(_edited.begin() + index, _edited[index]->clone()); TODO CLONE FILTER
         }
         FillUI({});
         UpdateFilters();
      }

      void AddFilter()
      {
         if (editor.NewFilterRequested)
            editor.NewFilterRequested();
      }

      void RemoveFilters()
      {
         // Note: clone in reverse index order to avoid changing indexes before processing them.
         auto selected = GetSelectedIndexes();
         std::reverse(selected.begin(), selected.end());
         for (int index : selected)
         {
            _edited.erase(_edited.begin() + index);
         }
         FillUI({});
         UpdateFilters();
      }

      void MoveFiltersUp()
      {
         // Treat each target index in order: find if it must receive the layer from below moving up.
         auto selected = GetSelectedIndexes();
         int target = 0;
         for (int& index : selected)
         {
            while (target < index-1)
               target++;
            if (target < index)
            {
               std::swap(_edited[target], _edited[index]);
               index--;
            }
            target++;
         }
         FillUI(selected);
         UpdateFilters();
      }

      void MoveFiltersDown()
      {
         // Treat each target index in reverse order: find if it must receive the layer from up moving down.
         auto selected = GetSelectedIndexes();
         std::reverse(selected.begin(), selected.end());
         int target = int(_edited.size()) - 1;
         for (int& index : selected)
         {
            while (target > index + 1)
               target--;
            if (target > index)
            {
               std::swap(_edited[target], _edited[index]);
               index++;
            }
            target--;
         }
         FillUI(selected);
         UpdateFilters();
      }

      static constexpr int FilterActiveColumn = 0;
      static constexpr int FilterNameColumn = 1;
      static constexpr int FilterDescColumn = 2;

      FiltersEditor& editor;
      Filters _edited;

      std::unique_ptr<QTreeView> _filtersList;
      std::unique_ptr<QPushButton> CloneLayer_button;
      std::unique_ptr<QPushButton> AddFilter_button;
      std::unique_ptr<QPushButton> RemoveFilters_button;
      std::unique_ptr<QPushButton> MoveFiltersUp_button;
      std::unique_ptr<QPushButton> MoveFiltersDown_button;

      int disable_feedback = 0;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // A QWidget to select and order filters.

   FiltersEditor::FiltersEditor(QWidget* parent, int copy_icon, int add_icon, int remove_icon, int move_up_icon, int move_down_icon)
   : QWidget(parent), _ui(std::make_unique<FiltersEditorUI>(*this, copy_icon, add_icon, remove_icon, move_up_icon, move_down_icon))
   {
   }

   void FiltersEditor::SetEdited(const TreeFilterPtr& edited)
   {
      if (!_ui)
         return;

      _ui->SetEdited(edited);
   }

   TreeFilterPtr FiltersEditor::GetEdited() const
   {
      if (!_ui)
         return {};

      return _ui->GetEdited();
   }

   void FiltersEditor::UpdateListContent()
   {
      if (!_ui)
         return;

      return _ui->UpdateListContent();
   }

   Filters FiltersEditor::GetSelectedFilters() const
   {
      if (!_ui)
         return {};

      return _ui->GetSelectedFilters();
   }
}

// vim: sw=3 : sts=3 : et : sta : 
