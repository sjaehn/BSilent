#include <cstring>
#include <stdexcept>
#include <lv2/lv2plug.in/ns/lv2core/lv2.h>
#include <array>
#include "Definitions.hpp"
#include "Ports.hpp"

class BSilent 
{
public:
	BSilent (const double rate, const char* bundle_path, const LV2_Feature* const* features);
	void run (uint32_t n_samples);
	void connect_port (uint32_t portnr, void* data);

private:
	std::array<void*, BSILENT_N_PORTS> ports;
};


BSilent::BSilent (const double rate, const char* bundle_path, const LV2_Feature* const* features)
{
	// Init ports
	ports.fill (NULL);
}

void BSilent::connect_port (uint32_t portnr, void* data)
{
	if (portnr < BSILENT_N_PORTS) ports[portnr] = data;
}

void BSilent::run (uint32_t n_samples)
{
	float* const audioOut = (float*) ports[BSILENT_AUDIO_OUT];
	if (audioOut) memset (audioOut, 0, n_samples * sizeof (float));
}

static LV2_Handle
instantiate	(const LV2_Descriptor* descriptor, double rate, const char* bundle_path, const LV2_Feature* const* features)
{
	BSilent* instance = new BSilent (rate, bundle_path, features);
	return (LV2_Handle) instance;
}

static void
connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	BSilent* bSilent = (BSilent*)instance;
	if (bSilent) bSilent->connect_port (port, data);
}

static void
run(LV2_Handle instance, uint32_t n_samples)
{
	BSilent* bSilent = (BSilent*) instance;
	if (bSilent) bSilent->run (n_samples);
}

static void
cleanup(LV2_Handle instance)
{
	BSilent* bSilent = (BSilent*) instance;
	if (bSilent) delete bSilent;
}

static const LV2_Descriptor descriptor =
{
	BSILENT_URI,
	instantiate,
	connect_port,
	NULL,
	run,
	NULL,
	cleanup,
	NULL
};

LV2_SYMBOL_EXPORT
const LV2_Descriptor*
lv2_descriptor(uint32_t index)
{
	switch (index)
	{
		case 0:  return &descriptor;
		default: return NULL;
	}
}
