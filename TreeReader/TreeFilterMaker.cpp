#include "TreeFilterMaker.h"

#include <sstream>
#include <iomanip>

namespace TreeReader
{
   using namespace std;

   namespace V1
   {
      wstring ConvertFilterToText(const TreeFilterPtr& filter);

      wstring ConvertFilterToText(const AcceptTreeFilter& filter)
      {
         wostringstream sstream;
         sstream << L"accept [ ]";
         return sstream.str();
      }

      wstring ConvertFilterToText(const ContainsTreeFilter& filter)
      {
         wostringstream sstream;
         sstream << L"contains [ " << quoted(filter.Contained) << L" ]";
         return sstream.str();
      }

      wstring ConvertFilterToText(const RegexTreeFilter& filter)
      {
         wostringstream sstream;
         // sstream << L"regex [ " << quoted(filter.Regex) << L" ]"; // TODO: keep original non-regex form.
         sstream << L"regex [" << L"]";
         return sstream.str();
      }

      wstring ConvertFilterToText(const NotTreeFilter& filter)
      {
         wostringstream sstream;
         sstream << L"not [ " << ConvertFilterToText(filter.Filter) << L" ]";
         return sstream.str();
      }

      wstring ConvertFiltersToText(std::vector<TreeFilterPtr> filters)
      {
         wostringstream sstream;

         auto begin = filters.begin();
         const auto end = filters.end();

         if (begin != end)
         {
            sstream << ConvertFilterToText(*begin);
            ++begin;
         }

         while (begin != end)
         {
            sstream << L", ";
            sstream << ConvertFilterToText(*begin);
            ++begin;
         }

         return sstream.str();
      }

      wstring ConvertFilterToText(const OrTreeFilter& filter)
      {
         wostringstream sstream;
         sstream << L"or [ " << ConvertFiltersToText(filter.Filters) << L" ]";
         return sstream.str();
      }

      wstring ConvertFilterToText(const AndTreeFilter& filter)
      {
         wostringstream sstream;
         sstream << L"and [ " << ConvertFiltersToText(filter.Filters) << L" ]";
         return sstream.str();
      }

      wstring ConvertFilterToText(const ApplyUnderTreeFilter& filter)
      {
         wostringstream sstream;
         sstream << L"under [ " << boolalpha << filter.IncludeSelf << L", " << ConvertFilterToText(filter.Under) << L", " << ConvertFilterToText(filter.Filter) << L" ]";
         return sstream.str();
      }

      wstring ConvertFilterToText(const RemoveChildrenTreeFilter& filter)
      {
         wostringstream sstream;
         sstream << L"no-child [ " << boolalpha << filter.RemoveSelf << L", " << ConvertFilterToText(filter.Filter) << L" ]";
         return sstream.str();
      }

      wstring ConvertFilterToText(const LevelRangeTreeFilter& filter)
      {
         wostringstream sstream;
         sstream << L"range [ " << filter.MinLevel << L", " << filter.MaxLevel << L" ]";
         return sstream.str();
      }

      wstring ConvertFilterToText(const TreeFilter& filter)
      {
         #define CALL_CONVERTER(a) if (const a* ptr = dynamic_cast<const a *>(&filter)) { return ConvertFilterToText(*ptr); }

         CALL_CONVERTER(AcceptTreeFilter)
         CALL_CONVERTER(ContainsTreeFilter)
         CALL_CONVERTER(RegexTreeFilter)
         CALL_CONVERTER(NotTreeFilter)
         CALL_CONVERTER(OrTreeFilter)
         CALL_CONVERTER(AndTreeFilter)
         CALL_CONVERTER(ApplyUnderTreeFilter)
         CALL_CONVERTER(RemoveChildrenTreeFilter)
         CALL_CONVERTER(LevelRangeTreeFilter)

         #undef CALL_CONVERTER

         return {};
      }

      wstring ConvertFilterToText(const TreeFilterPtr& filter)
      {
         if (!filter)
            return {};

         return ConvertFilterToText(*filter);
      }

      wstring ConvertFiltersToText(const TreeFilterPtr& filter)
      {
         wostringstream sstream;
         sstream << L"V1: " << ConvertFilterToText(filter);
         return sstream.str();
      }

      template <class T>
      TreeFilterPtr ConvertTextToFilter(wistringstream& sstream);

      TreeFilterPtr ConvertTextToFilter(wistringstream& sstream);

      void EatClosingBrace(wistringstream& sstream)
      {
         // Eat the closing ']'.
         wchar_t brace;
         sstream >> skipws >> brace;
         // TODO: assert that it is a ']'
      }

      template <>
      TreeFilterPtr ConvertTextToFilter<AcceptTreeFilter>(wistringstream& sstream)
      {
         EatClosingBrace(sstream);
         return All();
      }

      template <>
      TreeFilterPtr ConvertTextToFilter<ContainsTreeFilter>(wistringstream& sstream)
      {
         wstring contained;
         sstream >> skipws >> quoted(contained);

         EatClosingBrace(sstream);

         return make_shared<ContainsTreeFilter>(contained);
      }

      template <>
      TreeFilterPtr ConvertTextToFilter<RegexTreeFilter>(wistringstream& sstream)
      {
         wregex regex;
         //sstream >> skipws >> quoted(regex);

         EatClosingBrace(sstream);

         return make_shared<RegexTreeFilter>(regex);
      }

      template <>
      TreeFilterPtr ConvertTextToFilter<NotTreeFilter>(wistringstream& sstream)
      {
         auto filter = ConvertTextToFilter(sstream);

         EatClosingBrace(sstream);

         return make_shared<NotTreeFilter>(filter);
      }

      std::vector<TreeFilterPtr> ConvertTextToMultiFilters(wistringstream& sstream)
      {
         std::vector<TreeFilterPtr> filters;

         while (true)
         {
            wchar_t comma;
            sstream >> skipws >> comma;
            if (comma != L',')
               sstream.putback(comma);
            if (comma == L']')
               break;

            auto filter = ConvertTextToFilter(sstream);
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
      TreeFilterPtr ConvertTextToFilter<ApplyUnderTreeFilter>(wistringstream& sstream)
      {
         bool includeSelf;
         wchar_t comma;
         sstream >> skipws >> boolalpha >> includeSelf >> skipws >> comma;

         auto under = ConvertTextToFilter(sstream);

         sstream >> skipws >> comma;
         auto filter = ConvertTextToFilter(sstream);

         EatClosingBrace(sstream);

         return make_shared<ApplyUnderTreeFilter>(under, filter, includeSelf);
      }

      template <>
      TreeFilterPtr ConvertTextToFilter<RemoveChildrenTreeFilter>(wistringstream& sstream)
      {
         bool removeSelf;
         wchar_t comma;
         sstream >> skipws >> boolalpha >> removeSelf >> skipws >> comma;

         auto filter = ConvertTextToFilter(sstream);

         EatClosingBrace(sstream);

         return make_shared<RemoveChildrenTreeFilter>(filter, removeSelf);
      }

      template<>
      TreeFilterPtr ConvertTextToFilter<LevelRangeTreeFilter>(wistringstream& sstream)
      {
         size_t minLevel, maxLevel;
         wchar_t comma;
         sstream >> skipws >> minLevel >> skipws >> comma >> skipws >> maxLevel;

         EatClosingBrace(sstream);

         return make_shared<LevelRangeTreeFilter>(minLevel, maxLevel);
      }

      TreeFilterPtr ConvertTextToFilter(wistringstream& sstream)
      {
         #define CALL_CONVERTER(a,b) if (name == a) { return ConvertTextToFilter<b>(sstream); } else

         wstring name;
         wchar_t brace;
         sstream >> skipws >> name >> skipws >> brace;

         CALL_CONVERTER(L"accept", AcceptTreeFilter)
         CALL_CONVERTER(L"contains", ContainsTreeFilter)
         CALL_CONVERTER(L"regex", RegexTreeFilter)
         CALL_CONVERTER(L"not", NotTreeFilter)
         CALL_CONVERTER(L"or", OrTreeFilter)
         CALL_CONVERTER(L"and", AndTreeFilter)
         CALL_CONVERTER(L"under", ApplyUnderTreeFilter)
         CALL_CONVERTER(L"no-child", RemoveChildrenTreeFilter)
         CALL_CONVERTER(L"range", LevelRangeTreeFilter)

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
         return V1::ConvertTextToFilter(sstream);
      }

      return {};
   }
}
