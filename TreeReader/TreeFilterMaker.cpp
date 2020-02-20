#include "TreeFilterMaker.h"

#include <sstream>
#include <iomanip>

namespace TreeReader
{
   using namespace std;

   namespace
   {
      vector<wstring> split(const wstring& s, wchar_t delim = L' ')
      {
         vector<wstring> result;
         wistringstream iss(s);
         wstring item;
         while (getline(iss, item, delim))
            result.emplace_back(item);
         return result;
      }

      wstring join(const vector<wstring>& parts, wchar_t delim = L' ')
      {
         wstring result;

         auto pos = parts.begin();
         const auto end = parts.end();
         if (pos != end)
         {
            result += *pos++;
         }

         while (pos != end)
         {
            result += delim;
            result += *pos++;
         }

         return result;
      }
   }

   namespace V1
   {
      wstring ConvertFilterToText(const TreeFilterPtr& filter, size_t indent);

      wstring ConvertFilterToText(const AcceptTreeFilter& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"accept [ ]";
         return sstream.str();
      }

      wstring ConvertFilterToText(const StopTreeFilter& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"stop [ ]";
         return sstream.str();
      }

      wstring ConvertFilterToText(const ContainsTreeFilter& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"contains [ " << quoted(filter.Contained) << L" ]";
         return sstream.str();
      }

      wstring ConvertFilterToText(const RegexTreeFilter& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"regex [ " << quoted(filter.RegexTextForm) << L" ]";
         return sstream.str();
      }

      wstring ConvertFilterToText(const NotTreeFilter& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"not [ " << ConvertFilterToText(filter.Filter, indent + 1) << L" ]";
         return sstream.str();
      }

      wstring ConvertFilterToText(const IfSubTreeTreeFilter& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"if-sub [ " << ConvertFilterToText(filter.Filter, indent + 1) << L" ]";
         return sstream.str();
      }

      wstring ConvertFilterToText(const IfSiblingTreeFilter& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"if-sib [ " << ConvertFilterToText(filter.Filter, indent + 1) << L" ]";
         return sstream.str();
      }

      wstring ConvertFiltersToText(vector<TreeFilterPtr> filters, size_t indent)
      {
         wostringstream sstream;

         auto begin = filters.begin();
         const auto end = filters.end();

         if (begin != end)
         {
            sstream << ConvertFilterToText(*begin, indent);
            ++begin;
         }

         while (begin != end)
         {
            sstream << L", ";
            sstream << ConvertFilterToText(*begin, indent);
            ++begin;
         }

         return sstream.str();
      }

      wstring ConvertFilterToText(const OrTreeFilter& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"or [ " << ConvertFiltersToText(filter.Filters, indent + 1) << L" ]";
         return sstream.str();
      }

      wstring ConvertFilterToText(const AndTreeFilter& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"and [ " << ConvertFiltersToText(filter.Filters, indent + 1) << L" ]";
         return sstream.str();
      }

      wstring ConvertFilterToText(const UnderTreeFilter& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"under [ " << boolalpha << filter.IncludeSelf << L", " << ConvertFilterToText(filter.Filter, indent + 1) << L" ]";
         return sstream.str();
      }

      wstring ConvertFilterToText(const RemoveChildrenTreeFilter& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"no-child [ " << boolalpha << filter.RemoveSelf << L", " << ConvertFilterToText(filter.Filter, indent + 1) << L" ]";
         return sstream.str();
      }

      wstring ConvertFilterToText(const LevelRangeTreeFilter& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"range [ " << filter.MinLevel << L", " << filter.MaxLevel << L" ]";
         return sstream.str();
      }

      wstring ConvertFilterToText(const TreeFilter& filter, size_t indent)
      {
         wstring indentText = wstring(L"\n") + wstring(indent, L' ');
         #define CALL_CONVERTER(a) if (const a* ptr = dynamic_cast<const a *>(&filter)) { return indentText + ConvertFilterToText(*ptr, indent); }

         CALL_CONVERTER(AcceptTreeFilter)
         CALL_CONVERTER(StopTreeFilter)
         CALL_CONVERTER(ContainsTreeFilter)
         CALL_CONVERTER(RegexTreeFilter)
         CALL_CONVERTER(NotTreeFilter)
         CALL_CONVERTER(IfSubTreeTreeFilter)
         CALL_CONVERTER(IfSiblingTreeFilter)
         CALL_CONVERTER(OrTreeFilter)
         CALL_CONVERTER(AndTreeFilter)
         CALL_CONVERTER(UnderTreeFilter)
         CALL_CONVERTER(RemoveChildrenTreeFilter)
         CALL_CONVERTER(LevelRangeTreeFilter)

         #undef CALL_CONVERTER

         return {};
      }

      wstring ConvertFilterToText(const TreeFilterPtr& filter, size_t indent)
      {
         if (!filter)
            return {};

         return ConvertFilterToText(*filter, indent);
      }

      wstring ConvertFiltersToText(const TreeFilterPtr& filter)
      {
         wostringstream sstream;
         sstream << L"V1: " << ConvertFilterToText(filter, 0);
         return sstream.str();
      }

      template <class T>
      TreeFilterPtr ConvertTextToFilter(wistringstream& sstream);

      TreeFilterPtr ConvertTextToFilters(wistringstream& sstream);

      void EatClosingBrace(wistringstream& sstream)
      {
         // Eat the closing ']'.
         wchar_t brace;
         sstream >> skipws >> brace;
      }

      template <>
      TreeFilterPtr ConvertTextToFilter<AcceptTreeFilter>(wistringstream& sstream)
      {
         EatClosingBrace(sstream);
         return All();
      }

      template <>
      TreeFilterPtr ConvertTextToFilter<StopTreeFilter>(wistringstream& sstream)
      {
         EatClosingBrace(sstream);
         return Stop();
      }

      template <>
      TreeFilterPtr ConvertTextToFilter<ContainsTreeFilter>(wistringstream& sstream)
      {
         wstring contained;
         sstream >> skipws >> quoted(contained);

         EatClosingBrace(sstream);

         return Contains(contained);
      }

      template <>
      TreeFilterPtr ConvertTextToFilter<RegexTreeFilter>(wistringstream& sstream)
      {
         wstring regex;
         sstream >> skipws >> quoted(regex);

         EatClosingBrace(sstream);

         return Regex(regex);
      }

      template <>
      TreeFilterPtr ConvertTextToFilter<NotTreeFilter>(wistringstream& sstream)
      {
         auto filter = ConvertTextToFilters(sstream);

         EatClosingBrace(sstream);

         return Not(filter);
      }

      template <>
      TreeFilterPtr ConvertTextToFilter<IfSubTreeTreeFilter>(wistringstream& sstream)
      {
         auto filter = ConvertTextToFilters(sstream);

         EatClosingBrace(sstream);

         return IfSubTree(filter);
      }

      template <>
      TreeFilterPtr ConvertTextToFilter<IfSiblingTreeFilter>(wistringstream& sstream)
      {
         auto filter = ConvertTextToFilters(sstream);

         EatClosingBrace(sstream);

         return IfSibling(filter);
      }

      vector<TreeFilterPtr> ConvertTextToMultiFilters(wistringstream& sstream)
      {
         vector<TreeFilterPtr> filters;

         while (true)
         {
            wchar_t comma;
            sstream >> skipws >> comma;
            if (comma != L',')
               sstream.putback(comma);
            if (comma == L']')
               break;

            auto filter = ConvertTextToFilters(sstream);
            if (filter)
               filters.push_back(filter);
         }

         return filters;
      }

      template <>
      TreeFilterPtr ConvertTextToFilter<OrTreeFilter>(wistringstream& sstream)
      {
         auto filters = ConvertTextToMultiFilters(sstream);

         EatClosingBrace(sstream);

         return make_shared<OrTreeFilter>(filters);
      }

      template <>
      TreeFilterPtr ConvertTextToFilter<AndTreeFilter>(wistringstream& sstream)
      {
         auto filters = ConvertTextToMultiFilters(sstream);

         EatClosingBrace(sstream);

         return make_shared<AndTreeFilter>(filters);
      }

      template <>
      TreeFilterPtr ConvertTextToFilter<UnderTreeFilter>(wistringstream& sstream)
      {
         bool includeSelf;
         wchar_t comma;
         sstream >> skipws >> boolalpha >> includeSelf >> skipws >> comma;

         auto filter = ConvertTextToFilters(sstream);

         EatClosingBrace(sstream);

         return Under(filter, includeSelf);
      }

      template <>
      TreeFilterPtr ConvertTextToFilter<RemoveChildrenTreeFilter>(wistringstream& sstream)
      {
         bool removeSelf;
         wchar_t comma;
         sstream >> skipws >> boolalpha >> removeSelf >> skipws >> comma;

         auto filter = ConvertTextToFilters(sstream);

         EatClosingBrace(sstream);

         return NoChild(filter, removeSelf);
      }

      template<>
      TreeFilterPtr ConvertTextToFilter<LevelRangeTreeFilter>(wistringstream& sstream)
      {
         size_t minLevel, maxLevel;
         wchar_t comma;
         sstream >> skipws >> minLevel >> skipws >> comma >> skipws >> maxLevel;

         EatClosingBrace(sstream);

         return LevelRange(minLevel, maxLevel);
      }

      TreeFilterPtr ConvertTextToFilters(wistringstream& sstream)
      {
         #define CALL_CONVERTER(a,b) if (name == a) { return ConvertTextToFilter<b>(sstream); } else

         wstring name;
         wchar_t brace;
         sstream >> skipws >> name >> skipws >> brace;

         CALL_CONVERTER(L"accept", AcceptTreeFilter)
         CALL_CONVERTER(L"contains", ContainsTreeFilter)
         CALL_CONVERTER(L"regex", RegexTreeFilter)
         CALL_CONVERTER(L"not", NotTreeFilter)
         CALL_CONVERTER(L"if-sub", IfSubTreeTreeFilter)
         CALL_CONVERTER(L"if-sib", IfSiblingTreeFilter)
         CALL_CONVERTER(L"or", OrTreeFilter)
         CALL_CONVERTER(L"and", AndTreeFilter)
         CALL_CONVERTER(L"under", UnderTreeFilter)
         CALL_CONVERTER(L"no-child", RemoveChildrenTreeFilter)
         CALL_CONVERTER(L"range", LevelRangeTreeFilter)
         CALL_CONVERTER(L"stop", StopTreeFilter)

         #undef CALL_CONVERTER

         return {};
      }
   }

   wstring ConvertFiltersToText(const TreeFilterPtr& filter)
   {
      return V1::ConvertFiltersToText(filter);
   }

   TreeFilterPtr ConvertTextToFilters(const wstring& text)
   {
      if (text.starts_with(L"V1: "))
      {
         wistringstream sstream(text.substr(4));
         return V1::ConvertTextToFilters(sstream);
      }

      return {};
   }

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

            if (part == L"!")
            {
               auto filter = make_shared<NotTreeFilter>();
               AddFilter(filter);
               neededFilter = &filter->Filter;
            }
            else if (part == L"?=")
            {
               auto filter = make_shared<IfSiblingTreeFilter>();
               AddFilter(filter);
               neededFilter = &filter->Filter;
            }
            else if (part == L"?>")
            {
               auto filter = make_shared<IfSubTreeTreeFilter>();
               AddFilter(filter);
               neededFilter = &filter->Filter;
            }
            else if (part == L"|")
            {
               if (!dynamic_pointer_cast<OrTreeFilter>(currentCombiner))
               {
                  auto filter = make_shared<OrTreeFilter>();
                  AddFilter(filter, true);
               }
            }
            else if (part == L"&")
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
            else if (part.starts_with(L"\""))
            {
               if (part.ends_with(L"\""))
               {
                  wstring text = part.substr(1, part.size() - 2);
                  auto filter = Contains(text);
                  AddFilter(filter);
               }
            }
            else if (part == L"<=")
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
            else if (part == L">")
            {
               auto filter = make_shared<UnderTreeFilter>();
               AddFilter(filter);
               neededFilter = &filter->Filter;
            }
            else if (part == L"#")
            {
               AddFilter(Stop());
            }
            else if (part == L"*")
            {
               AddFilter(All());
            }
            else
            {
               auto filter = Contains(part);
               AddFilter(filter);
            }
         }

         return result ? result : previous;
      }

      TreeFilterPtr ConvertTextToFilters(const wstring& text)
      {
         vector<wstring> parts = split(text);
         reverse(parts.begin(), parts.end());
         return ConvertTextToFilters(parts);
      }
   }

   TreeFilterPtr ConvertSimpleTextToFilters(const wstring& text)
   {
      return S1::ConvertTextToFilters(text);
   }
}
