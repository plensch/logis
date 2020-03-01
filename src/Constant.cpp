#include "plugin.hpp"


struct Constant : Module {
	enum ParamIds {
		ON_PARAM,
		VOLTAGE3_PARAM,
		VOLTAGE2_PARAM,
		VOLTAGE1_PARAM,
		VOLTAGE0_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		NUM_INPUTS
	};
	enum OutputIds {
		CONSTV3_OUTPUT,
		CONSTV2_OUTPUT,
		CONSTV1_OUTPUT,
		CONSTV0_OUTPUT,
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
		configParam(VOLTAGE0_PARAM, -10.f, 10.f, 0.f, "Voltage", "V");
		configParam(VOLTAGE1_PARAM, -10.f, 10.f, 0.f, "Voltage", "V");
		configParam(VOLTAGE2_PARAM, -10.f, 10.f, 0.f, "Voltage", "V");
		configParam(VOLTAGE3_PARAM, -10.f, 10.f, 0.f, "Voltage", "V");
	}

	void process(const ProcessArgs& args) override {
        // global on off
        if (onTrigger.process(params[ON_PARAM].getValue() > 0.f)) {
            on = !on;
        }

        // set LED
        lights[ON_LIGHT].setBrightness(on ? 0.9f : 0.f);

        outputs[CONSTV0_OUTPUT].setVoltage(on ? params[VOLTAGE0_PARAM].getValue() : 0.f);
        outputs[CONSTV1_OUTPUT].setVoltage(on ? params[VOLTAGE1_PARAM].getValue() : 0.f);
        outputs[CONSTV2_OUTPUT].setVoltage(on ? params[VOLTAGE2_PARAM].getValue() : 0.f);
        outputs[CONSTV3_OUTPUT].setVoltage(on ? params[VOLTAGE3_PARAM].getValue() : 0.f);
	}
};

struct ConstantWidget : ModuleWidget {
	ConstantWidget(Constant* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Constant.svg")));
        
        // top screw
		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
        // bottom screw
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, box.size - 15)));

		addParam(createParamCentered<LEDBezel>(mm2px(Vec(7.62, 12.037)), module, Constant::ON_PARAM));
        addChild(createLightCentered<LEDBezelLight<GreenLight>>(mm2px(Vec(7.62, 12.037)), module, Constant::ON_LIGHT));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(7.62, 23.223)), module, Constant::VOLTAGE3_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(7.62, 48.094)), module, Constant::VOLTAGE2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(7.62, 72.965)), module, Constant::VOLTAGE1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(7.62, 97.835)), module, Constant::VOLTAGE0_PARAM));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.62, 35.658)), module, Constant::CONSTV3_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.62, 60.529)), module, Constant::CONSTV2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.62, 85.4)), module, Constant::CONSTV1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.62, 110.271)), module, Constant::CONSTV0_OUTPUT));
	}
};

Model* modelConstant = createModel<Constant, ConstantWidget>("Constant");
