/** Dependencies */
import type { NodeTemplate } from '../../utils';
import type { NodeIconType, PinType } from '../../types';

/**
 * @brief Properties for generating a function node.
 * @param name The name of the function.
 * @param category The category of the function.
 * @param description A brief description of the function.
 * @param tags Tags associated with the function.
 * @param icon The icon type for the function node.
 * @param inputs An array of input pin types or objects defining input pins.
 * @param outputs An array of output pin types or objects defining output pins.
 */
interface FunctionProps {
  name: string;
  category?: string;
  description?: string;
  tags?: string[];
  icon?: NodeIconType;
  inputs: (PinType | { type: PinType; label?: string })[];
  outputs: (PinType | { type: PinType; label?: string })[];
  pure?: boolean;
  snippet: string;
}

/**
 * @brief Generates a function node template.
 * @param props The properties for the function node.
 * @returns A NodeTemplate representing the function node.
 */
export function generateFunctionNode(props: FunctionProps): NodeTemplate {
  const { name, category = 'Functions', description, tags, inputs, outputs, pure = false, snippet, icon } = props;

  return {
    id: name.replace(/\s+/g, '_').toLowerCase(),
    name: name,
    category: category,
    description: description,
    icon: icon ? icon : pure ? 'pure-function-call' : 'function-call',
    tags: tags,
    data: {
      id: '',
      type: 'function',
      header: {
        label: name,
        icon: icon ? icon : pure ? 'pure-function-call' : 'function-call',
        type: pure ? 'pure-function-call' : 'function-call'
      },
      inputs: inputs.map((input, index) => {
        if (typeof input === 'string') {
          return { id: '', type: input };
        } else {
          return { id: '', type: input.type, label: input.label || `Input ${index + 1}` };
        }
      }),
      outputs: outputs.map((output, index) => {
        if (typeof output === 'string') {
          return { id: '', type: output };
        } else {
          return { id: '', type: output.type, label: output.label || `Output ${index + 1}` };
        }
      })
    },
    snippet: snippet
  };
}
