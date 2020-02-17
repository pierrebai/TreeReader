#include "SimpleTreeReader.h"

#include <fstream>
#include <sstream>
#include <string_view>

namespace TreeReader
{
   using namespace std;
   using namespace std::filesystem;

   TextTree ReadSimpleTextTree(const path& path, const ReadSimpleTextTreeOptions& options)
   {
      wifstream file(path);
      return ReadSimpleTextTree(file, options);
   }

   static std::pair<size_t, size_t> GetIndent(const wstring& buffer, size_t count, const ReadSimpleTextTreeOptions& options)
   {
      wsmatch match;
      if (!regex_search(buffer, match, options.IndentRegex))
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

   TextTree ReadSimpleTextTree(wistream& stream, const ReadSimpleTextTreeOptions& options)
   {
      auto lines = make_shared<TextLines>();
      std::vector<size_t> indents;
      {
         wstring buffer;
         buffer.resize(64 * 1024, L' ');

         while (stream.getline(buffer.data(), buffer.size()))
         {
            const size_t count = size_t(stream.gcount());
            const auto [indent, textIndex] = GetIndent(buffer, count, options);

            lines->emplace_back(buffer.c_str() + textIndex);
            indents.emplace_back(indent);
         }
      }

      TextTree tree;

      tree.SourceTextLines = lines;

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

         const Text* newText = &(*lines)[i];
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
