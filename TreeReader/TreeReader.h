#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iostream>

typedef std::wstring Text;
typedef std::vector<Text> TextLines;

struct TextTree
{
   struct Node
   {
      // Index into the source text lines.
      const Text* TextPtr = 0;

      // Index into the node vector.
      size_t FirstChildIndex = -1;

      // Number of children of this node.
      // They are all consecutive in the node vector.
      size_t ChildrenCount = 0;
   };

   std::shared_ptr<const TextLines> SourceTextLines;
   std::vector<Node> Nodes;
};

std::wostream& operator<<(std::wostream& stream, const TextTree& tree);


struct TextFilter
{
   virtual bool IsKept(const Text& text) const = 0;
};

struct ContainsTextFilter : TextFilter
{
   const Text Contained;

   ContainsTextFilter(const Text& text) : Contained(text) { }

   bool IsKept(const Text& text) const override { return text.find(Contained) != std::wstring::npos; }
};

void FilterTree(const TextTree& sourceTree, TextTree& filteredTree, const TextFilter& filter);
