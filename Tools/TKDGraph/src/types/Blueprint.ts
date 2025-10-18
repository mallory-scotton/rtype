/** Dependencies */
import type { NodeData } from './Node';
import type { Connection } from './Connection';
import type { PinType } from './Pin';

/**
 * @brief Blueprint Type enumeration
 * @description Represents a blueprint consisting of multiple nodes.
 */
export type BlueprintType =
  | 'AActor'
  | 'APawn'
  | 'AGameMode'
  | 'AHud'
  | 'UActorComponent'
  | 'AController'
  | 'APlayerController'
  | 'AAIController'
  | 'ANetworkController';

/**
 * @brief Node Entry Interface
 * @description Represents a single node within a blueprint.
 */
export interface NodeEntry {
  data: NodeData;
  position: { x: number; y: number };
  dimensions?: { width: number; height: number };
}

/**
 * @brief Property entry Interface
 * @description Represents a property within a blueprint.
 */
export interface PropertyEntry {
  name: string;
  type: Exclude<PinType, 'exec'>;
  value: any;
  options?: Record<string, any>;
}

/**
 * @brief Function Entry Interface
 * @description Represents a function within a blueprint.
 */
export interface FunctionEntry {
  name: string;
  returnType: string;
  parameters: Array<{ name: string; type: string }>;
}

/**
 * @brief Blueprint Interface
 * @description Represents the structure of a blueprint with nodes and type.
 */
export interface BlueprintData {
  className: string;
  nodes: NodeEntry[];
  connections: Connection[];
  type: BlueprintType;
  properties: PropertyEntry[];
  functions: FunctionEntry[];
}
