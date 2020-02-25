#include "TreeReader.h"

#include <iostream>

using namespace std;
using namespace TreeReader;

int wmain(int argc, wchar_t** argv)
{
   CommandsContext ctx;

   wstring programName = argc > 0 ? argv[0] : L"TreeFilter";
   vector<wstring> args(argv + min(1, argc), argv + argc);

   while (true)
   {
      wstring result = ParseCommands(args, ctx);
      if (!result.empty())
         wcout << result << endl;

      if (ctx.Filtered)
         PrintTree(wcout, *ctx.Filtered, ctx.Options.OutputLineIndent) << endl;

      if (!ctx.Options.IsInteractive)
         break;

      wcout << L"New arguments: "; 
      wchar_t buffer[256];
      wcin.getline(buffer, sizeof(buffer)/sizeof(buffer[0]));
      if (!wcin || !buffer[0])
         break;
      args = split(buffer);
   }
}
