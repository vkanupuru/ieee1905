/*
 *  Broadband Forum BUS (Broadband User Services) Work Area
 *  
 *  Copyright (c) 2017, Broadband Forum
 *  Copyright (c) 2017, MaxLinear, Inc. and its affiliates
 *  
 *  Redistribution and use in source and binary forms, with or
 *  without modification, are permitted provided that the following
 *  conditions are met:
 *  
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  
 *  2. Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials
 *     provided with the distribution.
 *  
 *  3. Neither the name of the copyright holder nor the names of its
 *     contributors may be used to endorse or promote products
 *     derived from this software without specific prior written
 *     permission.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 *  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 *  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 *  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 *  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *  
 *  The above license is used as a license under copyright only.
 *  Please reference the Forum IPR Policy for patent licensing terms
 *  <https://www.broadband-forum.org/ipr-policy>.
 *  
 *  Any moral rights which are necessary to exercise under the above
 *  license grant are also deemed granted under this license.
 */

#include "platform.h"
#include "utils.h"

////////////////////////////////////////////////////////////////////////////////
// Public API
////////////////////////////////////////////////////////////////////////////////
//
void print_callback(void (*write_function)(const char *fmt, ...), const char *prefix, INT8U size, const char *name, const char *fmt, void *p)
{

       if (0 == PLATFORM_MEMCMP(fmt, "%s", 3))
       {
           // Strings are printed with triple quotes surrounding them
           //
           write_function("%s%s: \"\"\"%s\"\"\"\n", prefix, name, p);
           return;
       }
       else if (0 == PLATFORM_MEMCMP(fmt, "%ipv4", 6))
       {
           // This is needed because "size == 4" on IPv4 addresses, but we don't
           // want them to be treated as 4 bytes integers, so we change the
           // "fmt" to "%d" and do *not* returns (so that the IPv4 ends up being
           // printed as a sequence of bytes.
           //
           fmt = "%d";
       }
       else
       {
           #define FMT_LINE_SIZE 20
           char fmt_line[FMT_LINE_SIZE];

           fmt_line[0] = 0x0;

           PLATFORM_SNPRINTF(fmt_line, FMT_LINE_SIZE-1, "%%s%%s: %s\n", fmt);
           fmt_line[FMT_LINE_SIZE-1] = 0x0;

           if (1 == size)
           {
               write_function(fmt_line, prefix, name, *(INT8U *)p);

               return;
           }
           else if (2 == size)
           {
               write_function(fmt_line, prefix, name, *(INT16U *)p);

               return;
           }
           else if (4 == size)
           {
               write_function(fmt_line, prefix, name, *(INT32U *)p);

               return;
           }
       }

       // If we get here, it's either an IPv4 address or a sequence of bytes
       //
       {
           #define AUX1_SIZE 200  // Store a whole output line
           #define AUX2_SIZE  20  // Store a fmt conversion

           INT16U i, j;

           char aux1[AUX1_SIZE];
           char aux2[AUX2_SIZE];

           INT16U space_left = AUX1_SIZE-1;

           aux1[0] = 0x00;
           PLATFORM_STRNCAT(aux1, "%s%s: ", AUX1_SIZE);

           for (i=0; i<size; i++)
           {
               // Write one element to aux2
               //
               PLATFORM_SNPRINTF(aux2, AUX2_SIZE-1, fmt, *((INT8U *)p+i));
               
               // Obtain its length
               //
               for (j=0; j<AUX2_SIZE; j++)
               {
                   if (aux2[j] == 0x00)
                   {
                       break;
                   }
               }

               // 'j' contains the number of chars in "aux2"
               // Check if there is enough space left in "aux1"
               //
               //   NOTE: The "+2" is because we are going to append to "aux1"
               //         the contents of "aux2" plus a ", " string (which is
               //         three chars long)
               //         The second "+2" is because of the final "\n"
               //
               if (j+2+2 > space_left)
               {
                   // No more space left
                   //
                   aux1[AUX1_SIZE-6] = '.';
                   aux1[AUX1_SIZE-5] = '.';
                   aux1[AUX1_SIZE-4] = '.';
                   aux1[AUX1_SIZE-3] = '.';
                   aux1[AUX1_SIZE-2] = '.';
                   aux1[AUX1_SIZE-1] = 0x00;
                   break;
               }

               // Append string to "aux1"
               //
               PLATFORM_STRNCAT(aux1, aux2, j);
               PLATFORM_STRNCAT(aux1, ", ", 2);
               space_left -= (j+2);
           }

           PLATFORM_STRNCAT(aux1, "\n", 2);
           write_function(aux1, prefix, name);
       }

       return;
}


