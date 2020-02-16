#pragma once

#include <string>

typedef std::wstring Text;

struct TextFilter
{
   virtual bool IsKept(const Text& text) = 0;
};

struct ContainsTextFilter : TextFilter
{
   const Text Contained;

   ContainsTextFilter(const Text& text) : Contained(text) { }

   bool IsKept(const Text& text) override { return text.find(Contained) != std::wstring::npos; }
};

struct CombineTextFilter : TextFilter
{
   TextFilter& Left;
   TextFilter& Right;

   CombineTextFilter(TextFilter& lhs, TextFilter& rhs) : Left(lhs), Right(rhs) { }
};

struct NotTextFilter : TextFilter
{
   TextFilter& Filter;

   NotTextFilter(TextFilter& filter) : Filter(filter) { }

   bool IsKept(const Text& text) override { return !Filter.IsKept(text); }
};

struct OrTextFilter : CombineTextFilter
{
   OrTextFilter(TextFilter& lhs, TextFilter& rhs) : CombineTextFilter(lhs, rhs) { }

   bool IsKept(const Text& text) override { return Left.IsKept(text) || Right.IsKept(text); }
};

struct AndTextFilter : CombineTextFilter
{
   AndTextFilter(TextFilter& lhs, TextFilter& rhs) : CombineTextFilter(lhs, rhs) { }

   bool IsKept(const Text& text) override { return Left.IsKept(text) && Right.IsKept(text); }
};
