// b_int_amode.tpl : Ballista Datatype Template for amode flag
// Copyright (C) 1998-2001  Carnegie Mellon University
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

name int b_int_amode;   

parent b_int;

includes
[
{
#include <sys/types.h>
#include <unistd.h>
#include "bTypes.h"
#include "b_int.h" 

}
]

global_defines
[
{
}
]

dials
[
  enum_dial AMODE : 
    R_OK,
    F_OK,
    RWX_OK,
    FRWX_OK;
]

access
[
{
   _theVariable = 0;
}
   F_OK, FRWX_OK  
   {
      _theVariable |= F_OK;
   }
   R_OK
   {
      _theVariable |= R_OK;
   }
   RWX_OK
   {
      _theVariable |= R_OK | W_OK | X_OK;
   }
]

commit
[
]

cleanup
[
]
