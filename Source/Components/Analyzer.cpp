#include "Analyzer.h"

Analyzer::Analyzer(AudioParameterFloat* out_parameter, AudioProcessorValueTreeState* in_state, util::parameter_type type) : out_parameter(out_parameter),in_parameters_state(in_state), type(type)
{
}

float Analyzer::get_last_value_parameter() const
{
}

