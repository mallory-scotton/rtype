/** Dependencies */
import { generateFunctionNode } from '../utils';

/**
 * @brief List of vector related functions
 */
export const VectorFunctions = [
  generateFunctionNode({
    name: 'Make Vector',
    description: 'Creates a vector from three float values',
    icon: 'make-struct',
    inputs: [
      { type: 'float', label: 'X' },
      { type: 'float', label: 'Y' },
      { type: 'float', label: 'Z' }
    ],
    outputs: [{ type: 'vector', label: 'Return Value' }],
    tags: ['vector', 'math', 'create'],
    snippet: 'FVector {OUTPUT_1} = FVector({INPUT_1}, {INPUT_2}, {INPUT_3});',
    pure: true
  }),
  generateFunctionNode({
    name: 'Break Vector',
    description: 'Breaks a vector into its three float components',
    icon: 'break-struct',
    inputs: [{ type: 'vector', label: 'In Vec' }],
    outputs: [
      { type: 'float', label: 'X' },
      { type: 'float', label: 'Y' },
      { type: 'float', label: 'Z' }
    ],
    tags: ['vector', 'math', 'decompose'],
    snippet: 'Float32 {OUTPUT_1} = {INPUT_1}.x; Float32 {OUTPUT_2} = {INPUT_1}.y; Float32 {OUTPUT_3} = {INPUT_1}.z;',
    pure: true
  })
];
