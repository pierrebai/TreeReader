#include "BuffersTextHolder.h"

#include <algorithm>

namespace TreeReader
{
   using namespace std;

   pair<wchar_t*, size_t> BuffersTextHolderReader::ReadLine(wistream& stream)
   {
      bool skipNewLines = true;

      wchar_t* line = PosInBuffer;
      while (true)
      {
         if (PosInBuffer >= BufferEnd)
         {
            // Record how much we will need to transfer between the buffer
            // and how big the new buffer must be.
            const size_t amountReadSoFar = PosInBuffer - line;
            const size_t bufferSize = max(size_t(64 * 1024), amountReadSoFar * 2);

            // Allocate new buffer.
            Holder->TextBuffers.emplace_back(make_shared<BuffersTextHolder::Buffer>());
            auto buffer = Holder->TextBuffers.back();
            // Note: allocate one more character to be able to always put a terminating null.
            buffer->resize(bufferSize + 1);

            // Copy over the data that was reasd so far in th eprevious buffer.
            if (amountReadSoFar > 0)
               std::copy(line, line + amountReadSoFar, buffer->data());

            // Adjust the variable to point into the new buffer.
            line = buffer->data();
            BufferEnd = line + bufferSize;
            PosInBuffer = line + amountReadSoFar;

            // Fill the empty part of the new buffer.
            // Stop if there was nothign left to read.
            stream.read(PosInBuffer, BufferEnd - PosInBuffer);
            const auto readAmount = stream.gcount();
            if (readAmount <= 0)
               break;
            BufferEnd = PosInBuffer + readAmount;
         }

         if (skipNewLines)
         {
            if (*PosInBuffer != '\n' && *PosInBuffer != '\r')
            {
               line = PosInBuffer;
               skipNewLines = false;
            }
         }
         else
         {
            if (*PosInBuffer == '\n' || *PosInBuffer == '\r')
            {
               *PosInBuffer = 0;
               PosInBuffer++;
               break;
            }
         }

         ++PosInBuffer;
      }

      return make_pair(line, PosInBuffer - line);
   }
}
