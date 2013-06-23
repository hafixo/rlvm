// -*- Mode: C++; tab-width:2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi:tw=80:et:ts=2:sts=2
//
// -----------------------------------------------------------------------
//
// This file is part of RLVM, a RealLive virtual machine clone.
//
// -----------------------------------------------------------------------
//
// Copyright (C) 2008 Elliot Glaysher
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
//
// -----------------------------------------------------------------------

#include "Modules/Module_Sys_index_series.hpp"

#include <sstream>

#include "MachineBase/RLMachine.hpp"
#include "MachineBase/RLModule.hpp"
#include "Utilities/math_util.hpp"

// -----------------------------------------------------------------------

int Sys_index_series::operator()(RLMachine& machine,
                                 int index,
                                 int offset,
                                 int init,
                                 IndexList::type index_list) {
  int value = init;
  bool previous_term_finished = false;

  for (IndexList::type::iterator it = index_list.begin();
       it != index_list.end(); ++it) {
    switch (it->type) {
      case 0: {
        if (previous_term_finished) {
          value = it->first;
          init = it->first;
        }
        break;
      }
      case 1: {
        // This is the only thing we reliably can do.
        int start = it->second.get<0>() + offset;
        int end = it->second.get<1>() + offset;
        int endval = it->second.get<2>();
        adder(index, start, end, endval, 0,
              value, init, previous_term_finished);
        break;
      }
      case 2: {
        int start = it->third.get<0>() + offset;
        int end = it->third.get<1>() + offset;
        int endval = it->third.get<2>();
        int mod = it->third.get<3>();
        adder(index, start, end, endval, mod,
              value, init, previous_term_finished);
        break;
      }
    }
  }

  return value;
}

void Sys_index_series::adder(int index,
                             int start,
                             int end,
                             int endval,
                             int mod,
                             int& value,
                             int& init,
                             bool& previous_term_finished) {
  if (index > start && index < end) {
    int amount = endval - init;
    value += Interpolate(start, index, end, amount, mod);
    previous_term_finished = false;
  } else if (index >= end) {
    // Prevent us from going over the endval.
    value = endval;
    init = endval;
    previous_term_finished = true;
  }
}

// -----------------------------------------------------------------------

void addIndexSeriesOpcode(RLModule& module) {
  module.addOpcode(800, 0, "index_series", new Sys_index_series);
}
