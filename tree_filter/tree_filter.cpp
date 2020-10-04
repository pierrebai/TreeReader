#include "dak/tree_reader/tree_filter_command_line.h"
#include "dak/tree_reader/tree_commands.h"
#include "dak/utility/text.h"

#include <iostream>

using namespace std;
using namespace dak::tree_reader;
using namespace dak::utility;

int wmain(int argc, wchar_t** argv)
{
   command_line_t ctx;

   wstring program_name = argc > 0 ? argv[0] : L"tree_filter";
   vector<wstring> args(argv + min(1, argc), argv + argc);

   while (true)
   {
      wstring result = ctx.parse_commands(args);
      if (!result.empty())
         wcout << result << endl;

      if (ctx.current_tree->get_filtered_tree())
         print_tree(wcout, *ctx.current_tree->get_filtered_tree(), ctx.options.output_line_indent) << endl;

      if (!ctx.is_interactive)
         break;

      wcout << L"New arguments: "; 
      wchar_t buffer[256];
      wcin.getline(buffer, sizeof(buffer)/sizeof(buffer[0]));
      if (!wcin || !buffer[0])
         break;
      args = split(buffer);
   }
}
