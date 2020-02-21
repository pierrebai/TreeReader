#include "SimpleTreeReader.h"
#include "BuffersTextHolder.h"

#include <fstream>
#include <sstream>
#include <algorithm>

namespace TreeReader
{
   using namespace std;
   using namespace std::filesystem;

   TextTree ReadSimpleTextTree(const path& path, const ReadSimpleTextTreeOptions& options)
   {
      wifstream file(path);
      return ReadSimpleTextTree(file, options);
   }

   static std::pair<size_t, size_t> GetIndent(const wchar_t* buffer, size_t count, const ReadSimpleTextTreeOptions& options)
   {
      if (options.SimpleIndent)
      {
         const size_t textIndex = wcsspn(buffer, L" \t");
         size_t indent = textIndex;
         for (size_t i = 0; i < textIndex; ++i)
            if (buffer[i] == L'\t')
               indent += options.TabSize - 1;
         return make_pair(indent, textIndex);
      }
      else
      {
         wcmatch match;
         if (!regex_search(buffer, buffer + count, match, options.IndentRegex))
            return make_pair(0, 0);

         // Calculate the indent. One space per character, except tabs which count for TabSize
         // characters. Since tabs were already counted for 1, we only add one less than TabSize
         // for each.
         wstring indentText = match[0].str();
         size_t indent = indentText.length();
         for (wchar_t c : indentText)
            if (c == L'\t')
               indent += options.TabSize - 1;

         return make_pair(indent, match.length());
      }
   }

   TextTree ReadSimpleTextTree(wistream& stream, const ReadSimpleTextTreeOptions& options)
   {
      BuffersTextHolderReader reader;

      vector<size_t> indents;
      vector<wchar_t*> lines;
      {
         while (true)
         {
            const auto [line, count] = reader.ReadLine(stream);
            if (count <= 0)
               break;
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
