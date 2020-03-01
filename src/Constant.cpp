#include "plugin.hpp"


struct Constant : Module {
	enum ParamIds {
		ON_PARAM,
        ENUMS(VOLTAGE_PARAMS, 4),
		NUM_PARAMS
	};
	enum InputIds {
		NUM_INPUTS
	};
	enum OutputIds {
		ENUMS(CONSTV_OUTPUTS, 4),
		NUM_OUTPUTS
	};
	enum LightIds {
        ON_LIGHT,
		NUM_LIGHTS
	};

    bool on = true;
    dsp::BooleanTrigger onTrigger;

	Constant() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(ON_PARAM, 0.f, 1.f, 0.f, "Global ON/OFF");
        for (int i = 0; i < 4; i++) {
            configParam(VOLTAGE_PARAMS + i, -10.f, 10.f, 0.f, "Voltage", "V");
        }
	}

	void process(const ProcessArgs& args) override {
        // global on off
        if (onTrigger.process(params[ON_PARAM].getValue() > 0.f)) {
            on = !on;
        }
        // set LED
        lights[ON_LIGHT].setBrightness(on);

        // set outputs
        for (int i = 0; i < 4; i++) {
            outputs[CONSTV_OUTPUTS + i].setVoltage(on ? params[VOLTAGE_PARAMS + i].getValue() : 0.f);
        }
	}
};

struct ConstantWidget : ModuleWidget {
	ConstantWidget(Constant* module) {
	setModule(module);
	setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Constant.svg")));
        
        // top screw
	addChild(createWidget<ScrewSilver>(Vec(0, 0)));
        // bottom screw
	addChild(createWidget<ScrewSilver>(Vec(30,365)));

	addParam(createParamCentered<LEDBezel>(mm2px(Vec(7.62, 12.037)), module, Constant::ON_PARAM));
    addChild(createLightCentered<LEDBezelLight<GreenLight>>(mm2px(Vec(7.62, 12.037)), module, Constant::ON_LIGHT));

	addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(7.62, 23.223)), module, Constant::VOLTAGE_PARAMS + 0));
	addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(7.62, 48.094)), module, Constant::VOLTAGE_PARAMS + 1));
	addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(7.62, 72.965)), module, Constant::VOLTAGE_PARAMS + 2));
	addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(7.62, 97.835)), module, Constant::VOLTAGE_PARAMS + 3));

	addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.62, 35.658 )), module, Constant::CONSTV_OUTPUTS + 0));
	addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.62, 60.529 )), module, Constant::CONSTV_OUTPUTS + 1));
	addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.62, 85.4   )), module, Constant::CONSTV_OUTPUTS + 2));
	addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.62, 110.271)), module, Constant::CONSTV_OUTPUTS + 3));
	}
};

Model* modelConstant = createModel<Constant, ConstantWidget>("Constant");
