#include "SimpleTreeReader.h"
#include "BuffersTextHolder.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <list>

namespace TreeReader
{
   using namespace std;
   using namespace std::filesystem;

   struct BuffersTextHolderWithFilteredLines : BuffersTextHolder
   {
      list<wstring> FilteredLines;
   };

   TextTree ReadSimpleTextTree(const path& path, const ReadSimpleTextTreeOptions& options)
   {
      wifstream file(path);
      return ReadSimpleTextTree(file, options);
   }

   static std::pair<size_t, size_t> GetIndent(const wchar_t* line, size_t count, const ReadSimpleTextTreeOptions& options)
   {
      const size_t textIndex = wcsspn(line, options.InputIndent.c_str());
      size_t indent = textIndex;
      for (size_t i = 0; i < textIndex; ++i)
         if (line[i] == L'\t')
            indent += options.TabSize - 1;
      return make_pair(indent, textIndex);
}

   TextTree ReadSimpleTextTree(wistream& stream, const ReadSimpleTextTreeOptions& options)
   {
      BuffersTextHolderReader reader;

      // Reset the text holder for this private version that can hold extra filtered lines.
      shared_ptr<BuffersTextHolderWithFilteredLines> holder;
      wregex inputFilter;
      const bool inputFilterUsed = !options.InputFilter.empty();
      if (inputFilterUsed)
      {
         inputFilter = wregex(options.InputFilter);
         reader.Holder = holder = make_shared<BuffersTextHolderWithFilteredLines>();
      }

      vector<size_t> indents;
      vector<wchar_t*> lines;
      {
         while (true)
         {
            auto result = reader.ReadLine(stream);
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
                  holder->FilteredLines.emplace_back(move(cleanedLine));
                  line = holder->FilteredLines.back().data();
                  count = cleanedCount;
               }
            }

            const auto [indent, textIndex] = GetIndent(line, count, options);

            lines.emplace_back(line + textIndex);
            indents.emplace_back(indent);
         }
      }

      TextTree tree;

      tree.SourceTextLines = reader.Holder;

      vector<size_t> previousIndents;
      vector<size_t> previousNodeIndexes;

      previousIndents.emplace_back(0);
      previousNodeIndexes.emplace_back(0);

      for (size_t i = 0; i < indents.size(); ++i)
      {
         const size_t newIndent = indents[i];

         size_t previousIndent = previousIndents.back();
         while (newIndent < previousIndent)
         {
            previousIndents.pop_back();
            previousNodeIndexes.pop_back();
            previousIndent = previousIndents.back();
         }

         const wchar_t* newText = lines[i];
         if (newIndent > previousIndent)
         {
            const size_t newIndex = tree.AddChild(previousNodeIndexes.back(), newText);
            previousIndents.emplace_back(newIndent);
            previousNodeIndexes.emplace_back(newIndex);
         }
         else if (newIndent == previousIndent)
         {
            const size_t newIndex = tree.AddSibling(previousNodeIndexes.back(), newText);
            previousIndents.back() = newIndent;
            previousNodeIndexes.back() = newIndex;
         }
      }

      return tree;
   }
}
