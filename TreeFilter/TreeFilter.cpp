#include "TreeFilterCommandLine.h"
#include "TreeReaderHelpers.h"

#include <iostream>

using namespace std;
using namespace TreeReader;

int wmain(int argc, wchar_t** argv)
{
   CommandLine ctx;

   wstring programName = argc > 0 ? argv[0] : L"TreeFilter";
   vector<wstring> args(argv + min(1, argc), argv + argc);

   while (true)
   {
      wstring result = ctx.ParseCommands(args);
      if (!result.empty())
         wcout << result << endl;

      // TODO
      //if (ctx.GetFilteredTree())
      //   PrintTree(wcout, *ctx.GetFilteredTree(), ctx.Options.OutputLineIndent) << endl;

      if (!ctx.IsInteractive)
         break;

      wcout << L"New arguments: "; 
      wchar_t buffer[256];
      wcin.getline(buffer, sizeof(buffer)/sizeof(buffer[0]));
      if (!wcin || !buffer[0])
         break;
      args = Split(buffer);
   }
}
