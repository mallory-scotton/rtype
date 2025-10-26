/** Dependencies */
import type { NodeTemplate } from '../utils';
import type { PropertyEntry } from '../types';

/**
 * @brief Generate a node template for setting a property
 * @param entry Property entry to generate the setter for
 * @returns Node template for the property setter
 */
export function generatePropertySetter(entry: PropertyEntry): NodeTemplate {
  return {
    id: `set_${entry.name}_${entry.type}`,
    name: `Set ${entry.name}`,
    category: 'Variables',
    description: `Sets the value of the property "${entry.name}"`,
    data: {
      id: '',
      type: 'setter',
      roundedBg: 'SET',
      roundedBgColor: true,
      inputs: [
        { id: '', type: 'exec' },
        { id: '', type: entry.type, label: entry.name }
      ],
      outputs: [
        { id: '', type: 'exec' },
        { id: '', type: entry.type }
      ]
    },
    snippet: `{INPUT_2} = {OUTPUT_2};\n{OUTPUT_1}`,
    tags: ['property', 'setter', entry.type]
  };
}

/**
 * @brief Generate a node template for getting a property
 * @param entry Property entry to generate the getter for
 * @returns Node template for the property getter
 */
export function generatePropertyGetter(entry: PropertyEntry): NodeTemplate {
  return {
    id: `get_${entry.name}_${entry.type}`,
    name: `Get ${entry.name}`,
    category: 'Variables',
    description: `Gets the value of the property "${entry.name}"`,
    data: {
      id: '',
      type: 'getter',
      roundedBg: true,
      inputs: [],
      outputs: [{ id: '', type: entry.type, label: entry.name }]
    },
    snippet: `${entry.type} {OUTPUT_1} = UProperty_${entry.name};`,
    tags: ['property', 'getter', entry.type]
  };
}
