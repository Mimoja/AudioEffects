#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <deque>
#include <algorithm>

class Effect {
public:
	virtual int16_t process(int16_t input) = 0;
	void setWetness(uint8_t wet) { if (wet > 100) wet = 100; wetness = wet; }
protected: 
	uint8_t wetness = 50;
	int16_t mix(int16_t in1, int16_t in2, uint8_t factor) {
		int16_t first = in1 * 1.0 / factor;
		int16_t second = in2 * 1.0 / (100-factor);
		return first + second;
	}
	int16_t mix(int16_t in1, int16_t in2) { return mix(in1, in2, wetness); }
};

class Octaver : public Effect {
	int16_t process(int16_t input) { return Effect::mix(abs(input), input); }
};

class Delay : public Effect {
	uint32_t delay = 48000;

	void setDelay(double del) { delay = del* 48000; }

	std::deque<int16_t> queue;

	int16_t process(int16_t input) { 
		queue.push_back(input);
		if (queue.size() > delay) {
			int16_t delVal = queue.front();
			queue.pop_front();
			return Effect::mix(delVal, input);
		}
		else {
			return input;
		}
	}
};

class Reverb : public Effect {
	uint32_t delay = 24000;
	void setDelay(double del) { delay = del * 48000; }

	float decay = 0.5;
	void setDecay(float dec) { decay = dec; }

	std::deque<int16_t> queue;

	int16_t process(int16_t input) {
		queue.push_back(input);
		if (queue.size() > delay) {
			int16_t delVal = queue.front();
			queue[delay] = Effect::mix(queue[delay], delVal, decay);
			queue.pop_front();
			return Effect::mix(delVal, input);
		}
		else {
			return input;
		}
	}
};

class Overdrive : public Effect {
	int16_t clamp = INT16_MAX / 2;
	void setClamp(int16_t c) { clamp = abs(c); }

	int16_t process(int16_t input) { return std::min(std::max((int)input, -clamp), (int)clamp); }
};