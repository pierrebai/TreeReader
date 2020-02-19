#include "TreeReader.h"

#include <iostream>

using namespace std;
using namespace TreeReader;

int wmain(int argc, wchar_t** argv)
{
   if (argc < 3)
   {
      wcerr << L"Usage: " << argv[0] << L" <tree-file> <filter-description>" << std::endl;
      return 1;
   }

   TreeFilterPtr filter = ConvertTextToFilters(argv[2]);
   if (!filter)
   {
      wcerr << L"Invalid filter: " << argv[2] << std::endl;
      return 1;
   }

   TextTree tree = ReadSimpleTextTree(filesystem::path(argv[1]));

   TextTree filtered;
   FilterTree(tree, filtered, filter);

   wcout << filtered << endl;
}
