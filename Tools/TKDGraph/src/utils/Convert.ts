/** Dependencies */
import type { PinType } from '../types';

/**
 * @brief Converts a pin type to its corresponding engine type.
 * @param pinType - The pin type to convert.
 * @returns The corresponding engine type as a string.
 */
export function convertPinTypeToEngineType(pinType: PinType): string {
  switch (pinType) {
    case 'byte':
      return 'Byte';
    case 'int':
      return 'Int32';
    case 'float':
      return 'Float32';
    case 'real':
      return 'Int64';
    case 'bool':
      return 'Bool';
    case 'string':
    case 'text':
    case 'name':
      return 'FString';
    case 'vector':
      return 'FVector';
    case 'rotator':
      return 'FRotator';
    case 'transform':
      return 'FTransform';
    case 'object':
      return 'UObject*';
    default:
      return 'auto';
  }
}

/**
 * @brief Converts a pin type to its default value.
 * @param pinType - The pin type to convert.
 * @returns The default value for the given pin type.
 */
export function convertPinTypeToDefaultValue(pinType: PinType): any {
  switch (pinType) {
    case 'byte':
    case 'int':
    case 'float':
    case 'real':
      return 0;
    case 'bool':
      return false;
    case 'string':
    case 'text':
    case 'name':
      return '';
    case 'vector':
    case 'rotator':
    case 'transform':
      return '';
    case 'object':
      return null;
    default:
      return null;
  }
}

/**
 * @brief Converts an operator symbol to its corresponding snippet operator.
 * @param operator - The operator symbol to convert.
 * @returns The corresponding snippet operator as a string.
 */
export function convertOperatorSymbolToSnippetOperator(operator: string): string {
  if (operator === '×') return '*';
  else if (operator === '÷') return '/';
  else if (operator === 'AND') return '&&';
  else if (operator === 'OR') return '||';
  else if (operator === 'XOR') return '^';
  else if (operator === 'NOT') return '!';
  return operator;
}
