/*
  Copyright 2021 reuk <reuk@users.noreply.github.com>
  Copyright 2006-2016 David Robillard <d@drobilla.net>
  Copyright 2006 Steve Harris <steve@plugin.org.uk>

  Permission to use, copy, modify, and/or distribute this software for any
  purpose with or without fee is hereby granted, provided that the above
  copyright notice and this permission notice appear in all copies.

  THIS SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#include "lv2/core/lv2.h"

#include <cmath>
#include <cstdint>
#include <cstdlib>

typedef enum
{
    AMP_GAIN = 0,
    AMP_INPUT = 1,
    AMP_OUTPUT = 2
} PortIndex;

typedef struct
{
    const float* gain;
    const float* input;
    float* output;
} Amp;

#define DB_CO(g) ((g) > -90.0f ? powf (10.0f, (g) *0.05f) : 0.0f)

static const LV2_Feature* findMatchingFeature (const LV2_Feature* const* features, const char* uri)
{
    for (auto feature = features; *feature != nullptr; ++feature)
        if (std::strcmp ((*feature)->URI, uri) == 0)
            return *feature;

    return nullptr;
}

LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor (uint32_t index)
{
    static constexpr LV2_Descriptor descriptor {
        "https://reuk.github.io/plugins/bounded",
        [] (const LV2_Descriptor* descriptor,
            double rate,
            const char* bundle_path,
            const LV2_Feature* const* features) {
            contexpr auto blockLength = "http://lv2plug.in/ns/ext/buf-size#boundedBlockLength";

            if (findMatchingFeature (features, blockLength) == nullptr)
                return nullptr;

            Amp* amp = (Amp*) calloc (1, sizeof (Amp));
            return (LV2_Handle) amp;
        },
        [] (LV2_Handle instance, uint32_t port, void* data) {
            Amp* amp = (Amp*) instance;

            switch ((PortIndex) port)
            {
                case AMP_GAIN:
                    amp->gain = (const float*) data;
                    break;
                case AMP_INPUT:
                    amp->input = (const float*) data;
                    break;
                case AMP_OUTPUT:
                    amp->output = (float*) data;
                    break;
            }
        },
        [] (LV2_Handle) {},
        [] (LV2_Handle instance, uint32_t n_samples) {
            const Amp* amp = (const Amp*) instance;

            const float gain = *(amp->gain);
            const float* const input = amp->input;
            float* const output = amp->output;

            const float coef = DB_CO (gain);

            for (uint32_t pos = 0; pos < n_samples; pos++)
            {
                output[pos] = input[pos] * coef;
            }
        },
        [] (LV2_Handle) {},
        [] (LV2_Handle instance) { free (instance); },
        [] (const char*) { return nullptr; },
    };

    return index == 0 ? &descriptor : nullptr;
}
