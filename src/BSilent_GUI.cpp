#include <cstdint>
#include <lv2/lv2plug.in/ns/lv2core/lv2.h>
#include <lv2/lv2plug.in/ns/extensions/ui/ui.h>
#include <lv2/lv2plug.in/ns/ext/atom/forge.h>
#include "BWidgets/BUtilities/to_string.hpp"
#include "BWidgets/BEvents/ExposeEvent.hpp"
#include "BWidgets/BStyles/Types/Fill.hpp"
#include "BWidgets/BWidgets/Window.hpp"
#include "BWidgets/BWidgets/ComboBox.hpp"
#include "BWidgets/BWidgets/ValueDial.hpp"
#include "BWidgets/BWidgets/HPianoRoll.hpp"
#include "Definitions.hpp"
#include "Ports.hpp"
#include "Urids.hpp"

#define BG_FILE "inc/surface.png"

class BSilent_GUI : public BWidgets::Window
{
public:
	BSilent_GUI (const char *bundle_path, const LV2_Feature *const *features, PuglNativeView parentWindow);
	void portEvent (uint32_t port_index, uint32_t buffer_size, uint32_t format, const void* buffer);
	virtual void onConfigureRequest (BEvents::Event* event) override;
	static void valueChangedCallback (BEvents::Event* event);
	static void pianoCallback (BEvents::Event* event);

	LV2UI_Write_Function write_function;
	LV2UI_Controller controller;

protected:
	BSilentURIs uris;
	LV2_Atom_Forge forge;
	std::string pluginPath;
	BWidgets::ComboBox presetMenu;
	BWidgets::ValueDial gainDial;
	BWidgets::ValueDial tuneDial;
	std::array<BWidgets::Valueable*, BSILENT_NR_CONTROLLERS> controllerWidgets;
	BWidgets::HPianoRoll pianoRoll;
	std::array<uint8_t, 128> velocities;

	// Style definitions
	BStyles::ColorMap fgColors = BStyles::ColorMap {{{0.9, 0.9, 0.9, 1.0}, {1.0, 1.0, 1.0, 1.0}, {0.1, 0.1, 0.1, 1.0}, {0.0, 0.0, 0.0, 0.0}}};
	BStyles::ColorMap kbColors = BStyles::ColorMap {{{0.2, 0.2, 1.0, 1.0}, {1.0, 1.0, 1.0, 1.0}, {0.1, 0.1, 0.1, 1.0}, {0.0, 0.0, 0.0, 0.0}}};
	BStyles::ColorMap txColors = BStyles::ColorMap {{{1.0, 1.0, 1.0, 1.0}, {1.0, 1.0, 1.0, 1.0}, {0.1, 0.1, 0.1, 1.0}, {0.0, 0.0, 0.0, 0.0}}};
	BStyles::ColorMap bgColors = BStyles::ColorMap {{{0.15, 0.15, 0.15, 1.0}, {0.3, 0.3, 0.3, 1.0}, {0.05, 0.05, 0.05, 1.0}, {0.0, 0.0, 0.0, 1.0}}};
	BStyles::Border menuBorder = {{BStyles::grey, 1.0}, 0.0, 0.0, 0.0};
	BStyles::Border labelborder = {BStyles::noLine, 4.0, 0.0, 0.0};
	BStyles::Fill menuBg = BStyles::Fill (BStyles::Color (0.05, 0.02, 0.01, 1.0));
	BStyles::Font ctLabelFont = BStyles::Font	("Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL, 12.0,
						   						 BStyles::Font::TextAlign::center, BStyles::Font::TextVAlign::middle);
	BStyles::Font lfLabelFont = BStyles::Font	("Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL, 12.0,
						   						 BStyles::Font::TextAlign::left, BStyles::Font::TextVAlign::middle);

	BStyles::Style style = BStyles::Style
	{
		{
			BUtilities::Urid::urid (BSILENT_URI "/dial"), 
			BUtilities::makeAny<BStyles::Style>
			(
				BStyles::Style
				{
					{
						BUtilities::Urid::urid (BSTYLES_STYLEPROPERTY_FGCOLORS_URI), 
						BUtilities::makeAny<BStyles::ColorMap>(fgColors)
					},
					{
						BUtilities::Urid::urid (BSTYLES_STYLEPROPERTY_BGCOLORS_URI), 
						BUtilities::makeAny<BStyles::ColorMap>(bgColors)
					},
					{
						BUtilities::Urid::urid (BSILENT_URI "/dial/label"), 
						BUtilities::makeAny<BStyles::Style>
						(
							BStyles::Style
							{
								{
									BUtilities::Urid::urid (BSTYLES_STYLEPROPERTY_FONT_URI), 
									BUtilities::makeAny<BStyles::Font>(ctLabelFont)
								},
								{
									BUtilities::Urid::urid (BSTYLES_STYLEPROPERTY_TXCOLORS_URI), 
									BUtilities::makeAny<BStyles::ColorMap>(txColors)
								}
							}
						)
					}
				}
			)
		},

		{
			BUtilities::Urid::urid (BSILENT_URI "/piano"), 
			BUtilities::makeAny<BStyles::Style>
			(
				BStyles::Style
				{
					{
						BUtilities::Urid::urid (BSTYLES_STYLEPROPERTY_FGCOLORS_URI), 
						BUtilities::makeAny<BStyles::ColorMap>(kbColors)
					}
				}
			)
		}
	};
};

BSilent_GUI::BSilent_GUI (const char *bundle_path, const LV2_Feature *const *features, PuglNativeView parentWindow) :
	BWidgets::Window (600, 320, parentWindow, BUTILITIES_URID_UNKNOWN_URID, "BSilent", true, PUGL_MODULE, 0),
	write_function (NULL),
	controller (NULL),
	pluginPath (bundle_path ? std::string (bundle_path) + ((strlen (bundle_path) > 0) && (bundle_path[strlen (bundle_path) - 1] != '/') ? "/" : "") : std::string ("")),
	presetMenu (100, 90, 140, 20, 0, 20, 140, 160, 
				{
					"Calmness", 
					"Quietness", 
					"Tranquility",
					"All muted",
					"Absolute silence", 
					"Cosmic silence",
				 	"Interstellar silence", 
					"Galactic silence"
				}, 
				1, 
				BUtilities::Urid::urid (BSILENT_URI "/menu")),
	gainDial	(110, 140, 60, 72, 0.0, -70.0, 30.0, 0.0, 
				 BWidgets::ValueTransferable<double>::noTransfer, BWidgets::ValueTransferable<double>::noTransfer, 
				 [] (const double& x) {return BUtilities::to_string (x, "%1.1f") + " db";}, BWidgets::ValueDial::stringToValue,
				 BUtilities::Urid::urid (BSILENT_URI "/dial")),
	tuneDial 	(400, 140, 60, 72, 0.0, -12.0, 12.0, 1.0, 
				 BWidgets::ValueTransferable<double>::noTransfer, BWidgets::ValueTransferable<double>::noTransfer, 
				 [] (const double& x) {return BUtilities::to_string (x, "%1.0f");}, BWidgets::ValueDial::stringToValue,
				 BUtilities::Urid::urid (BSILENT_URI "/dial")),
	pianoRoll (40, 280, 520, 40, 0, 127, std::vector<uint8_t>{}, BUtilities::Urid::urid (BSILENT_URI "/piano"))

{
	// Link controllerWidgets
	controllerWidgets[BSILENT_PRESET] = (BWidgets::Valueable*) &presetMenu;
	controllerWidgets[BSILENT_GAIN] = (BWidgets::Valueable*) &gainDial;
	controllerWidgets[BSILENT_TUNE] = (BWidgets::Valueable*) &tuneDial;

	// Set callbacks
	for (BWidgets::Valueable* c : controllerWidgets) c->setCallbackFunction (BEvents::Event::EventType::valueChangedEvent, valueChangedCallback);
	pianoRoll.setCallbackFunction (BEvents::Event::EventType::valueChangedEvent, pianoCallback);

	// Init params
	setStyle (style);
	setBackground (BStyles::Fill (pluginPath + BG_FILE));
	gainDial.setClickable (false);
	tuneDial.setClickable (false);
	pianoRoll.activate();
	velocities.fill (0);

	// Add widgets
	for (BWidgets::Valueable* c : controllerWidgets) 
	{
		if (dynamic_cast<BWidgets::Widget*> (c)) add (dynamic_cast<BWidgets::Widget*> (c));
	}
	add (&pianoRoll);

	//Scan host features for URID map
	LV2_URID_Map* map = NULL;
	for (int i = 0; features[i]; ++i)
	{
		if (strcmp(features[i]->URI, LV2_URID__map) == 0)
		{
			map = (LV2_URID_Map*) features[i]->data;
		}
	}
	if (!map) throw std::invalid_argument ("Host does not support urid:map");

	//Map URIS
	getURIs (map, &uris);

	// Initialize forge
	lv2_atom_forge_init (&forge, map);
	
}

void BSilent_GUI::portEvent (uint32_t port_index, uint32_t buffer_size, uint32_t format, const void* buffer)
{
	if (port_index == BSILENT_MIDI_IN)
	{
		fprintf (stderr, "MIDI_IN:\n");
	}

	if ((port_index >= BSILENT_CONTROLLERS) && (port_index < BSILENT_NR_CONTROLLERS) && (format == 0))
	{
		float* pval = (float*) buffer;
		if (port_index == BSILENT_CONTROLLERS + BSILENT_PRESET) presetMenu.setValue (*pval + 1);
		else 
		{
			BWidgets::ValueableTyped<double>* v = dynamic_cast<BWidgets::ValueableTyped<double>*> (controllerWidgets[port_index - BSILENT_CONTROLLERS]);
			if (v) v->setValue (*pval);
		}
	}
}

void BSilent_GUI::onConfigureRequest (BEvents::Event* event)
{
	Window::onConfigureRequest (event);

	BEvents::ExposeEvent* ee = dynamic_cast<BEvents::ExposeEvent*>(event);
	if (!ee) return;
	double sz = (ee->getArea().getWidth() / 600.0 > ee->getArea().getHeight() / 320.0 ? ee->getArea().getHeight() / 320.0 : ee->getArea().getWidth() / 600.0);
	setZoom (sz);
}

void BSilent_GUI::valueChangedCallback(BEvents::Event* event)
{
	if (!event) return;
	BWidgets::Widget* widget = event->getWidget ();
	if (!widget) return;
	BWidgets::Valueable* valueable = dynamic_cast<BWidgets::Valueable*> (widget);
	if (!valueable) return;
	BSilent_GUI* ui = (BSilent_GUI*) widget->getMainWindow();
	if (!ui) return;

	// Identify controller
	for (int i = 0; i < BSILENT_NR_CONTROLLERS; ++i)
	{
		if (valueable == ui->controllerWidgets[i])
		{
			// Sample ComboBox
			if (i == BSILENT_PRESET)
			{
				BWidgets::ValueableTyped<size_t>* vt = dynamic_cast<BWidgets::ValueableTyped<size_t>*> (valueable);
				if (vt) 
				{
					float value = std::min (static_cast<float>(vt->getValue() - 1), 9.0f);
					ui->write_function(ui->controller, BSILENT_CONTROLLERS + i, sizeof(float), 0, &value);
				}
			}

			// Dials
			else
			{
				BWidgets::ValueableTyped<double>* vt = dynamic_cast<BWidgets::ValueableTyped<double>*> (valueable);
				if (vt) 
				{
					float value = vt->getValue();
					ui->write_function(ui->controller, BSILENT_CONTROLLERS + i, sizeof(float), 0, &value);
				}
			}
			
			break;
		}
	}
}

void BSilent_GUI::pianoCallback (BEvents::Event* event)
{
	if (!event) return;
	BWidgets::HPianoRoll* widget = dynamic_cast<BWidgets::HPianoRoll*> (event->getWidget ());
	if (!widget) return;
	BSilent_GUI* ui = (BSilent_GUI*) widget->getMainWindow();
	if (!ui) return;

	for (uint8_t i = 0; i < 128; ++i)
	{
		const uint8_t vel = widget->getKey(i);
		if (vel != ui->velocities[i])
		{
			ui->velocities[i] = vel;
			uint8_t obj_buf[64];
			lv2_atom_forge_set_buffer(&ui->forge, obj_buf, sizeof(obj_buf));
			LV2_URID urid = (vel != 0 ? ui->uris.bsilent_noteOn : ui->uris.bsilent_noteOff);
			LV2_Atom_Forge_Frame frame;
			LV2_Atom* msg = (LV2_Atom*)lv2_atom_forge_object(&ui->forge, &frame, 0, ui->uris.bsilent_keyboardEvent);
			lv2_atom_forge_key (&ui->forge, urid);
			lv2_atom_forge_int (&ui->forge, i);
			lv2_atom_forge_key (&ui->forge, ui->uris.bsilent_velocity);
			lv2_atom_forge_int (&ui->forge, vel);
			lv2_atom_forge_pop (&ui->forge, &frame);
			ui->write_function (ui->controller, BSILENT_MIDI_IN, lv2_atom_total_size (msg), ui->uris.atom_eventTransfer, msg);
		}
	}
}

static LV2UI_Handle instantiate (const LV2UI_Descriptor *descriptor, const char *plugin_uri, const char *bundle_path,
						  LV2UI_Write_Function write_function, LV2UI_Controller controller, LV2UI_Widget *widget,
						  const LV2_Feature *const *features)
{
	PuglNativeView parentWindow = 0;

	if (strcmp(plugin_uri, BSILENT_URI) != 0)
	{
		std::cerr << "BSilent_GUI: This GUI does not support plugin with URI " << plugin_uri << std::endl;
		return NULL;
	}

	for (int i = 0; features[i]; ++i)
	{
		if (!strcmp(features[i]->URI, LV2_UI__parent)) parentWindow = (PuglNativeView) features[i]->data;
	}
	if (parentWindow == 0) std::cerr << "BSilent_GUI: No parent window.\n";

	BSilent_GUI* ui = new BSilent_GUI (bundle_path, features, parentWindow);

	if (ui)
	{
		ui->controller = controller;
		ui->write_function = write_function;

		PuglNativeView nativeWindow = puglGetNativeView (ui->getView ());
		*widget = (LV2UI_Widget) nativeWindow;
	}
	else std::cerr << "BSilent_GUI: Couldn't instantiate.\n";
	return (LV2UI_Handle) ui;
}

static void cleanup(LV2UI_Handle ui)
{
	BSilent_GUI* pluginGui = (BSilent_GUI*) ui;
	if (pluginGui) delete pluginGui;
}

static void portEvent(LV2UI_Handle ui, uint32_t port_index, uint32_t buffer_size, uint32_t format, const void* buffer)
{
	BSilent_GUI* pluginGui = (BSilent_GUI*) ui;
	if (pluginGui) pluginGui->portEvent(port_index, buffer_size, format, buffer);
}

static int callIdle(LV2UI_Handle ui)
{
	BSilent_GUI* pluginGui = (BSilent_GUI*) ui;
	if (pluginGui) pluginGui->handleEvents ();
	return 0;
}

static int callResize (LV2UI_Handle ui, int width, int height)
{
	BSilent_GUI* self = (BSilent_GUI*) ui;
	if (!self) return 0;
	BEvents::ExposeEvent* ev = new BEvents::ExposeEvent (self, self, BEvents::Event::EventType::configureRequestEvent, self->getPosition().x, self->getPosition().y, width, height);
	self->addEventToQueue (ev);
	return 0;
}

static const LV2UI_Idle_Interface idle = {callIdle};
static const LV2UI_Resize resize = {nullptr, callResize} ;

static const void* extensionData(const char* uri)
{
	if (!strcmp(uri, LV2_UI__idleInterface)) return &idle;
	else if(!strcmp(uri, LV2_UI__resize)) return &resize;
	else return NULL;
}


static const LV2UI_Descriptor guiDescriptor =
{
		BSILENT_GUI_URI,
		instantiate,
		cleanup,
		portEvent,
		extensionData
};

// LV2 Symbol Export
LV2_SYMBOL_EXPORT const LV2UI_Descriptor *lv2ui_descriptor(uint32_t index)
{
	switch (index)
	{
		case 0: return &guiDescriptor;
		default:return NULL;
	}
}
