/* Copyright 2019 Kristofer Björnson
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

/** @file Index.cpp
 *  @brief Generates and validates @link Index Indices@endlink.
 *
 *  @author Kristofer Björnson
 */

#include "TBTK/Index.h"
#include "TBTK/Validation/Validation.h"

#include <iostream>

using namespace std;
using namespace TBTK;

int NUM_TESTS = 1;

int main(int argc, char **argv){
	Mode mode;
	int id;
	init(argc, argv, mode, id, "Index");

	if(mode == NumTests){
		cout << NUM_TESTS;

		return 0;
	}

	Index index;
	switch(id){
	case 0:
		index = Index({{1, 2, 3}, {4, 5}});
		break;
	default:
		TBTKExit(
			"Index",
			"Unknown test id '" << id << "'.",
			""
		);
	}

	execute<Index>(index, mode, id, "Index");

	return 0;
}
