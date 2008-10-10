#include "distance_model.h"
#include <math.h>

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
		if (clamped && distance < reference_distance) {
			distance = reference_distance;
		} 
		
		if (distance > max_distance)
			distance = max_distance;
			
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


float clunk::DistanceModel::doppler_pitch(const v3<float> &sl, const v3<float> &s_vel, const v3<float> &l_vel) const {
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
