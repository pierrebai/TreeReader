#include "dak/tree_reader/buffers_text_holder.h"

#include <algorithm>

namespace dak::tree_reader
{
   using namespace std;

   pair<wchar_t*, size_t> buffers_text_holder_reader::read_line(wistream& stream)
   {
      bool skipNewlines = true;

      wchar_t* line = pos_in_buffer;
      while (true)
      {
         if (pos_in_buffer >= buffer_end)
         {
            // Record how much we will need to transfer between the buffer
            // and how big the new buffer must be.
            const size_t amountReadSoFar = pos_in_buffer - line;
            const size_t bufferSize = max(size_t(64 * 1024), amountReadSoFar * 2);

            // allocate new buffer.
            holder->text_buffers.emplace_back(make_shared<buffers_text_holder::buffer>());
            auto buffer = holder->text_buffers.back();
            // note: allocate one more character to be able to always put a terminating null.
            buffer->resize(bufferSize + 1);

            // Copy over the data that was reasd so far in th eprevious buffer.
            if (amountReadSoFar > 0)
               std::copy(line, line + amountReadSoFar, buffer->data());

            // Adjust the variable to point into the new buffer.
            line = buffer->data();
            buffer_end = line + bufferSize;
            pos_in_buffer = line + amountReadSoFar;

            // Fill the empty part of the new buffer.
            // stop if there was nothign left to read.
            stream.read(pos_in_buffer, buffer_end - pos_in_buffer);
            const auto readAmount = stream.gcount();
            if (readAmount <= 0)
               break;
            buffer_end = pos_in_buffer + readAmount;
         }

         if (skipNewlines)
         {
            if (*pos_in_buffer != '\n' && *pos_in_buffer != '\r')
            {
               line = pos_in_buffer;
               skipNewlines = false;
            }
         }
         else
         {
            if (*pos_in_buffer == '\n' || *pos_in_buffer == '\r')
            {
               *pos_in_buffer = 0;
               pos_in_buffer++;
               break;
            }
         }

         ++pos_in_buffer;
      }

      return make_pair(line, pos_in_buffer - line);
   }
}
