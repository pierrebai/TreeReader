#include "dak/tree_reader/simple_tree_writer.h"
#include "dak/tree_reader/text_tree_visitor.h"

#include <fstream>

namespace dak::tree_reader
{
   using namespace std;

   std::wostream& print_tree(std::wostream& stream, const text_tree_t& tree, const std::wstring& indentation)
   {
      visit_in_order(tree, [&stream, &indentation](const text_tree_t& tree, const text_tree_t::node_t& node, size_t level)
      {
         for (size_t indent = 0; indent < level; ++indent)
            stream << indentation;

         stream << node.text_ptr << L"\n";

         return tree_visitor_t::result_t();
      });
      return stream;
   }

   wostream& operator<<(wostream& stream, const text_tree_t& tree)
   {
      return print_tree(stream, tree);
   }

   void save_simple_text_tree(const std::filesystem::path& path, const text_tree_t& tree, const std::wstring& indentation)
   {
      wofstream stream(path);
      print_tree(stream, tree, indentation);
   }

   void save_simple_text_tree(std::wostream& stream, const text_tree_t& tree, const std::wstring& indentation)
   {
      print_tree(stream, tree, indentation);
   }
}
