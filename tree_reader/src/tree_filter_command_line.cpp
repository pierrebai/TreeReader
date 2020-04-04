#include "dak/tree_reader/tree_filter_command_line.h"
#include "dak/tree_reader/tree_commands.h"
#include "dak/tree_reader/tree_filter_maker.h"
#include "dak/utility/text.h"

#include <sstream>

namespace dak::tree_reader
{
   using namespace std;

   wstring command_line::get_help() const
   {
      wostringstream stream;

      stream << L"Commands:" << endl;
      stream << L"  v1: use v1 text-to-filter conversion." << endl;
      stream << L"  no-v1: use the simple text-to-filter conversion." << endl;
      stream << L"  interactive: use an interactive prompt to enter options, file name or filters." << endl;
      stream << L"  no-interactive: turn off the interactive mode." << endl;
      stream << L"  help: print this help." << endl;
      stream << L"  debug: print debug information while processing other commands." << endl;
      stream << L"  no-debug: turn off debugging." << endl;
      stream << L"  input-filter ''regex'': filter input lines using the given regular expression." << endl;
      stream << L"  input-indent ''text'': detect the indentation of each line using the given characters." << endl;
      stream << L"  output-indent ''text'': indent the printed lines with the given text." << endl;
      stream << L"  load ''file name'': load a text tree from the given file." << endl;
      stream << L"       (The tree is pushed on the active tree stack, ready to be filtered.)" << endl;
      stream << L"  save ''file name'': save the tree into the named file." << endl;
      stream << L"  filter ''filter'': convert the given textual filters description into filters." << endl;
      stream << L"  push-filtered: use the current filtered tree as input to the filters." << endl;
      stream << L"  pop-tree: pop the current tree and use the previous tree as input to the filters." << endl;
      stream << L"  then: apply the filters immediately, push the result as being the current tree and starts new filters." << endl;
      stream << L"  name ''name'': give a name to the current filter." << endl;
      stream << L"  save-filters ''file name'': save all named filters to the given file." << endl;
      stream << L"  load-filters ''file name'': load named filters from the given file." << endl;
      stream << L"  list-filters: list all the named filters." << endl;

      return stream.str();
   }

   void command_line::append_filter_text(const std::wstring& text)
   {
      if (!filter_text.empty())
         filter_text += L' ';
      filter_text += text;
   }

   void command_line::clear_filter_text()
   {
      filter_text = L"";
   }

   wstring command_line::create_filter()
   {
      return create_filter(filter_text);
   }

   wstring command_line::create_filter(const wstring& filterText)
   {
      if (!current_tree)
         return L"";

      wostringstream stream;

      current_tree->set_filter(use_v1
               ? convert_text_to_filter(filterText, *_known_filters)
               : convert_simple_text_to_filter(filterText, *_known_filters));

      if (debug)
      {
         auto& ctx = _trees.back();

         if (current_tree->get_filter())
            stream << L"named_filters: " << convert_filter_to_text(current_tree->get_filter()) << endl;
         else
            stream << L"Invalid filter: " << filterText << endl;
      }
      return stream.str();
   }

   wstring command_line::list_filters()
   {
      wostringstream sstream;
      for (const auto& [name, filter] : _known_filters->all())
         sstream << name << endl;
      return sstream.str();
   }

   wstring command_line::parse_commands(const wstring& cmdText)
   {
      return parse_commands(utility::split(cmdText));
   }

   wstring command_line::parse_commands(const vector<wstring>& cmds)
   {
      wstring result;

      // Backup current settings to detect changes.
      const commands_options previous_options = options;
      auto previous_filter_text = filter_text;
      auto previous_tree_filename = current_tree ? current_tree->get_original_tree_filename() : wstring();
      auto previous_filter = current_tree ? current_tree->get_filter() : tree_filter_ptr();
      auto previous_ctx = current_tree;

      filter_text = L"";

      for (size_t i = 0; i < cmds.size(); ++i)
      {
         const wstring& cmd = cmds[i];
         if (cmd == L"v1")
         {
            use_v1 = true;
         }
         if (cmd == L"no-v1")
         {
            use_v1 = false;
         }
         else if (cmd == L"interactive")
         {
            is_interactive = true;
         }
         else if (cmd == L"no-interactive")
         {
            is_interactive = false;
         }
         else if (cmd == L"help")
         {
            result += get_help();
         }
         else if (cmd == L"-d" || cmd == L"debug")
         {
            debug = true;
         }
         else if (cmd == L"no-debug")
         {
            debug = false;
         }
         else if (cmd == L"input-filter" && i + 1 < cmds.size())
         {
            set_input_filter(cmds[++i]);
         }
         else if (cmd == L"input-indent" && i + 1 < cmds.size())
         {
            set_input_indentation(cmds[++i]);
         }
         else if (cmd == L"output-indent" && i + 1 < cmds.size())
         {
            set_output_indentation(cmds[++i]);
         }
         else if (cmd == L"load" && i + 1 < cmds.size())
         {
            current_tree = load_tree(cmds[++i]);
         }
         else if (cmd == L"save" && i + 1 < cmds.size())
         {
            if (current_tree)
               current_tree->save_filtered_tree(cmds[++i], options);
         }
         else if (cmd == L"filter" && i + 1 < cmds.size())
         {
            append_filter_text(cmds[++i]);
         }
         else if (cmd == L"push-filtered")
         {
            if (current_tree)
              current_tree = create_tree_from_filtered(current_tree);
         }
         else if (cmd == L"pop-tree")
         {
            remove_tree(current_tree);
            // TODO: would need to recover old context.
            current_tree = nullptr;
         }
         else if (cmd == L"then")
         {
            if (current_tree)
            {
               result += create_filter();
               current_tree->apply_filter_to_tree();
               current_tree = create_tree_from_filtered(current_tree);
               clear_filter_text();
               previous_filter_text = L"";
            }
         }
         else if (cmd == L"name" && i + 1 < cmds.size())
         {
            if (current_tree)
            {
               result += create_filter();
               name_filter(cmds[++i], current_tree);
            }
         }
         else if (cmd == L"save-filters" && i + 1 < cmds.size())
         {
            save_named_filters(cmds[++i]);
         }
         else if (cmd == L"load-filters" && i + 1 < cmds.size())
         {
            load_named_filters(cmds[++i]);
         }
         else if (cmd == L"list-filters")
         {
            result += list_filters();
         }
         else
         {
            append_filter_text(cmd);
         }
      }

      if (filter_text.empty())
         filter_text = previous_filter_text;

      const bool filter_text_changed = (previous_filter_text != filter_text);
      if (filter_text_changed)
         result += create_filter();

      if (current_tree)
      {
         const bool options_changed = (previous_options != options);
         const bool read_options_changed = (previous_options.read_options != options.read_options);
         const bool file_changed = (previous_tree_filename != current_tree->get_original_tree_filename());
         const bool filter_changed = (filter_text_changed || previous_filter != current_tree->get_filter());
         const bool tree_changed = (previous_ctx != current_tree);

         if (file_changed || filter_changed || options_changed || read_options_changed || tree_changed)
            current_tree->apply_filter_to_tree();
      }

      return result;
   }
}
