/** Dependencies */
import type { NodeTemplate } from '../../utils';
import { generateFunctionNode } from './utils';

/**
 * @brief Predefined network function nodes.
 * @description This array contains predefined function nodes related to networking operations.
 */
export const NetworkFunctions: NodeTemplate[] = [
  generateFunctionNode({
    name: 'Is Locally Controlled',
    description: 'Checks if the actor is locally controlled',
    category: 'Network',
    icon: 'pure-function-call',
    inputs: ['object'],
    outputs: [{ type: 'bool', label: 'Return Value' }],
    tags: ['network', 'actor', 'control'],
    snippet: 'Bool {OUTPUT_1} = {INPUT_1}->IsLocallyControlled();',
    pure: true
  }),
  generateFunctionNode({
    name: 'Is Authority',
    description: 'Checks if the actor has authority',
    category: 'Network',
    icon: 'pure-function-call',
    inputs: ['object'],
    outputs: [{ type: 'bool', label: 'Return Value' }],
    tags: ['network', 'actor', 'control'],
    snippet: 'Bool {OUTPUT_1} = {INPUT_1}->IsAuthority();',
    pure: true
  }),
  generateFunctionNode({
    name: 'Is Simulated',
    description: 'Checks if the actor is simulated',
    category: 'Network',
    icon: 'pure-function-call',
    inputs: ['object'],
    outputs: [{ type: 'bool', label: 'Return Value' }],
    tags: ['network', 'actor', 'control'],
    snippet: 'Bool {OUTPUT_1} = {INPUT_1}->IsSimulated();',
    pure: true
  }),
  generateFunctionNode({
    name: 'Switch Net Role',
    description: 'Switches based on the network role of the actor',
    category: 'Network',
    icon: 'select',
    inputs: ['exec', 'object'],
    outputs: [
      { type: 'exec', label: 'None' },
      { type: 'exec', label: 'Authority' },
      { type: 'exec', label: 'Autonomous Proxy' },
      { type: 'exec', label: 'Simulated Proxy' }
    ],
    tags: ['network', 'actor', 'control'],
    snippet:
      'switch({INPUT_2}->GetNetRole())\n{\ncase ENetRole::None:\n{{OUTPUT_1}\nbreak;}\ncase ENetRole::Authority:\n{{OUTPUT_2}\nbreak;}\ncase ENetRole::AutonomousProxy:\n{{OUTPUT_3}\nbreak;}\ncase ENetRole::SimulatedProxy:\n{{OUTPUT_4}\nbreak;}\ndefault:\nbreak;\n}',
    pure: false
  })
];
