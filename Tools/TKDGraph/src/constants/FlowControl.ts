/** Dependencies */
import type { NodeTemplate } from '../utils';

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
    snippet: `if ({CONDITION})\n{INDENT}{\n{INDENT+1}{TRUE}\n{INDENT}}\n{INDENT}else\n{INDENT}{\n{INDENT+1}{FALSE}\n{INDENT}}`
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
    snippet: `{INPUT}\n{INDENT}{THEN_1}\n{INDENT}{THEN_2}\n{INDENT}{THEN_3}`
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
    snippet: `for (int i = 0; i < {N}; i++)\n{INDENT}{\n{INDENT+1}{LOOP_BODY}\n{INDENT}}`
  }
];
