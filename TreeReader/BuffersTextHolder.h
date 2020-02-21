#pragma once

#include "TextTree.h"

namespace TreeReader
{
   struct BuffersTextHolder : TextHolder
   {
      typedef std::vector<wchar_t> Buffer;
      typedef std::shared_ptr<Buffer> BufferPtr;
      typedef std::vector<BufferPtr> Buffers;

      Buffers TextBuffers;
   };

   struct BuffersTextHolderReader
   {
      std::shared_ptr<BuffersTextHolder> Holder = std::make_shared<BuffersTextHolder>();

      wchar_t* PosInBuffer = nullptr;
      wchar_t* BufferEnd = nullptr;

      std::pair<wchar_t*, size_t> ReadLine(std::wistream& stream);
   };
}
