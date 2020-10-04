#include "dak/tree_reader/text_tree.h"
#include "dak/tree_reader/text_lines_text_holder.h"

namespace dak::tree_reader_tests
{
	std::shared_ptr<tree_reader::text_lines_text_holder_t> create_text_lines();
	tree_reader::text_tree_t create_simple_tree();
}
