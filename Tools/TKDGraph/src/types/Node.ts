/** Dependencies */
import type { PinData } from './Pin';

/**
 * @brief Node types
 * @description Different types of nodes that can exist within the graph.
 */
export type NodeType = 'function' | 'event' | 'getter' | 'setter' | 'cast' | 'operator' | 'comment' | 'knot';

/**
 * @brief Represents the type of node headers.
 * @description This enum can be expanded to include different header types if needed.
 */
export type NodeHeaderType = 'event' | 'function-call' | 'pure-function-call' | 'macro' | 'cast';

/**
 * @brief Represents the type of icons that can be associated with nodes.
 * @description This enum can be expanded to include different icon types if needed.
 */
export type NodeIconType =
  | 'event'
  | 'pure-function-call'
  | 'function-call'
  | 'isvalid'
  | 'break-struct'
  | 'make-struct'
  | 'make-array'
  | 'input-key'
  | 'flipflop'
  | 'sequence'
  | 'do_n'
  | 'do_once'
  | 'loop'
  | 'foreach'
  | 'gate'
  | 'spawn-actor'
  | 'make-map'
  | 'exec-branch'
  | 'event-custom'
  | 'macro'
  | 'pill'
  | 'select'
  | 'input-mouse'
  | 'timeline'
  | 'switch'
  | 'input-touch'
  | 'input-gamepad'
  | 'blueprint-node'
  | 'cast';

/**
 * @brief Represents the header data of a node.
 * @description This interface can be expanded to include properties related to the node's header.
 */
export interface NodeHeaderData {
  type?: NodeHeaderType;
  label?: string;
  icon?: NodeIconType;
  backgroundColor?: string;
}

/**
 * @brief Represents the data structure of a node in the graph.
 * @description This interface defines the essential properties of a node,
 */
export interface NodeData {
  id: string;
  type: NodeType;
  label?: string;
  inputs?: PinData[];
  outputs?: PinData[];
  roundedBg?: boolean | string;
  roundedBgColor?: boolean;
  backgroundColor?: string;
  header?: NodeHeaderData;
}
