#include "TreeFilterMaker.h"
#include "TreeREaderHelpers.h"

#include <sstream>
#include <iomanip>

namespace TreeReader
{
   using namespace std;

   namespace S1
   {
      pair<wstring, bool> GetNextPart(wistream& stream)
      {
         wchar_t firstLetter = 0;
         do
         {
            stream.get(firstLetter);
         } while (stream && firstLetter == L' ');
         stream.unget();

         wstring part;
         bool wasQuoted = false;
         if (firstLetter == L'"')
         {
            wasQuoted = true;
            stream >> quoted(part);
         }
         else
         {
            stream >> part;
         }

         return make_pair(part, wasQuoted);
      }

      size_t GetNextCount(wistream& stream)
      {
         const auto [countText, countQuoted] = GetNextPart(stream);

         wistringstream sstream(countText);
         size_t count;
         sstream >> count;

         return count;
      }

      TreeFilterPtr ConvertTextToFilters(wistream& stream, const NamedFilters& named)
      {
         TreeFilterPtr result;
         TreeFilterPtr previous;
         TreeFilterPtr* neededFilter = nullptr;
         shared_ptr<CombineTreeFilter> currentCombiner;

         auto AddFilter = [&](TreeFilterPtr filter, bool asCombiner = false)
         {
            if (neededFilter)
            {
               *neededFilter = filter;
               neededFilter = nullptr;
            }
            else if (currentCombiner)
            {
               currentCombiner->Filters.push_back(filter);
            }
            else if (previous)
            {
               if (auto combiner = dynamic_pointer_cast<CombineTreeFilter>(filter); asCombiner && combiner)
               {
                  currentCombiner = combiner;
                  filter = nullptr;
               }
               else
               {
                  currentCombiner = make_shared<AndTreeFilter>();
               }

               currentCombiner->Filters.push_back(previous);
               previous = nullptr;

               if (filter)
                  currentCombiner->Filters.push_back(filter);

               if (!result)
                  result = currentCombiner;
            }
            else
            {
               if (auto combiner = dynamic_pointer_cast<CombineTreeFilter>(filter); asCombiner && combiner)
               {
                  currentCombiner = combiner;
                  result = currentCombiner;
               }
               else
               {
                  previous = filter;
               }
            }
         };

         while (stream)
         {
            auto [part, wasQuoted] = GetNextPart(stream);
            if (part.empty())
               continue;

            if (wasQuoted)
            {
               auto filter = Contains(part);
               AddFilter(filter);
            }
            else if (part == L"!" || part == L"not")
            {
               auto filter = make_shared<NotTreeFilter>();
               AddFilter(filter);
               neededFilter = &filter->Filter;
            }
            else if (part[0] == L'@')
            {
               const wstring name = part.substr(1);
               auto filter = named.Get(name);
               AddFilter(filter);
            }
            else if (part == L"?=" || part == L"sibling")
            {
               auto filter = make_shared<IfSiblingTreeFilter>();
               AddFilter(filter);
               neededFilter = &filter->Filter;
            }
            else if (part == L"?>" || part == L"child")
            {
               auto filter = make_shared<IfSubTreeTreeFilter>();
               AddFilter(filter);
               neededFilter = &filter->Filter;
            }
            else if (part == L"|" || part == L"or" || part == L"any")
            {
               if (!dynamic_pointer_cast<OrTreeFilter>(currentCombiner))
               {
                  auto filter = make_shared<OrTreeFilter>();
                  AddFilter(filter, true);
               }
            }
            else if (part == L"&" || part == L"and" || part == L"all")
            {
               if (!dynamic_pointer_cast<AndTreeFilter>(currentCombiner))
               {
                  auto filter = make_shared<AndTreeFilter>();
                  AddFilter(filter, true);
               }
            }
            else if (part == L"(")
            {
               auto filter = ConvertTextToFilters(stream, named);
               AddFilter(filter);
            }
            else if (part == L")")
            {
               // Assume we were recursed into.
               break;
            }
            else if (part == L"<=" || part == L"max")
            {
               auto filter = MaxLevel(GetNextCount(stream));
               AddFilter(filter);
            }
            else if (part == L">" || part == L"under")
            {
               auto filter = make_shared<UnderTreeFilter>();
               AddFilter(filter);
               neededFilter = &filter->Filter;
            }
            else if (part == L"." || part == L"stop")
            {
               AddFilter(Stop());
            }
            else if (part == L"~" || part == L"until")
            {
               auto filter = make_shared<UntilTreeFilter>();
               AddFilter(filter);
               neededFilter = &filter->Filter;
            }
            else if (part == L"*" || part == L"accept")
            {
               AddFilter(Accept());
            }
            else if (part == L"unique")
            {
               AddFilter(Unique());
            }
            else
            {
               auto filter = Contains(part);
               AddFilter(filter);
            }
         }

         return result ? result : previous;
      }

      TreeFilterPtr ConvertTextToFilters(const wstring& text, const NamedFilters& named)
      {
         wistringstream stream(text);
         TreeFilterPtr filter = ConvertTextToFilters(stream, named);
         UpdateNamedFilters(filter, named);
         return filter;
      }
   }

   TreeFilterPtr ConvertSimpleTextToFilters(const wstring& text, const NamedFilters& named)
   {
      return S1::ConvertTextToFilters(text, named);
   }
}
