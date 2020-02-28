#include "TreeFilterMaker.h"
#include "TreeREaderHelpers.h"

#include <sstream>
#include <iomanip>

namespace TreeReader
{
   using namespace std;

   namespace S1
   {
      TreeFilterPtr ConvertTextToFilters(vector<wstring>& parts)
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

         while (parts.size() > 0)
         {
            const wstring part = move(parts.back());
            parts.pop_back();

            if (part == L"!" || part == L"not")
            {
               auto filter = make_shared<NotTreeFilter>();
               AddFilter(filter);
               neededFilter = &filter->Filter;
            }
            else if (part == L"@")
            {
               if (parts.size() > 0)
               {
                  const wstring name = move(parts.back());
                  parts.pop_back();
                  auto filter = Named(name);
                  AddFilter(filter);
               }
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
               auto filter = ConvertTextToFilters(parts);
               AddFilter(filter);
            }
            else if (part == L")")
            {
               // Assume we were recursed into.
               break;
            }
            else if (part.front() == L'\"')
            {
               if (part.back() == L'\"')
               {
                  wstring text = part.substr(1, part.size() - 2);
                  auto filter = Contains(text);
                  AddFilter(filter);
               }
            }
            else if (part == L"<=" || part == L"max")
            {
               if (parts.size() > 0)
               {
                  const wstring countText = move(parts.back());
                  parts.pop_back();

                  wistringstream sstream(countText);
                  size_t count;
                  sstream >> count;

                  auto filter = MaxLevel(count);
                  AddFilter(filter);
               }
            }
            else if (part == L">" || part == L"under")
            {
               auto filter = make_shared<UnderTreeFilter>();
               AddFilter(filter);
               neededFilter = &filter->Filter;
            }
            else if (part == L"#>" || part == L"count-children")
            {
               if (parts.size() > 0)
               {
                  const wstring countText = move(parts.back());
                  parts.pop_back();

                  wistringstream sstream(countText);
                  size_t count;
                  sstream >> count;

                  auto filter = make_shared<CountChildrenTreeFilter>();
                  filter->Count = count;
                  AddFilter(filter);
                  neededFilter = &filter->Filter;
               }
            }
            else if (part == L"#=" || part == L"count-siblings")
            {
               if (parts.size() > 0)
               {
                  const wstring countText = move(parts.back());
                  parts.pop_back();

                  wistringstream sstream(countText);
                  size_t count;
                  sstream >> count;

                  auto filter = make_shared<CountSiblingsTreeFilter>();
                  filter->Count = count;
                  AddFilter(filter);
                  neededFilter = &filter->Filter;
               }
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
         vector<wstring> parts = split(text);
         reverse(parts.begin(), parts.end());
         TreeFilterPtr filter = ConvertTextToFilters(parts);
         UpdateNamedFilters(filter, named);
         return filter;
      }
   }

   TreeFilterPtr ConvertSimpleTextToFilters(const wstring& text, const NamedFilters& named)
   {
      return S1::ConvertTextToFilters(text, named);
   }
}
