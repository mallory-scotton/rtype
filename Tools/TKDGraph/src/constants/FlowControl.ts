/** Dependencies */
import type { NodeTemplate } from '../utils';
import type { PinType } from '../types';

/**
 * @brief List of flow control node templates.
 */
export const FlowControl: NodeTemplate[] = [
  {
    id: 'flow_branch',
    name: 'Branch',
    category: 'Flow Control',
    description: 'Executes one of the two output execution pins based on the boolean input.',
    tags: ['flow', 'branch', 'if', 'condition'],
    data: {
      id: '',
      type: 'function',
      header: {
        label: 'Branch',
        type: 'macro',
        icon: 'exec-branch'
      },
      inputs: [
        { id: '', type: 'exec' },
        { id: '', type: 'bool', label: 'Condition' }
      ],
      outputs: [
        { id: '', type: 'exec', label: 'True' },
        { id: '', type: 'exec', label: 'False' }
      ],
      label: 'Branch'
    },
    snippet: `if ({INPUT_2})\n{\n{OUTPUT_1}\n}\nelse\n{\n{OUTPUT_2}\n}`
  },
  {
    id: 'flow_sequence',
    name: 'Sequence',
    category: 'Flow Control',
    description: 'Executes output execution pins in order.',
    tags: ['flow', 'sequence', 'exec'],
    data: {
      id: '',
      type: 'function',
      header: {
        label: 'Sequence',
        type: 'macro',
        icon: 'sequence'
      },
      inputs: [{ id: '', type: 'exec' }],
      outputs: [
        { id: '', type: 'exec', label: 'Then 1' },
        { id: '', type: 'exec', label: 'Then 2' },
        { id: '', type: 'exec', label: 'Then 3' }
      ],
      label: 'Sequence'
    },
    snippet: `{\n{OUTPUT_1}\n}\n{\n{OUTPUT_2}\n}\n{\n{OUTPUT_3}\n}`
  },
  {
    id: 'flow_do_n',
    name: 'Do N',
    category: 'Flow Control',
    description: 'Executes the output execution pin N times based on the integer input.',
    tags: ['flow', 'do', 'loop', 'exec'],
    data: {
      id: '',
      type: 'function',
      header: {
        label: 'Do N',
        type: 'macro',
        icon: 'do_n'
      },
      inputs: [
        { id: '', type: 'exec' },
        { id: '', type: 'int', label: 'N' }
      ],
      outputs: [
        { id: '', type: 'exec', label: 'Loop Body' },
        { id: '', type: 'exec', label: 'Completed' }
      ],
      label: 'Do N'
    },
    snippet: `for (int i = 0; i < {INTPUT_2}; i++)\n{\n{OUTPUT_1}\n}\n{OUTPUT_2}`
  },
  {
    id: 'flow_is_valid',
    name: 'Is Valid',
    category: 'Flow Control',
    description: 'Checks if the input object is valid (not null).',
    tags: ['flow', 'is valid', 'null check'],
    data: {
      id: '',
      type: 'function',
      header: {
        label: 'Is Valid',
        type: 'macro',
        icon: 'isvalid'
      },
      inputs: [
        { id: '', type: 'exec', label: 'Exec' },
        { id: '', type: 'object', label: 'Input Object' }
      ],
      outputs: [
        { id: '', type: 'exec', label: 'Is Valid' },
        { id: '', type: 'exec', label: 'Is Not Valid' }
      ],
      label: 'Is Valid'
    },
    snippet: `if ({INPUT_2} != nullptr)\n{\n{OUTPUT_1}\n}\nelse\n{\n{OUTPUT_2}\n}`
  },
  ...[
    'byte',
    'int',
    'float',
    'string',
    'bool',
    'exec',
    'vector',
    'rotator',
    'struct',
    'object',
    'transform',
    'name',
    'text',
    'real'
  ].map((type) => {
    return {
      id: `flow_knot_${type}`,
      name: `Knot (${type})`,
      category: 'Flow Control',
      description: `A simple knot node for ${type} type.`,
      tags: ['flow', 'knot', type],
      data: {
        id: '',
        type: 'knot',
        inputs: [{ id: '', type: type as PinType, hideInput: true }],
        outputs: [{ id: '', type: type as PinType, hideInput: true }]
      },
      public: false,
      snippet: `{OUTPUT_1}`
    } as NodeTemplate;
  })
];
