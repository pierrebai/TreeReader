#include "dak/tree_reader/tree_filter_maker.h"

#include <sstream>
#include <iomanip>

namespace dak::tree_reader
{
   using namespace std;

   namespace V1
   {
      wstring ConvertFilterToText(const tree_filter_ptr& filter, size_t indent);

      wstring ConvertFilterToText(const accept_tree_filter& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"accept [ ]";
         return sstream.str();
      }

      wstring ConvertFilterToText(const stop_tree_filter& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"stop [ ]";
         return sstream.str();
      }

      wstring ConvertFilterToText(const stop_when_kept_tree_filter& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"stop-when-kept [ " << ConvertFilterToText(filter.filter, indent + 1) << L" ]";
         return sstream.str();
      }

      wstring ConvertFilterToText(const until_tree_filter& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"until [ " << ConvertFilterToText(filter.filter, indent + 1) << L" ]";
         return sstream.str();
      }

      wstring ConvertFilterToText(const contains_tree_filter& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"contains [ " << quoted(filter.Contained) << L" ]";
         return sstream.str();
      }

      wstring ConvertFilterToText(const unique_tree_filter& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"unique [ ]";
         return sstream.str();
      }

      wstring ConvertFilterToText(const regex_tree_filter& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"regex [ " << quoted(filter.regexTextForm) << L" ]";
         return sstream.str();
      }

      wstring ConvertFilterToText(const not_tree_filter& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"not [ " << ConvertFilterToText(filter.filter, indent + 1) << L" ]";
         return sstream.str();
      }

      wstring ConvertFilterToText(const if_subtree_tree_filter& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"if-sub [ " << ConvertFilterToText(filter.filter, indent + 1) << L" ]";
         return sstream.str();
      }

      wstring ConvertFilterToText(const if_sibling_tree_filter& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"if-sib [ " << ConvertFilterToText(filter.filter, indent + 1) << L" ]";
         return sstream.str();
      }

      wstring convert_filter_to_text(vector<tree_filter_ptr> filters, size_t indent)
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

      wstring ConvertFilterToText(const or_tree_filter& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"or [ " << convert_filter_to_text(filter.named_filters, indent + 1) << L" ]";
         return sstream.str();
      }

      wstring ConvertFilterToText(const and_tree_filter& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"and [ " << convert_filter_to_text(filter.named_filters, indent + 1) << L" ]";
         return sstream.str();
      }

      wstring ConvertFilterToText(const under_tree_filter& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"under [ " << boolalpha << filter.include_self << L", " << ConvertFilterToText(filter.filter, indent + 1) << L" ]";
         return sstream.str();
      }

      wstring ConvertFilterToText(const remove_children_tree_filter& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"no-child [ " << boolalpha << filter.include_self << L", " << ConvertFilterToText(filter.filter, indent + 1) << L" ]";
         return sstream.str();
      }

      wstring ConvertFilterToText(const level_range_tree_filter& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"range [ " << filter.min_level << L", " << filter.max_level << L" ]";
         return sstream.str();
      }

      wstring ConvertFilterToText(const named_tree_filter& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"named [ " << quoted(filter.Name) << L" ]";
         return sstream.str();
      }

      wstring ConvertFilterToText(const tree_filter& filter, size_t indent)
      {
         wstring indentText = wstring(L"\n") + wstring(indent, L' ');
         #define CALL_CONVERTER(a) if (const a* ptr = dynamic_cast<const a *>(&filter)) { return indentText + ConvertFilterToText(*ptr, indent); }

         CALL_CONVERTER(accept_tree_filter)
         CALL_CONVERTER(stop_tree_filter)
         CALL_CONVERTER(stop_when_kept_tree_filter)
         CALL_CONVERTER(until_tree_filter)
         CALL_CONVERTER(contains_tree_filter)
         CALL_CONVERTER(unique_tree_filter)
         CALL_CONVERTER(regex_tree_filter)
         CALL_CONVERTER(not_tree_filter)
         CALL_CONVERTER(if_subtree_tree_filter)
         CALL_CONVERTER(if_sibling_tree_filter)
         CALL_CONVERTER(or_tree_filter)
         CALL_CONVERTER(and_tree_filter)
         CALL_CONVERTER(under_tree_filter)
         CALL_CONVERTER(remove_children_tree_filter)
         CALL_CONVERTER(level_range_tree_filter)
         CALL_CONVERTER(named_tree_filter)

         #undef CALL_CONVERTER

         return {};
      }

      wstring ConvertFilterToText(const tree_filter_ptr& filter, size_t indent)
      {
         if (!filter)
            return {};

         return ConvertFilterToText(*filter, indent);
      }

      wstring convert_filter_to_text(const tree_filter_ptr& filter)
      {
         wostringstream sstream;
         sstream << L"V1: " << ConvertFilterToText(filter, 0);
         return sstream.str();
      }

      template <class T>
      tree_filter_ptr ConvertTextToFilter(wistringstream& sstream);

      tree_filter_ptr convert_text_to_filter(wistringstream& sstream);

      void EatClosingBrace(wistringstream& sstream)
      {
         // Eat the closing ']'.
         wchar_t brace;
         sstream >> skipws >> brace;
      }

      template <>
      tree_filter_ptr ConvertTextToFilter<accept_tree_filter>(wistringstream& sstream)
      {
         EatClosingBrace(sstream);
         return accept();
      }

      template <>
      tree_filter_ptr ConvertTextToFilter<stop_tree_filter>(wistringstream& sstream)
      {
         EatClosingBrace(sstream);
         return stop();
      }

      template <>
      tree_filter_ptr ConvertTextToFilter<stop_when_kept_tree_filter>(wistringstream& sstream)
      {
         auto filter = convert_text_to_filter(sstream);

         EatClosingBrace(sstream);

         return stop_when_kept(filter);
      }

      template <>
      tree_filter_ptr ConvertTextToFilter<until_tree_filter>(wistringstream& sstream)
      {
         auto filter = convert_text_to_filter(sstream);

         EatClosingBrace(sstream);

         return until(filter);
      }

      template <>
      tree_filter_ptr ConvertTextToFilter<contains_tree_filter>(wistringstream& sstream)
      {
         wstring contained;
         sstream >> skipws >> quoted(contained);

         EatClosingBrace(sstream);

         return contains(contained);
      }

      template <>
      tree_filter_ptr ConvertTextToFilter<unique_tree_filter>(wistringstream& sstream)
      {
         EatClosingBrace(sstream);
         return unique();
      }

      template <>
      tree_filter_ptr ConvertTextToFilter<regex_tree_filter>(wistringstream& sstream)
      {
         wstring regex;
         sstream >> skipws >> quoted(regex);

         EatClosingBrace(sstream);

         return dak::tree_reader::regex(regex);
      }

      template <>
      tree_filter_ptr ConvertTextToFilter<not_tree_filter>(wistringstream& sstream)
      {
         auto filter = convert_text_to_filter(sstream);

         EatClosingBrace(sstream);

         return not(filter);
      }

      template <>
      tree_filter_ptr ConvertTextToFilter<if_subtree_tree_filter>(wistringstream& sstream)
      {
         auto filter = convert_text_to_filter(sstream);

         EatClosingBrace(sstream);

         return if_subtree(filter);
      }

      template <>
      tree_filter_ptr ConvertTextToFilter<if_sibling_tree_filter>(wistringstream& sstream)
      {
         auto filter = convert_text_to_filter(sstream);

         EatClosingBrace(sstream);

         return if_sibling(filter);
      }

      vector<tree_filter_ptr> ConvertTextToMultiFilters(wistringstream& sstream)
      {
         vector<tree_filter_ptr> filters;

         while (sstream)
         {
            wchar_t comma;
            sstream >> skipws >> comma;
            if (comma != L',')
               sstream.putback(comma);
            if (comma == L']')
               break;

            auto filter = convert_text_to_filter(sstream);
            if (filter)
               filters.push_back(filter);
         }

         return filters;
      }

      template <>
      tree_filter_ptr ConvertTextToFilter<or_tree_filter>(wistringstream& sstream)
      {
         auto filters = ConvertTextToMultiFilters(sstream);

         EatClosingBrace(sstream);

         return Any(filters);
      }

      template <>
      tree_filter_ptr ConvertTextToFilter<and_tree_filter>(wistringstream& sstream)
      {
         auto filters = ConvertTextToMultiFilters(sstream);

         EatClosingBrace(sstream);

         return all(filters);
      }

      template <>
      tree_filter_ptr ConvertTextToFilter<under_tree_filter>(wistringstream& sstream)
      {
         bool includeSelf;
         wchar_t comma;
         sstream >> skipws >> boolalpha >> includeSelf >> skipws >> comma;

         auto filter = convert_text_to_filter(sstream);

         EatClosingBrace(sstream);

         return under(filter, includeSelf);
      }

      template <>
      tree_filter_ptr ConvertTextToFilter<remove_children_tree_filter>(wistringstream& sstream)
      {
         bool removeSelf;
         wchar_t comma;
         sstream >> skipws >> boolalpha >> removeSelf >> skipws >> comma;

         auto filter = convert_text_to_filter(sstream);

         EatClosingBrace(sstream);

         return no_child(filter, removeSelf);
      }

      template<>
      tree_filter_ptr ConvertTextToFilter<level_range_tree_filter>(wistringstream& sstream)
      {
         size_t minLevel, maxLevel;
         wchar_t comma;
         sstream >> skipws >> minLevel >> skipws >> comma >> skipws >> maxLevel;

         EatClosingBrace(sstream);

         return level_range(minLevel, maxLevel);
      }

      template<>
      tree_filter_ptr ConvertTextToFilter<named_tree_filter>(wistringstream& sstream)
      {
         wstring name;
         sstream >> skipws >> quoted(name);

         EatClosingBrace(sstream);

         auto named = make_shared<named_tree_filter>();
         named->Name = name;
         return named;
      }

      tree_filter_ptr convert_text_to_filter(wistringstream& sstream)
      {
         #define CALL_CONVERTER(a,b) if (name == a) { return ConvertTextToFilter<b>(sstream); } else

         wstring name;
         wchar_t brace;
         sstream >> skipws >> name >> skipws >> brace;

         CALL_CONVERTER(L"accept", accept_tree_filter)
         CALL_CONVERTER(L"contains", contains_tree_filter)
         CALL_CONVERTER(L"unique", unique_tree_filter)
         CALL_CONVERTER(L"regex", regex_tree_filter)
         CALL_CONVERTER(L"not", not_tree_filter)
         CALL_CONVERTER(L"if-sub", if_subtree_tree_filter)
         CALL_CONVERTER(L"if-sib", if_sibling_tree_filter)
         CALL_CONVERTER(L"or", or_tree_filter)
         CALL_CONVERTER(L"and", and_tree_filter)
         CALL_CONVERTER(L"under", under_tree_filter)
         CALL_CONVERTER(L"no-child", remove_children_tree_filter)
         CALL_CONVERTER(L"range", level_range_tree_filter)
         CALL_CONVERTER(L"stop", stop_tree_filter)
         CALL_CONVERTER(L"stop-when-kept", stop_when_kept_tree_filter)
         CALL_CONVERTER(L"until", until_tree_filter)
         CALL_CONVERTER(L"named", named_tree_filter)

         #undef CALL_CONVERTER

         return {};
      }

      tree_filter_ptr convert_text_to_filter(wistringstream& sstream, const named_filters& named)
      {
         tree_filter_ptr filter = convert_text_to_filter(sstream);
         update_named_filters(filter, named);
         return filter;
      }
   }

   wstring convert_filter_to_text(const tree_filter_ptr& filter)
   {
      return V1::convert_filter_to_text(filter);
   }

   tree_filter_ptr convert_text_to_filter(const wstring& text, const named_filters& named)
   {
      if (text.find(L"V1: ") == 0)
      {
         wistringstream sstream(text.substr(4));
         return V1::convert_text_to_filter(sstream, named);
      }

      return {};
   }
}
