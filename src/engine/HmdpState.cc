/**
 * Copyright 2014 Emmanuel Benazera beniz@droidnik.fr
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "HmdpState.h"
#include "HmdpWorld.h"
#include "BspTreeOperations.h"
#include <algorithm>

using namespace hmdp_loader;

static unsigned int seed = 4294967291UL; // prime.
unsigned int murmurhash2 ( const void * key, int len, unsigned int seed )
{
	// 'm' and 'r' are mixing constants generated offline.
	// They're not really 'magic', they just happen to work well.

	const unsigned int m = 0x5bd1e995;
	const int r = 24;

	// Initialize the hash to a 'random' value

	unsigned int h = seed ^ len;

	// Mix 4 bytes at a time into the hash

	const unsigned char * data = (const unsigned char *)key;

	while(len >= 4)
	{
		unsigned int k = *(unsigned int *)data;

		k *= m; 
		k ^= k >> r; 
		k *= m; 
		
		h *= m; 
		h ^= k;

		data += 4;
		len -= 4;
	}
	
	// Handle the last few bytes of the input array

	switch(len)
	{
	case 3: h ^= data[2] << 16;
	case 2: h ^= data[1] << 8;
	case 1: h ^= data[0];
	        h *= m;
	};

	// Do a few final mixes of the hash to ensure the last few
	// bytes are well-incorporated.

	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h;
} 

namespace hmdp_engine
{

int HmdpState::m_statesCount = 0;

HmdpState::HmdpState ()
  : m_stateIndex (HmdpState::m_statesCount), m_stateCSD (NULL), m_residual(0.0), m_priority(0.0)
{
  HmdpState::m_statesCount++;
  m_stateVF = new PiecewiseConstantValueFunction (static_cast<int> (HmdpWorld::getNResources ()),
						  HmdpWorld::getRscLowBounds (), HmdpWorld::getRscHighBounds (), 0.0);  /* initialized to zero, pwc */
}

HmdpState::HmdpState (ContinuousStateDistribution *csd)
  : m_stateIndex (HmdpState::m_statesCount), m_stateCSD (csd), m_residual(0.0), m_priority(0.0)
{
  HmdpState::m_statesCount++;
  m_stateVF = new PiecewiseConstantValueFunction (static_cast<int> (HmdpWorld::getNResources ()),
						  HmdpWorld::getRscLowBounds (), HmdpWorld::getRscHighBounds (), 0.0);  /* initialized to zero, pwc */
}

HmdpState::HmdpState (const HmdpState &hst)
  : m_stateIndex (HmdpState::m_statesCount), m_residual(hst.getResidual()), m_priority(hst.getPriority())
{
  HmdpState::m_statesCount++;
  
  /* copy value map and atom set elements */
#ifdef HAVE_PPDDL
  for (AtomSet::const_iterator ai = hst.getDiscStateConst ().begin ();
       ai != hst.getDiscStateConst ().end (); ai++)
    m_atoms.insert (*ai);
  for (ValueMap::const_iterator vi = hst.getContStateConst ().begin ();
       vi != hst.getContStateConst ().end (); vi++)
    m_values.insert (*vi);
#endif
  
  if (hst.getVF ())
    m_stateVF = static_cast<ValueFunction*> (BspTreeOperations::copyTree (hst.getVF ()));
  else m_stateVF = 0;

  if (hst.getCSD ())
    m_stateCSD = static_cast<ContinuousStateDistribution*> (BspTreeOperations::copyTree (hst.getCSD ()));
  else m_stateCSD = 0;
}

HmdpState::~HmdpState ()
{
  if (m_stateVF)
    BspTree::deleteBspTree (m_stateVF);
  if (m_stateCSD)
    BspTree::deleteBspTree (m_stateCSD);
}

bool HmdpState::isEqual (const HmdpState &hst)
{
#ifdef HAVE_PPDDL
  if (m_values.size () != hst.getContStateConst ().size ()) return false;
  std::unordered_map<const Application*, Rational>::const_iterator vmi;
  for (vmi = m_values.begin (); vmi != m_values.end (); vmi++)
    if (hst.getContStateConst ().find ((*vmi).first) == hst.getContStateConst ().end ())
      return false;
  if (m_atoms.size () != hst.getDiscStateConst ().size ()) return false;
  std::unordered_set<const Atom*>::const_iterator asi;
  for (asi = m_atoms.begin (); asi != m_atoms.end (); asi++)
    if (hst.getDiscStateConst ().find ((*asi)) == hst.getDiscStateConst ().end ())
      return false;
  return true;
#else
  return (m_stateIndex == hst.getStateIndex ());
#endif
}

bool HmdpState::isDiscreteEqual (const HmdpState &hst)
{
#ifdef HAVE_PPDDL
  if (m_atoms.size () != hst.getDiscStateConst ().size ()) return false;
  std::unordered_set<const Atom*>::const_iterator asi;
  for (asi = m_atoms.begin (); asi != m_atoms.end (); asi++)
    if (hst.getDiscStateConst ().find ((*asi)) == hst.getDiscStateConst ().end ())
      return false;
  return true;
#else
  return (m_stateIndex == hst.getStateIndex ());
#endif
}

void HmdpState::setVF (ValueFunction *vf)
{
  if (m_stateVF)
    BspTree::deleteBspTree (m_stateVF);
  m_stateVF = vf;
}

void HmdpState::setCSD (ContinuousStateDistribution *csd)
{
  if (m_stateCSD)
    BspTree::deleteBspTree (m_stateCSD);
  m_stateCSD = csd;
}

void HmdpState::setCSDToNull ()
{
    m_stateCSD = NULL;
}

std::string HmdpState::to_str() const
{
#ifdef HAVE_PPDDL
  const AtomSet &atmst = getDiscStateConst ();
  int total = atmst.size();
  std::vector<std::string> atoms;
  atoms.reserve(total);
  int size = 0;
  for (AtomSet::const_iterator it = atmst.begin(); it != atmst.end(); it++) {
    std::string tomstr = "";
    HmdpPpddlLoader::printAtom (tomstr, *(*it));
    size += static_cast<int> (tomstr.length ());
    atoms.push_back(tomstr);
  }  
  std::string concat;
  std::stable_sort(atoms.begin(),atoms.end());
  for (auto vit=atoms.begin();vit!=atoms.end();++vit)
    concat += (*vit);
  return concat;
#else
  return ""; // beware.
#endif
}

unsigned int HmdpState::to_uint() const
{
  std::string str = to_str();
  return murmurhash2(str.c_str(),str.length(),seed);
}
  
/* printing */
void HmdpState::print (std::ostream &out)
{
  out << "{state index: " << m_stateIndex << std::endl;
#ifdef HAVE_PPDDL
  HmdpWorld::printState (out, m_values, m_atoms);
#endif
  /* if (m_stateVF)
    {
      out << "\nstate value function: ";
      m_stateVF->print (out, HmdpWorld::getRscLowBounds (), HmdpWorld::getRscHighBounds ());
    }
  if (m_stateCSD)
    {
      out << "\nstate continuous distribution over resources: ";
      m_stateCSD->print (out, HmdpWorld::getRscLowBounds (), HmdpWorld::getRscHighBounds ());
    }*/
  out << "}\n";
}

} /* end of namespace */
