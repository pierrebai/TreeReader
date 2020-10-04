#include "dak/tree_reader/simple_tree_reader.h"
#include "dak/tree_reader/buffers_text_holder.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <list>

namespace dak::tree_reader
{
   using namespace std;
   using namespace std::filesystem;
   using node = text_tree_t::node_t;

   struct buffers_text_holder_with_filtered_lines : buffers_text_holder_t
   {
      list<wstring> filtered_lines;
   };

   text_tree_t load_simple_text_tree(const path& path, const load_simple_text_tree_options_t& options)
   {
      wifstream stream(path);
      return load_simple_text_tree(stream, options);
   }

   static std::pair<size_t, size_t> getIndent(const wchar_t* line, size_t count, const load_simple_text_tree_options_t& options)
   {
      const size_t text_index = wcsspn(line, options.input_indent.c_str());
      size_t indent = text_index;
      for (size_t i = 0; i < text_index; ++i)
         if (line[i] == L'\t')
            indent += options.tab_size - 1;
      return make_pair(indent, text_index);
}

   text_tree_t load_simple_text_tree(wistream& stream, const load_simple_text_tree_options_t& options)
   {
      buffers_text_holder_reader_t reader;

      // Reset the text holder for this private version that can hold extra filtered lines.
      shared_ptr<buffers_text_holder_with_filtered_lines> holder;
      wregex input_filter;
      const bool input_filter_used = !options.input_filter.empty();
      if (input_filter_used)
      {
         input_filter = wregex(options.input_filter);
         reader.holder = holder = make_shared<buffers_text_holder_with_filtered_lines>();
      }

      vector<size_t> indents;
      vector<wchar_t*> lines;
      {
         while (true)
         {
            auto result = reader.read_line(stream);
            wchar_t* line = result.first;
            size_t count = result.second;
            if (count <= 0)
               break;

            if (input_filter_used)
            {
               auto pos = wcregex_iterator(line, line + count, input_filter);
               auto end = wcregex_iterator();
               if (pos == end)
                  continue;

               wstring cleaned_line;
               for (; pos != end; ++pos)
                  cleaned_line += pos->str();

               const size_t cleaned_count = cleaned_line.size();
               if (cleaned_count < count)
               {
                  holder->filtered_lines.emplace_back(move(cleaned_line));
                  line = holder->filtered_lines.back().data();
                  count = cleaned_count;
               }
            }

            const auto [indent, text_index] = getIndent(line, count, options);

            lines.emplace_back(line + text_index);
            indents.emplace_back(indent);
         }
      }

      text_tree_t tree;

      tree.source_text_lines = reader.holder;

      if (indents.empty())
         return tree;

      vector<size_t> previous_indents;
      vector<node *> previous_nodes;

      previous_indents.emplace_back(indents[0]);
      previous_nodes.emplace_back(nullptr);

      for (size_t i = 0; i < indents.size(); ++i)
      {
         const size_t new_indent = indents[i];

         size_t previous_indent = previous_indents.back();
         while (new_indent < previous_indent)
         {
            previous_indents.pop_back();
            previous_nodes.pop_back();
            previous_indent = previous_indents.back();
         }

         const wchar_t* new_text = lines[i];
         node* addUnder = (new_indent > previous_indent) ? previous_nodes.back()
                        : previous_nodes.back() ? previous_nodes.back()->parent : nullptr;
         node * newnode = tree.add_child(addUnder, new_text);
         previous_indents.emplace_back(new_indent);
         previous_nodes.emplace_back(newnode);
      }

      return tree;
   }
}
