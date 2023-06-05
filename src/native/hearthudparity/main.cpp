#include <hook.h>
#include <symbol.h>

#include "main.hpp"


void HeartHudParityModule::initialize() {

	DLHandleManager::initializeHandle("libminecraftpe.so", "mcpe");

	

}


MAIN {
	Module* main_module = new HeartHudParityModule();
}