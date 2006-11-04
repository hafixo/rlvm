// This file is part of RLVM, a RealLive virutal machine clone.
//
// -----------------------------------------------------------------------
//
// Copyright (C) 2006 El Riot
//  
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//  
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//  
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//  
// -----------------------------------------------------------------------

#ifndef __Module_Mem_hpp__
#define __Module_Mem_hpp__

/**
 * @file   Module_Mem.hpp
 * @author Elliot Glaysher
 * @date   Sat Oct  7 11:17:55 2006
 * 
 * @brief  Declares the Memory module (mod<1:11>).
 */

#include "../MachineBase/RLModule.hpp"

class MemModule : public RLModule {
public:
  MemModule();
};

#endif