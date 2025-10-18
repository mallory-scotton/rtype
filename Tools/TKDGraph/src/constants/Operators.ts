/** Dependencies */
import type { PinType } from '../types';
import type { NodeTemplate } from '../utils';
import { convertPinTypeToEngineType, convertOperatorSymbolToSnippetOperator } from '../utils';

/**
 * @brief Generates an operator node template.
 * @param inputs - The input pin types.
 * @param output - The output pin types.
 * @param operator - The operator symbol.
 * @returns The generated node template.
 */
function generateOperator(inputs: PinType[], output: PinType, operator: string): NodeTemplate {
  if (inputs.length === 0 || !output) {
    throw new Error('Operator must have at least one input and one output.');
  }

  if (operator.trim() === '') {
    throw new Error('Operator must be a non-empty string.');
  }

  const tags = ['operator', operator];

  if (
    inputs[0] === 'float' ||
    inputs[0] === 'int' ||
    inputs[0] === 'real' ||
    inputs[0] === 'byte' ||
    inputs[0] === 'vector' ||
    inputs[0] === 'rotator' ||
    inputs[0] === 'transform'
  ) {
    tags.push('math', 'arithmetic');
  }

  if (inputs[0] === 'bool' || output[0] === 'bool') {
    tags.push('logic', 'comparison');
  }

  if (inputs[0] === 'string' || inputs[0] === 'text' || inputs[0] === 'name') {
    tags.push('text', 'string', 'concatenation');
  }

  let snippetOperator = convertOperatorSymbolToSnippetOperator(operator);

  return {
    id: `operator_${operator}-${inputs.join('_')}-${output}`,
    name: `${inputs.join(` ${operator} `)} -> ${output}`,
    category: 'Operators',
    data: {
      id: '',
      roundedBg: true,
      inputs: inputs.map((type) => ({
        id: '',
        type
      })),
      outputs: [
        {
          id: '',
          type: output
        }
      ],
      label: operator,
      type: 'operator'
    },
    tags: tags,
    snippet: `${convertPinTypeToEngineType(output)} {RESULT} = ${
      inputs.length > 1
        ? inputs.map((_, index) => `{${'ABCDEFGHIJKLMNOPQRSTUVWXYZ'[index]}}`).join(` ${snippetOperator} `)
        : `${snippetOperator}{A}`
    };`
  };
}

/**
 * @brief List of operator node templates.
 */
export const Operators: NodeTemplate[] = [
  generateOperator(['float', 'float'], 'float', '+'),
  generateOperator(['float', 'float'], 'float', '-'),
  generateOperator(['float', 'float'], 'float', '×'),
  generateOperator(['float', 'float'], 'float', '÷'),
  generateOperator(['float', 'float'], 'bool', '<'),
  generateOperator(['float', 'float'], 'bool', '>'),
  generateOperator(['float', 'float'], 'bool', '<='),
  generateOperator(['float', 'float'], 'bool', '>='),
  generateOperator(['float', 'float'], 'bool', '=='),
  generateOperator(['float', 'float'], 'bool', '!='),
  generateOperator(['int', 'int'], 'int', '+'),
  generateOperator(['int', 'int'], 'int', '-'),
  generateOperator(['int', 'int'], 'int', '×'),
  generateOperator(['int', 'int'], 'int', '÷'),
  generateOperator(['int', 'int'], 'bool', '<'),
  generateOperator(['int', 'int'], 'bool', '>'),
  generateOperator(['int', 'int'], 'bool', '<='),
  generateOperator(['int', 'int'], 'bool', '>='),
  generateOperator(['int', 'int'], 'bool', '=='),
  generateOperator(['int', 'int'], 'bool', '!='),
  generateOperator(['real', 'real'], 'real', '+'),
  generateOperator(['real', 'real'], 'real', '-'),
  generateOperator(['real', 'real'], 'real', '×'),
  generateOperator(['real', 'real'], 'real', '÷'),
  generateOperator(['real', 'real'], 'bool', '<'),
  generateOperator(['real', 'real'], 'bool', '>'),
  generateOperator(['real', 'real'], 'bool', '<='),
  generateOperator(['real', 'real'], 'bool', '>='),
  generateOperator(['real', 'real'], 'bool', '=='),
  generateOperator(['real', 'real'], 'bool', '!='),
  generateOperator(['byte', 'byte'], 'byte', '+'),
  generateOperator(['byte', 'byte'], 'byte', '-'),
  generateOperator(['byte', 'byte'], 'byte', '×'),
  generateOperator(['byte', 'byte'], 'byte', '÷'),
  generateOperator(['byte', 'byte'], 'bool', '<'),
  generateOperator(['byte', 'byte'], 'bool', '>'),
  generateOperator(['byte', 'byte'], 'bool', '<='),
  generateOperator(['byte', 'byte'], 'bool', '>='),
  generateOperator(['byte', 'byte'], 'bool', '=='),
  generateOperator(['byte', 'byte'], 'bool', '!='),
  generateOperator(['vector', 'vector'], 'vector', '+'),
  generateOperator(['vector', 'vector'], 'vector', '-'),
  generateOperator(['vector', 'vector'], 'vector', '×'),
  generateOperator(['vector', 'vector'], 'vector', '÷'),
  generateOperator(['vector', 'vector'], 'bool', '<'),
  generateOperator(['vector', 'vector'], 'bool', '>'),
  generateOperator(['vector', 'vector'], 'bool', '<='),
  generateOperator(['vector', 'vector'], 'bool', '>='),
  generateOperator(['vector', 'vector'], 'bool', '=='),
  generateOperator(['vector', 'vector'], 'bool', '!='),
  generateOperator(['rotator', 'rotator'], 'rotator', '+'),
  generateOperator(['rotator', 'rotator'], 'rotator', '-'),
  generateOperator(['rotator', 'rotator'], 'rotator', '×'),
  generateOperator(['rotator', 'rotator'], 'rotator', '÷'),
  generateOperator(['rotator', 'rotator'], 'bool', '<'),
  generateOperator(['rotator', 'rotator'], 'bool', '>'),
  generateOperator(['rotator', 'rotator'], 'bool', '<='),
  generateOperator(['rotator', 'rotator'], 'bool', '>='),
  generateOperator(['rotator', 'rotator'], 'bool', '=='),
  generateOperator(['rotator', 'rotator'], 'bool', '!='),
  generateOperator(['transform', 'transform'], 'transform', '+'),
  generateOperator(['transform', 'transform'], 'transform', '-'),
  generateOperator(['transform', 'transform'], 'transform', '×'),
  generateOperator(['transform', 'transform'], 'transform', '÷'),
  generateOperator(['transform', 'transform'], 'bool', '<'),
  generateOperator(['transform', 'transform'], 'bool', '>'),
  generateOperator(['transform', 'transform'], 'bool', '<='),
  generateOperator(['transform', 'transform'], 'bool', '>='),
  generateOperator(['transform', 'transform'], 'bool', '=='),
  generateOperator(['transform', 'transform'], 'bool', '!='),
  generateOperator(['bool', 'bool'], 'bool', 'AND'),
  generateOperator(['bool', 'bool'], 'bool', 'OR'),
  generateOperator(['bool', 'bool'], 'bool', 'XOR'),
  generateOperator(['bool'], 'bool', 'NOT'),
  generateOperator(['string', 'string'], 'string', '+'),
  generateOperator(['string', 'string'], 'bool', '=='),
  generateOperator(['string', 'string'], 'bool', '!='),
  generateOperator(['text', 'text'], 'text', '+'),
  generateOperator(['text', 'text'], 'bool', '=='),
  generateOperator(['text', 'text'], 'bool', '!='),
  generateOperator(['name', 'name'], 'name', '+'),
  generateOperator(['name', 'name'], 'bool', '=='),
  generateOperator(['name', 'name'], 'bool', '!=')
];

/**
 * @brief Generates a cast operator node template.
 * @param input - The input pin type.
 * @param output - The output pin type.
 * @returns The generated node template.
 */
function generateCastOperator(input: PinType, output: PinType): NodeTemplate {
  return {
    id: `cast_${input}_to_${output}`,
    name: `Cast ${input} to ${output}`,
    category: 'Operators',
    data: {
      id: '',
      roundedBg: true,
      inputs: [{ id: '', type: input }],
      outputs: [{ id: '', type: output }],
      type: 'cast'
    },
    tags: ['operator', 'cast', `${input}_to_${output}`],
    snippet: `${convertPinTypeToEngineType(output)} {RESULT} = static_cast<${convertPinTypeToEngineType(output)}>({A});`
  };
}

/**
 * @brief List of cast operator node templates.
 */
export const CastOperators: NodeTemplate[] = [
  ...['float', 'int', 'real', 'byte', 'bool']
    .map((fromType) => {
      return ['float', 'int', 'real', 'byte', 'bool']
        .filter((toType) => fromType !== toType)
        .map((toType) => {
          return generateCastOperator(fromType as PinType, toType as PinType);
        });
    })
    .reduce((acc, val) => acc.concat(val), []),
  ...['float', 'int', 'real', 'byte', 'bool'].map((fromType) => {
    return generateCastOperator(fromType as PinType, 'string');
  }),
  ...['float', 'int', 'real', 'byte', 'bool'].map((fromType) => {
    return generateCastOperator(fromType as PinType, 'text');
  }),
  ...['float', 'int', 'real', 'byte', 'bool'].map((fromType) => {
    return generateCastOperator(fromType as PinType, 'name');
  })
];
