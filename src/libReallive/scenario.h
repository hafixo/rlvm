#ifndef SCENARIO_H
#define SCENARIO_H

#include "defs.h"
#include "bytecode.h"

namespace libReallive {

#include "scenario_internals.h"

class Scenario {
	Header header;
	Script script;
public:
	Scenario(const char* data, const size_t length);
	Scenario(const FilePos& fp);

	// Strip a scenario of non-essential metadata.
	Scenario& strip();

	// Run a peephole optimisation pass.
	Scenario& optimise();
	
	// Rebuild a scenario to bytecode, optionally removing unnecessary debugging metadata at the same time.
	const string* rebuild();

	// Access to script
	typedef BytecodeList::const_iterator const_iterator;
	typedef BytecodeList::iterator iterator;

  /// Locate the entrypoint
  const_iterator findEntrypoint(int entrypoint) const;
	
	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;
	const size_t size() const;
	iterator insert(iterator pos, BytecodeElement* elt);
	iterator erase(iterator pos);
	void erase(iterator first, iterator last);
	iterator find_next(const ElementType what, iterator where);
 	const_iterator find_next(const ElementType what, const_iterator where) const;
};

// Inline definitions for Scenario

inline
Scenario::Scenario(const char* data, const size_t length) : header(data, length), script(header, data, length) 
{
}
inline
Scenario::Scenario(const FilePos& fp) : header(fp.data, fp.length), script(header, fp.data, fp.length)
{
}
inline Scenario& 
Scenario::strip()
{
	script.invalidate();
	script.strip = true;
	return *this;
}
inline Scenario::iterator
Scenario::begin()
{
	return script.elts.begin(); 
}
inline Scenario::iterator
Scenario::end()
{ 
	return script.elts.end(); 
}
inline Scenario::const_iterator
Scenario::begin() const
{ 
	return script.elts.begin(); 
}
inline Scenario::const_iterator
Scenario::end() const
{ 
	return script.elts.end(); 
}
inline const size_t
Scenario::size() const
{ 
	return script.elts.size();
}
inline Scenario::iterator
Scenario::insert(iterator pos, BytecodeElement* elt)
{ 
	return script.elts.insert(pos, elt); 
}
inline void
Scenario::erase(Scenario::iterator first, Scenario::iterator last)
{ 
	if (first != last) do { erase(--last); } while (first != last); 
}
inline Scenario::iterator
Scenario::erase(Scenario::iterator pos)
{ 
	Pointers* ptrs = pos->get_pointers();
	if (ptrs) for (Pointers::iterator it = ptrs->begin(); it != ptrs->end(); ++it) script.remove_label(*it, pos);
	script.remove_elt(pos);
	return pos;
}
inline Scenario::iterator
Scenario::find_next(const ElementType what, Scenario::iterator where)
{
	if (where != end()) do { ++where; } while (where != end() && where->type() != what); return where;
}
inline Scenario::const_iterator
Scenario::find_next(const ElementType what, Scenario::const_iterator where) const
{
	if (where != end()) do { ++where; } while (where != end() && where->type() != what); return where;
}

}

#endif