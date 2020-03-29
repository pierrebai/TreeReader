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
   using node = text_tree::node;

   struct buffersTextholderWithFilteredlines : buffers_text_holder
   {
      list<wstring> Filteredlines;
   };

   text_tree load_simple_text_tree(const path& path, const load_simple_text_tree_options& options)
   {
      wifstream stream(path);
      return load_simple_text_tree(stream, options);
   }

   static std::pair<size_t, size_t> getIndent(const wchar_t* line, size_t count, const load_simple_text_tree_options& options)
   {
      const size_t textIndex = wcsspn(line, options.input_indent.c_str());
      size_t indent = textIndex;
      for (size_t i = 0; i < textIndex; ++i)
         if (line[i] == L'\t')
            indent += options.tab_size - 1;
      return make_pair(indent, textIndex);
}

   text_tree load_simple_text_tree(wistream& stream, const load_simple_text_tree_options& options)
   {
      buffers_text_holder_reader reader;

      // reset the text holder for this private version that can hold extra filtered lines.
      shared_ptr<buffersTextholderWithFilteredlines> holder;
      wregex inputFilter;
      const bool inputFilterUsed = !options.input_filter.empty();
      if (inputFilterUsed)
      {
         inputFilter = wregex(options.input_filter);
         reader.holder = holder = make_shared<buffersTextholderWithFilteredlines>();
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

            if (inputFilterUsed)
            {
               auto pos = wcregex_iterator(line, line + count, inputFilter);
               auto end = wcregex_iterator();
               if (pos == end)
                  continue;

               wstring cleanedLine;
               for (; pos != end; ++pos)
                  cleanedLine += pos->str();

               const size_t cleanedCount = cleanedLine.size();
               if (cleanedCount < count)
               {
                  holder->Filteredlines.emplace_back(move(cleanedLine));
                  line = holder->Filteredlines.back().data();
                  count = cleanedCount;
               }
            }

            const auto [indent, textIndex] = getIndent(line, count, options);

            lines.emplace_back(line + textIndex);
            indents.emplace_back(indent);
         }
      }

      text_tree tree;

      tree.source_text_lines = reader.holder;

      if (indents.empty())
         return tree;

      vector<size_t> previousIndents;
      vector<node *> previousnodes;

      previousIndents.emplace_back(indents[0]);
      previousnodes.emplace_back(nullptr);

      for (size_t i = 0; i < indents.size(); ++i)
      {
         const size_t newIndent = indents[i];

         size_t previousIndent = previousIndents.back();
         while (newIndent < previousIndent)
         {
            previousIndents.pop_back();
            previousnodes.pop_back();
            previousIndent = previousIndents.back();
         }

         const wchar_t* newText = lines[i];
         node* addUnder = (newIndent > previousIndent) ? previousnodes.back()
                        : previousnodes.back() ? previousnodes.back()->parent : nullptr;
         node * newnode = tree.add_child(addUnder, newText);
         previousIndents.emplace_back(newIndent);
         previousnodes.emplace_back(newnode);
      }

      return tree;
   }
}
