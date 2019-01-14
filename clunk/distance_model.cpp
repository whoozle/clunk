/*
MIT License

Copyright (c) 2008-2019 Netive Media Group & Vladimir Menshakov

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <clunk/distance_model.h>

float clunk::DistanceModel::gain(float distance) const {
	float gain = 0;
	distance /= distance_divisor;
	switch(type) {
	case Inverse: 
		if (clamped) {
			if (distance < reference_distance)
				distance = reference_distance;
			if (distance > max_distance)
				distance = max_distance;
		}
		gain = reference_distance / (reference_distance + rolloff_factor * (distance - reference_distance));
		break;
		
	case Linear: 
		if (clamped) {
			if (distance < reference_distance)
				distance = reference_distance;
			if (distance > max_distance)
				distance = max_distance;			
		} 
		
		gain = 1 - rolloff_factor * (distance - reference_distance) / (max_distance - reference_distance);
		break;
	
	case Exponent: 
		if (clamped) {
			if (distance < reference_distance)
				distance = reference_distance;
			if (distance > max_distance)
				distance = max_distance;
		}
		gain = powf(distance / reference_distance, - rolloff_factor);
		
		break;	
	}

	if (gain < 0)
		gain = 0;
	else if (gain > 1)
		gain = 1;

	return gain;
}


float clunk::DistanceModel::doppler_pitch(const v3f &sl, const v3f &s_vel, const v3f &l_vel) const {
	if (doppler_factor <= 0)
		return 1.0f;
	
	float len = sl.length();
	if (len <= 0)
		return 1.0f;
	
	float max_speed = speed_of_sound / doppler_factor;
	
	float vls = sl.dot_product(l_vel) / len;
	if (vls > max_speed)
		vls = max_speed;

	float vss = sl.dot_product(s_vel) / len;
	if (vss > max_speed)
		vss = max_speed;
	
	return (speed_of_sound - doppler_factor * vls) / (speed_of_sound - doppler_factor * vss);
}
