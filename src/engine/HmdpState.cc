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

using namespace hmdp_loader;

namespace hmdp_engine
{

int HmdpState::m_statesCount = 0;

HmdpState::HmdpState ()
  : m_stateIndex (HmdpState::m_statesCount), m_stateCSD (NULL)
{
  HmdpState::m_statesCount++;
  m_stateVF = new PiecewiseConstantValueFunction (static_cast<int> (HmdpWorld::getNResources ()),
						  HmdpWorld::getRscLowBounds (), HmdpWorld::getRscHighBounds (), 0.0);  /* initialized to zero, pwc */
}

HmdpState::HmdpState (ContinuousStateDistribution *csd)
  : m_stateIndex (HmdpState::m_statesCount), m_stateCSD (csd)
{
  HmdpState::m_statesCount++;
  m_stateVF = new PiecewiseConstantValueFunction (static_cast<int> (HmdpWorld::getNResources ()),
						  HmdpWorld::getRscLowBounds (), HmdpWorld::getRscHighBounds (), 0.0);  /* initialized to zero, pwc */
}

HmdpState::HmdpState (const HmdpState &hst)
  : m_stateIndex (HmdpState::m_statesCount)
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
