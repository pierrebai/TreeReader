#include "TreeFilterCommands.h"
#include "TreeFilterMaker.h"
#include "TreeReaderHelpers.h"
#include "SimpleTreeWriter.h"

#include <sstream>

namespace TreeReader
{
   using namespace std;

   /////////////////////////////////////////////////////////////////////////
   //
   // Tree loading and saving.

   void CommandsContext::SetInputFilter(const std::wstring& filterRegex)
   {
      Options.ReadOptions.InputFilter = filterRegex;
   }

   void CommandsContext::SetInputIndent(const std::wstring& indentText)
   {
      Options.ReadOptions.InputIndent = indentText;
   }

   void CommandsContext::SetOutputIndent(const std::wstring& indentText)
   {
      Options.OutputLineIndent = indentText;
   }

   wstring CommandsContext::LoadTree(const filesystem::path& filename)
   {
      TreeFileName = filename;
      auto newTree = make_shared<TextTree>(ReadSimpleTextTree(filesystem::path(TreeFileName), Options.ReadOptions));
      if (newTree && newTree->Roots.size() > 0)
      {
         Trees.emplace_back(move(newTree));
         return {};
      }
      else
      {
         return L"Tree file was invalid or empty.\n";
      }
   }

   void CommandsContext::SaveFilteredTree(const filesystem::path& filename)
   {
      FilteredFileName = filename;
      if (Filtered)
      {
         WriteSimpleTextTree(filesystem::path(FilteredFileName), *Filtered, Options.OutputLineIndent);
         FilteredWasSaved = true;
      }
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Current filter.

   void CommandsContext::SetFilter(const TreeFilterPtr& filter)
   {
      Filter = filter;
      CommitFilterToUndo();
   }

   void CommandsContext::ApplyFilterToTree()
   {
      if (Trees.size() <= 0)
         return;

      if (Filter)
      {
         Filtered = make_shared<TextTree>();
         FilterTree(*Trees.back(), *Filtered, Filter);
         FilteredWasSaved = false;
      }
      else
      {
         Filtered = make_shared<TextTree>(*Trees.back());
         // Note: pure copy of input tree are considered to have been saved.
         FilteredWasSaved = true;
      }
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Named filters management.

   NamedFilterPtr CommandsContext::NameFilter(const std::wstring& filterName)
   {
      return NameFilter(filterName, Filter);
   }

   NamedFilterPtr CommandsContext::NameFilter(const std::wstring& filterName, const TreeFilterPtr& filter)
   {
      return KnownFilters->Add(filterName, filter);
   }

   bool CommandsContext::RemoveNamedFilter(const std::wstring& filterName)
   {
      return KnownFilters->Remove(filterName);
   }

   vector<NamedFilterPtr> CommandsContext::GetNamedFilters() const
   {
      vector<NamedFilterPtr> filters;

      for (const auto& [name, filter] : KnownFilters->All())
         filters.push_back(filter);

      return filters;
   }

   void CommandsContext::SaveNamedFilters(const std::filesystem::path& filename)
   {
      if (KnownFilters->All().size() > 0)
         WriteNamedFilters(filename, *KnownFilters);
   }

   void CommandsContext::LoadNamedFilters(const std::filesystem::path& filename)
   {
      auto filters = ReadNamedFilters(filename);
      KnownFilters->Merge(filters);
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Current text tree and filtered tree.

   std::shared_ptr<TextTree> CommandsContext::GetCurrentTree() const
   {
      if (Trees.size() <= 0)
         return {};

      return Trees.back();
   }

   std::shared_ptr<TextTree> CommandsContext::GetFilteredTree() const
   {
      return Filtered;
   }

   void CommandsContext::PushFilteredAsTree()
   {
      if (Filtered)
      {
         Trees.emplace_back(move(Filtered));
      }
   }
   
   void CommandsContext::PopTree()
   {
      if (Trees.size() > 0)
         Trees.pop_back();
   }
   
   /////////////////////////////////////////////////////////////////////////
   //
   // Undo / redo.

   void CommandsContext::DeadedFilters(any& data)
   {
      data = ConvertFiltersToText(Filter);
   }

   void CommandsContext::AwakenFilters(const any& data)
   {
      // Note: do not call SetFilter as it would put it in undo/redo...
      Filter = ConvertTextToFilters(any_cast<wstring>(data), *KnownFilters);;
   }

   void CommandsContext::AwakenToEmptyFilters()
   {
      // Note: do not call SetFilter as it would put it in undo/redo...
      Filter = nullptr;
   }

   void CommandsContext::ClearUndoStack()
   {
      UndoRedo.Clear();
      // Note: allow undoing back to an empty filter list. To enable this, there must be an empty initial commit.
      UndoRedo.Commit({ 0, nullptr, [self = this](const any&) { self->AwakenToEmptyFilters(); } });
   }

   void CommandsContext::CommitFilterToUndo()
   {
      UndoRedo.Commit(
         {
            ConvertFiltersToText(Filter),
            [self = this](any& data) { self->DeadedFilters(data); },
            [self = this](const any& data) { self->AwakenFilters(data); }
         });
   }
     
   void CommandsContext::Undo()
   {
      UndoRedo.Undo();
   }

   void CommandsContext::Redo()
   {
      UndoRedo.Redo();
   }

   bool CommandsContext::HasUndo() const
   {
      return UndoRedo.HasUndo();
   }


   bool CommandsContext::HasRedo() const
   {
      return UndoRedo.HasRedo();
   }
}
