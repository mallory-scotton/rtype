/**
 * @brief Pin types
 */
export type PinType =
  | 'byte'
  | 'int'
  | 'float'
  | 'string'
  | 'bool'
  | 'exec'
  | 'vector'
  | 'rotator'
  | 'struct'
  | 'object'
  | 'transform'
  | 'name'
  | 'text'
  | 'real';

/**
 * @brief Pin direction types
 */
export type PinDirection = 'input' | 'output';

/**
 * @brief Pin data interface
 */
export interface PinData {
  id: string;
  type: PinType;
  label?: string;
  filled?: boolean;
  value?: any;
  noMarginBottom?: boolean;
}
