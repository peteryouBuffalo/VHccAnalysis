#!/bin/bash

#https://github.com/andrzejnovak/combine_postfits

export PATH="$HOME/.pyenv/bin:$PATH"
eval "$(pyenv init --path)"
eval "$(pyenv init -)"
eval "$(pyenv virtualenv-init -)"

cd DoFit_NLO_VV_breakVV_Peter_2025FebJan/tfMC_00_tfRes_00/All

combine_postfits -i fitDiagnosticsTest.root -o ./ --MC
