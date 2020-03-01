#include "plugin.hpp"


struct Trigger : Module {
	enum ParamIds {
		LATCH_PARAM,
        ENUMS(TRIGGER_PARAMS, 4),
		NUM_PARAMS
	};
	enum InputIds {
		NUM_INPUTS
	};
	enum OutputIds {
        ENUMS(TRIGGER_OUTPUTS, 4),
		NUM_OUTPUTS
	};
	enum LightIds {
        LATCH_LIGHT,
        ENUMS(TRIGGER_LIGHTS, 4),
		NUM_LIGHTS
	};

    bool latch = false;
    bool constPulse[4] = {false,false,false,false};

    dsp::BooleanTrigger tapTriggers[4];
    dsp::BooleanTrigger latchTrigger;
	dsp::PulseGenerator pulseGenerators[4];

	Trigger() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(LATCH_PARAM, 0.f, 1.f, 0.f, "Latch");
        for (int i = 0; i < 4; i++) {
            configParam(TRIGGER_PARAMS + i, 0.f, 1.f, 0.f, string::f("Trigger %d",i));
        }
	}

	void process(const ProcessArgs& args) override {
        for (int i = 0; i < 4; i++) {
            // set latch on/off
            if (latchTrigger.process(params[LATCH_PARAM].getValue() > 0.f)) {
                latch = !latch;
            }
            // set latch LED
            lights[LATCH_LIGHT].setBrightness(latch);

            // triggers
            bool tap = params[TRIGGER_PARAMS + i].getValue() > 0.f;
            if(tapTriggers[i].process(tap)) {
                if(latch) {
                    constPulse[i] = !constPulse[i];
                } else {
                    pulseGenerators[i].trigger(1e-3f);
                }
            }

            bool pulse = false;
            if (latch) {
                pulse = constPulse[i];
                lights[TRIGGER_LIGHTS + i].setBrightness(pulse);
            } else {
                pulse = pulseGenerators[i].process(args.sampleTime);
                lights[TRIGGER_LIGHTS + i].setBrightness(tap);
            }
            outputs[TRIGGER_OUTPUTS + i].setVoltage(pulse ? 10.f : 0.f);
        }
	}
};


struct TriggerWidget : ModuleWidget {
	TriggerWidget(Trigger* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Trigger.svg")));

        // top screw
		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
        // bottom screw
		addChild(createWidget<ScrewSilver>(Vec(30, 365)));

		addParam(createParamCentered<LEDBezel>(mm2px(Vec(7.62, 12.037)), module, Trigger::LATCH_PARAM));
		addParam(createParamCentered<LEDBezel>(mm2px(Vec(7.62, 23.223)), module, Trigger::TRIGGER_PARAMS + 0));
		addParam(createParamCentered<LEDBezel>(mm2px(Vec(7.62, 48.094)), module, Trigger::TRIGGER_PARAMS + 1));
		addParam(createParamCentered<LEDBezel>(mm2px(Vec(7.62, 72.965)), module, Trigger::TRIGGER_PARAMS + 2));
		addParam(createParamCentered<LEDBezel>(mm2px(Vec(7.62, 97.835)), module, Trigger::TRIGGER_PARAMS + 3));

        addChild(createLightCentered<LEDBezelLight<GreenLight>>(mm2px(Vec(7.62, 12.037)), module, Trigger::LATCH_LIGHT));
        addChild(createLightCentered<LEDBezelLight<GreenLight>>(mm2px(Vec(7.62, 23.223)), module, Trigger::TRIGGER_LIGHTS + 0));
        addChild(createLightCentered<LEDBezelLight<GreenLight>>(mm2px(Vec(7.62, 48.094)), module, Trigger::TRIGGER_LIGHTS + 1));
        addChild(createLightCentered<LEDBezelLight<GreenLight>>(mm2px(Vec(7.62, 72.965)), module, Trigger::TRIGGER_LIGHTS + 2));
        addChild(createLightCentered<LEDBezelLight<GreenLight>>(mm2px(Vec(7.62, 97.835)), module, Trigger::TRIGGER_LIGHTS + 3));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.62, 35.658 )), module, Trigger::TRIGGER_OUTPUTS + 0));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.62, 60.529 )), module, Trigger::TRIGGER_OUTPUTS + 1));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.62, 85.4   )), module, Trigger::TRIGGER_OUTPUTS + 2));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.62, 110.271)), module, Trigger::TRIGGER_OUTPUTS + 3));
	}
};

Model* modelTrigger = createModel<Trigger, TriggerWidget>("Trigger");
