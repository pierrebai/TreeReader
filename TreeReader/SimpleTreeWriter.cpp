#include "SimpleTreeWriter.h"
#include "TextTreeVisitor.h"

#include <fstream>

namespace TreeReader
{
   using namespace std;

   std::wostream& PrintTree(std::wostream& stream, const TextTree& tree, const std::wstring& indentation)
   {
      VisitInOrder(tree, [&stream, &indentation](const TextTree& tree, const TextTree::Node& node, size_t level)
      {
         for (size_t indent = 0; indent < level; ++indent)
            stream << indentation;

         stream << node.TextPtr << L"\n";

         return TreeVisitor::Result();
      });
      return stream;
   }

   wostream& operator<<(wostream& stream, const TextTree& tree)
   {
      return PrintTree(stream, tree);
   }

   void WriteSimpleTextTree(const std::filesystem::path& path, const TextTree& tree, const std::wstring& indentation)
   {
      wofstream stream(path);
      PrintTree(stream, tree, indentation);
   }

   void WriteSimpleTextTree(std::wostream& stream, const TextTree& tree, const std::wstring& indentation)
   {
      PrintTree(stream, tree, indentation);
   }
}
