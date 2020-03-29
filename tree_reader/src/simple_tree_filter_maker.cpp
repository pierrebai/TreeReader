#include "dak/tree_reader/tree_filter_maker.h"
#include "dak/utility/text.h"

#include <sstream>
#include <iomanip>

namespace dak::tree_reader
{
   using namespace std;

   namespace S1
   {
      pair<wstring, bool> get_next_part(wistream& stream)
      {
         wchar_t first_letter = 0;
         do
         {
            stream.get(first_letter);
         } while (stream && first_letter == L' ');
         stream.unget();

         wstring part;
         bool was_quoted = false;
         if (first_letter == L'"')
         {
            was_quoted = true;
            stream >> quoted(part);
         }
         else
         {
            stream >> part;
         }

         return make_pair(part, was_quoted);
      }

      size_t get_next_count(wistream& stream)
      {
         const auto [countText, countQuoted] = get_next_part(stream);

         wistringstream sstream(countText);
         size_t count;
         sstream >> count;

         return count;
      }

      tree_filter_ptr convert_text_to_filter(wistream& stream, const named_filters& named)
      {
         tree_filter_ptr result;
         tree_filter_ptr previous;
         tree_filter_ptr* needed_filter = nullptr;
         shared_ptr<combine_tree_filter> current_combiner;

         auto add_filter = [&](tree_filter_ptr filter, bool asCombiner = false)
         {
            if (needed_filter)
            {
               *needed_filter = filter;
               needed_filter = nullptr;
            }
            else if (current_combiner)
            {
               current_combiner->named_filters.push_back(filter);
            }
            else if (previous)
            {
               if (auto combiner = dynamic_pointer_cast<combine_tree_filter>(filter); asCombiner && combiner)
               {
                  current_combiner = combiner;
                  filter = nullptr;
               }
               else
               {
                  current_combiner = make_shared<and_tree_filter>();
               }

               current_combiner->named_filters.push_back(previous);
               previous = nullptr;

               if (filter)
                  current_combiner->named_filters.push_back(filter);

               if (!result)
                  result = current_combiner;
            }
            else
            {
               if (auto combiner = dynamic_pointer_cast<combine_tree_filter>(filter); asCombiner && combiner)
               {
                  current_combiner = combiner;
                  result = current_combiner;
               }
               else
               {
                  previous = filter;
               }
            }
         };

         while (stream)
         {
            auto [part, was_quoted] = get_next_part(stream);
            if (part.empty())
               continue;

            if (was_quoted)
            {
               auto filter = contains(part);
               add_filter(filter);
            }
            else if (part == L"!" || part == L"not")
            {
               auto filter = make_shared<not_tree_filter>();
               add_filter(filter);
               needed_filter = &filter->filter;
            }
            else if (part[0] == L'@')
            {
               const wstring name = part.substr(1);
               auto filter = named.get(name);
               add_filter(filter);
            }
            else if (part == L"?=" || part == L"sibling")
            {
               auto filter = make_shared<if_sibling_tree_filter>();
               add_filter(filter);
               needed_filter = &filter->filter;
            }
            else if (part == L"?>" || part == L"child")
            {
               auto filter = make_shared<if_subtree_tree_filter>();
               add_filter(filter);
               needed_filter = &filter->filter;
            }
            else if (part == L"|" || part == L"or" || part == L"any")
            {
               if (!dynamic_pointer_cast<or_tree_filter>(current_combiner))
               {
                  auto filter = make_shared<or_tree_filter>();
                  add_filter(filter, true);
               }
            }
            else if (part == L"&" || part == L"and" || part == L"all")
            {
               if (!dynamic_pointer_cast<and_tree_filter>(current_combiner))
               {
                  auto filter = make_shared<and_tree_filter>();
                  add_filter(filter, true);
               }
            }
            else if (part == L"(")
            {
               auto filter = convert_text_to_filter(stream, named);
               add_filter(filter);
            }
            else if (part == L")")
            {
               // Assume we were recursed into.
               break;
            }
            else if (part == L"<=" || part == L"max")
            {
               auto filter = max_level(get_next_count(stream));
               add_filter(filter);
            }
            else if (part == L">" || part == L"under")
            {
               auto filter = make_shared<under_tree_filter>();
               add_filter(filter);
               needed_filter = &filter->filter;
            }
            else if (part == L"." || part == L"stop")
            {
               add_filter(stop());
            }
            else if (part == L"~" || part == L"until")
            {
               auto filter = make_shared<until_tree_filter>();
               add_filter(filter);
               needed_filter = &filter->filter;
            }
            else if (part == L"*" || part == L"accept")
            {
               add_filter(accept());
            }
            else if (part == L"unique")
            {
               add_filter(unique());
            }
            else
            {
               auto filter = contains(part);
               add_filter(filter);
            }
         }

         return result ? result : previous;
      }

      tree_filter_ptr convert_text_to_filter(const wstring& text, const named_filters& named)
      {
         wistringstream stream(text);
         tree_filter_ptr filter = convert_text_to_filter(stream, named);
         update_named_filters(filter, named);
         return filter;
      }
   }

   tree_filter_ptr convert_simple_text_to_filter(const wstring& text, const named_filters& named)
   {
      return S1::convert_text_to_filter(text, named);
   }
}
