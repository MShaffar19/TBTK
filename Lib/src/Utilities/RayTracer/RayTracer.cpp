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

/** @file RayTracer.cpp
 *
 *  @author Kristofer Björnson
 */

#include "../../../include/Utilities/RayTracer/RayTracer.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

namespace TBTK{

RayTracer::RayTracer(){
	cameraPosition = Vector3d({0, 0, 10});
	focus = Vector3d({0, 0, 0});
	up = Vector3d({0, 1, 0});

	width = 600;
	height = 400;

	stateRadius = 0.5;
}

RayTracer::~RayTracer(){
}

void RayTracer::plot(const Model& model, const Property::Density &density){
	const IndexDescriptor &indexDescriptor = density.getIndexDescriptor();
	TBTKAssert(
		indexDescriptor.getFormat() == IndexDescriptor::Format::Custom,
		"RayTracer::plot()",
		"Only storage format IndexDescriptor::Format::Custom supported.",
		"Use calculateProperty(patterns) instead of "
			<< "calculateProperty(pattern, ranges) when extracting"
			<< " properties."
	);

	trace(
		indexDescriptor,
		model,
		[&density](const HitDescriptor &hitDescriptor) -> RayTracer::Color
		{
			Color color;
			color.r = density(hitDescriptor.getIndex());
			color.g = density(hitDescriptor.getIndex());
			color.b = density(hitDescriptor.getIndex());

			return color;
		}
	);
}

void RayTracer::plot(
	const Model& model,
	const Property::Magnetization &magnetization
){
	const IndexDescriptor &indexDescriptor = magnetization.getIndexDescriptor();
	TBTKAssert(
		indexDescriptor.getFormat() == IndexDescriptor::Format::Custom,
		"RayTracer::plot()",
		"Only storage format IndexDescriptor::Format::Custom supported.",
		"Use calculateProperty(patterns) instead of "
			<< "calculateProperty(pattern, ranges) when extracting"
			<< " properties."
	);

	trace(
		indexDescriptor,
		model,
		[&magnetization](const HitDescriptor &hitDescriptor) -> RayTracer::Color
		{
			Vector3d directionFromObject = hitDescriptor.getDirectionFromObject();

			Color color;
/*			color.r = density(hit);
			color.g = density(hit);
			color.b = density(hit);*/

			return color;
		}
	);
}

void RayTracer::plot(
	const Model& model,
	const Property::WaveFunction &waveFunction,
	unsigned int state
){
	const IndexDescriptor &indexDescriptor = waveFunction.getIndexDescriptor();
	TBTKAssert(
		indexDescriptor.getFormat() == IndexDescriptor::Format::Custom,
		"RayTracer::plot()",
		"Only storage format IndexDescriptor::Format::Custom supported.",
		"Use calculateProperty(patterns) instead of "
			<< "calculateProperty(pattern, ranges) when extracting"
			<< " properties."
	);

	trace(
		indexDescriptor,
		model,
		[&waveFunction, state](const HitDescriptor &hitDescriptor) -> RayTracer::Color
		{
			complex<double> amplitude = waveFunction(hitDescriptor.getIndex(), state);
			double absolute = abs(amplitude);
			double argument = arg(amplitude);
			if(argument < 0)
				argument += 2*M_PI;
			Color color;
			color.r = absolute*(2*M_PI - argument);
			color.g = 0;
			color.b = absolute*argument;

			return color;
		}
	);
}

void RayTracer::trace(
	const IndexDescriptor &indexDescriptor,
	const Model &model,
	function<Color(const HitDescriptor &hitDescriptor)> &&lambdaColorPicker
//	Color (*colorCallback)(const Index &hit)
){
	Vector3d unitY = up.unit();
	Vector3d unitX = ((focus - cameraPosition)*up).unit();
	unitY = (unitX*(focus - cameraPosition)).unit();
	double scaleFactor = (focus - cameraPosition).norm()/width;

	const Geometry *geometry = model.getGeometry();

	const IndexTree &indexTree = indexDescriptor.getIndexTree();
	IndexTree::Iterator iterator = indexTree.begin();
	const Index *index;
	vector<Vector3d> coordinates;
	while((index = iterator.getIndex())){
		Index i = *index;
		for(unsigned int n = 0; n < i.size(); n++)
			if(i.at(n) < 0)
				i.at(n) = IDX_ALL;

		vector<Index> indices = model.getHoppingAmplitudeSet()->getIndexList(i);

		coordinates.push_back(Vector3d({0., 0., 0.}));
		for(unsigned int n = 0; n < indices.size(); n++){
			const double *c = geometry->getCoordinates(indices.at(n));
			coordinates.back().x += c[0]/indices.size();
			coordinates.back().y += c[1]/indices.size();
			coordinates.back().z += c[2]/indices.size();
		}

		iterator.searchNext();
	}

	Mat canvas = Mat::zeros(height, width, CV_32FC3);
	for(unsigned int x = 0; x < width; x++){
		for(unsigned int y = 0; y < height; y++){
			Vector3d target = focus
				+ (scaleFactor*((double)x - width/2))*unitX
				+ (scaleFactor*((double)y - height/2))*unitY;
			Vector3d direction = (target - cameraPosition).unit();

			vector<unsigned int> hits;
			for(unsigned int n = 0; n < coordinates.size(); n++)
				if(((coordinates.at(n) - cameraPosition)*direction).norm() < stateRadius)
					hits.push_back(n);

			double valueR = 0.;
			double valueG = 0.;
			double valueB = 0.;
			if(hits.size() > 0){
				double minDistance = (coordinates.at(hits.at(0)) - cameraPosition).norm();
				unsigned int minDistanceIndex = hits.at(0);
				for(unsigned int n = 1; n < hits.size(); n++){
					double distance = (coordinates.at(hits.at(n)) - cameraPosition).norm();
					if(distance < minDistance){
						minDistance = distance;
						minDistanceIndex = hits.at(n);
					}
				}

				HitDescriptor hitDescriptor;
				hitDescriptor.setIndex(
					indexTree.getPhysicalIndex(
						minDistanceIndex
					)
				);
				hitDescriptor.setCoordinate(
					coordinates.at(
						hits.at(
							minDistanceIndex
						)
					)
				);
				Color color = lambdaColorPicker(
					hitDescriptor
				);
				valueR = color.r;
				valueG = color.g;
				valueB = color.b;
			}

			canvas.at<Vec3f>(y, x)[0] = valueB;
			canvas.at<Vec3f>(y, x)[1] = valueG;
			canvas.at<Vec3f>(y, x)[2] = valueR;
		}
	}

	double minValue = canvas.at<Vec3f>(0, 0)[0];
	double maxValue = canvas.at<Vec3f>(0, 0)[0];
	for(unsigned int x = 0; x < width; x++){
		for(unsigned int y = 0; y < height; y++){
			for(int n = 0; n < 3; n++){
				if(canvas.at<Vec3f>(y, x)[n] < minValue)
					minValue = canvas.at<Vec3f>(y, x)[n];
				if(canvas.at<Vec3f>(y, x)[n] > minValue)
					maxValue = canvas.at<Vec3f>(y, x)[n];
			}
		}
	}

	Mat image = Mat::zeros(height, width, CV_8UC3);
	for(unsigned int x = 0; x < width; x++)
		for(unsigned int y = 0; y < height; y++)
			for(unsigned int n = 0; n < 3; n++)
				image.at<Vec3b>(y, x)[n] = 255*((canvas.at<Vec3f>(y, x)[n])/(maxValue - minValue));

	imwrite("figures/Density.png", image);
}

RayTracer::HitDescriptor::HitDescriptor(){
	directionFromObject = nullptr;
}

RayTracer::HitDescriptor::HitDescriptor(const HitDescriptor &hitDescriptor){
	if(hitDescriptor.directionFromObject == nullptr){
		directionFromObject = nullptr;
	}
	else{
		directionFromObject = new Vector3d(
			*hitDescriptor.directionFromObject
		);
	}
}

RayTracer::HitDescriptor::HitDescriptor(HitDescriptor &&hitDescriptor){
	if(hitDescriptor.directionFromObject == nullptr){
		directionFromObject = nullptr;
	}
	else{
		directionFromObject = hitDescriptor.directionFromObject;
		hitDescriptor.directionFromObject = nullptr;
	}
}

RayTracer::HitDescriptor::~HitDescriptor(){
	if(directionFromObject != nullptr)
		delete directionFromObject;
}

RayTracer::HitDescriptor& RayTracer::HitDescriptor::operator=(
	const HitDescriptor &rhs
){
	if(rhs.directionFromObject == nullptr)
		directionFromObject = nullptr;
	else
		directionFromObject = new Vector3d(*rhs.directionFromObject);

	return *this;
}

RayTracer::HitDescriptor& RayTracer::HitDescriptor::operator=(
	HitDescriptor &&rhs
){
	if(this != &rhs){
		if(rhs.directionFromObject == nullptr){
			directionFromObject = nullptr;
		}
		else{
			directionFromObject = rhs.directionFromObject;
			rhs.directionFromObject = nullptr;
		}
	}

	return *this;
}

const Vector3d& RayTracer::HitDescriptor::getDirectionFromObject() const{
	if(directionFromObject != nullptr)
		return *directionFromObject;

/*	Vector3d v = objectCoordinate - cameraPosition;
	double a = Vector3d::dotProduct(v, direction);*/
}

};	//End of namespace TBTK