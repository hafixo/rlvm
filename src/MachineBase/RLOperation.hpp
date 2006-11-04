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

/** 
 * @file
 * @ingroup RLOperation
 * @brief Defines all the base RLOperations and their type checking structs.
 */

#ifndef __RLOperation__h__
#define __RLOperation__h__

#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/tuple/tuple.hpp>

//#include "libReallive/defs.h"
#include "libReallive/bytecode.h"
#include "libReallive/expression.h"

#include "MachineBase/RLMachine.hpp"
#include "MachineBase/reference.hpp"

#include <iostream>

// ------------------------------------------------------------ Real

/**
 * @defgroup RLOperation RLOperations/Type structures
 *
 * Defines the base classes from which the all of the opcode
 * implementations derive from. This heiarchy of classes works by
 * having one of your operation classes, which handles a specific
 * prototype of a specific opcode, derfive from one of the subclases
 * of RLOperation, specifically RLOP_{Void,Store}_{# of
 * parameters}. The template parameters of these subclasses refer to
 * the types of the parameters.
 *
 * Valid type parameters are IntConstant_T, IntReference_T,
 * StrConstant_T, StrReference_T, Argc_T (takes another type as a
 * parameter), 
 * 
 * @{
 */

/** 
 * @brief Root of the entire operation heiarchy.
 *
 * An RLOperation object implements an individual bytecode command.
 *
 * RLOperations are grouped into RLModule s, which are then added to
 * the RLMachine.
 */
struct RLOperation {
  /** 
   * Check made as to whether the instruction pointer should be
   * incremented after the instruction is executed. Override this in
   * the *rare* case where an instruction messes about with the
   * instruction pointer and we don't want to mess with it afterwards.
   * The default implmentation returns true; 99% of instructions want
   * the instruction pointer to be advanced automaticly.
   */
  virtual bool advanceInstructionPointer() { return true; }

  /** The type checker is called by the Module to make sure the parameters read in
   * are of the expected type.
   */
  virtual bool checkTypes(RLMachine& machine, 
                          boost::ptr_vector<libReallive::ExpressionPiece>& parameters) = 0;

  /** The dispatch function is implemented on a per type basis and is called by the
   * Module, after checking to make sure that the 
   */
  virtual void dispatch(RLMachine& machine, 
                        boost::ptr_vector<libReallive::ExpressionPiece>& parameters) = 0;

  /** 
   * Takes a raw, unparsed parameter string, parses it, and places the
   * result in outputParameters
   * 
   * @param parameter The input, unparsed parameter string
   * @param outputParameters The output ptr_vector to place the 
   * resulting ExpressionPieces
   */
  void addParameterTo(const std::string& parameter, 
                      boost::ptr_vector<libReallive::ExpressionPiece>& outputParameters);

  /** 
   * Parses the parameters in the CommandElement passed in into an
   * output ptr_vector that contains parsed ExpressionPieces for each 
   * 
   * @param ff The incoming CommandElement
   * @param output The output ptr_vector, filled with the parsed parameters
   */
  void parseParameters(const libReallive::CommandElement& ff, 
                       boost::ptr_vector<libReallive::ExpressionPiece>& output);

  /** The public interface used by the RLModule; how a method is dispatched.
   *
   * @param machine RLMachine to operate on
   * @param f CommandElement (assumed to be FunctionElement in default
   * implementation) to work on.
   */
  virtual void dispatchFunction(RLMachine& machine, 
                                const libReallive::CommandElement& f);
};

// -----------------------------------------------------------------------

/**
 * Type definition for a Constant integer value. 
 * 
 * This struct is used to define the parameter types of a RLOperation
 * subclass, and should not be used directly. It should only be used
 * as a template parameter to one of those classes, or of another type
 * definition struct.
 */
struct IntConstant_T {
  /// The output type of this type struct
  typedef int type;

  /// Convert the incoming parameter objects into the resulting type
  static type getData(RLMachine& machine,
                      boost::ptr_vector<libReallive::ExpressionPiece>& p,
                      int position) {
    return p[position].integerValue(machine);
   }

  /// Verify that the incoming parameter objects meet the desired types
  static bool verifyType(boost::ptr_vector<libReallive::ExpressionPiece>& p, int position);

  enum {
    isRealTypestruct = true
  };
};

// -----------------------------------------------------------------------

/** 
 * Type definition for a reference into the RLMachine's memory,
 * referencing an integer value.
 *
 * This struct is used to define the parameter types of a RLOperation
 * subclass, and should not be used directly. It should only be used
 * as a template parameter to one of those classes, or of another type
 * definition struct.
 */
struct IntReference_T {
  /// We pass iterators to the 
  typedef IntReferenceIterator type;

  /// Convert the incoming parameter objects into the resulting type
  static type getData(RLMachine& machine, 
                      boost::ptr_vector<libReallive::ExpressionPiece>& p, 
                      int position) {
    return static_cast<libReallive::MemoryReference&>(p[position]).
      getIntegerReferenceIterator(machine);
  }

  /// Verify that the incoming parameter objects meet the desired types
  static bool verifyType(boost::ptr_vector<libReallive::ExpressionPiece>& p,
                         int position);

  enum {
    isRealTypestruct = true
  };
};

// -----------------------------------------------------------------------

/** 
 * Type definition for a constant string value. 
 * 
 * This struct is used to define the parameter types of a RLOperation
 * subclass, and should not be used directly. It should only be used
 * as a template parameter to one of those classes, or of another type
 * definition struct.
 */
struct StrConstant_T {
  /// The output type of this type struct
  typedef std::string type;

  /// Convert the incoming parameter objects into the resulting type
  static type getData(RLMachine& machine, 
                      boost::ptr_vector<libReallive::ExpressionPiece>& p,
                      int position) { 
    return p[position].getStringValue(machine);
  }

  /// Verify that the incoming parameter objects meet the desired types
  static bool verifyType(boost::ptr_vector<libReallive::ExpressionPiece>& p,
                         int position);

  enum {
    isRealTypestruct = true
  };
};

// -----------------------------------------------------------------------

/** 
 * Type struct for a reference into the RLMachine's memory,
 * referencing a string value.
 * 
 * This struct is used to define the parameter types of a RLOperation
 * subclass, and should not be used directly. It should only be used
 * as a template parameter to one of those classes, or of another type
 * definition struct.
 */
struct StrReference_T {
  /// The output type of this type struct
  typedef StringReferenceIterator type;

  /// Convert the incoming parameter objects into the resulting type
  static type getData(RLMachine& machine, 
                      boost::ptr_vector<libReallive::ExpressionPiece>& p, 
                      int position) {
    return static_cast<libReallive::MemoryReference&>(p[position]).
      getStringReferenceIterator(machine);
  }

  /// Verify that the incoming parameter objects meet the desired types
  static bool verifyType(boost::ptr_vector<libReallive::ExpressionPiece>& p,
                         int position);

  enum {
    isRealTypestruct = true
  };
};

// -----------------------------------------------------------------------

/** 
 * Type struct that implements the argc concept. 
 *
 * This type struct can only be used as the last element in a type
 * definition. (This is not checked for at runtime; I'm not even sure
 * how I'd check this concept.) This type struct takes a type struct
 * as its parameter type, and then will accept a variable number of
 * items of that type.
 */
template<typename CON>
struct Argc_T {
  /// The output type of this type struct
  typedef std::vector<typename CON::type> type;

  /** Convert the incoming parameter objects into the resulting type.
   * Passes each parameter down to 
   */
  static type getData(RLMachine& machine, 
                      boost::ptr_vector<libReallive::ExpressionPiece>& p,
                      int position);

  /** Verify that the incoming parameter objects meet the desired types,
   * by passing each object from its spot in the parameters on to 
   * the template argument type.
   *
   * @return true if all parameters are of the correct type.
   */
  static bool verifyType(boost::ptr_vector<libReallive::ExpressionPiece>& p, 
                         int position);

  enum {
    isRealTypestruct = true
  };
};

// -----------------------------------------------------------------------

/** 
 * Type definition that implements the complex parameter concept.
 *
 * It really should have been called tuple, but the name's stuck
 * now. Takes two other type structs as template parameters.
 */
template<typename A, typename B>
struct Complex2_T {
  /// The output type of this type struct
  typedef boost::tuple<typename A::type, typename B::type> type;

  /// Convert the incoming parameter objects into the resulting type.
  static type getData(RLMachine& machine, boost::ptr_vector<libReallive::ExpressionPiece>& p,
                      int position) {
    libReallive::ComplexExpressionPiece& sp = 
      static_cast<libReallive::ComplexExpressionPiece&>(p[position]);
    return boost::tuple<typename A::type, typename B::type>(
      A::getData(machine, sp.getContainedPieces(), 0), 
      B::getData(machine, sp.getContainedPieces(), 1));
  }

  /// Takes a type and makes sure that 
  static bool verifyType(boost::ptr_vector<libReallive::ExpressionPiece>& p, int position) {
    // Verify the size of the vector, that we have a special parameter, and then
    // make sure all the 
    bool typeOK = position < p.size();
    typeOK = typeOK && p[position].isComplexParameter(); 
    if(typeOK) {
      libReallive::ComplexExpressionPiece& sp = 
        static_cast<libReallive::ComplexExpressionPiece&>(p[position]);
      typeOK = typeOK && A::verifyType(sp.getContainedPieces(), 0);
      typeOK = typeOK && A::verifyType(sp.getContainedPieces(), 1);
    }
    return typeOK;
  }

  enum {
    isRealTypestruct = true
  };
};

// -----------------------------------------------------------------------

struct emptyStruct { };

/// Defines a null type for the Special parameter.
struct Empty_T {
  typedef emptyStruct type;

  /// Convert the incoming parameter objects into the resulting type.
  static type getData(RLMachine& machine, 
                      boost::ptr_vector<libReallive::ExpressionPiece>& p,
                      int position) {
    return emptyStruct();
  }

  static bool verifyType(boost::ptr_vector<libReallive::ExpressionPiece>& p,
                         int position) {
    return true;
  }

  enum {
    isRealTypestruct = false
  };
};

/** 
 * Type definition that implements the special parameter concept; the
 * way to expect multiple different types in a parameter slot. 
 */
template<typename A, typename B = Empty_T, typename C = Empty_T, 
         typename D = Empty_T, typename E = Empty_T>
struct Special_T {
  /// Internal unionish structure which we pass in to the 
  struct Parameter {
    // 0 = A, 1 = B
    int type;

    typename A::type first;
    typename B::type second;
    typename C::type third;
    typename D::type fourth;
    typename E::type fifth;
  };

  /// Export our internal struct as our external type
  typedef Parameter type;

  /// Convert the incoming parameter objects into the resulting type.
  static type getData(RLMachine& machine, 
                      boost::ptr_vector<libReallive::ExpressionPiece>& p,
                      int position)
  {
    libReallive::SpecialExpressionPiece& sp = 
      static_cast<libReallive::SpecialExpressionPiece&>(p[position]);
    Parameter par;
    par.type = sp.getOverloadTag();
    switch(par.type) {
    case 0:
      par.first = A::getData(machine, sp.getContainedPieces(), 0);
      break;
    case 1:
      par.second = B::getData(machine, sp.getContainedPieces(), 0);
      break;
    case 2:
      par.third = C::getData(machine, sp.getContainedPieces(), 0);
      break;
    case 3:
      par.fourth = D::getData(machine, sp.getContainedPieces(), 0);
      break;
    case 4:
      par.fifth = E::getData(machine, sp.getContainedPieces(), 0);
      break;
    default:
      throw libReallive::Error("Illegal overload in Special2_T::getData()");
    };

    return par;
  }

  /// Takes a type and makes sure that 
  static bool verifyType(boost::ptr_vector<libReallive::ExpressionPiece>& p,
                         int position) 
  {
    libReallive::SpecialExpressionPiece& sp = 
      static_cast<libReallive::SpecialExpressionPiece&>(p[position]);
    Parameter par;
    par.type = sp.getOverloadTag();
    switch(par.type) {
    case 0:
      return A::verifyType(sp.getContainedPieces(), 0);
    case 1:
      return B::verifyType(sp.getContainedPieces(), 0);
    case 2:
      return C::verifyType(sp.getContainedPieces(), 0);
    case 3:
      return D::verifyType(sp.getContainedPieces(), 0);
    case 4:
      return E::verifyType(sp.getContainedPieces(), 0);
    default:
      throw libReallive::Error("Illegal overload in Special2_T::verifyType()");
    };
  }
};

// ----------------------------------------------------------------------

/**
 * Implements a special case operation. This should be used with
 * things that don't follow the usually function syntax in the
 * bytecode, such as weird gotos, et cetera.
 *
 * RLOp_SpecialCase gives you complete control of the dispatch,
 * performing no type checking, no parameter conversion, and no
 * implicit instruction pointer advancement. 
 * 
 * @warning This is almost certainly not what you want. This is only
 * used to define handlers for CommandElements that aren't
 * FunctionElements. Meaning the Gotos and Select.
 * @see JmpModule
 */
struct RLOp_SpecialCase : public RLOperation {
  /** 
   * Empty function defined simply to obey the interface
   */
  bool checkTypes(RLMachine& machine, 
                  boost::ptr_vector<libReallive::ExpressionPiece>& parameters);

  /** 
   * Empty function defined simply to obey the interface
   */
  void dispatch(RLMachine& machine, 
                boost::ptr_vector<libReallive::ExpressionPiece>& parameters);

  void dispatchFunction(RLMachine& machine, 
                        const libReallive::CommandElement& f);

  /// Method that is overridden by all subclasses to implement the
  /// function of this opcode
  virtual void operator()(RLMachine&, const libReallive::CommandElement&) = 0;
};

// -----------------------------------------------------------------------

/** RLOp that does not return a value.
 * 
 * @param A Type struct representing type to check for.
 * @param B Type struct representing type to check for.
 * @param C Type struct representing type to check for.
 * @param D Type struct representing type to check for.
 */
template<typename A = Empty_T, typename B = Empty_T, typename C = Empty_T, 
         typename D = Empty_T, typename E = Empty_T, typename F = Empty_T,
         typename G = Empty_T, typename H = Empty_T, typename I = Empty_T,
         typename J = Empty_T, typename K = Empty_T, typename L = Empty_T,
         typename M = Empty_T, typename N = Empty_T, typename O = Empty_T,
         typename P = Empty_T>
struct RLOp_Void : public RLOperation {
  typedef typename A::type firstType;
  typedef typename B::type secondType;
  typedef typename C::type thirdType;
  typedef typename D::type fourthType;
  typedef typename E::type fifthType;
  typedef typename F::type sixthType;
  typedef typename G::type seventhType;
  typedef typename H::type eighthType;
  typedef typename I::type ninthType;
  typedef typename J::type tenthType;
  typedef typename K::type eleventhType;
  typedef typename L::type twelvthType;
  typedef typename M::type thirteenthType;

  bool checkTypes(RLMachine& machine, 
                  boost::ptr_vector<libReallive::ExpressionPiece>& parameters) 
  {
    return (!A::isRealTypestruct  || A::verifyType(parameters, 0)) &&
      (!B::isRealTypestruct || B::verifyType(parameters, 1)) &&
      (!C::isRealTypestruct || C::verifyType(parameters, 2)) &&
      (!D::isRealTypestruct || D::verifyType(parameters, 3)) &&
      (!E::isRealTypestruct || E::verifyType(parameters, 4)) &&
      (!F::isRealTypestruct || F::verifyType(parameters, 5)) &&
      (!G::isRealTypestruct || G::verifyType(parameters, 6)) &&
      (!H::isRealTypestruct || H::verifyType(parameters, 7)) &&
      (!I::isRealTypestruct || I::verifyType(parameters, 8)) &&
      (!J::isRealTypestruct || J::verifyType(parameters, 9)) &&
      (!K::isRealTypestruct || K::verifyType(parameters, 10)) &&
      (!L::isRealTypestruct || L::verifyType(parameters, 11)) &&
      (!M::isRealTypestruct || M::verifyType(parameters, 12));
  }

  void dispatch(RLMachine& machine, 
                boost::ptr_vector<libReallive::ExpressionPiece>& parameters) 
  {
    // The following is fucking ugly. If anyone knows a way to make
    // this pretty, but still get gcc to reason about template types
    // and optimize away everything but one of these instances, I'd
    // love to hear from you.
    if(!A::isRealTypestruct)
    {
      operator()(machine);
    }
    else if(A::isRealTypestruct && !B::isRealTypestruct)
    {
      operator()(machine, A::getData(machine, parameters, 0));
    }
    else if(A::isRealTypestruct && B::isRealTypestruct &&
            !C::isRealTypestruct)
    {
      operator()(machine, A::getData(machine, parameters, 0),
                 B::getData(machine, parameters, 1));
    }
    else if(A::isRealTypestruct && B::isRealTypestruct &&
            C::isRealTypestruct && !D::isRealTypestruct)
    {
      operator()(machine, A::getData(machine, parameters, 0),
                 B::getData(machine, parameters, 1),
                 C::getData(machine, parameters, 2));
    }
    else if(A::isRealTypestruct && B::isRealTypestruct &&
            C::isRealTypestruct && D::isRealTypestruct &&
            !E::isRealTypestruct)
    {
      operator()(machine, A::getData(machine, parameters, 0),
                 B::getData(machine, parameters, 1), 
                 C::getData(machine, parameters, 2),
                 D::getData(machine, parameters, 3));
    }   
    else if(A::isRealTypestruct && B::isRealTypestruct &&
            C::isRealTypestruct && D::isRealTypestruct &&
            E::isRealTypestruct && !F::isRealTypestruct)
    {
      operator()(machine, A::getData(machine, parameters, 0),
                 B::getData(machine, parameters, 1), 
                 C::getData(machine, parameters, 2),
                 D::getData(machine, parameters, 3),
                 E::getData(machine, parameters, 4));
    }   
    else if(A::isRealTypestruct && B::isRealTypestruct &&
            C::isRealTypestruct && D::isRealTypestruct &&
            E::isRealTypestruct && F::isRealTypestruct &&
            !G::isRealTypestruct)
    {
      operator()(machine, A::getData(machine, parameters, 0),
                 B::getData(machine, parameters, 1), 
                 C::getData(machine, parameters, 2),
                 D::getData(machine, parameters, 3),
                 E::getData(machine, parameters, 4),
                 F::getData(machine, parameters, 5));
    }   
    else if(A::isRealTypestruct && B::isRealTypestruct &&
            C::isRealTypestruct && D::isRealTypestruct &&
            E::isRealTypestruct && F::isRealTypestruct &&
            G::isRealTypestruct && !H::isRealTypestruct)
    {
      operator()(machine, A::getData(machine, parameters, 0),
                 B::getData(machine, parameters, 1), 
                 C::getData(machine, parameters, 2),
                 D::getData(machine, parameters, 3),
                 E::getData(machine, parameters, 4),
                 F::getData(machine, parameters, 5),
                 G::getData(machine, parameters, 6));
    }   
    else if(A::isRealTypestruct && B::isRealTypestruct &&
            C::isRealTypestruct && D::isRealTypestruct &&
            E::isRealTypestruct && F::isRealTypestruct &&
            G::isRealTypestruct && H::isRealTypestruct &&
            !I::isRealTypestruct)
    {
      operator()(machine, A::getData(machine, parameters, 0),
                 B::getData(machine, parameters, 1), 
                 C::getData(machine, parameters, 2),
                 D::getData(machine, parameters, 3),
                 E::getData(machine, parameters, 4),
                 F::getData(machine, parameters, 5),
                 G::getData(machine, parameters, 6),
                 H::getData(machine, parameters, 7));
    }   
    else if(A::isRealTypestruct && B::isRealTypestruct &&
            C::isRealTypestruct && D::isRealTypestruct &&
            E::isRealTypestruct && F::isRealTypestruct &&
            G::isRealTypestruct && H::isRealTypestruct &&
            I::isRealTypestruct && !J::isRealTypestruct)
    {
      operator()(machine, A::getData(machine, parameters, 0),
                 B::getData(machine, parameters, 1), 
                 C::getData(machine, parameters, 2),
                 D::getData(machine, parameters, 3),
                 E::getData(machine, parameters, 4),
                 F::getData(machine, parameters, 5),
                 G::getData(machine, parameters, 6),
                 H::getData(machine, parameters, 7),
                 I::getData(machine, parameters, 8));
    }   
    else if(A::isRealTypestruct && B::isRealTypestruct &&
            C::isRealTypestruct && D::isRealTypestruct &&
            E::isRealTypestruct && F::isRealTypestruct &&
            G::isRealTypestruct && H::isRealTypestruct &&
            I::isRealTypestruct && J::isRealTypestruct &&
            !K::isRealTypestruct)
    {
      operator()(machine, A::getData(machine, parameters, 0),
                 B::getData(machine, parameters, 1), 
                 C::getData(machine, parameters, 2),
                 D::getData(machine, parameters, 3),
                 E::getData(machine, parameters, 4),
                 F::getData(machine, parameters, 5),
                 G::getData(machine, parameters, 6),
                 H::getData(machine, parameters, 7),
                 I::getData(machine, parameters, 8),
                 J::getData(machine, parameters, 9));
    }   
    else if(A::isRealTypestruct && B::isRealTypestruct &&
            C::isRealTypestruct && D::isRealTypestruct &&
            E::isRealTypestruct && F::isRealTypestruct &&
            G::isRealTypestruct && H::isRealTypestruct &&
            I::isRealTypestruct && J::isRealTypestruct &&
            K::isRealTypestruct && !L::isRealTypestruct)
    {
      operator()(machine, A::getData(machine, parameters, 0),
                 B::getData(machine, parameters, 1), 
                 C::getData(machine, parameters, 2),
                 D::getData(machine, parameters, 3),
                 E::getData(machine, parameters, 4),
                 F::getData(machine, parameters, 5),
                 G::getData(machine, parameters, 6),
                 H::getData(machine, parameters, 7),
                 I::getData(machine, parameters, 8),
                 J::getData(machine, parameters, 9),
                 K::getData(machine, parameters, 10));
    }
    else if(A::isRealTypestruct && B::isRealTypestruct &&
            C::isRealTypestruct && D::isRealTypestruct &&
            E::isRealTypestruct && F::isRealTypestruct &&
            G::isRealTypestruct && H::isRealTypestruct &&
            I::isRealTypestruct && J::isRealTypestruct &&
            K::isRealTypestruct && L::isRealTypestruct &&
            !M::isRealTypestruct)
    {
      operator()(machine, A::getData(machine, parameters, 0),
                 B::getData(machine, parameters, 1), 
                 C::getData(machine, parameters, 2),
                 D::getData(machine, parameters, 3),
                 E::getData(machine, parameters, 4),
                 F::getData(machine, parameters, 5),
                 G::getData(machine, parameters, 6),
                 H::getData(machine, parameters, 7),
                 I::getData(machine, parameters, 8),
                 J::getData(machine, parameters, 9),
                 K::getData(machine, parameters, 10),
                 L::getData(machine, parameters, 11));
    }
    else
    {
      operator()(machine, A::getData(machine, parameters, 0),
                 B::getData(machine, parameters, 1), 
                 C::getData(machine, parameters, 2),
                 D::getData(machine, parameters, 3),
                 E::getData(machine, parameters, 4),
                 F::getData(machine, parameters, 5),
                 G::getData(machine, parameters, 6),
                 H::getData(machine, parameters, 7),
                 I::getData(machine, parameters, 8),
                 J::getData(machine, parameters, 9),
                 K::getData(machine, parameters, 10),
                 L::getData(machine, parameters, 11),
                 M::getData(machine, parameters, 12));
    }
  }

  /// Method that is overridden by all subclasses to implement the
  /// function of this opcode
  virtual void operator()(RLMachine&) { }
  virtual void operator()(RLMachine&, firstType) { }
  virtual void operator()(RLMachine&, firstType, secondType) { }
  virtual void operator()(RLMachine&, firstType, secondType, thirdType) { }
  virtual void operator()(RLMachine&, firstType, secondType, thirdType, fourthType) { }

  virtual void operator()(RLMachine&, firstType, secondType, thirdType, fourthType, fifthType) { }
  virtual void operator()(RLMachine&, firstType, secondType, thirdType, fourthType, fifthType, sixthType) { }
  virtual void operator()(RLMachine&, firstType, secondType, thirdType, fourthType, fifthType, sixthType, seventhType) { }
  virtual void operator()(RLMachine&, firstType, secondType, thirdType, fourthType, fifthType, sixthType, seventhType, eighthType) { }
  virtual void operator()(RLMachine&, firstType, secondType, thirdType, fourthType, fifthType, sixthType, seventhType, eighthType, ninthType) { }
  virtual void operator()(RLMachine&, firstType, secondType, thirdType, fourthType, fifthType, sixthType, seventhType, eighthType, ninthType, tenthType) { }
  virtual void operator()(RLMachine&, firstType, secondType, thirdType, fourthType, fifthType, sixthType, seventhType, eighthType, ninthType, tenthType, eleventhType) { }
  virtual void operator()(RLMachine&, firstType, secondType, thirdType, fourthType, fifthType, sixthType, seventhType, eighthType, ninthType, tenthType, eleventhType, twelvthType) { }
  virtual void operator()(RLMachine&, firstType, secondType, thirdType, fourthType, fifthType, sixthType, seventhType, eighthType, ninthType, tenthType, eleventhType, twelvthType, thirteenthType) { }
};

// Partial specialization for RLOp_Store::checkTypes for when
// everything is empty (aka an operation that takes no parameters)
template<>
inline bool RLOp_Void<Empty_T, Empty_T, Empty_T, Empty_T, Empty_T, Empty_T, Empty_T, 
                      Empty_T, Empty_T, Empty_T, Empty_T, Empty_T, Empty_T>::
checkTypes(RLMachine& machine, boost::ptr_vector<libReallive::ExpressionPiece>& parameters) 
{
  return parameters.size() == 0;
}

// -----------------------------------------------------------------------

/** RLOp that stores the return in the store register. 
 * 
 * @param A Type struct representing type to check for.
 * @param B Type struct representing type to check for.
 * @param C Type struct representing type to check for.
 * @param D Type struct representing type to check for.
 */
template<typename A = Empty_T, typename B = Empty_T, typename C = Empty_T, 
         typename D = Empty_T, typename E = Empty_T, typename F = Empty_T,
         typename G = Empty_T, typename H = Empty_T, typename I = Empty_T,
         typename J = Empty_T, typename K = Empty_T, typename L = Empty_T,
         typename M = Empty_T, typename N = Empty_T, typename O = Empty_T,
         typename P = Empty_T>
struct RLOp_Store : public RLOperation {
  typedef typename A::type firstType;
  typedef typename B::type secondType;
  typedef typename C::type thirdType;
  typedef typename D::type fourthType;
  typedef typename E::type fifthType;
  typedef typename F::type sixthType;
  typedef typename G::type seventhType;
  typedef typename H::type eighthType;
  typedef typename I::type ninthType;
  typedef typename J::type tenthType;
  typedef typename K::type eleventhType;
  typedef typename L::type twelvthType;
  typedef typename M::type thirteenthType;

  bool checkTypes(RLMachine& machine, 
                  boost::ptr_vector<libReallive::ExpressionPiece>& parameters) 
  {
    return (!A::isRealTypestruct  || A::verifyType(parameters, 0)) &&
      (!B::isRealTypestruct || B::verifyType(parameters, 1)) &&
      (!C::isRealTypestruct || C::verifyType(parameters, 2)) &&
      (!D::isRealTypestruct || D::verifyType(parameters, 3)) &&
      (!E::isRealTypestruct || E::verifyType(parameters, 4)) &&
      (!F::isRealTypestruct || F::verifyType(parameters, 5)) &&
      (!G::isRealTypestruct || G::verifyType(parameters, 6)) &&
      (!H::isRealTypestruct || H::verifyType(parameters, 7)) &&
      (!I::isRealTypestruct || I::verifyType(parameters, 8)) &&
      (!J::isRealTypestruct || J::verifyType(parameters, 9)) &&
      (!K::isRealTypestruct || K::verifyType(parameters, 10)) &&
      (!L::isRealTypestruct || L::verifyType(parameters, 11)) &&
      (!M::isRealTypestruct || M::verifyType(parameters, 12));
  }

  void dispatch(RLMachine& machine, 
                boost::ptr_vector<libReallive::ExpressionPiece>& parameters) 
  {
    int store;

    // The following is fucking ugly. If anyone knows a way to make
    // this pretty, but still get gcc to reason about template types
    // and optimize away everything but one of these instances, I'd
    // love to hear from you.
    if(!A::isRealTypestruct)
    {
      store = operator()(machine);
    }
    else if(A::isRealTypestruct && !B::isRealTypestruct)
    {
      store = operator()(machine, A::getData(machine, parameters, 0));
    }
    else if(A::isRealTypestruct && B::isRealTypestruct &&
            !C::isRealTypestruct)
    {
      store = operator()(machine, A::getData(machine, parameters, 0),
                         B::getData(machine, parameters, 1));
    }
    else if(A::isRealTypestruct && B::isRealTypestruct &&
            C::isRealTypestruct && !D::isRealTypestruct)
    {
      store = operator()(machine, A::getData(machine, parameters, 0),
                         B::getData(machine, parameters, 1),
                         C::getData(machine, parameters, 2));
    }
    else if(A::isRealTypestruct && B::isRealTypestruct &&
            C::isRealTypestruct && D::isRealTypestruct &&
            !E::isRealTypestruct)
    {
      store = operator()(machine, A::getData(machine, parameters, 0),
                 B::getData(machine, parameters, 1), 
                 C::getData(machine, parameters, 2),
                 D::getData(machine, parameters, 3));
    }   
    else if(A::isRealTypestruct && B::isRealTypestruct &&
            C::isRealTypestruct && D::isRealTypestruct &&
            E::isRealTypestruct && !F::isRealTypestruct)
    {
      store = operator()(machine, A::getData(machine, parameters, 0),
                 B::getData(machine, parameters, 1), 
                 C::getData(machine, parameters, 2),
                 D::getData(machine, parameters, 3),
                 E::getData(machine, parameters, 4));
    }   
    else if(A::isRealTypestruct && B::isRealTypestruct &&
            C::isRealTypestruct && D::isRealTypestruct &&
            E::isRealTypestruct && F::isRealTypestruct &&
            !G::isRealTypestruct)
    {
      store = operator()(machine, A::getData(machine, parameters, 0),
                 B::getData(machine, parameters, 1), 
                 C::getData(machine, parameters, 2),
                 D::getData(machine, parameters, 3),
                 E::getData(machine, parameters, 4),
                 F::getData(machine, parameters, 5));
    }   
    else if(A::isRealTypestruct && B::isRealTypestruct &&
            C::isRealTypestruct && D::isRealTypestruct &&
            E::isRealTypestruct && F::isRealTypestruct &&
            G::isRealTypestruct && !H::isRealTypestruct)
    {
      store = operator()(machine, A::getData(machine, parameters, 0),
                 B::getData(machine, parameters, 1), 
                 C::getData(machine, parameters, 2),
                 D::getData(machine, parameters, 3),
                 E::getData(machine, parameters, 4),
                 F::getData(machine, parameters, 5),
                 G::getData(machine, parameters, 6));
    }   
    else if(A::isRealTypestruct && B::isRealTypestruct &&
            C::isRealTypestruct && D::isRealTypestruct &&
            E::isRealTypestruct && F::isRealTypestruct &&
            G::isRealTypestruct && H::isRealTypestruct &&
            !I::isRealTypestruct)
    {
      store = operator()(machine, A::getData(machine, parameters, 0),
                 B::getData(machine, parameters, 1), 
                 C::getData(machine, parameters, 2),
                 D::getData(machine, parameters, 3),
                 E::getData(machine, parameters, 4),
                 F::getData(machine, parameters, 5),
                 G::getData(machine, parameters, 6),
                 H::getData(machine, parameters, 7));
    }   
    else if(A::isRealTypestruct && B::isRealTypestruct &&
            C::isRealTypestruct && D::isRealTypestruct &&
            E::isRealTypestruct && F::isRealTypestruct &&
            G::isRealTypestruct && H::isRealTypestruct &&
            I::isRealTypestruct && !J::isRealTypestruct)
    {
      store = operator()(machine, A::getData(machine, parameters, 0),
                 B::getData(machine, parameters, 1), 
                 C::getData(machine, parameters, 2),
                 D::getData(machine, parameters, 3),
                 E::getData(machine, parameters, 4),
                 F::getData(machine, parameters, 5),
                 G::getData(machine, parameters, 6),
                 H::getData(machine, parameters, 7),
                 I::getData(machine, parameters, 8));
    }   
    else if(A::isRealTypestruct && B::isRealTypestruct &&
            C::isRealTypestruct && D::isRealTypestruct &&
            E::isRealTypestruct && F::isRealTypestruct &&
            G::isRealTypestruct && H::isRealTypestruct &&
            I::isRealTypestruct && J::isRealTypestruct &&
            !K::isRealTypestruct)
    {
      store = operator()(machine, A::getData(machine, parameters, 0),
                 B::getData(machine, parameters, 1), 
                 C::getData(machine, parameters, 2),
                 D::getData(machine, parameters, 3),
                 E::getData(machine, parameters, 4),
                 F::getData(machine, parameters, 5),
                 G::getData(machine, parameters, 6),
                 H::getData(machine, parameters, 7),
                 I::getData(machine, parameters, 8),
                 J::getData(machine, parameters, 9));
    }   
    else if(A::isRealTypestruct && B::isRealTypestruct &&
            C::isRealTypestruct && D::isRealTypestruct &&
            E::isRealTypestruct && F::isRealTypestruct &&
            G::isRealTypestruct && H::isRealTypestruct &&
            I::isRealTypestruct && J::isRealTypestruct &&
            K::isRealTypestruct && !L::isRealTypestruct)
    {
      store = operator()(machine, A::getData(machine, parameters, 0),
                 B::getData(machine, parameters, 1), 
                 C::getData(machine, parameters, 2),
                 D::getData(machine, parameters, 3),
                 E::getData(machine, parameters, 4),
                 F::getData(machine, parameters, 5),
                 G::getData(machine, parameters, 6),
                 H::getData(machine, parameters, 7),
                 I::getData(machine, parameters, 8),
                 J::getData(machine, parameters, 9),
                 K::getData(machine, parameters, 10));
    }
    else if(A::isRealTypestruct && B::isRealTypestruct &&
            C::isRealTypestruct && D::isRealTypestruct &&
            E::isRealTypestruct && F::isRealTypestruct &&
            G::isRealTypestruct && H::isRealTypestruct &&
            I::isRealTypestruct && J::isRealTypestruct &&
            K::isRealTypestruct && L::isRealTypestruct &&
            !M::isRealTypestruct)
    {
      store = operator()(machine, A::getData(machine, parameters, 0),
                 B::getData(machine, parameters, 1), 
                 C::getData(machine, parameters, 2),
                 D::getData(machine, parameters, 3),
                 E::getData(machine, parameters, 4),
                 F::getData(machine, parameters, 5),
                 G::getData(machine, parameters, 6),
                 H::getData(machine, parameters, 7),
                 I::getData(machine, parameters, 8),
                 J::getData(machine, parameters, 9),
                 K::getData(machine, parameters, 10),
                 L::getData(machine, parameters, 11));
    }
    else
    {
      store = operator()(machine, A::getData(machine, parameters, 0),
                 B::getData(machine, parameters, 1), 
                 C::getData(machine, parameters, 2),
                 D::getData(machine, parameters, 3),
                 E::getData(machine, parameters, 4),
                 F::getData(machine, parameters, 5),
                 G::getData(machine, parameters, 6),
                 H::getData(machine, parameters, 7),
                 I::getData(machine, parameters, 8),
                 J::getData(machine, parameters, 9),
                 K::getData(machine, parameters, 10),
                 L::getData(machine, parameters, 11),
                 M::getData(machine, parameters, 12));
    }

    machine.setStoreRegister(store);
  }

  /// Method that is overridden by all subclasses to implement the
  /// function of this opcode
  virtual int operator()(RLMachine&) { }
  virtual int operator()(RLMachine&, firstType) { }
  virtual int operator()(RLMachine&, firstType, secondType) { }
  virtual int operator()(RLMachine&, firstType, secondType, thirdType) { }
  virtual int operator()(RLMachine&, firstType, secondType, thirdType, fourthType) { }


  virtual int operator()(RLMachine&, firstType, secondType, thirdType, fourthType, fifthType) { }
  virtual int operator()(RLMachine&, firstType, secondType, thirdType, fourthType, fifthType, sixthType) { }
  virtual int operator()(RLMachine&, firstType, secondType, thirdType, fourthType, fifthType, sixthType, seventhType) { }
  virtual int operator()(RLMachine&, firstType, secondType, thirdType, fourthType, fifthType, sixthType, seventhType, eighthType) { }
  virtual int operator()(RLMachine&, firstType, secondType, thirdType, fourthType, fifthType, sixthType, seventhType, eighthType, ninthType) { }
  virtual int operator()(RLMachine&, firstType, secondType, thirdType, fourthType, fifthType, sixthType, seventhType, eighthType, ninthType, tenthType) { }
  virtual int operator()(RLMachine&, firstType, secondType, thirdType, fourthType, fifthType, sixthType, seventhType, eighthType, ninthType, tenthType, eleventhType) { }
  virtual int operator()(RLMachine&, firstType, secondType, thirdType, fourthType, fifthType, sixthType, seventhType, eighthType, ninthType, tenthType, eleventhType, twelvthType) { }
  virtual int operator()(RLMachine&, firstType, secondType, thirdType, fourthType, fifthType, sixthType, seventhType, eighthType, ninthType, tenthType, eleventhType, twelvthType, thirteenthType) { }
};

// Partial specialization for RLOp_Store::checkTypes for when
// everything is empty (aka an operation that takes no parameters)
template<>
inline bool RLOp_Store<Empty_T, Empty_T, Empty_T, Empty_T, Empty_T, Empty_T, Empty_T, 
                      Empty_T, Empty_T, Empty_T, Empty_T, Empty_T, Empty_T>::
checkTypes(RLMachine& machine, boost::ptr_vector<libReallive::ExpressionPiece>& parameters) 
{
  return parameters.size() == 0;
}
// @}

// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// IMPLEMENTATION OF NON-INLINE FUNCTIONS FROM ABOVE DECLARATIONS
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------



// -----------------------------------------------------------------------

template<typename CON>
struct Argc_T<CON>::type Argc_T<CON>::getData(RLMachine& machine, 
                     boost::ptr_vector<libReallive::ExpressionPiece>& p,
                     int position) {
  type returnVector;
//  std::cerr << "(" << position << ", " << p.size() << std::endl;
  for(int i = position; i < p.size(); ++i)
    returnVector.push_back(CON::getData(machine, p, i));

  return returnVector;
}

// -----------------------------------------------------------------------

template<typename CON>
bool Argc_T<CON>::verifyType(boost::ptr_vector<libReallive::ExpressionPiece>& p,
                             int position) {
  for(int i = position; i < p.size(); ++i) {
    if(!CON::verifyType(p, i)) {
      return false;
    } 
  }

  return true;
}

#endif