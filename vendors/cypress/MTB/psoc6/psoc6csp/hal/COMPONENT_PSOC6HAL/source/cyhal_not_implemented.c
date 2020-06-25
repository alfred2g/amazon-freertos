/***************************************************************************//**
* \file cyhal_not_implemented.c
*
* \brief
* This file contains place holder functions for drivers that are not yet
* fully implemented for the target device. Prototype or stub implmentations
* are made available for to prevevent build failures on some toolchains or
* to allow for initial testing. As the drivers are implemented, these items
* will be deleted or moved into the appropriate driver.
*
********************************************************************************
* \copyright
* Copyright 2018-2020 Cypress Semiconductor Corporation
* SPDX-License-Identifier: Apache-2.0
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

#include "cyhal_interconnect.h"
#include "cyhal_triggers.h"
#include "cy_utils.h"

#if defined(__cplusplus)
extern "C"
{
#endif


/* Stub definitions for trigger connection items that will be autogenerated. */

/* Trigger type for each input                                    ~400b */
typedef enum
{
    SRC0,
    SRC1,
    SRC2,
    MUX_OUT0,
} cyhal_trigger_src;

/* Trigger type for each output                                    ~100b */
/* Note: Non-fake version implemented in cyhal_triggers */
typedef enum
{
    DEST0,
    DEST1,
    MUX_IN1,
} cyhal_trigger_dest_fake;

// Two fake muxes.
// Mux0
//  Inputs: SRC0, SRC1
//  Outputs: DEST0, MUX_IN1
// Mux1:
//  Inputs: SRC2, MUX_OUT0
//  Outputs: DEST1

/* Maps each cyhal_destination_t to a mux index                    ~100b */
/* Note: Non-fake version implemented in cyhal_triggers */
const uint8_t cyhal_dest_to_mux_fake[] =
{
    0, // DEST0
    1, // DEST1
    0, // MUX_IN1
};

/* Maps each cyhal_destination_t to a specific output in its mux    ~100b */
/* Note: Non-fake version implemented in cyhal_triggers */
const uint8_t cyhal_mux_dest_index_fake[] =
{
    0, // DEST0
    0, // DEST1
    1, // MUX_IN1
};

/* Number of sources for each mux                                ~30b */
const uint8_t cyhal_source_count_per_mux[] = {2, 2};
static const cyhal_source_t cyhal_mux0_sources[] = { SRC0, SRC1, }; // MUX0
static const cyhal_source_t cyhal_mux1_sources[] = { SRC2, MUX_OUT0, }; // MUX1
/* Mux index to a table of cyhal_source_t                             ~2400b (2b * 15muxes * 80sources_per_mux) (could be 1/2 the size if there are less than 255 sources) */
const cyhal_source_t* cyhal_mux_to_sources[] = { cyhal_mux0_sources, cyhal_mux1_sources };

/* Mapping from cyhal_source_t to cyhal_destination_t for intra mux connections     ~80b*/
const cyhal_dest_t cyhal_intra_trigger_source[] =
{
    (cyhal_dest_t)CYHAL_INTERCONNECT_MUX_NOT_CONTINUATION, // SRC0
    (cyhal_dest_t)CYHAL_INTERCONNECT_MUX_NOT_CONTINUATION, // SRC1,
    (cyhal_dest_t)CYHAL_INTERCONNECT_MUX_NOT_CONTINUATION, // SRC2,
    (cyhal_dest_t)MUX_IN1 // MUX_OUT0
};



/* Protototype for interconnect driver's trigger connection API. */

/** Trigger type */
typedef enum
{
    CY_TRIGGER_LEVEL, //!< Level triggered
    CY_TRIGGER_EDGE,  //!< Edge triggered
    CY_TRIGGER_EITHER,//!< Level or edge triggered
} cyhal_trigger_type_t;

/** Number of muxes */
#define MUX_COUNT
/** Mux identiifer the one-to-one triggers */
#define ONE_TO_ONE_IDENT 0x80
/** Determines whether a mux is one-to-one */
#define IS_1TO1(muxId) (ONE_TO_ONE_IDENT == (muxId & ONE_TO_ONE_IDENT))
#define WRITE_REGISTER(muxIdx, sourceId, destId)

/* Maps each cyhal_destination_t to a mux index */
//extern uint8_t cyhal_dest_to_mux[];
/* Maps each cyhal_destination_t to a specific output in its mux */
//extern uint8_t cyhal_mux_dest_index[];
/* Number of sources for each mux */
//extern uint8_t cyhal_source_count_per_mux[];
/* Mux index to a table of cyhal_source_t */
//extern cyhal_source_t* cyhal_mux_to_sources[];
/* Mapping from cyhal_source_t to cyhal_destination_t for intra mux connections */
//extern cyhal_dest_t cyhal_intra_trigger_source[];

bool cyhal_has_connection(uint8_t mux, uint8_t outputIdx)
{
    CY_UNUSED_PARAMETER(mux);
    CY_UNUSED_PARAMETER(outputIdx);
    return false;
}

/*
 * Connects two digital terminals. The source will be routed to the destination via the trigger routing
 * framework in the device. This supports both the trigger mux routing and the 1:1 triggers. As expected,
 * a single source can drive multiple destinations, but a destination can only be driven by a single source.
 * If the output is already connected, or the connection can not be established an error will be returned.
 */
cy_rslt_t cyhal_connect_trigger(cyhal_source_t source, cyhal_dest_t dest)
{
    uint8_t muxIdx = cyhal_dest_to_mux_fake[dest];
    uint8_t destId = dest - cyhal_mux_dest_index_fake[dest];
    uint8_t sourceCount = cyhal_source_count_per_mux[muxIdx];

    if (cyhal_has_connection(muxIdx, destId))
    {
        return CYHAL_CONNECT_RSLT_ALREADY_CONNECTED;
    }

    for (uint8_t sourceId = 0; sourceId < sourceCount; sourceId++)
    {
        cyhal_source_t foundSource = cyhal_mux_to_sources[muxIdx][sourceId];
        if (foundSource == source)
        {
            if (IS_1TO1(muxIdx) && sourceId != destId)
            {
                return CYHAL_CONNECT_RSLT_INVALID_TRIGGER_CONNECTION;
            }

            WRITE_REGISTER (muxIdx, sourceId, destId);
            return CY_RSLT_SUCCESS;
        }
        else
        {
            cyhal_dest_t intraDest = cyhal_intra_trigger_source[foundSource];
            if (CYHAL_INTERCONNECT_MUX_NOT_CONTINUATION != intraDest)
            {
                // This destination can be driven by the output of another mux.
                uint8_t upstreamMuxIdx = cyhal_dest_to_mux_fake[intraDest];
                uint8_t intraDestId = intraDest - cyhal_mux_dest_index_fake[intraDest];
                uint8_t upstreamMuxSourceCount = cyhal_source_count_per_mux[upstreamMuxIdx];
                cy_rslt_t result = CYHAL_CONNECT_RSLT_NO_CONNECTION;

                if (cyhal_has_connection(upstreamMuxIdx, intraDestId))
                {
                    result = CYHAL_CONNECT_RSLT_ALREADY_CONNECTED;
                }

                for (uint8_t upstreamSourceId = 0; upstreamSourceId < upstreamMuxSourceCount; upstreamSourceId++)
                {
                    cyhal_source_t upstreamSource = cyhal_mux_to_sources[upstreamMuxIdx][intraDestId];
                    if (upstreamSource == source)
                    {
                        if (IS_1TO1(upstreamMuxIdx) && sourceId != intraDestId)
                        {
                            result = CYHAL_CONNECT_RSLT_INVALID_TRIGGER_CONNECTION;
                        }

                        WRITE_REGISTER (upstreamMuxIdx, sourceId, intraDestId);
                        result = CY_RSLT_SUCCESS;
                        break;
                    }
                }

                if (result == CY_RSLT_SUCCESS)
                {
                    WRITE_REGISTER (muxIdx, sourceId, destId);
                    return result;
                }
            }
        }
    }
    return CYHAL_CONNECT_RSLT_NO_CONNECTION;
}

#if defined(__cplusplus)
}
#endif