#ifndef CLUNK_DISTANCE_MODEL_H__
#define CLUNK_DISTANCE_MODEL_H__

#include "export_clunk.h"
#include "v3.h"

namespace clunk {

//!Distance model used in context.

struct CLUNKAPI DistanceModel {
	//!Type of the distance model: inversed, linear or exponent.
	enum Type { Inverse, Linear, Exponent };

	//!Type of the distance model
	Type type;
	//!Clamping means cutting off max_distance, you must set max_distance to some value then.
	bool clamped;
	
	//!Minimal distance for distance model, default == 1
	float reference_distance;
	//!Maximum distance for clamping distance models. MUST NOT BE DEFAULT
	float max_distance; 

	//!Rolloff factor for distance models
	float rolloff_factor;
	
	//!Dopple factor. 0 disables dopple effect
	float doppler_factor;
	
	//!Speed of sound
	float speed_of_sound;
	
	//! distance_divisor
	float distance_divisor;
	
	/*!
		\brief Constructor
		\param[in] type type of the distance model: inversed, linear or exponent.
		\param[in] clamped means that max_distance will be used.
		\param[in] max_distance maximum distance for the model.
	*/ 
	DistanceModel(Type type, bool clamped, float max_distance = 0): type(type), clamped(clamped), 
	reference_distance(1), max_distance(max_distance), rolloff_factor(1), doppler_factor(0), 
	speed_of_sound(343.3f), distance_divisor(1) 
	{}
	
	//! Computes gain by distance. Return values is in [0-1] range.
	float gain(float distance) const;
	//! Computes doppler pitch.
	float doppler_pitch(const v3<float> &sl, const v3<float> &s_vel, const v3<float> &l_vel) const;
};

}

#endif

