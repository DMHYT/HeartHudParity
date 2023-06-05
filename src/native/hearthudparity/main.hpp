#include <mod.h>

#include "recovered.hpp"

#ifndef HEARTHUDPARITY_MAIN_HPP
#define HEARTHUDPARITY_MAIN_HPP


class HeartHudParityModule : public Module {
	public:
	HeartHudParityModule(): Module("hearthudparity") {};
	virtual void initialize();
};


#endif //HEARTHUDPARITY_MAIN_HPP