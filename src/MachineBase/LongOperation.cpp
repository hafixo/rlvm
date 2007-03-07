// This file is part of RLVM, a RealLive virtual machine clone.
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

#include "MachineBase/LongOperation.hpp"

// -----------------------------------------------------------------------
// LongOperation
// -----------------------------------------------------------------------

LongOperation::~LongOperation() 
{}

// -----------------------------------------------------------------------
// PerformAfterLongOperationDecorator
// -----------------------------------------------------------------------

PerformAfterLongOperationDecorator::PerformAfterLongOperationDecorator(
  LongOperation* inOp)
  : m_operation(inOp)
{
}

// -----------------------------------------------------------------------

PerformAfterLongOperationDecorator::~PerformAfterLongOperationDecorator()
{
}

// -----------------------------------------------------------------------

bool PerformAfterLongOperationDecorator::operator()(RLMachine& machine)
{
  bool retVal = (*m_operation)(machine);
  if(retVal)
    performAfterLongOperation(machine);

  return retVal;
}
