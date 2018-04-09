/* Copyright 2017 Kristofer Björnson
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/** @file ManyBodyContext.cpp
 *
 *  @author Kristofer Björnson
 */

#include "TBTK/ManyBodyContext.h"
#include "TBTK/Streams.h"

using namespace std;

namespace TBTK{

//ManyBodyContext::ManyBodyContext(FockSpace<BitRegister> *fockSpace
ManyBodyContext::ManyBodyContext(const SingleParticleContext *singleParticleContext
) :
/*	brFockSpace(fockSpace),
	ebrFockSpace(NULL),
	interactionAmplitudeSet(fockSpace->getHoppingAmplitudeSet())*/
	brFockSpace(nullptr),
	ebrFockSpace(nullptr),
	interactionAmplitudeSet(nullptr)
{
/*	brFockSpace = make_shared<FockSpace<BitRegister>>(fockSpace);
	ebrFockSpace = make_shared<FockSpace<ExtensiveBitRegister>>(NULL);*/
	interactionAmplitudeSet.reset(new InteractionAmplitudeSet(singleParticleContext));
	if(singleParticleContext->getBasisSize() < 32){	//Only 31 states can be encoded in a FockState<BitRegister>.
		brFockSpace.reset(
			new FockSpace<BitRegister>(
				singleParticleContext,
				singleParticleContext->getStatistics(),
				1	//Only one particle per state at the moment
			)
		);
	}
	else{
		ebrFockSpace.reset(
			new FockSpace<ExtensiveBitRegister>(
				singleParticleContext,
				singleParticleContext->getStatistics(),
				1	//Only one particle per state at the moment
			)
		);
	}

	interactionAmplitudeSet.reset(
		new InteractionAmplitudeSet(
			singleParticleContext
		)
	);
}

/*ManyBodyContext::ManyBodyContext(FockSpace<ExtensiveBitRegister> *fockSpace
) :
	brFockSpace(NULL),
	ebrFockSpace(fockSpace),
	interactionAmplitudeSet(fockSpace->getHoppingAmplitudeSet())
{
}*/

ManyBodyContext::~ManyBodyContext(){
/*	if(brFockSpace != NULL)
		delete brFockSpace;

	if(ebrFockSpace != NULL)
		delete ebrFockSpace;*/
}

};	//End of namespace TBTK
