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

inline std::shared_ptr<PassTextFilter> Pass() { return std::make_shared<PassTextFilter>(); }
inline std::shared_ptr<ContainsTextFilter> Contains(const Text& text) { return std::make_shared<ContainsTextFilter>(text); }
inline std::shared_ptr<NotTextFilter> Not(const std::shared_ptr<TextFilter>& filter) { return std::make_shared<NotTextFilter>(filter); }
inline std::shared_ptr<OrTextFilter> Or(const std::shared_ptr<TextFilter>& lhs, const std::shared_ptr<TextFilter>& rhs) { return std::make_shared<OrTextFilter>(lhs, rhs); }
inline std::shared_ptr<AndTextFilter> And(const std::shared_ptr<TextFilter>& lhs, const std::shared_ptr<TextFilter>& rhs) { return std::make_shared<AndTextFilter>(lhs, rhs); }
