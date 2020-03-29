#include "dak/tree_reader/global_commands.h"
#include "dak/tree_reader/tree_commands.h"
#include "dak/tree_reader/tree_filter_maker.h"
#include "dak/utility/text.h"
#include "dak/tree_reader/simple_tree_writer.h"

#include <sstream>
#include <fstream>
#include <iomanip>

namespace dak::tree_reader
{
   using namespace std;

   /////////////////////////////////////////////////////////////////////////
   //
   // Context constructor.
   global_commands::global_commands()
   {
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Tree loading and saving.

   void global_commands::set_input_filter(const wstring& filterregex)
   {
      options.read_options.input_filter = filterregex;
   }

   void global_commands::set_input_indentation(const wstring& indentText)
   {
      options.read_options.input_indent = indentText;
   }

   void global_commands::set_output_indentation(const wstring& indentText)
   {
      options.output_line_indent = indentText;
   }

   tree_commands_ptr global_commands::load_tree(const filesystem::path& filename)
   {
      auto newTree = make_shared<text_tree>(load_simple_text_tree(filename, options.read_options));
      if (newTree && newTree->roots.size() > 0)
      {
         auto treeCmd = make_shared<tree_commands>(newTree, filename, _known_filters, _undo_redo);
         _trees.emplace_back(treeCmd);
         return treeCmd;
      }
      else
      {
         return {};
      }
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Named filters management.

   named_filter_ptr global_commands::name_filter(const wstring& filterName, const tree_commands_ptr& tree)
   {
      tree->set_filter_name(filterName);
      return name_filter(filterName, tree->get_filter());
   }

   named_filter_ptr global_commands::name_filter(const wstring& filterName, const tree_filter_ptr& filter)
   {
      return _known_filters->add(filterName, filter);
   }

   bool global_commands::remove_Filter(const wstring& filterName)
   {
      return _known_filters->remove(filterName);
   }

   void global_commands::save_named_filters(const filesystem::path& filename)
   {
      if (_known_filters->all().size() > 0)
         dak::tree_reader::save_named_filters(filename, *_known_filters);
   }

   void global_commands::load_named_filters(const filesystem::path& filename)
   {
      auto filters = dak::tree_reader::load_named_filters(filename);
      _known_filters->merge(filters);
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // options.

   void global_commands::save_options(const filesystem::path& filename)
   {
      wofstream stream(filename);
      save_options(stream);
   }

   void global_commands::save_options(std::wostream& stream)
   {
      stream << L"V1: " << L"\n"
      << L"output-indent: "   << quoted(options.output_line_indent) << L"\n"
      << L"input-filter: "    << quoted(options.read_options.input_filter) << L"\n"
      << L"input-indent: "    << quoted(options.read_options.input_indent) << L"\n"
      << L"tab-size: "        << options.read_options.tab_size << L"\n";
   }

   void global_commands::load_options(const filesystem::path& filename)
   {
      wifstream stream(filename);
      load_options(stream);
   }

   void global_commands::load_options(std::wistream& stream)
   {
      wstring v1;
      stream >> v1;
      if (v1 != L"V1:")
         return;

      while (stream)
      {
         wstring item;
         stream >> skipws >> item >> skipws;
         if (item == L"output-indent:")
         {
            stream >> quoted(options.output_line_indent);

         }
         else if (item == L"input-filter:")
         {
            stream >> quoted(options.read_options.input_filter);

         }
         else if (item == L"input-indent:")
         {
            stream >> quoted(options.read_options.input_indent);

         }
         else if (item == L"tab-size:")
         {
            stream >> options.read_options.tab_size;

         }
      }
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Current text tree.

   void global_commands::remove_tree(const tree_commands_ptr& tree)
   {
      const auto pos = find(_trees.begin(), _trees.end(), tree);
      if (pos == _trees.end())
         return;

      _trees.erase(pos);
   }

   tree_commands_ptr global_commands::create_tree_from_filtered(const tree_commands_ptr& tree)
   {
      if (!tree)
         return {};

      auto newCtx = make_shared<tree_commands>(tree->get_filtered_tree(), tree->get_filtered_tree_filename(), _known_filters, _undo_redo);

      _trees.emplace_back(newCtx);

      return newCtx;
   }

   void global_commands::clear_undo_stack()
   {
      _undo_redo->clear();
   }

}
