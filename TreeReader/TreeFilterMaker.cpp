#include "TreeFilterMaker.h"

#include <sstream>
#include <iomanip>

namespace TreeReader
{
   using namespace std;

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

      wstring ConvertFilterToText(const UntilTreeFilter& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"until [ " << ConvertFilterToText(filter.Filter, indent + 1) << L" ]";
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

      wstring ConvertFilterToText(const CountChildrenTreeFilter& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"count-sub [ " << boolalpha << filter.IncludeSelf << L", " << filter.Count << ", " << ConvertFilterToText(filter.Filter, indent + 1) << L" ]";
         return sstream.str();
      }

      wstring ConvertFilterToText(const CountSiblingsTreeFilter& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"count-sib [ " << boolalpha << filter.IncludeSelf << L", " << filter.Count << ", " << ConvertFilterToText(filter.Filter, indent + 1) << L" ]";
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
         sstream << L"no-child [ " << boolalpha << filter.IncludeSelf << L", " << ConvertFilterToText(filter.Filter, indent + 1) << L" ]";
         return sstream.str();
      }

      wstring ConvertFilterToText(const LevelRangeTreeFilter& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"range [ " << filter.MinLevel << L", " << filter.MaxLevel << L" ]";
         return sstream.str();
      }

      wstring ConvertFilterToText(const NamedTreeFilter& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"named [ " << quoted(filter.Name) << L" ]";
         return sstream.str();
      }

      wstring ConvertFilterToText(const TreeFilter& filter, size_t indent)
      {
         wstring indentText = wstring(L"\n") + wstring(indent, L' ');
         #define CALL_CONVERTER(a) if (const a* ptr = dynamic_cast<const a *>(&filter)) { return indentText + ConvertFilterToText(*ptr, indent); }

         CALL_CONVERTER(AcceptTreeFilter)
         CALL_CONVERTER(StopTreeFilter)
         CALL_CONVERTER(UntilTreeFilter)
         CALL_CONVERTER(ContainsTreeFilter)
         CALL_CONVERTER(RegexTreeFilter)
         CALL_CONVERTER(NotTreeFilter)
         CALL_CONVERTER(IfSubTreeTreeFilter)
         CALL_CONVERTER(IfSiblingTreeFilter)
         CALL_CONVERTER(CountChildrenTreeFilter)
         CALL_CONVERTER(CountSiblingsTreeFilter)
         CALL_CONVERTER(OrTreeFilter)
         CALL_CONVERTER(AndTreeFilter)
         CALL_CONVERTER(UnderTreeFilter)
         CALL_CONVERTER(RemoveChildrenTreeFilter)
         CALL_CONVERTER(LevelRangeTreeFilter)
         CALL_CONVERTER(NamedTreeFilter)

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
         return Accept();
      }

      template <>
      TreeFilterPtr ConvertTextToFilter<StopTreeFilter>(wistringstream& sstream)
      {
         EatClosingBrace(sstream);
         return Stop();
      }

      template <>
      TreeFilterPtr ConvertTextToFilter<UntilTreeFilter>(wistringstream& sstream)
      {
         auto filter = ConvertTextToFilters(sstream);

         EatClosingBrace(sstream);

         return Until(filter);
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

      template <>
      TreeFilterPtr ConvertTextToFilter<CountChildrenTreeFilter>(wistringstream& sstream)
      {
         bool includeSelf;
         wchar_t comma;
         sstream >> skipws >> boolalpha >> includeSelf >> skipws >> comma;

         size_t count;
         sstream >> skipws >> count >> skipws >> comma;

         auto filter = ConvertTextToFilters(sstream);

         EatClosingBrace(sstream);

         return CountChildren(filter, count, includeSelf);
      }

      template <>
      TreeFilterPtr ConvertTextToFilter<CountSiblingsTreeFilter>(wistringstream& sstream)
      {
         bool includeSelf;
         wchar_t comma;
         sstream >> skipws >> boolalpha >> includeSelf >> skipws >> comma;

         size_t count;
         sstream >> skipws >> count >> skipws >> comma;

         auto filter = ConvertTextToFilters(sstream);

         EatClosingBrace(sstream);

         return CountSiblings(filter, count, includeSelf);
      }

      vector<TreeFilterPtr> ConvertTextToMultiFilters(wistringstream& sstream)
      {
         vector<TreeFilterPtr> filters;

         while (sstream)
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

         return Any(filters);
      }

      template <>
      TreeFilterPtr ConvertTextToFilter<AndTreeFilter>(wistringstream& sstream)
      {
         auto filters = ConvertTextToMultiFilters(sstream);

         EatClosingBrace(sstream);

         return All(filters);
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

      template<>
      TreeFilterPtr ConvertTextToFilter<NamedTreeFilter>(wistringstream& sstream)
      {
         wstring name;
         sstream >> skipws >> quoted(name);

         EatClosingBrace(sstream);

         return Named(name);
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
         CALL_CONVERTER(L"count-sub", CountChildrenTreeFilter)
         CALL_CONVERTER(L"count-sib", CountSiblingsTreeFilter)
         CALL_CONVERTER(L"or", OrTreeFilter)
         CALL_CONVERTER(L"and", AndTreeFilter)
         CALL_CONVERTER(L"under", UnderTreeFilter)
         CALL_CONVERTER(L"no-child", RemoveChildrenTreeFilter)
         CALL_CONVERTER(L"range", LevelRangeTreeFilter)
         CALL_CONVERTER(L"stop", StopTreeFilter)
         CALL_CONVERTER(L"until", UntilTreeFilter)
         CALL_CONVERTER(L"named", NamedTreeFilter)

         #undef CALL_CONVERTER

         return {};
      }

      TreeFilterPtr ConvertTextToFilters(wistringstream& sstream, const NamedFilters& named)
      {
         TreeFilterPtr filter = ConvertTextToFilters(sstream);
         UpdateNamedFilters(filter, named);
         return filter;
      }
   }

   wstring ConvertFiltersToText(const TreeFilterPtr& filter)
   {
      return V1::ConvertFiltersToText(filter);
   }

   TreeFilterPtr ConvertTextToFilters(const wstring& text, const NamedFilters& named)
   {
      if (text.find(L"V1: ") == 0)
      {
         wistringstream sstream(text.substr(4));
         return V1::ConvertTextToFilters(sstream, named);
      }

      return {};
   }
}
