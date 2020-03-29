#pragma once

#include "TextTree.h"

namespace TreeReader
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // Holds text as a vector of raw characters buffers.
   // Uses indirection to make sure each buffer does not change address.

   struct BuffersTextHolder : TextHolder
   {
      typedef std::vector<wchar_t> Buffer;
      typedef std::shared_ptr<Buffer> BufferPtr;
      typedef std::vector<BufferPtr> Buffers;

      Buffers TextBuffers;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // Read text lines from an input stream and stores them in the holder.

   struct BuffersTextHolderReader
   {
      std::shared_ptr<BuffersTextHolder> Holder = std::make_shared<BuffersTextHolder>();

      wchar_t* PosInBuffer = nullptr;
      wchar_t* BufferEnd = nullptr;

      std::pair<wchar_t*, size_t> ReadLine(std::wistream& stream);
   };
}
