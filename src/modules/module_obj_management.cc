// -*- Mode: C++; tab-width:2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi:tw=80:et:ts=2:sts=2
//
// -----------------------------------------------------------------------
//
// This file is part of RLVM, a RealLive virtual machine clone.
//
// -----------------------------------------------------------------------
//
// Copyright (C) 2006 Elliot Glaysher
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

#include "modules/module_obj_management.h"

#include "machine/general_operations.h"
#include "machine/properties.h"
#include "machine/rloperation.h"
#include "machine/rlmachine.h"
#include "machine/rlmodule.h"
#include "modules/module_obj.h"
#include "systems/base/graphics_object.h"
#include "systems/base/graphics_system.h"
#include "systems/base/parent_graphics_object_data.h"
#include "systems/base/system.h"

// -----------------------------------------------------------------------

namespace {

struct objCopyFgToBg_0 : public RLOp_Void_1<IntConstant_T> {
  void operator()(RLMachine& machine, int buf) {
    GraphicsSystem& sys = machine.system().graphics();
    GraphicsObject& go = sys.GetObject(OBJ_FG, buf);
    sys.SetObject(OBJ_BG, buf, go);
  }
};

struct objCopyFgToBg_1 : public RLOp_Void_2<IntConstant_T, IntConstant_T> {
  void operator()(RLMachine& machine, int start, int end) {
    GraphicsSystem& sys = machine.system().graphics();

    for (int i = start; i <= end; ++i) {
      GraphicsObject& go = sys.GetObject(OBJ_FG, i);
      sys.SetObject(OBJ_BG, i, go);
    }
  }
};

struct objCopy : public RLOp_Void_2<IntConstant_T, IntConstant_T> {
  int from_fgbg_, to_fgbg_;
  objCopy(int from, int to) : from_fgbg_(from), to_fgbg_(to) {}

  void operator()(RLMachine& machine, int sbuf, int dbuf) {
    GraphicsSystem& sys = machine.system().graphics();
    GraphicsObject& go = sys.GetObject(from_fgbg_, sbuf);
    sys.SetObject(to_fgbg_, dbuf, go);
  }
};

struct objClear_0 : public RLOp_Void_1<IntConstant_T> {
  void operator()(RLMachine& machine, int buf) {
    GraphicsObject& obj = GetGraphicsObject(machine, this, buf);
    obj.ClearObject();
  }
};

struct objClear_1 : public RLOp_Void_2<IntConstant_T, IntConstant_T> {
  // I highly suspect that this has range semantics like
  // SetWipeCopyTo_1, but none of the games I own use this
  // function.
  void operator()(RLMachine& machine, int min, int max) {
    // Inclusive ranges make baby Kerrigan and Ritchie cry.
    max++;

    for (int i = min; i < max; ++i) {
      GetGraphicsObject(machine, this, i).ClearObject();
    }
  }
};

struct objDelete_0 : public RLOp_Void_1<IntConstant_T> {
  void operator()(RLMachine& machine, int buf) {
    GraphicsObject& obj = GetGraphicsObject(machine, this, buf);
    obj.DeleteObject();
  }
};

struct objDelete_1 : public RLOp_Void_2<IntConstant_T, IntConstant_T> {
  void operator()(RLMachine& machine, int min, int max) {
    // Inclusive ranges make baby Kerrigan and Ritchie cry.
    max++;

    for (int i = min; i < max; ++i) {
      GetGraphicsObject(machine, this, i).DeleteObject();
    }
  }
};

struct SetWipeCopyTo_0 : public RLOp_Void_1<IntConstant_T> {
  int val_;
  explicit SetWipeCopyTo_0(int value) : val_(value) {}

  void operator()(RLMachine& machine, int buf) {
    GetGraphicsObject(machine, this, buf).SetWipeCopy(val_);
  }
};

struct SetWipeCopyTo_1 : public RLOp_Void_2<IntConstant_T, IntConstant_T> {
  int val_;
  explicit SetWipeCopyTo_1(int value) : val_(value) {}

  void operator()(RLMachine& machine, int min, int numObjsToSet) {
    int maxObj = min + numObjsToSet;
    for (int i = min; i < maxObj; ++i) {
      GetGraphicsObject(machine, this, i).SetWipeCopy(val_);
    }
  }
};

void addObjManagementFunctions(RLModule& m) {
  m.AddOpcode(4, 0, "objWipeCopyOn", new SetWipeCopyTo_0(1));
  m.AddOpcode(4, 1, "objWipeCopyOn", new SetWipeCopyTo_1(1));
  m.AddOpcode(5, 0, "objWipeCopyOff", new SetWipeCopyTo_0(0));
  m.AddOpcode(5, 1, "objWipeCopyOff", new SetWipeCopyTo_1(0));

  m.AddOpcode(10, 0, "objClear", new objClear_0);
  m.AddOpcode(10, 1, "objClear", new objClear_1);
  m.AddOpcode(11, 0, "objDelete", new objDelete_0);
  m.AddOpcode(11, 1, "objDelete", new objDelete_1);
}

struct objChildCopy : public RLOp_Void_2<IntConstant_T, IntConstant_T> {
  int fgbg_;
  explicit objChildCopy(int fgbg) : fgbg_(fgbg) {}

  void operator()(RLMachine& machine, int sbuf, int dbuf) {
    GraphicsSystem& sys = machine.system().graphics();

    // By the time we enter this method, our parameters have already been
    // tampered with by the ChildObjAdaptor. So use P_PARENTOBJ as our toplevel
    // object.
    int parentobj;
    if (GetProperty(P_PARENTOBJ, parentobj)) {
      GraphicsObject& go = sys.GetObject(fgbg_, parentobj);
      EnsureIsParentObject(go, sys.GetObjectLayerSize());

      // Pick out the object data.
      ParentGraphicsObjectData& parent =
          static_cast<ParentGraphicsObjectData&>(go.GetObjectData());

      GraphicsObject& src_obj = parent.GetObject(sbuf);
      parent.SetObject(dbuf, src_obj);
    }
  }
};

}  // namespace

// -----------------------------------------------------------------------

ObjCopyFgToBg::ObjCopyFgToBg() : RLModule("ObjCopyFgToBg", 1, 60) {
  AddOpcode(0, 0, "objFree", CallFunction(&GraphicsSystem::ClearObject));

  AddOpcode(1, 0, "objEraseWipeCopy", new SetWipeCopyTo_0(0));

  // This may be wrong; the function is undocumented, but this appears
  // to fix the display problem in Kanon OP.
  AddOpcode(2, 0, "objCopyFgToBg", new objCopyFgToBg_0);
  AddOpcode(2, 1, "objCopyFgToBg", new objCopyFgToBg_1);

  AddOpcode(
      100, 0, "objClearAll", CallFunction(&GraphicsSystem::ClearAllObjects));
  AddOpcode(110,
            0,
            "objResetPropertiesAll",
            CallFunction(&GraphicsSystem::ResetAllObjectsProperties));

  // Experimentation shows that op<1:60:111, 0> looks like a synonmy for
  // op<1:60:100, 0>. May have differences?
  AddOpcode(
      111, 0, "objClearAll2", CallFunction(&GraphicsSystem::ClearAllObjects));
}

// -----------------------------------------------------------------------

ObjFgManagement::ObjFgManagement() : RLModule("ObjFgManagement", 1, 61) {
  AddOpcode(2, 0, "objCopy", new objCopy(OBJ_FG, OBJ_FG));
  AddOpcode(3, 0, "objCopyToBg", new objCopy(OBJ_FG, OBJ_BG));

  addObjManagementFunctions(*this);
  SetProperty(P_FGBG, OBJ_FG);
}

// -----------------------------------------------------------------------

ObjBgManagement::ObjBgManagement() : RLModule("ObjBgManagement", 1, 62) {
  AddOpcode(2, 0, "objBgCopyToFg", new objCopy(OBJ_BG, OBJ_FG));
  AddOpcode(3, 0, "objBgCopy", new objCopy(OBJ_BG, OBJ_BG));

  addObjManagementFunctions(*this);
  SetProperty(P_FGBG, OBJ_BG);
}

// -----------------------------------------------------------------------

ChildObjFgManagement::ChildObjFgManagement()
    : MappedRLModule(ChildObjMappingFun, "ChildObjFgManagement", 2, 61) {
  AddOpcode(2, 0, "objSetCopy", new objCopy(OBJ_FG, OBJ_FG));
  AddOpcode(3, 0, "objSetCopyToBg", new objCopy(OBJ_FG, OBJ_BG));

  AddOpcode(14, 0, "objChildCopy", new objChildCopy(OBJ_FG));

  addObjManagementFunctions(*this);
  SetProperty(P_FGBG, OBJ_FG);
}

// -----------------------------------------------------------------------

ChildObjBgManagement::ChildObjBgManagement()
    : MappedRLModule(ChildObjMappingFun, "ChildObjFgManagement", 2, 62) {
  AddOpcode(2, 0, "objSetBgCopyToFg", new objCopy(OBJ_BG, OBJ_FG));
  AddOpcode(3, 0, "objSetBgCopy", new objCopy(OBJ_BG, OBJ_BG));

  AddOpcode(14, 0, "objChildCopy", new objChildCopy(OBJ_BG));

  addObjManagementFunctions(*this);
  SetProperty(P_FGBG, OBJ_BG);
}

// -----------------------------------------------------------------------
