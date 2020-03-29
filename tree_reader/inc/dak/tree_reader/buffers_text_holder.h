#pragma once

#include "dak/tree_reader/text_tree.h"

namespace dak::tree_reader
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // Holds text as a vector of raw characters buffers.
   // Uses indirection to make sure each buffer does not change address.

   struct buffers_text_holder : text_holder
   {
      typedef std::vector<wchar_t> buffer;
      typedef std::shared_ptr<buffer> buffer_ptr;
      typedef std::vector<buffer_ptr> buffers;

      buffers text_buffers;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // Read text lines from an input stream and stores them in the holder.

   struct buffers_text_holder_reader
   {
      std::shared_ptr<buffers_text_holder> holder = std::make_shared<buffers_text_holder>();

      wchar_t* pos_in_buffer = nullptr;
      wchar_t* buffer_end = nullptr;

      std::pair<wchar_t*, size_t> read_line(std::wistream& stream);
   };
}
