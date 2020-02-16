#pragma once

#include <string>
#include <memory>
#include <vector>

typedef std::wstring Text;

struct TextFilter
{
   virtual bool IsKept(const Text& text) = 0;
};

struct PassTextFilter : TextFilter
{
   bool IsKept(const Text& text) override { return true; }
};

struct ContainsTextFilter : TextFilter
{
   const Text Contained;

   ContainsTextFilter(const Text& text) : Contained(text) { }

   bool IsKept(const Text& text) override { return text.find(Contained) != std::wstring::npos; }
};

struct CombineTextFilter : TextFilter
{
   std::vector<std::shared_ptr<TextFilter>> Filters;

   CombineTextFilter() = default;

   CombineTextFilter(const std::shared_ptr<TextFilter>& lhs, const std::shared_ptr<TextFilter>& rhs) { Filters.push_back(lhs); Filters.push_back(rhs); }
};

struct NotTextFilter : TextFilter
{
   std::shared_ptr<TextFilter> Filter;

   NotTextFilter(const std::shared_ptr<TextFilter>& filter) : Filter(filter) { }

   bool IsKept(const Text& text) override;
};

struct OrTextFilter : CombineTextFilter
{
   OrTextFilter() = default;
   OrTextFilter(const std::shared_ptr<TextFilter>& lhs, const std::shared_ptr<TextFilter>& rhs) : CombineTextFilter(lhs, rhs) { }

   bool IsKept(const Text& text) override;
};

struct AndTextFilter : CombineTextFilter
{
   AndTextFilter() = default;
   AndTextFilter(const std::shared_ptr<TextFilter>& lhs, const std::shared_ptr<TextFilter>& rhs) : CombineTextFilter(lhs, rhs) { }

   bool IsKept(const Text& text) override;
};
