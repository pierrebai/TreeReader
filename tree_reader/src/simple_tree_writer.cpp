#include "dak/tree_reader/simple_tree_writer.h"
#include "dak/tree_reader/text_tree_visitor.h"

#include <fstream>

namespace dak::tree_reader
{
   using namespace std;

   std::wostream& print_tree(std::wostream& stream, const text_tree& tree, const std::wstring& indentation)
   {
      visit_in_order(tree, [&stream, &indentation](const text_tree& tree, const text_tree::node& node, size_t level)
      {
         for (size_t indent = 0; indent < level; ++indent)
            stream << indentation;

         stream << node.text_ptr << L"\n";

         return tree_visitor::result();
      });
      return stream;
   }

   wostream& operator<<(wostream& stream, const text_tree& tree)
   {
      return print_tree(stream, tree);
   }

   void save_simple_text_tree(const std::filesystem::path& path, const text_tree& tree, const std::wstring& indentation)
   {
      wofstream stream(path);
      print_tree(stream, tree, indentation);
   }

   void save_simple_text_tree(std::wostream& stream, const text_tree& tree, const std::wstring& indentation)
   {
      print_tree(stream, tree, indentation);
   }
}
